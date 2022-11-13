#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <type_traits>

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
        // ...
        ////////////////////////////////////////////////////////////////

        template<typename Self, is_single_filter_expression<table_t> Filter>
            requires(!filter_t::valid)
        [[nodiscard]] auto where(this Self&& self, Filter&& filter)
        {
            // TODO: Check table instances in filter match table.

            return DeleteQuery<T, std::decay_t<Filter>, O, L>(*std::forward<Self>(self).table,
                                                              Where<std::decay_t<Filter>>(std::forward<Filter>(filter)),
                                                              std::forward<Self>(self).order,
                                                              std::forward<Self>(self).limit);
        }

        template<typename Self, is_order_by_expression<table_t> Order>
            requires(!order_t::valid)
        [[nodiscard]] auto orderBy(this Self&& self, Order&& order)
        {
            // TODO: Check table instances in order match table.

            return DeleteQuery<T, F, std::decay_t<Order>, L>(*std::forward<Self>(self).table,
                                                             std::forward<Self>(self).filter,
                                                             OrderBy<std::decay_t<Order>>(std::forward<Order>(order)),
                                                             std::forward<Self>(self).limit);
        }

        template<typename Self>
            requires(!limit_t::valid)
        [[nodiscard]] auto limitOffset(this Self&& self, const int64_t limit, const int64_t offset)
        {
            return DeleteQuery<T, F, O, std::true_type>(*std::forward<Self>(self).table,
                                                        std::forward<Self>(self).filter,
                                                        std::forward<Self>(self).order,
                                                        Limit<std::true_type>(limit, offset));
        }

        template<typename Self>
        [[nodiscard]] std::string toString(this Self&& self)
        {
            auto index = 0;

            // DELETE FROM <table> WHERE <expr> ORDER BY <expr> LIMIT <val> OFFSET <val>;
            auto sql = std::format("DELETE FROM {0} {1} {2} {3};",
                                   std::forward<Self>(self).table->getName(),
                                   std::forward<Self>(self).filter.toString(index),
                                   std::forward<Self>(self).order.toString(),
                                   std::forward<Self>(self).limit.toString());

            return sql;
        }

        template<typename Self>
        [[nodiscard]] auto operator()(this Self&& self, BindParameters bind)
        {
            // Construct statement. Note: This generates the bind indices of all filter expressions
            // and should therefore happen before the BaseFilterExpressionPtr construction below.
            auto stmt = std::make_unique<Statement>(self.table->getDatabase(), self.toString(), true);
            if (!stmt->isPrepared())
                throw SqliteError(std::format("Failed to prepare statement \"{}\"", stmt->getSql()),
                                  stmt->getResult()->code);

            BaseFilterExpressionPtr f;
            if constexpr (filter_t::valid)
                f = std::make_unique<typename filter_t::filter_t>(std::forward<Self>(self).filter.filter);

            // Bind parameters.
            if (f && any(bind)) f->bind(*stmt, bind);

            return DeleteStatement<table_t>(std::move(stmt), std::move(f));
        }
    };
}  // namespace sql
