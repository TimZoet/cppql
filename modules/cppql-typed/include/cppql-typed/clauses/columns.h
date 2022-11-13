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
    template<is_column_expression C, is_column_expression... Cs>
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

        Columns() = delete;

        Columns(const Columns& other) = default;

        Columns(Columns&& other) noexcept = default;

        // TODO: Construct from C, Cs... instead and do make_tuple in here instead of at call site.
        explicit Columns(row_t cs) : columns(std::move(cs)) {}

        ~Columns() noexcept = default;

        Columns& operator=(const Columns& other) = default;

        Columns& operator=(Columns&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate comma-separated list of column names.
         * \tparam Self Self.
         * \param self Self.
         * \return String with format "table-name.column-name[0],...,table-name.column-name[N]".
         */
        template<typename Self>
        [[nodiscard]] std::string toString(this Self&& self)
        {
            auto cols = [&self]<std::size_t I, std::size_t... Is>(std::index_sequence<I, Is...>)
            {
                if constexpr (sizeof...(Is) == 0)
                    return std::get<I>(self.columns).fullName();
                else
                    return std::get<I>(self.columns).fullName() + (... + ("," + std::get<Is>(self.columns).fullName()));
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
