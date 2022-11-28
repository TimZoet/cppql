#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
#include <tuple>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/expressions/column_expression.h"

namespace sql
{
    template<is_result_expression... Cs>
    class Columns
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using row_t                  = std::tuple<Cs...>;
        static constexpr size_t size = sizeof...(Cs);

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Columns(const Columns& other) = default;

        Columns(Columns&& other) noexcept = default;

        explicit Columns(Cs... cs) : columns(std::make_tuple(std::move(cs)...)) {}

        ~Columns() noexcept = default;

        Columns& operator=(const Columns& other) = default;

        Columns& operator=(Columns&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate comma-separated list of column names.
         * \return String with format "column-name[0],...,column-name[N]".
         */
        [[nodiscard]] std::string toString() const
        {
            if constexpr (sizeof...(Cs) == 0)
                return {};
            else
            {
                const auto cols = [&]<std::size_t I, std::size_t... Is>(std::index_sequence<I, Is...>)
                {
                    if constexpr (sizeof...(Is) == 0)
                        return std::get<I>(columns).name();
                    else
                        return std::get<I>(columns).name() + (... + ("," + std::get<Is>(columns).name()));
                };

                return cols(std::index_sequence_for<Cs...>());
            }
        }

        /**
         * \brief Generate comma-separated list of column names including table names.
         * \return String with format "table-name.column-name[0],...,table-name.column-name[N]".
         */
        [[nodiscard]] std::string toStringFull() const
        {
            if constexpr (sizeof...(Cs) == 0)
                return {};
            else
            {
                const auto cols = [&]<std::size_t I, std::size_t... Is>(std::index_sequence<I, Is...>)
                {
                    if constexpr (sizeof...(Is) == 0)
                        return std::get<I>(columns).toString();
                    else
                        return std::get<I>(columns).toString() + (... + ("," + std::get<Is>(columns).toString()));
                };

                return cols(std::index_sequence_for<Cs...>());
            }
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        row_t columns;
    };
}  // namespace sql
