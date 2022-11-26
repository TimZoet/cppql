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
    class Having
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid = false;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Having() = default;

        Having(const Having& other) = default;

        Having(Having&& other) noexcept = default;

        ~Having() noexcept = default;

        Having& operator=(const Having& other) = default;

        Having& operator=(Having&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        static void generateIndices(int32_t&) {}

        [[nodiscard]] static std::string toString() { return {}; }
    };

    template<is_filter_expression F>
    class Having<F>
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

        Having() = delete;

        Having(const Having& other) = default;

        Having(Having&& other) noexcept = default;

        explicit Having(filter_t f) : filter(std::move(f)) {}

        ~Having() noexcept = default;

        Having& operator=(const Having& other) = default;

        Having& operator=(Having&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        void generateIndices(int32_t& idx) { filter.generateIndices(idx); }

        /**
         * \brief Generate HAVING clause with filter expression.
         * \return String with format "HAVING <expr>".
         */
        [[nodiscard]] std::string toString() { return std::format(" HAVING {}", filter.toString()); }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        filter_t filter;
    };
}  // namespace sql
