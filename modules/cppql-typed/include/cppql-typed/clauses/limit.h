#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <string>
#include <type_traits>

namespace sql
{
    template<typename T>
    class Limit
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid = false;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Limit() = default;

        Limit(const Limit& other) = default;

        Limit(Limit&& other) noexcept = default;

        ~Limit() noexcept = default;

        Limit& operator=(const Limit& other) = default;

        Limit& operator=(Limit&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] static std::string toString() { return {}; }
    };

    template<std::same_as<std::true_type> T>
    class Limit<T>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid = true;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Limit() = delete;

        Limit(const Limit& other) = default;

        Limit(Limit&& other) noexcept = default;

        Limit(const int64_t l, const int64_t o) : limit(l), offset(o) {}

        ~Limit() noexcept = default;

        Limit& operator=(const Limit& other) = default;

        Limit& operator=(Limit&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate LIMIT OFFSET clause.
         * \return String with format "LIMIT <val> OFFSET <val>".
         */
        [[nodiscard]] std::string toString() const { return std::format("LIMIT {0} OFFSET {1}", limit, offset); }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        int64_t limit;
        int64_t offset;
    };
}  // namespace sql
