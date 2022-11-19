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
     * \brief The AverageAggregate function type formats a string for the AVG aggregate function.
     * \tparam R Return type.
     * \tparam Distinct Add DISTINCT to expression.
     */
    template<typename R, bool Distinct>
    class AverageAggregate
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using value_t = R;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        AverageAggregate() = delete;

        AverageAggregate(const AverageAggregate&) = delete;

        AverageAggregate(AverageAggregate&&) noexcept = delete;

        ~AverageAggregate() noexcept = delete;

        AverageAggregate& operator=(const AverageAggregate&) = delete;

        AverageAggregate& operator=(AverageAggregate&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate AVG aggregate.
         * \param column Column name.
         * \return String with format "AVG([DISTINCT] <col>)".
         */
        [[nodiscard]] static std::string toString(std::string column)
        {
            if constexpr (Distinct)
                return std::format("AVG(DISTINCT {0})", std::move(column));
            else
                return std::format("AVG({0})", std::move(column));
        }
    };

    /**
     * \brief Create aggregate expression with AVG function.
     * \tparam R Return type of aggregate. Defaults to float.
     * \tparam Distinct Add DISTINCT to expression.
     * \tparam C ColumnExpression type.
     * \param col ColumnExpression object.
     * \return AggregateExpression object.
     */
    template<typename R = float, bool Distinct = false, is_column_expression C>
    auto avg(C&& col)
    {
        return AggregateExpression<std::remove_cvref_t<C>, AverageAggregate<R, Distinct>>(std::forward<C>(col));
    }
}  // namespace sql
