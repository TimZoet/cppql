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
     * \brief The MaxAggregate function type formats a string for the MAX aggregate function.
     * \tparam R Return type.
     * \tparam Distinct Add DISTINCT to expression.
     */
    template<typename R, bool Distinct>
    class MaxAggregate
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using value_t = R;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        MaxAggregate() = delete;

        MaxAggregate(const MaxAggregate&) = delete;

        MaxAggregate(MaxAggregate&&) noexcept = delete;

        ~MaxAggregate() noexcept = delete;

        MaxAggregate& operator=(const MaxAggregate&) = delete;

        MaxAggregate& operator=(MaxAggregate&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate MAX aggregate.
         * \param column Column name.
         * \return String with format "MAX([DISTINCT] <col>)".
         */
        [[nodiscard]] static std::string toString(std::string column)
        {
            if constexpr (Distinct)
                return std::format("MAX(DISTINCT {0})", std::move(column));
            else
                return std::format("MAX({0})", std::move(column));
        }
    };

    /**
     * \brief Create aggregate expression with MAX function.
     * \tparam R Return type of aggregate. Defaults to float.
     * \tparam Distinct Add DISTINCT to expression.
     * \tparam C ColumnExpression type.
     * \param col ColumnExpression object.
     * \return AggregateExpression object.
     */
    template<typename R = float, bool Distinct = false, is_column_expression C>
    auto max(C&& col)
    {
        return AggregateExpression<std::remove_cvref_t<C>, MaxAggregate<R, Distinct>>(std::forward<C>(col));
    }
}  // namespace sql
