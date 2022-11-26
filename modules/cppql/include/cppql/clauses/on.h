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
    class On
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid = false;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        On() = default;

        On(const On& other) = default;

        On(On&& other) noexcept = default;

        ~On() noexcept = default;

        On& operator=(const On& other) = default;

        On& operator=(On&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        static void generateIndices(int32_t&) {}

        [[nodiscard]] static std::string toString() { return {}; }
    };

    template<is_filter_expression F>
    class On<F>
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

        On() = delete;

        On(const On& other) = default;

        On(On&& other) noexcept = default;

        explicit On(filter_t f) : filter(std::move(f)) {}

        ~On() noexcept = default;

        On& operator=(const On& other) = default;

        On& operator=(On&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        void generateIndices(int32_t& idx) { filter.generateIndices(idx); }

        /**
         * \brief Generate ON clause with filter expression.
         * \return String with format "ON <expr>".
         */
        [[nodiscard]] std::string toString() { return std::format("ON {}", filter.toString()); }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        filter_t filter;
    };
}  // namespace sql
