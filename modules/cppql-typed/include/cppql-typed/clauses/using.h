#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <optional>
#include <string>
#include <tuple>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/column_expression.h"

namespace sql
{
    template<typename... Cs>
    class Using
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////
        
        static constexpr bool valid = false;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Using() = delete;

        Using(const Using& other) = default;

        Using(Using&& other) noexcept = default;

        explicit Using(std::nullopt_t) : columns(std::nullopt) {}

        virtual ~Using() noexcept = default;

        Using& operator=(const Using& other) = default;

        Using& operator=(Using&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        template<typename Self>
        [[nodiscard]] std::string toString(this Self&&)
        {
            return {};
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////
        
        std::nullopt_t columns;
    };

    template<is_column_expression C, is_column_expression... Cs>
    class Using<C, Cs...>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid = true;
        using row_t = std::tuple<C, Cs...>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Using() = default;

        Using(const Using& other) = default;

        Using(Using&& other) noexcept = default;

        Using(C c, Cs... cs) : columns(std::make_tuple(std::move(c), std::move(cs)...)) {}

        virtual ~Using() noexcept = default;

        Using& operator=(const Using& other) = default;

        Using& operator=(Using&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        template<typename Self>
        [[nodiscard]] std::string toString(this Self&& self)
        {
            auto cols = [&self]<std::size_t I, std::size_t... Is>(std::index_sequence<I, Is...>)
            {
                if constexpr (sizeof...(Is) == 0)
                    return "USING (" + std::get<I>(self.columns).toString() + ")";
                else
                    return "USING (" + std::get<I>(self.columns).toString() + (... + ("," + std::get<Is>(self.columns).toString())) + ")";
            };

            return cols(std::index_sequence_for<C, Cs...>());
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        row_t columns;
    };
}  // namespace sql
