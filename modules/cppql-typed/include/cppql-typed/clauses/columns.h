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

#include "cppql-typed/expressions/column_expression.h"

namespace sql
{
    // TODO: Is this class ever instantiated with 0 arguments? E.g. when doing a default insert? Will toString even work then?
    // Or should this class get a specialization for non-zero column count?
    template<is_column_expression... Cs>
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
            const auto cols = [&]<std::size_t I, std::size_t... Is>(std::index_sequence<I, Is...>)
            {
                if constexpr (sizeof...(Is) == 0)
                    return std::get<I>(columns).name();
                else
                    return std::get<I>(columns).name() + (... + ("," + std::get<Is>(columns).name()));
            };

            return cols(std::index_sequence_for<Cs...>());
        }

        /**
         * \brief Generate comma-separated list of column names including table names.
         * \return String with format "table-name.column-name[0],...,table-name.column-name[N]".
         */
        [[nodiscard]] std::string toStringFull() const
        {
            const auto cols = [&]<std::size_t I, std::size_t... Is>(std::index_sequence<I, Is...>)
            {
                if constexpr (sizeof...(Is) == 0)
                    return std::get<I>(columns).fullName();
                else
                    return std::get<I>(columns).fullName() + (... + ("," + std::get<Is>(columns).fullName()));
            };

            return cols(std::index_sequence_for<Cs...>());
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        row_t columns;
    };
}  // namespace sql
