#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/expressions/column_expression.h"

namespace sql
{
    template<typename...>
    class GroupBy
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid = false;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GroupBy() = default;

        GroupBy(const GroupBy& other) = default;

        GroupBy(GroupBy&& other) noexcept = default;

        ~GroupBy() noexcept = default;

        GroupBy& operator=(const GroupBy& other) = default;

        GroupBy& operator=(GroupBy&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] static std::string toString() { return {}; }
    };

    template<is_column_expression C, is_column_expression... Cs>
    class GroupBy<C, Cs...>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid  = true;
        using group_t                = std::tuple<C, Cs...>;
        static constexpr size_t size = 1 + sizeof...(Cs);

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        GroupBy() = delete;

        GroupBy(const GroupBy& other) = default;

        GroupBy(GroupBy&& other) noexcept = default;

        explicit GroupBy(C c, Cs... cs) : group(std::make_tuple(std::move(c), std::move(cs)...)) {}

        ~GroupBy() noexcept = default;

        GroupBy& operator=(const GroupBy& other) = default;

        GroupBy& operator=(GroupBy&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate ORDER BY clause.
         * \return String with format "GROUP BY table-name.column-name[0],...,table-name.column-name[N]".
         */
        [[nodiscard]] std::string toString() const
        {
            const auto cols = [&]<std::size_t I, std::size_t... Is>(std::index_sequence<I, Is...>)
            {
                if constexpr (sizeof...(Is) == 0)
                    return std::get<I>(group).fullName();
                else
                    return std::get<I>(group).fullName() + (... + ("," + std::get<Is>(group).fullName()));
            };

            return std::format("GROUP BY {0}", cols(std::index_sequence_for<C, Cs...>{}));
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        group_t group;
    };
}  // namespace sql
