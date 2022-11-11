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
#include "cppql-typed/clauses/order_by.h"
#include "cppql-typed/clauses/where.h"
#include "cppql-typed/expressions/column_expression.h"
#include "cppql-typed/expressions/filter_expression.h"
#include "cppql-typed/expressions/limit_expression.h"
#include "cppql-typed/queries/select.h"

namespace sql
{
    template<typename J, typename F, typename O, is_column_expression C, is_column_expression... Cs>
    class ComplexSelect
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using join_t = J;
        using columns_t = Columns<C, Cs...>;
        using filter_t = Where<F>;
        using order_t = OrderBy<O>;
        using limit_t = std::optional<LimitExpression>;
        using table_list_t = typename join_t::table_list_t;

        join_t    join;
        columns_t columns;
        filter_t  filter;
        order_t   order;
        limit_t   limit;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComplexSelect(const ComplexSelect& other) = default;

        ComplexSelect(ComplexSelect&& other) noexcept  = default;

        ComplexSelect(join_t j, columns_t cs) : join(std::move(j)), columns(std::move(cs)), filter(std::nullopt), order(std::nullopt)
        {
        }

        ComplexSelect(join_t j, columns_t cs, filter_t f, order_t o) : join(std::move(j)), columns(std::move(cs)), filter(std::move(f)), order(std::move(o))
        {
        }

        ~ComplexSelect() noexcept = default;

        ComplexSelect& operator=(const ComplexSelect& other) = default;

        ComplexSelect& operator=(ComplexSelect&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Filter results by an expression. This select should not have a filter applied yet.
         * \tparam Self Self type.
         * \tparam Filter FilterExpression type.
         * \param self Self.
         * \param filter Expression to filter results by.
         * \return ComplexSelect with filter expression.
         */
        template<typename Self, is_valid_filter_expression<table_list_t> Filter> requires (!filter_t::valid)
        auto where(this Self&& self, Filter&& filter)
        {
            // TODO: Check table instances in filter match tables in joins.

            return ComplexSelect<J, std::decay_t<Filter>, O, C, Cs...>(
                std::forward<Self>(self).join,
                std::forward<Self>(self).columns,
                Where<std::decay_t<Filter>>(std::forward<Filter>(filter)),
                std::forward<Self>(self).order
            );
        }

        /**
         * \brief Order result rows by an expression. This select should not have an order applied yet.
         * \tparam Self Self type.
         * \tparam Order OrderByExpression type.
         * \param self Self.
         * \param order Expression to order rows by.
         * \return ComplexSelect with filter expression.
         */
        template<typename Self, is_valid_order_by_expression<table_list_t> Order> requires (!order_t::valid)
        auto orderBy(this Self&& self, Order&& order)
        {
            // TODO: Check table instances in filter match tables in joins.

            return ComplexSelect<J, F, std::decay_t<Order>, C, Cs...>(
                std::forward<Self>(self).join,
                std::forward<Self>(self).columns,
                std::forward<Self>(self).filter,
                OrderBy<std::decay_t<Order>>(std::forward<Order>(order))
            );
        }

        // TODO: Aggregates.
        // TODO: Group by.
        // TODO: Having.
        // TODO: Unions.
        // TODO: Aliases.

        template<typename Self>
        auto&& limitOffset(this Self&& self, LimitExpression limitExpression)
        {
            std::forward<Self>(self).limit = std::move(limitExpression);
            return std::forward<Self>(self);
        }
        
        template<typename Self>
        [[nodiscard]] std::string toString(this Self&& self)
        {
            auto index = 0;

            // SELECT <cols> FROM <table> JOIN <tables...> WHERE <expr> ORDER BY <expr> LIMIT <val> OFFSET <val>;
            auto sql = std::format(
                "SELECT {0} FROM {1} {2} {3} {4};",
                std::forward<Self>(self).columns.toString(index),
                std::forward<Self>(self).join.toString(index),
                std::forward<Self>(self).filter.toString(index),
                std::forward<Self>(self).order.toString(),
                std::forward<Self>(self).limit ? std::forward<Self>(self).limit->toString() : ""
            );

            return sql;
        }

        template<typename Self>
        [[nodiscard]] auto compile(this Self&& self)
        {
            auto select = []<std::size_t... Is>(auto&& self, std::index_sequence<Is...>)
            {
                auto f = optionalToPtr(std::forward<Self>(self).filter.filter);
                
                auto stmt = std::make_unique<Statement>(std::forward<Self>(self).join.right->getDatabase(), std::forward<Self>(self).toString(), true);
                if (!stmt->isPrepared())
                    throw SqliteError(std::format("Failed to prepare statement \"{}\"", stmt->getSql()),
                        stmt->getResult()->code);

                // TODO: Bind parameters properly.
                if (f) f->bind(*stmt, BindParameters::All);
                
                return Select<typename columns_t::table_t::row_t, col_t<Is, typename columns_t::table_t>...>(std::move(stmt), std::move(f));
            };

            return select(std::forward<Self>(self), std::index_sequence_for<C, Cs...>());
        }
    };
}  // namespace sql
