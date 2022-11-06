#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <format>
#include <memory>
#include <optional>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/column_expression.h"

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

    template<typename T, typename Table>
    concept is_order_by_expression_or_none =
      is_order_by_expression<T, Table> || std::same_as<std::remove_cvref_t<T>, std::nullopt_t>;

    ////////////////////////////////////////////////////////////////
    // OrderByExpression class.
    ////////////////////////////////////////////////////////////////

    enum class Order
    {
        Asc,
        Desc
    };

    enum class Nulls
    {
        None,
        First,
        Last
    };

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
        using table_t = T;

        struct Element
        {
            /**
             * \brief Column to order.
             */
            BaseColumnExpressionPtr<T> column;

            /**
             * \brief Order in which the values of this column are sorted.
             */
            Order order = Order::Asc;

            /**
             * \brief Optional order for NULLS.
             */
            Nulls nulls = Nulls::None;
        };

        OrderByExpression() = delete;

        OrderByExpression(const OrderByExpression& other);

        OrderByExpression(OrderByExpression&& other) noexcept;

        OrderByExpression(BaseColumnExpressionPtr<T> col, Order orderBy, Nulls nlls);

        ~OrderByExpression() = default;

        OrderByExpression& operator=(const OrderByExpression& other);

        OrderByExpression& operator=(OrderByExpression&& other) noexcept;

        /**
         * \brief Generate ORDER BY string.
         * \param table Table.
         * \return ORDER BY string.
         */
        [[nodiscard]] std::string toString(const Table& table) const;

        std::vector<Element> elements;
    };

    ////////////////////////////////////////////////////////////////
    // Implementation.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    OrderByExpression<T>::OrderByExpression(const OrderByExpression& other)
    {
        *this = other;
    }

    template<typename T>
    OrderByExpression<T>::OrderByExpression(OrderByExpression&& other) noexcept : elements(std::move(other.elements))
    {
    }

    template<typename T>
    OrderByExpression<T>::OrderByExpression(BaseColumnExpressionPtr<T> col, const Order orderBy, const Nulls nlls)

    {
        elements.emplace_back(std::move(col), orderBy, nlls);
    }

    template<typename T>
    OrderByExpression<T>& OrderByExpression<T>::operator=(const OrderByExpression& other)
    {
        for (const auto& elem : other.elements) elements.emplace_back(elem.column->clone(), elem.order, elem.nulls);
        return *this;
    }

    template<typename T>
    OrderByExpression<T>& OrderByExpression<T>::operator=(OrderByExpression&& other) noexcept
    {
        elements = std::move(other.elements);
        return *this;
    }

    template<typename T>
    std::string OrderByExpression<T>::toString(const Table& table) const
    {
        bool              first = true;
        std::stringstream s;
        s << "ORDER BY";
        for (const auto& elem : elements)
        {
            if (!first) s << ",";
            first = false;
            s << " " << elem.column->toString(table) << (elem.order == Order::Asc ? " ASC" : " DESC");
            if (elem.nulls == Nulls::First)
                s << " NULLS FIRST";
            else if (elem.nulls == Nulls::Last)
                s << " NULLS LAST";
        }
        return s.str();
    }

    ////////////////////////////////////////////////////////////////
    // Order.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Order column in ascending order.
     * \tparam C Column type.
     * \param col Column to order by.
     * \param nulls NULL ordering.
     * \return OrderByExpression object.
     */
    template<is_column_expression C>
    auto ascending(C&& col, Nulls nulls = Nulls::None)
    {
        return OrderByExpression<typename C::table_t>(col.clone(), Order::Asc, nulls);
    }

    /**
     * \brief Order column in descending order.
     * \tparam C Column type.
     * \param col Column to order by.
     * \param nulls NULL ordering.
     * \return OrderByExpression object.
     */
    template<is_column_expression C>
    auto descending(C&& col, Nulls nulls = Nulls::None)
    {
        return OrderByExpression<typename C::table_t>(col.clone(), Order::Desc, nulls);
    }

    template<_is_order_by_expression T>
    auto operator+(T lhs, T rhs)
    {
        for (auto& elem : rhs.elements) lhs.elements.emplace_back(std::move(elem));
        return std::forward<T>(lhs);
    }

}  // namespace sql
