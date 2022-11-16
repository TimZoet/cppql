#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <tuple>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/base_filter_expression.h"

namespace sql
{
    template<typename... Ts>
    class FilterExpression final : public BaseFilterExpression
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        FilterExpression() = default;

        FilterExpression(const FilterExpression&) = default;

        FilterExpression(FilterExpression&&) noexcept = default;

        FilterExpression(Ts... expr) : expressions(std::make_tuple<Ts...>(std::move(expr)...)) {}

        ~FilterExpression() noexcept override = default;

        FilterExpression& operator=(const FilterExpression&) = default;

        FilterExpression& operator=(FilterExpression&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Bind.
        ////////////////////////////////////////////////////////////////

        void bind(Statement& stmt, const BindParameters bind) const override
        {
            const auto b = [this]<size_t... Is>(std::index_sequence<Is...>, Statement & ss, const BindParameters bb)
            {
                (std::get<Is>(expressions).bind(ss, bb), ...);
            };

            b(std::index_sequence_for<Ts...>(), stmt, bind);
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        std::tuple<Ts...> expressions;
    };
}  // namespace sql
