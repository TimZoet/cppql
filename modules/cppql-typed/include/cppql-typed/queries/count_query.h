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

#include "cppql-typed/clauses/where.h"
#include "cppql-typed/expressions/filter_expression.h"
#include "cppql-typed/statements/count_statement.h"

namespace sql
{
    template<is_typed_table T, is_filter_expression_or_none F>
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
        // Query.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Filter results by an expression. This query should not have a filter applied yet.
         * \tparam Self Self type.
         * \tparam Filter FilterExpression type.
         * \param self Self.
         * \param filter Expression to filter results by.
         * \return CountQuery with filter expression.
         */
        template<typename Self, is_valid_filter_expression<std::tuple<table_t>> Filter>
            requires(!filter_t::valid)
        [[nodiscard]] auto where(this Self&& self, Filter&& filter)
        {
            if (!filter.containsTables(*self.table))
                throw CppqlError(std::format(
                  "Cannot apply filter to query because the expression contains a table not in the query."));

            return CountQuery<T, std::remove_cvref_t<Filter>>(
              *std::forward<Self>(self).table, Where<std::remove_cvref_t<Filter>>(std::forward<Filter>(filter)));
        }

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string toString()
        {
            // SELECT COUNT(*) FROM <table> WHERE <expr>;
            auto sql = std::format("SELECT COUNT(*) FROM {0} {1};", table->getName(), filter.toString());

            return sql;
        }

        /**
         * \brief Generate CountStatement object. Generates and compiles SQL code.
         * \tparam Self Self type.
         * \param self Self.
         * \return CountStatement.
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
                f = std::make_unique<FilterExpression<typename filter_t::filter_t>>(
                  std::forward<Self>(self).filter.filter);

            return CountStatement(std::move(stmt), std::move(f));
        }
    };
}  // namespace sql
