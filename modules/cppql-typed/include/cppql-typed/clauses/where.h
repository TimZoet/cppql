#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <optional>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/filter_expression.h"

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

        Where() = delete;

        Where(const Where& other) = default;

        Where(Where&& other) noexcept = default;

        explicit Where(std::nullopt_t) : filter(std::nullopt) {}

        virtual ~Where() noexcept = default;

        Where& operator=(const Where& other) = default;

        Where& operator=(Where&& other) noexcept = default;

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
        
        std::nullopt_t filter;
    };

    template<typename F> requires (!std::same_as<std::nullopt_t, F>) // TODO: Test for filterexpression?
    class Where<F>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid = true;
        using filter_t = F;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Where() = default;

        Where(const Where& other) = default;

        Where(Where&& other) noexcept = default;

        explicit Where(filter_t f) : filter(std::move(f)) {}

        virtual ~Where() noexcept = default;

        Where& operator=(const Where& other) = default;

        Where& operator=(Where&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        template<typename Self>
        [[nodiscard]] std::string toString(this Self&& self, int32_t& pIndex)
        {
            return std::format(" WHERE {}", std::forward<Self>(self).filter.toString(pIndex));
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        filter_t filter;
    };
}  // namespace sql
