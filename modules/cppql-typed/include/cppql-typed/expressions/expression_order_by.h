#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <format>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/expression_column.h"

namespace sql
{
    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    class OrderByExpression;

    template<typename T>
    using OrderByExpressionPtr = std::unique_ptr<OrderByExpression<T>>;

    template<typename T>
    concept _is_order_by_expression = std::same_as<T, OrderByExpression<typename T::table_t>>;

    template<typename T, typename Table>
    concept is_order_by_expression = std::same_as<T, OrderByExpression<Table>>;

    ////////////////////////////////////////////////////////////////
    // OrderByExpression class.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief The OrderByExpression class is used in conjunction
     * with query objects to order rows. It holds a (sequence of)
     * column(s) and generates the ORDER BY <cols> part of a
     * statement.
     * \tparam T Table type.
     */
    template<typename T>
    class OrderByExpression
    {
    public:
        enum class Order
        {
            Asc,
            Desc
        };

        using table_t = T;

        OrderByExpression() = delete;

        OrderByExpression(const OrderByExpression& other);

        OrderByExpression(OrderByExpression&& other) noexcept;

        OrderByExpression(BaseColumnExpressionPtr<T> col, Order orderBy);

        OrderByExpression(BaseColumnExpressionPtr<T> col, OrderByExpressionPtr<T> nxt, Order orderBy);

        ~OrderByExpression() = default;

        OrderByExpression& operator=(const OrderByExpression& other);

        OrderByExpression& operator=(OrderByExpression&& other) noexcept;

        /**
         * \brief Generate ORDER BY string.
         * \param table Table.
         * \return ORDER BY string.
         */
        [[nodiscard]] std::string toString(const Table& table) const;

    private:
        [[nodiscard]] std::string toString(const Table& table, std::string prev) const;

        template<_is_order_by_expression O, is_column_expression C>
        requires(same_table<O, C>) friend O operator+(O order, C&& col);

        template<_is_order_by_expression O, is_column_expression C>
        requires(same_table<O, C>) friend O operator-(O order, C&& col);

        template<_is_order_by_expression O>
        friend O operator+(O lhs, O&& rhs);

        /**
         * \brief Column to order.
         */
        BaseColumnExpressionPtr<T> column;

        /**
         * \brief Optional expression that holds the next column(s) to order by.
         */
        OrderByExpressionPtr<T> next;

        /**
         * \brief Order in which the values of this column are sorted.
         */
        Order order;
    };

    ////////////////////////////////////////////////////////////////
    // Implementation.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    OrderByExpression<T>::OrderByExpression(const OrderByExpression& other) :
        column(other.column->clone()),
        next(other.next ? std::make_unique<OrderByExpression<T>>(*other.next) : nullptr),
        order(other.order)
    {
    }

    template<typename T>
    OrderByExpression<T>::OrderByExpression(OrderByExpression&& other) noexcept :
        column(std::move(other.column)), next(std::move(other.next)), order(other.order)
    {
    }

    template<typename T>
    OrderByExpression<T>::OrderByExpression(BaseColumnExpressionPtr<T> col, const Order orderBy) :
        column(std::move(col)), next(), order(orderBy)
    {
    }

    template<typename T>
    OrderByExpression<T>::OrderByExpression(BaseColumnExpressionPtr<T> col,
                                            OrderByExpressionPtr<T>    nxt,
                                            const Order                orderBy) :
        column(std::move(col)), next(std::move(nxt)), order(orderBy)
    {
    }

    template<typename T>
    OrderByExpression<T>& OrderByExpression<T>::operator=(const OrderByExpression& other)
    {
        column = other.column->clone();
        next   = other.next ? std::make_unique<OrderByExpression<T>>(*other.next) : nullptr;
        order  = other.order;
        return *this;
    }

    template<typename T>
    OrderByExpression<T>& OrderByExpression<T>::operator=(OrderByExpression&& other) noexcept
    {
        column = std::move(other.column);
        next   = std::move(other.next);
        order  = other.order;
        return *this;
    }

    template<typename T>
    std::string OrderByExpression<T>::toString(const Table& table) const
    {
        return toString(table, "");
    }

    template<typename T>
    std::string OrderByExpression<T>::toString(const Table& table, std::string prev) const
    {
        auto c = column->toString(table);
        auto o = order == Order::Asc ? std::string("ASC") : std::string("DESC");

        // Format "<column_name> <order>," string and recurse on next expression in sequence.
        if (next) return next->toString(table, std::format("{0} {1},", std::move(c), std::move(o)));

        // Last expression in sequence, format complete ORDER BY string.
        return std::format("ORDER BY {0} {1} {2}", std::move(prev), std::move(c), std::move(o));
    }

    ////////////////////////////////////////////////////////////////
    // Ascending order.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Order column in ascending order.
     * \tparam C Column type.
     * \param col Column to order by.
     * \return OrderByExpression object.
     */
    template<is_column_expression C>
    auto operator+(C&& col)
    {
        using O = OrderByExpression<typename C::table_t>;
        return O(std::make_unique<C>(std::forward<C>(col)), O::Order::Asc);
    }

    /**
     * \brief Order column in ascending order.
     * \tparam O OrderByExpression type.
     * \tparam C Column type.
     * \param order Preceding OrderByExpression object.
     * \param col Column to order by.
     * \return OrderByExpression object.
     */
    template<_is_order_by_expression O, is_column_expression C>
    requires(same_table<O, C>) O operator+(O order, C&& col)
    {
        return O(std::move(order.column), std::make_unique<O>(+std::forward<C>(col)), order.order);
    }

    ////////////////////////////////////////////////////////////////
    // Descending order.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Order column in descending order.
     * \tparam C Column type.
     * \param col Column to order by.
     * \return OrderByExpression object.
     */
    template<is_column_expression C>
    auto operator-(C&& col)
    {
        using O = OrderByExpression<typename C::table_t>;
        return O(std::make_unique<C>(std::forward<C>(col)), O::Order::Desc);
    }

    /**
     * \brief Order column in descending order.
     * \tparam O OrderByExpression type.
     * \tparam C Column type.
     * \param order Preceding OrderByExpression object.
     * \param col Column to order by.
     * \return OrderByExpression object.
     */
    template<_is_order_by_expression O, is_column_expression C>
    requires(same_table<O, C>) O operator-(O order, C&& col)
    {
        return O(std::move(order.column), std::make_unique<O>(-std::forward<C>(col)), order.order);
    }

    ////////////////////////////////////////////////////////////////
    // Concatenate.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Concatenate two OrderByExpression objects.
     * \tparam O OrderByExpression type.
     * \param lhs Left hand side.
     * \param rhs Right hand side.
     * \return OrderByExpression object.
     */
    template<_is_order_by_expression O>
    O operator+(O lhs, O&& rhs)
    {
        return O(std::move(lhs.column), std::make_unique<O>(std::forward<O>(rhs)), lhs.order);
    }
}  // namespace sql
