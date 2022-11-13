#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <optional>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/order_by_expression.h"

namespace sql
{
    template<typename O>
    class OrderBy
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid = false;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        OrderBy() = default;

        OrderBy(const OrderBy& other) = default;

        OrderBy(OrderBy&& other) noexcept = default;

        ~OrderBy() noexcept = default;

        OrderBy& operator=(const OrderBy& other) = default;

        OrderBy& operator=(OrderBy&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] static std::string toString() { return {}; }
    };

    template<_is_order_by_expression O>
    class OrderBy<O>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid = true;
        using order_t               = O;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        OrderBy() = delete;

        OrderBy(const OrderBy& other) = default;

        OrderBy(OrderBy&& other) noexcept = default;

        explicit OrderBy(order_t o) : order(std::move(o)) {}

        ~OrderBy() noexcept = default;

        OrderBy& operator=(const OrderBy& other) = default;

        OrderBy& operator=(OrderBy&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate ORDER BY clause.
         * \return String with format "ORDER BY table-name.column-name[0] <ASC|DESC>,...,table-name.column-name[N] <ASC|DESC>".
         */
        [[nodiscard]] std::string toString() const { return order.toString(); }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        order_t order;
    };
}  // namespace sql
