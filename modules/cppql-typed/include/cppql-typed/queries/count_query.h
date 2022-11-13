#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/clauses/where.h"
#include "cppql-typed/expressions/filter_expression.h"
#include "cppql-typed/statements/count_statement.h"

namespace sql
{
    template<typename T, typename F>
    class CountQuery
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using table_t  = T;
        using filter_t = Where<F>;

        Table*   table;
        filter_t filter;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        CountQuery() = delete;

        CountQuery(const CountQuery& other) = default;

        CountQuery(CountQuery&& other) noexcept = default;

        explicit CountQuery(Table& t) : table(&t), filter(std::nullopt) {}

        CountQuery(Table& t, filter_t f) : table(&t), filter(std::move(f)) {}

        ~CountQuery() noexcept = default;

        CountQuery& operator=(const CountQuery& other) = default;

        CountQuery& operator=(CountQuery&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        template<typename Self, is_single_filter_expression<table_t> Filter>
            requires(!filter_t::valid)
        [[nodiscard]] auto where(this Self&& self, Filter&& filter)
        {
            // TODO: Check table instances in filter match table.

            return CountQuery<T, std::decay_t<Filter>>(*std::forward<Self>(self).table,
                                                       Where<std::decay_t<Filter>>(std::forward<Filter>(filter)));
        }

        template<typename Self>
        [[nodiscard]] std::string toString(this Self&& self)
        {
            auto index = 0;

            // SELECT COUNT(*) FROM <table> WHERE <expr>;
            auto sql = std::format("SELECT COUNT(*) FROM {0} {1};",
                                   std::forward<Self>(self).table->getName(),
                                   std::forward<Self>(self).filter.toString(index));

            return sql;
        }

        template<typename Self>
        [[nodiscard]] auto operator()(this Self&& self, const BindParameters bind)
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

            return CountStatement<table_t>(std::move(stmt), std::move(f));
        }
    };
}  // namespace sql
