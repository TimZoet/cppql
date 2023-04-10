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
#include "cppql/clauses/group_by.h"
#include "cppql/clauses/having.h"
#include "cppql/clauses/limit.h"
#include "cppql/clauses/order_by.h"
#include "cppql/clauses/union.h"
#include "cppql/clauses/where.h"
#include "cppql/expressions/column_expression.h"
#include "cppql/expressions/filter_expression.h"
#include "cppql/statements/select_statement.h"
#include "cppql/statements/select_one_statement.h"
#include "cppql/typed/fwd.h"

namespace sql
{
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

    /**
     * \brief The SelectQuery class can be used to prepare a statement that selects rows from a table. It is
     * constructed using the select method of the TypedTable and Join classes. The generated code is of the
     * format "SELECT cols FROM table JOIN tables... WHERE expr GROUP BY cols HAVING expr union ORDER BY expr LIMIT
     * val OFFSET val;".
     * \tparam R Return type. Must be constructible from the value types of {C, Cs...}.
     * \tparam J TypedTable type or Join type.
     * \tparam F Filter expression type (or std::nullopt_t if not yet initialized). Is used to generate the WHERE clause.
     * \tparam O OrderBy expression type (or std::nullopt_t if not yet initialized). Is used to generate the ORDER BY clause.
     * \tparam L std::true_type to indicate a limit and offset value have been set (or std::nullopt_t if not yet initialized). Is used to generate the LIMIT OFFSET clause.
     * \tparam G GroupBy<Cols...> type (or std::nullopt_t if not yet initialized). Is used to generate the GROUP BY clause.
     * \tparam H Filter expression type (or std::nullopt_t if not yet initialized). Is used to generate the HAVING clause.
     * \tparam U Union<Qs...> type (or std::nullopt_t if not yet initialized), where Qs... is a set of valid SelectQuery types. Is used to generate the sequence of UNION clauses.
     * \tparam C Result expression type.
     * \tparam Cs Result expression types.
     */
    template<typename R,
             is_join_or_typed_table         J,
             is_filter_expression_or_none   F,
             is_order_by_expression_or_none O,
             is_true_type_or_none           L,
             is_group_by_or_none            G,
             is_filter_expression_or_none   H,
             is_union_or_none               U,
             is_result_expression           C,
             is_result_expression... Cs>
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
        using group_t      = std::conditional_t<std::same_as<std::nullopt_t, G>, GroupBy<std::nullopt_t>, G>;
        using having_t     = Having<H>;
        using union_t      = std::conditional_t<std::same_as<std::nullopt_t, U>, Union<std::nullopt_t>, U>;
        using table_list_t = lazy_table_list_t<join_t>;

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        join_t    join;
        columns_t columns;
        filter_t  filter;
        order_t   order;
        limit_t   limit;
        group_t   groups;
        having_t  havings;
        union_t   unionClause;

    public:
        UnionOperator unionOp = UnionOperator::Union;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        SelectQuery() = delete;

        SelectQuery(const SelectQuery& other) = default;

        SelectQuery(SelectQuery&& other) noexcept = default;

        SelectQuery(join_t j, columns_t cs) : join(std::move(j)), columns(std::move(cs)) {}

        SelectQuery(join_t j, columns_t cs, filter_t f, order_t o, limit_t l, group_t g, having_t h, union_t u) :
            join(std::move(j)),
            columns(std::move(cs)),
            filter(std::move(f)),
            order(std::move(o)),
            limit(std::move(l)),
            groups(std::move(g)),
            havings(std::move(h)),
            unionClause(std::move(u))
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

            return SelectQuery<R, J, std::remove_cvref_t<Filter>, O, L, G, H, U, C, Cs...>(
              std::forward<Self>(self).join,
              std::forward<Self>(self).columns,
              Where<std::remove_cvref_t<Filter>>(std::forward<Filter>(filter)),
              std::forward<Self>(self).order,
              std::forward<Self>(self).limit,
              std::forward<Self>(self).groups,
              std::forward<Self>(self).havings,
              std::forward<Self>(self).unionClause);
        }

        template<typename Self,
                 is_valid_column_expression<table_list_t> Col,
                 is_valid_column_expression<table_list_t>... Cols>
            requires(!group_t::valid)
        [[nodiscard]] auto groupBy(this Self&& self, Col&& col, Cols&&... cols)
        {
            if constexpr (is_table)
            {
                if ((!col.containsTables(self.join.getTable()) || ... || !cols.containsTables(self.join.getTable())))
                    throw CppqlError(std::format("Cannot apply group by to query because at least one of the columns "
                                                 "contains a table not in the query."));
            }
            else
            {
                if ((!self.join.containsTables(col, self.join.getTable()) || ... ||
                     !self.join.containsTables(cols, self.join.getTable())))
                    throw CppqlError(std::format("Cannot apply group by to query because at least one of the columns "
                                                 "contains a table not in the query."));
            }

            using clause = GroupBy<std::remove_cvref_t<Col>, std::remove_cvref_t<Cols>...>;
            return SelectQuery<R, J, F, O, L, clause, H, U, C, Cs...>(
              std::forward<Self>(self).join,
              std::forward<Self>(self).columns,
              std::forward<Self>(self).filter,
              std::forward<Self>(self).order,
              std::forward<Self>(self).limit,
              clause(std::forward<Col>(col), std::forward<Cols>(cols)...),
              std::forward<Self>(self).havings,
              std::forward<Self>(self).unionClause);
        }


        template<typename Self, is_valid_filter_expression<table_list_t> Filter>
            requires(group_t::valid && !having_t::valid)
        [[nodiscard]] auto having(this Self&& self, Filter&& filter)
        {
            if constexpr (is_table)
            {
                if (!filter.containsTables(self.join.getTable()))
                    throw CppqlError(std::format(
                      "Cannot apply having to query because the expression contains a table not in the query."));
            }
            else
            {
                if (!self.join.containsTables(filter, self.join.getTable()))
                    throw CppqlError(std::format(
                      "Cannot apply having to query because the expression contains a table not in the query."));
            }

            return SelectQuery<R, J, F, O, L, G, std::remove_cvref_t<Filter>, U, C, Cs...>(
              std::forward<Self>(self).join,
              std::forward<Self>(self).columns,
              std::forward<Self>(self).filter,
              std::forward<Self>(self).order,
              std::forward<Self>(self).limit,
              std::forward<Self>(self).groups,
              Having<std::remove_cvref_t<Filter>>(std::forward<Filter>(filter)),
              std::forward<Self>(self).unionClause);
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

            return SelectQuery<R, J, F, std::remove_cvref_t<Order>, L, G, H, U, C, Cs...>(
              std::forward<Self>(self).join,
              std::forward<Self>(self).columns,
              std::forward<Self>(self).filter,
              OrderBy<std::remove_cvref_t<Order>>(std::forward<Order>(order)),
              std::forward<Self>(self).limit,
              std::forward<Self>(self).groups,
              std::forward<Self>(self).havings,
              std::forward<Self>(self).unionClause);
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
            return SelectQuery<R, J, F, O, std::true_type, G, H, U, C, Cs...>(std::forward<Self>(self).join,
                                                                              std::forward<Self>(self).columns,
                                                                              std::forward<Self>(self).filter,
                                                                              std::forward<Self>(self).order,
                                                                              Limit<std::true_type>(limit, offset),
                                                                              std::forward<Self>(self).groups,
                                                                              std::forward<Self>(self).havings,
                                                                              std::forward<Self>(self).unionClause);
        }

        /**
         * \brief Create union between this query and other query. Other query should have the same return type, and should not have an order or limit applied.
         * \tparam Self Self type.
         * \tparam Q Other query type.
         * \param self Self.
         * \param op Union operator.
         * \param query Query to create a union with.
         * \return SelectQuery with (additional) union.
         */
        template<typename Self, typename Q>
            requires(!std::remove_cvref_t<Q>::order_t::valid && !std::remove_cvref_t<Q>::limit_t::valid &&
                     std::same_as<return_t, typename std::remove_cvref_t<Q>::return_t>)
        [[nodiscard]] auto unions(this Self&& self, const UnionOperator op, Q&& query)
        {
            // This query already has a valid Union clause. Append new query to it.
            if constexpr (union_t::valid)
            {
                using clause = merge_unions_t<std::remove_cvref_t<Q>, typename union_t::query_t>;
                return SelectQuery<R, J, F, O, L, G, H, clause, C, Cs...>(
                  std::forward<Self>(self).join,
                  std::forward<Self>(self).columns,
                  std::forward<Self>(self).filter,
                  std::forward<Self>(self).order,
                  std::forward<Self>(self).limit,
                  std::forward<Self>(self).groups,
                  std::forward<Self>(self).havings,
                  clause(op, std::forward<Self>(self).unionClause.query, std::forward<Q>(query)));
            }
            // This query does not yet have a Union clause. Construct with single query.
            else
            {
                using clause = Union<std::remove_cvref_t<Q>>;
                return SelectQuery<R, J, F, O, L, G, H, clause, C, Cs...>(std::forward<Self>(self).join,
                                                                          std::forward<Self>(self).columns,
                                                                          std::forward<Self>(self).filter,
                                                                          std::forward<Self>(self).order,
                                                                          std::forward<Self>(self).limit,
                                                                          std::forward<Self>(self).groups,
                                                                          std::forward<Self>(self).havings,
                                                                          clause(op, std::forward<Q>(query)));
            }
        }

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string toString()
        {
            // SELECT <cols> FROM <table> JOIN <tables...> WHERE <expr> GROUP BY <cols> <HAVING> <expr> <union> ORDER BY <expr> LIMIT <val> OFFSET <val>
            auto sql = std::format("SELECT {0} FROM {1} {2} {3} {4} {5} {6} {7}",
                                   columns.toStringFull(),
                                   join.toString(),
                                   filter.toString(),
                                   groups.toString(),
                                   havings.toString(),
                                   unionClause.toString(),
                                   order.toString(),
                                   limit.toString());

            return sql;
        }

        void generateIndices(int32_t& idx)
        {
            if constexpr (!is_table) join.generateIndices(idx);
            filter.generateIndices(idx);
            havings.generateIndices(idx);
            unionClause.generateIndices(idx);
        }

        [[nodiscard]] auto getFilters()
        {
            if constexpr (is_table)
            {
                return std::tuple_cat(filter.getFilters(), havings.getFilters(), unionClause.getFilters());
            }
            else
            {
                return std::tuple_cat(
                  join.getFilters(), filter.getFilters(), havings.getFilters(), unionClause.getFilters());
            }
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
            int32_t idx = 0;
            self.generateIndices(idx);

            auto select = []<std::size_t... Is>(auto&& self, std::index_sequence<Is...>)
            {
                // Construct statement. Note: This generates the bind indices of all filter expressions
                // and should therefore happen before the BaseFilterExpressionPtr construction below.
                auto stmt = std::make_unique<Statement>(
                  self.join.getTable().getDatabase(), std::format("{0};", self.toString()), true);
                if (!stmt->isPrepared())
                    throw SqliteError(std::format("Failed to prepare statement \"{}\"", stmt->getSql()),
                                      stmt->getResult()->code,
                                      stmt->getResult()->extendedCode);

                // Concatenate the filter expressions of any joins, unions and other clauses.
                BaseFilterExpressionPtr f;

                auto filters = self.getFilters();
                if constexpr (std::tuple_size_v<decltype(filters)> != 0)
                {
                    f = []<size_t... Js, typename T>(std::index_sequence<Js...>, T && fs)
                    {
                        return std::make_unique<FilterExpression<std::tuple_element_t<Js, T>...>>(
                          std::get<Js>(std::forward<T>(fs))...);
                    }
                    (std::make_index_sequence<std::tuple_size_v<decltype(filters)>>{}, std::move(filters));
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
