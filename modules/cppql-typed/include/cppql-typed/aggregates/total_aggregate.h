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
     * \brief The TotalAggregate function type formats a string for the TOTAL aggregate function.
     * \tparam R Return type.
     * \tparam Distinct Add DISTINCT to expression.
     */
    template<typename R, bool Distinct>
    class TotalAggregate
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using value_t = R;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        TotalAggregate() = delete;

        TotalAggregate(const TotalAggregate&) = delete;

        TotalAggregate(TotalAggregate&&) noexcept = delete;

        ~TotalAggregate() noexcept = delete;

        TotalAggregate& operator=(const TotalAggregate&) = delete;

        TotalAggregate& operator=(TotalAggregate&&) noexcept = delete;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate TOTAL aggregate.
         * \param column Column name.
         * \return String with format "TOTAL([DISTINCT] <col>)".
         */
        [[nodiscard]] static std::string toString(std::string column)
        {
            if constexpr (Distinct)
                return std::format("TOTAL(DISTINCT {0})", std::move(column));
            else
                return std::format("TOTAL({0})", std::move(column));
        }
    };

    /**
     * \brief Create aggregate expression with TOTAL function.
     * \tparam R Return type of aggregate. Defaults to float.
     * \tparam Distinct Add DISTINCT to expression.
     * \tparam C ColumnExpression type.
     * \param col ColumnExpression object.
     * \return AggregateExpression object.
     */
    template<typename R = float, bool Distinct = false, is_column_expression C>
    auto total(C&& col)
    {
        return AggregateExpression<std::remove_cvref_t<C>, TotalAggregate<R>>(std::forward<C>(col));
    }
}  // namespace sql
