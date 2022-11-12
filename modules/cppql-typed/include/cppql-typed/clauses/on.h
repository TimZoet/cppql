#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <optional>
#include <string>
#include <type_traits>

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

        On() = delete;

        On(const On& other) = default;

        On(On&& other) noexcept = default;

        explicit On(std::nullopt_t) : filter(std::nullopt) {}

        virtual ~On() noexcept = default;

        On& operator=(const On& other) = default;

        On& operator=(On&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        template<typename Self>
        [[nodiscard]] std::string toString(this Self&&, int32_t&)
        {
            return {};
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////
        
        std::nullopt_t filter;
    };

    template<typename F> requires (!std::same_as<std::nullopt_t, F>) // TODO: Test for filterexpression?
    class On<F>
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

        On() = default;

        On(const On& other) = default;

        On(On&& other) noexcept = default;

        explicit On(filter_t f) : filter(std::move(f)) {}

        virtual ~On() noexcept = default;

        On& operator=(const On& other) = default;

        On& operator=(On&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        template<typename Self>
        [[nodiscard]] std::string toString(this Self&& self, int32_t& pIndex)
        {
            return std::format(" ON {}", std::forward<Self>(self).filter.toString(pIndex));
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        filter_t filter;
    };
}  // namespace sql
