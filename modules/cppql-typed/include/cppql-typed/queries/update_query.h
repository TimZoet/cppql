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
#include "cppql-typed/statements/update_statement.h"

namespace sql
{
    template<typename T, typename F, typename O, typename L, is_column_expression C, is_column_expression... Cs>
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

        UpdateQuery(Table& t, columns_t cs) :
            table(&t), columns(std::move(cs))
        {
        }

        UpdateQuery(Table& t, columns_t cs, filter_t f, order_t o, limit_t l) :
            table(&t), columns(std::move(cs)), filter(std::move(f)), order(std::move(o)), limit(std::move(l))
        {
        }

        ~UpdateQuery() noexcept = default;

        UpdateQuery& operator=(const UpdateQuery& other) = default;

        UpdateQuery& operator=(UpdateQuery&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        template<typename Self, is_single_filter_expression<table_t> Filter>
            requires(!filter_t::valid)
        [[nodiscard]] auto where(this Self&& self, Filter&& filter)
        {
            // TODO: Check table instances in filter match tables in joins.

            return UpdateQuery<T, std::decay_t<Filter>, O, L, C, Cs...>(
              *std::forward<Self>(self).table,
              std::forward<Self>(self).columns,
              Where<std::decay_t<Filter>>(std::forward<Filter>(filter)),
              std::forward<Self>(self).order,
              std::forward<Self>(self).limit);
        }

        template<typename Self, is_order_by_expression<table_t> Order>
            requires(!order_t::valid)
        [[nodiscard]] auto orderBy(this Self&& self, Order&& order)
        {
            // TODO: Check table instances in filter match tables in joins.

            return UpdateQuery<T, F, std::decay_t<Order>, L, C, Cs...>(
              *std::forward<Self>(self).table,
              std::forward<Self>(self).columns,
              std::forward<Self>(self).filter,
              OrderBy<std::decay_t<Order>>(std::forward<Order>(order)),
              std::forward<Self>(self).limit);
        }

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

        [[nodiscard]] std::string toString()
        {
            auto index = 1;

            std::string vals = "?1";
            for (; index < columns_t::size; index++) vals += std::format(",?{0}", index + 1);

            // UPDATE <table> SET (<cols>) = (<vals>) WHERE <expr> ORDER BY <expr> LIMIT <val> OFFSET <val>
            auto sql = std::format("UPDATE {0} SET ({1}) = ({2}) {3} {4};",
                                   table->getName(),
                                   columns.toString(),
                                   std::move(vals),
                                   filter.toString(index),
                                   order.toString(),
                                   limit.toString());

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

            return UpdateStatement<table_t, C, Cs...>(std::move(stmt), std::move(f));
        }
    };
}  // namespace sql
