#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/error/cppql_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/clauses/limit.h"
#include "cppql-typed/clauses/order_by.h"
#include "cppql-typed/clauses/where.h"
#include "cppql-typed/expressions/filter_expression.h"
#include "cppql-typed/statements/delete_statement.h"

namespace sql
{
    template<typename T, typename F, typename O, typename L>
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
        template<typename Self, is_single_filter_expression<table_t> Filter>
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
        template<typename Self, is_order_by_expression<table_t> Order>
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

        /**
         * \brief Generate DeleteStatement object. Generates and compiles SQL code and binds requested parameters.
         * \tparam Self Self type.
         * \param self Self.
         * \return DeleteStatement.
         */
        template<typename Self>
        [[nodiscard]] auto compile(this Self&& self)
        {
            int32_t index = 0;
            std::forward<Self>(self).filter.generateIndices(index);

            // Construct statement from generated SQL.
            auto stmt = std::make_unique<Statement>(self.table->getDatabase(), self.toString(), true);
            if (!stmt->isPrepared())
                throw SqliteError(std::format("Failed to prepare statement \"{}\"", stmt->getSql()),
                                  stmt->getResult()->code);

            // Optionally create filter expression.
            BaseFilterExpressionPtr f;
            if constexpr (filter_t::valid)
                f = std::make_unique<typename filter_t::filter_t>(std::forward<Self>(self).filter.filter);

            return DeleteStatement(std::move(stmt), std::move(f));
        }
    };
}  // namespace sql
