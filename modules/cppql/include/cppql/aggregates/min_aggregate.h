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
     * \brief The MinAggregate function type formats a string for the MIN aggregate function.
     * \tparam R Return type.
     * \tparam Distinct Add DISTINCT to expression.
     */
    template<typename R, bool Distinct>
    class MinAggregate
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using value_t = R;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MinAggregate() = delete;

        MinAggregate(const MinAggregate&) = delete;

        MinAggregate(MinAggregate&&) noexcept = delete;

        ~MinAggregate() noexcept = delete;

        MinAggregate& operator=(const MinAggregate&) = delete;

        MinAggregate& operator=(MinAggregate&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate MIN aggregate.
         * \param column Column name.
         * \return String with format "MIN([DISTINCT] <col>)".
         */
        [[nodiscard]] static std::string toString(std::string column)
        {
            if constexpr (Distinct)
                return std::format("MIN(DISTINCT {0})", std::move(column));
            else
                return std::format("MIN({0})", std::move(column));
        }
    };

    /**
     * \brief Create aggregate expression with MIN function.
     * \tparam R Return type of aggregate. Defaults to float.
     * \tparam Distinct Add DISTINCT to expression.
     * \tparam C ColumnExpression type.
     * \param col ColumnExpression object.
     * \return AggregateExpression object.
     */
    template<typename R = float, bool Distinct = false, is_column_expression C>
    auto min(C&& col)
    {
        return AggregateExpression<std::remove_cvref_t<C>, MinAggregate<R, Distinct>>(std::forward<C>(col));
    }
}  // namespace sql
