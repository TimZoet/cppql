#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <string>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/expressions/filter_expression.h"

namespace sql
{
    template<typename F>
    class Where
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid = false;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Where() = default;

        Where(const Where& other) = default;

        Where(Where&& other) noexcept = default;

        ~Where() noexcept = default;

        Where& operator=(const Where& other) = default;

        Where& operator=(Where&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        static void generateIndices(int32_t&) {}

        [[nodiscard]] static std::string toString() { return {}; }
    };

    template<is_filter_expression F>
    class Where<F>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid = true;
        using filter_t              = F;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Where() = delete;

        Where(const Where& other) = default;

        Where(Where&& other) noexcept = default;

        explicit Where(filter_t f) : filter(std::move(f)) {}

        ~Where() noexcept = default;

        Where& operator=(const Where& other) = default;

        Where& operator=(Where&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        void generateIndices(int32_t& idx) { filter.generateIndices(idx); }

        /**
         * \brief Generate WHERE clause with filter expression.
         * \return String with format "WHERE <expr>".
         */
        [[nodiscard]] std::string toString() { return std::format(" WHERE {}", filter.toString()); }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        filter_t filter;
    };
}  // namespace sql
