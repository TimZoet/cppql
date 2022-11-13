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

        ~Using() noexcept = default;

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

        Using() = delete;

        Using(const Using& other) = default;

        Using(Using&& other) noexcept = default;

        explicit Using(C c, Cs... cs) : columns(std::make_tuple(std::move(c), std::move(cs)...)) {}

        ~Using() noexcept = default;

        Using& operator=(const Using& other) = default;

        Using& operator=(Using&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate USING clause from columns.
         * \tparam Self Self.
         * \param self Self.
         * \return String with format "USING(column-name[0],...,column-name[N])".
         */
        template<typename Self>
        [[nodiscard]] std::string toString(this Self&& self)
        {
            auto cols = [&self]<std::size_t I, std::size_t... Is>(std::index_sequence<I, Is...>)
            {
                if constexpr (sizeof...(Is) == 0)
                    return "USING (" + std::get<I>(self.columns).name() + ")";
                else
                    return "USING (" + std::get<I>(self.columns).name() + (... + ("," + std::get<Is>(self.columns).name())) + ")";
            };

            return cols(std::index_sequence_for<C, Cs...>());
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        row_t columns;
    };
}  // namespace sql
