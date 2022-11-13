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

        [[nodiscard]] static std::string toString(int32_t&) { return {}; }
    };

    template<typename F>
        requires(!std::same_as<std::nullopt_t, F>)  // TODO: Test for filterexpression?
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

        /**
         * \brief Generate WHERE clause with filter expression.
         * \param pIndex Counter.
         * \return String with format "WHERE <expr>".
         */
        [[nodiscard]] std::string toString(int32_t& pIndex)
        {
            return std::format(" WHERE {}", filter.toString(pIndex));
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        filter_t filter;
    };
}  // namespace sql
