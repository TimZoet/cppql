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

        On() = default;

        On(const On& other) = default;

        On(On&& other) noexcept = default;

        ~On() noexcept = default;

        On& operator=(const On& other) = default;

        On& operator=(On&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] static std::string toString(int32_t&)
        {
            return {};
        }
    };

    template<typename F>
        requires(!std::same_as<std::nullopt_t, F>)  // TODO: Test for filterexpression?
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

        /**
         * \brief Generate ON clause with filter expression.
         * \param pIndex Counter.
         * \return String with format "ON <expr>".
         */
        [[nodiscard]] std::string toString(int32_t& pIndex)
        {
            return std::format("ON {}", filter.toString(pIndex));
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        filter_t filter;
    };
}  // namespace sql
