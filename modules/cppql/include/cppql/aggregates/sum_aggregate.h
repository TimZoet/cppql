#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <string>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/expressions/aggregate_expression.h"
#include "cppql/typed/fwd.h"

namespace sql
{
    /**
     * \brief The SumAggregate function type formats a string for the SUM aggregate function.
     * \tparam R Return type.
     * \tparam Distinct Add DISTINCT to expression.
     */
    template<typename R, bool Distinct>
    class SumAggregate
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using value_t = R;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        SumAggregate() = delete;

        SumAggregate(const SumAggregate&) = delete;

        SumAggregate(SumAggregate&&) noexcept = delete;

        ~SumAggregate() noexcept = delete;

        SumAggregate& operator=(const SumAggregate&) = delete;

        SumAggregate& operator=(SumAggregate&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate SUM aggregate.
         * \param column Column name.
         * \return String with format "SUM([DISTINCT] <col>)".
         */
        [[nodiscard]] static std::string toString(std::string column)
        {
            if constexpr (Distinct)
                return std::format("SUM(DISTINCT {0})", std::move(column));
            else
                return std::format("SUM({0})", std::move(column));
        }
    };

    /**
     * \brief Create aggregate expression with SUM function.
     * \tparam R Return type of aggregate. Defaults to float.
     * \tparam Distinct Add DISTINCT to expression.
     * \tparam C ColumnExpression type.
     * \param col ColumnExpression object.
     * \return AggregateExpression object.
     */
    template<typename R = float, bool Distinct = false, is_column_expression C>
    auto sum(C&& col)
    {
        return AggregateExpression<std::remove_cvref_t<C>, SumAggregate<R>>(std::forward<C>(col));
    }
}  // namespace sql
