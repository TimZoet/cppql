#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <tuple>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/column_expression.h"

namespace sql
{
    template<typename C, typename... Cs>
    class Columns
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////
        
        using row_t = std::tuple<C, Cs...>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Columns() = default;

        Columns(const Columns& other) = default;

        Columns(Columns&& other) noexcept = default;

        explicit Columns(row_t cs) : columns(std::move(cs)) {}

        virtual ~Columns() noexcept = default;

        Columns& operator=(const Columns& other) = default;

        Columns& operator=(Columns&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        template<typename Self>
        [[nodiscard]] std::string toString(this Self&& self, int32_t& index)
        {
            auto cols = [&self, &index]<std::size_t I, std::size_t... Is>(std::index_sequence<I, Is...>)
            {
                if constexpr (sizeof...(Is) == 0)
                    return std::get<I>(self.columns).toString(index);
                else
                    return std::get<I>(self.columns).toString(index) + (... + ("," + std::get<Is>(self.columns).toString(index)));
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
