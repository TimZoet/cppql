#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/clauses/columns.h"
#include "cppql-typed/clauses/limit.h"
#include "cppql-typed/clauses/order_by.h"
#include "cppql-typed/clauses/where.h"
#include "cppql-typed/expressions/column_expression.h"
#include "cppql-typed/expressions/filter_expression.h"
#include "cppql-typed/statements/select_statement.h"
#include "cppql-typed/statements/select_one_statement.h"

namespace sql
{
    // TODO: Aggregates.
    // TODO: Group by.
    // TODO: Having.
    // TODO: Unions.
    // TODO: Aliases.

    ////////////////////////////////////////////////////////////////

    template<typename T>
    struct lazy_table_list
    {
        using type = std::tuple<T>;
    };

    template<is_join T>
    struct lazy_table_list<T>
    {
        using type = typename T::table_list_t;
    };

    template<typename T>
    using lazy_table_list_t = typename lazy_table_list<T>::type;

    template<typename R,
             typename J,
             typename F,
             typename O,
             typename L,
             is_column_expression C,
             is_column_expression... Cs>
        requires(constructible_from<R, typename C::value_t, typename Cs::value_t...>)
    class SelectQuery
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool is_table = !is_join<J>;
        using join_t                   = J;
        using columns_t                = Columns<C, Cs...>;
        using return_t                 = R;
        using filter_t                 = Where<F>;
        using order_t                  = OrderBy<O>;
        using limit_t                  = Limit<L>;
        using table_list_t             = lazy_table_list_t<join_t>;

        join_t    join;
        columns_t columns;
        filter_t  filter;
        order_t   order;
        limit_t   limit;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        SelectQuery() = delete;

        SelectQuery(const SelectQuery& other) = default;

        SelectQuery(SelectQuery&& other) noexcept = default;

        SelectQuery(join_t j, columns_t cs) : join(std::move(j)), columns(std::move(cs)) {}

        SelectQuery(join_t j, columns_t cs, filter_t f, order_t o, limit_t l) :
            join(std::move(j)), columns(std::move(cs)), filter(std::move(f)), order(std::move(o)), limit(std::move(l))
        {
        }

        ~SelectQuery() noexcept = default;

        SelectQuery& operator=(const SelectQuery& other) = default;

        SelectQuery& operator=(SelectQuery&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Query.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Filter results by an expression. This query should not have a filter applied yet.
         * \tparam Self Self type.
         * \tparam Filter FilterExpression type.
         * \param self Self.
         * \param filter Expression to filter results by.
         * \return SelectQuery with filter expression.
         */
        template<typename Self, is_valid_filter_expression<table_list_t> Filter>
            requires(!filter_t::valid)
        [[nodiscard]] auto where(this Self&& self, Filter&& filter)
        {
            if constexpr (is_table)
            {
                if (!filter.containsTables(self.join.getTable()))
                    throw CppqlError(std::format(
                      "Cannot apply filter to query because the expression contains a table not in the query."));
            }
            else
            {
                if (!self.join.containsTables(filter, self.join.getTable()))
                    throw CppqlError(std::format(
                      "Cannot apply filter to query because the expression contains a table not in the query."));
            }

            return SelectQuery<R, J, std::remove_cvref_t<Filter>, O, L, C, Cs...>(
              std::forward<Self>(self).join,
              std::forward<Self>(self).columns,
              Where<std::remove_cvref_t<Filter>>(std::forward<Filter>(filter)),
              std::forward<Self>(self).order,
              std::forward<Self>(self).limit);
        }

        /**
         * \brief Order result rows by an expression. This query should not have an order applied yet.
         * \tparam Self Self type.
         * \tparam Order OrderByExpression type.
         * \param self Self.
         * \param order Expression to order rows by.
         * \return SelectQuery with order by expression.
         */
        template<typename Self, is_valid_order_by_expression<table_list_t> Order>
            requires(!order_t::valid)
        [[nodiscard]] auto orderBy(this Self&& self, Order&& order)
        {
            if constexpr (is_table)
            {
                if (!order.containsTables(self.join.getTable()))
                    throw CppqlError(std::format(
                      "Cannot apply ordering to query because the expression contains a table not in the query."));
            }
            else
            {
                if (!self.join.containsTables(order, self.join.getTable()))
                    throw CppqlError(std::format(
                      "Cannot apply ordering to query because the expression contains a table not in the query."));
            }

            return SelectQuery<R, J, F, std::remove_cvref_t<Order>, L, C, Cs...>(
              std::forward<Self>(self).join,
              std::forward<Self>(self).columns,
              std::forward<Self>(self).filter,
              OrderBy<std::remove_cvref_t<Order>>(std::forward<Order>(order)),
              std::forward<Self>(self).limit);
        }

        /**
         * \brief Limit and offset result rows. This query should not have a limit applied yet.
         * \tparam Self Self type.
         * \param self Self.
         * \param limit Number of rows to limit results by.
         * \param offset Number of rows to offset results by.
         * \return SelectQuery with limit and offset.
         */
        template<typename Self>
            requires(!limit_t::valid)
        [[nodiscard]] auto limitOffset(this Self&& self, const int64_t limit, const int64_t offset)
        {
            return SelectQuery<R, J, F, O, std::true_type, C, Cs...>(std::forward<Self>(self).join,
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
            // SELECT <cols> FROM <table> JOIN <tables...> WHERE <expr> ORDER BY <expr> LIMIT <val> OFFSET <val>;
            auto sql = std::format("SELECT {0} FROM {1} {2} {3} {4};",
                                   columns.toStringFull(),
                                   join.toString(),
                                   filter.toString(),
                                   order.toString(),
                                   limit.toString());

            return sql;
        }

        /**
         * \brief Generate SelectStatement object. Generates and compiles SQL code and binds requested parameters.
         * \tparam Self Self type.
         * \param self Self.
         * \return SelectStatement.
         */
        template<typename Self>
        [[nodiscard]] auto compile(this Self&& self)
        {
            int32_t index = 0;
            if constexpr (!is_table) std::forward<Self>(self).join.generateIndices(index);
            std::forward<Self>(self).filter.generateIndices(index);

            auto select = []<std::size_t... Is>(auto&& self, std::index_sequence<Is...>)
            {
                // Construct statement. Note: This generates the bind indices of all filter expressions
                // and should therefore happen before the BaseFilterExpressionPtr construction below.
                auto stmt = std::make_unique<Statement>(self.join.getTable().getDatabase(), self.toString(), true);
                if (!stmt->isPrepared())
                    throw SqliteError(std::format("Failed to prepare statement \"{}\"", stmt->getSql()),
                                      stmt->getResult()->code);

                // Concatenate the filter expressions of any joints and where.
                BaseFilterExpressionPtr f;
                if constexpr (is_table)
                {
                    if constexpr (filter_t::valid)
                        f = std::make_unique<FilterExpression<typename filter_t::filter_t>>(
                          std::forward<Self>(self).filter.filter);
                }
                else
                {
                    if constexpr (filter_t::valid)
                        f = std::forward<Self>(self).join.getFilters(std::forward<Self>(self).filter.filter);
                    else
                        f = std::forward<Self>(self).join.getFilters();
                }

                // Construct typed statement.
                return SelectStatement<return_t, typename C::value_t, typename Cs::value_t...>(std::move(stmt),
                                                                                               std::move(f));
            };

            return select(std::forward<Self>(self), std::index_sequence_for<C, Cs...>());
        }

        /**
         * \brief Generate SelectOneStatement object. Generates and compiles SQL code and binds requested parameters.
         * \tparam Self Self type.
         * \param self Self.
         * \return SelectOneStatement.
         */
        template<typename Self>
        [[nodiscard]] auto compileOne(this Self&& self)
        {
            return SelectOneStatement(std::forward<Self>(self).compile());
        }
    };
}  // namespace sql
