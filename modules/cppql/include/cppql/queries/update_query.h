#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/clauses/columns.h"
#include "cppql/clauses/limit.h"
#include "cppql/clauses/order_by.h"
#include "cppql/clauses/where.h"
#include "cppql/expressions/column_expression.h"
#include "cppql/expressions/filter_expression.h"
#include "cppql/statements/update_statement.h"

namespace sql
{
    /**
     * \brief The UpdateQuery class can be used to prepare a statement that updates existing rows in a table. It is
     * constructed using the update method of the TypedTable class. The generated code is of the format "UPDATE table
     * SET (cols) = (vals) WHERE expr ORDER BY expr LIMIT val OFFSET val;".
     * \tparam T TypedTable type.
     * \tparam F Filter expression type (or std::nullopt_t if not yet initialized). Is used to generate the WHERE clause.
     * \tparam O OrderBy expression type (or std::nullopt_t if not yet initialized). Is used to generate the ORDER BY clause.
     * \tparam L std::true_type to indicate a limit and offset value have been set (or std::nullopt_t if not yet initialized). Is used to generate the LIMIT OFFSET clause.
     * \tparam C List of ColumnExpression type. Must be a valid column of T.
     * \tparam Cs List of ColumnExpression types. Must be a valid column of T.
     */
    template<is_typed_table                 T,
             is_filter_expression_or_none   F,
             is_order_by_expression_or_none O,
             is_true_type_or_none           L,
             is_column_expression           C,
             is_column_expression... Cs>
    class UpdateQuery
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using table_t   = T;
        using filter_t  = Where<F>;
        using order_t   = OrderBy<O>;
        using limit_t   = Limit<L>;
        using columns_t = Columns<C, Cs...>;

        Table*    table;
        columns_t columns;
        filter_t  filter;
        order_t   order;
        limit_t   limit;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UpdateQuery() = delete;

        UpdateQuery(const UpdateQuery& other) = default;

        UpdateQuery(UpdateQuery&& other) noexcept = default;

        UpdateQuery(Table& t, columns_t cs) : table(&t), columns(std::move(cs)) {}

        UpdateQuery(Table& t, columns_t cs, filter_t f, order_t o, limit_t l) :
            table(&t), columns(std::move(cs)), filter(std::move(f)), order(std::move(o)), limit(std::move(l))
        {
        }

        ~UpdateQuery() noexcept = default;

        UpdateQuery& operator=(const UpdateQuery& other) = default;

        UpdateQuery& operator=(UpdateQuery&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Query.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Filter updated rows by an expression.
         * \tparam Self Self type.
         * \tparam Filter FilterExpression type.
         * \param self Self.
         * \param filter Expression to filter results by.
         * \return UpdateQuery with filter expression.
         */
        template<typename Self, is_valid_filter_expression<std::tuple<table_t>> Filter>
            requires(!filter_t::valid)
        [[nodiscard]] auto where(this Self&& self, Filter&& filter)
        {
            if (!filter.containsTables(*self.table))
                throw CppqlError(std::format(
                  "Cannot apply filter to query because the expression contains a table not in the query."));

            return UpdateQuery<T, std::remove_cvref_t<Filter>, O, L, C, Cs...>(
              *std::forward<Self>(self).table,
              std::forward<Self>(self).columns,
              Where<std::remove_cvref_t<Filter>>(std::forward<Filter>(filter)),
              std::forward<Self>(self).order,
              std::forward<Self>(self).limit);
        }

        /**
         * \brief Order updated rows by an expression. This query should not have an order applied yet.
         * \tparam Self Self type.
         * \tparam Order OrderByExpression type.
         * \param self Self.
         * \param order Expression to order rows by.
         * \return UpdateQuery with order by expression.
         */
        template<typename Self, is_valid_order_by_expression<std::tuple<table_t>> Order>
            requires(!order_t::valid)
        [[nodiscard]] auto orderBy(this Self&& self, Order&& order)
        {
            if (!order.containsTables(*self.table))
                throw CppqlError(std::format(
                  "Cannot apply ordering to query because the expression contains a table not in the query."));

            return UpdateQuery<T, F, std::remove_cvref_t<Order>, L, C, Cs...>(
              *std::forward<Self>(self).table,
              std::forward<Self>(self).columns,
              std::forward<Self>(self).filter,
              OrderBy<std::remove_cvref_t<Order>>(std::forward<Order>(order)),
              std::forward<Self>(self).limit);
        }

        /**
         * \brief Limit and offset updated rows. This query should not have a limit applied yet.
         * \tparam Self Self type.
         * \param self Self.
         * \param limit Number of rows to limit updated rows by.
         * \param offset Number of rows to offset updated rows by.
         * \return UpdateQuery with limit and offset.
         */
        template<typename Self>
            requires(!limit_t::valid)
        [[nodiscard]] auto limitOffset(this Self&& self, const int64_t limit, const int64_t offset)
        {
            return UpdateQuery<T, F, O, std::true_type, C, Cs...>(*std::forward<Self>(self).table,
                                                                  std::forward<Self>(self).columns,
                                                                  std::forward<Self>(self).filter,
                                                                  std::forward<Self>(self).order,
                                                                  Limit<std::true_type>(limit, offset));
        }

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string toString()
        {
            auto index = 1;

            std::string vals = "?1";
            for (; index < columns_t::size; index++) vals += std::format(",?{0}", index + 1);

            // UPDATE <table> SET (<cols>) = (<vals>) WHERE <expr> ORDER BY <expr> LIMIT <val> OFFSET <val>;
            auto sql = std::format("UPDATE {0} SET ({1}) = ({2}) {3} {4} {5};",
                                   table->getName(),
                                   columns.toString(),
                                   std::move(vals),
                                   filter.toString(),
                                   order.toString(),
                                   limit.toString());

            return sql;
        }

        void generateIndices()
        {
            int32_t index = columns_t::size;
            filter.generateIndices(index);
        }

        /**
         * \brief Generate UpdateStatement object. Generates and compiles SQL code and binds requested parameters.
         * \tparam Self Self type.
         * \param self Self.
         * \return UpdateStatement.
         */
        template<typename Self>
        [[nodiscard]] auto compile(this Self&& self)
        {
            self.generateIndices();

            // Construct statement. Note: This generates the bind indices of all filter expressions
            // and should therefore happen before the BaseFilterExpressionPtr construction below.
            auto stmt = std::make_unique<Statement>(self.table->getDatabase(), self.toString(), true);
            if (!stmt->isPrepared())
                throw SqliteError(std::format("Failed to prepare statement \"{}\"", stmt->getSql()),
                                  stmt->getResult()->code);

            BaseFilterExpressionPtr f;
            if constexpr (filter_t::valid)
                f = std::make_unique<FilterExpression<typename filter_t::filter_t>>(
                  std::forward<Self>(self).filter.filter);

            return UpdateStatement<typename C::value_t, typename Cs::value_t...>(std::move(stmt), std::move(f));
        }
    };
}  // namespace sql
