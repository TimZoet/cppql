#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <string>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/fwd.h"
#include "cppql-typed/expressions/aggregate_expression.h"

namespace sql
{
    /**
     * \brief The CountAggregate function type formats a string for the COUNT aggregate function.
     * \tparam R Return type.
     * \tparam Distinct Add DISTINCT to expression.
     */
    template<typename R, bool Distinct>
    class CountAggregate
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using value_t = R;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        CountAggregate() = delete;

        CountAggregate(const CountAggregate&) = delete;

        CountAggregate(CountAggregate&&) noexcept = delete;

        ~CountAggregate() noexcept = delete;

        CountAggregate& operator=(const CountAggregate&) = delete;

        CountAggregate& operator=(CountAggregate&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate COUNT aggregate.
         * \param column Column name.
         * \return String with format "COUNT([DISTINCT] <col>)".
         */
        [[nodiscard]] static std::string toString(std::string column)
        {
            if constexpr (Distinct)
                return std::format("COUNT(DISTINCT {0})", std::move(column));
            else
                return std::format("COUNT({0})", std::move(column));
        }
    };

    /**
     * \brief Create aggregate expression with COUNT function.
     * \tparam R Return type of aggregate. Defaults to int64_t.
     * \tparam Distinct Add DISTINCT to expression.
     * \tparam C ColumnExpression type.
     * \param col ColumnExpression object.
     * \return AggregateExpression object.
     */
    template<typename R = int64_t, bool Distinct = false, is_column_expression C>
    auto count(C&& col)
    {
        return AggregateExpression<std::remove_cvref_t<C>, CountAggregate<R>>(std::forward<C>(col));
    }
}  // namespace sql
