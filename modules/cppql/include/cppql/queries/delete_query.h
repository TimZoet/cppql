#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/clauses/limit.h"
#include "cppql/clauses/order_by.h"
#include "cppql/clauses/where.h"
#include "cppql/error/cppql_error.h"
#include "cppql/expressions/filter_expression.h"
#include "cppql/statements/delete_statement.h"

namespace sql
{
    /**
     * \brief The DeleteQuery class can be used to prepare a statement counting the number of rows in a table. It is
     * constructed using the del method of the TypedTable class. The generated code is of the format "DELETE FROM
     * table WHERE expr ORDER BY expr LIMIT val OFFSET val;".
     * \tparam T TypedTable type.
     * \tparam F Filter expression type (or std::nullopt_t if not yet initialized). Is used to generate the WHERE clause.
     * \tparam O OrderBy expression type (or std::nullopt_t if not yet initialized). Is used to generate the ORDER BY clause.
     * \tparam L std::true_type to indicate a limit and offset value have been set (or std::nullopt_t if not yet initialized). Is used to generate the LIMIT OFFSET clause.
     */
    template<is_typed_table T, is_filter_expression_or_none F, is_order_by_expression_or_none O, is_true_type_or_none L>
    class DeleteQuery
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using table_t  = T;
        using filter_t = Where<F>;
        using order_t  = OrderBy<O>;
        using limit_t  = Limit<L>;

        Table*   table;
        filter_t filter;
        order_t  order;
        limit_t  limit;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        DeleteQuery() = delete;

        DeleteQuery(const DeleteQuery& other) = default;

        DeleteQuery(DeleteQuery&& other) noexcept = default;

        explicit DeleteQuery(Table& t) : table(&t) {}

        DeleteQuery(Table& t, filter_t f, order_t o, limit_t l) :
            table(&t), filter(std::move(f)), order(std::move(o)), limit(std::move(l))
        {
        }

        ~DeleteQuery() noexcept = default;

        DeleteQuery& operator=(const DeleteQuery& other) = default;

        DeleteQuery& operator=(DeleteQuery&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Query.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Filter deleted rows by an expression. This query should not have a filter applied yet.
         * \tparam Self Self type.
         * \tparam Filter FilterExpression type.
         * \param self Self.
         * \param filter Expression to filter deleted rows by.
         * \return DeleteQuery with filter expression.
         */
        template<typename Self, is_valid_filter_expression<std::tuple<table_t>> Filter>
            requires(!filter_t::valid)
        [[nodiscard]] auto where(this Self&& self, Filter&& filter)
        {
            if (!filter.containsTables(*self.table))
                throw CppqlError(std::format(
                  "Cannot apply filter to query because the expression contains a table not in the query."));

            return DeleteQuery<T, std::remove_cvref_t<Filter>, O, L>(
              *std::forward<Self>(self).table,
              Where<std::remove_cvref_t<Filter>>(std::forward<Filter>(filter)),
              std::forward<Self>(self).order,
              std::forward<Self>(self).limit);
        }

        /**
         * \brief Order deleted rows by an expression. This query should not have an order applied yet.
         * \tparam Self Self type.
         * \tparam Order OrderByExpression type.
         * \param self Self.
         * \param order Expression to order rows by.
         * \return DeleteQuery with order by expression.
         */
        template<typename Self, is_valid_order_by_expression<std::tuple<table_t>> Order>
            requires(!order_t::valid)
        [[nodiscard]] auto orderBy(this Self&& self, Order&& order)
        {
            if (!order.containsTables(*self.table))
                throw CppqlError(std::format(
                  "Cannot apply ordering to query because the expression contains a table not in the query."));

            return DeleteQuery<T, F, std::remove_cvref_t<Order>, L>(
              *std::forward<Self>(self).table,
              std::forward<Self>(self).filter,
              OrderBy<std::remove_cvref_t<Order>>(std::forward<Order>(order)),
              std::forward<Self>(self).limit);
        }

        /**
         * \brief Limit and offset deleted rows. This query should not have a limit applied yet.
         * \tparam Self Self type.
         * \param self Self.
         * \param limit Number to limit deleted rows by.
         * \param offset Number to offset deleted rows by.
         * \return DeleteQuery with limit and offset.
         */
        template<typename Self>
            requires(!limit_t::valid)
        [[nodiscard]] auto limitOffset(this Self&& self, const int64_t limit, const int64_t offset)
        {
            return DeleteQuery<T, F, O, std::true_type>(*std::forward<Self>(self).table,
                                                        std::forward<Self>(self).filter,
                                                        std::forward<Self>(self).order,
                                                        Limit<std::true_type>(limit, offset));
        }

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string toString()
        {
            // DELETE FROM <table> WHERE <expr> ORDER BY <expr> LIMIT <val> OFFSET <val>;
            auto sql = std::format(
              "DELETE FROM {0} {1} {2} {3};", table->getName(), filter.toString(), order.toString(), limit.toString());

            return sql;
        }

        void generateIndices()
        {
            int32_t index = 0;
            filter.generateIndices(index);
        }

        /**
         * \brief Generate DeleteStatement object. Generates and compiles SQL code and binds requested parameters.
         * \tparam Self Self type.
         * \param self Self.
         * \return DeleteStatement.
         */
        template<typename Self>
        [[nodiscard]] auto compile(this Self&& self)
        {
            self.generateIndices();

            // Construct statement from generated SQL.
            auto stmt = std::make_unique<Statement>(self.table->getDatabase(), self.toString(), true);
            if (!stmt->isPrepared())
                throw SqliteError(std::format("Failed to prepare statement \"{}\"", stmt->getSql()),
                                  stmt->getResult()->code,
                                  stmt->getResult()->extendedCode);

            // Optionally create filter expression.
            BaseFilterExpressionPtr f;
            if constexpr (filter_t::valid)
                f = std::make_unique<FilterExpression<typename filter_t::filter_t>>(
                  std::forward<Self>(self).filter.filter);

            return DeleteStatement(std::move(stmt), std::move(f));
        }
    };
}  // namespace sql
