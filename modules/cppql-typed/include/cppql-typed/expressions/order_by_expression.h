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
    // TODO: Rework to be templated on a list of columns to avoid dynamic allocations, allowing ordering on multiple cols.
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
            //BaseColumnExpressionPtr<T> column;
            std::string column;

            /**
             * \brief Order in which the values of this column are sorted.
             */
            Order order = Order::Asc;

            /**
             * \brief Optional order for NULLS.
             */
            Nulls nulls = Nulls::None;
        };

        OrderByExpression() = default;

        OrderByExpression(const OrderByExpression& other);

        OrderByExpression(OrderByExpression&& other) noexcept;

        //OrderByExpression(BaseColumnExpressionPtr<T> col, Order orderBy, Nulls nlls);
        OrderByExpression(std::string col, Order orderBy, Nulls nlls);

        ~OrderByExpression() = default;

        OrderByExpression& operator=(const OrderByExpression& other);

        OrderByExpression& operator=(OrderByExpression&& other) noexcept;

        [[nodiscard]] bool containsTables(const auto&... ) const
        {
            return true;
        }

        /**
         * \brief Generate ORDER BY string.
         * \return ORDER BY string.
         */
        [[nodiscard]] std::string toString() const;

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
    OrderByExpression<T>::OrderByExpression(std::string col, const Order orderBy, const Nulls nlls)

    {
        elements.emplace_back(std::move(col), orderBy, nlls);
    }

    template<typename T>
    OrderByExpression<T>& OrderByExpression<T>::operator=(const OrderByExpression& other)
    {
        for (const auto& elem : other.elements) elements.emplace_back(elem);
        return *this;
    }

    template<typename T>
    OrderByExpression<T>& OrderByExpression<T>::operator=(OrderByExpression&& other) noexcept
    {
        elements = std::move(other.elements);
        return *this;
    }

    template<typename T>
    std::string OrderByExpression<T>::toString() const
    {
        bool              first = true;
        std::stringstream s;
        s << "ORDER BY";
        for (const auto& elem : elements)
        {
            if (!first) s << ",";
            first = false;
            s << " " << elem.column << (elem.order == Order::Asc ? " ASC" : " DESC");
            if (elem.nulls == Nulls::First)
                s << " NULLS FIRST";
            else if (elem.nulls == Nulls::Last)
                s << " NULLS LAST";
        }
        return s.str();
    }

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    using OrderByExpressionPtr = std::unique_ptr<OrderByExpression<T>>;

    template<typename T>
    concept _is_order_by_expression = std::same_as<T, OrderByExpression<typename T::table_t>>;

    template<typename T, typename Table>
    concept is_order_by_expression = std::same_as<T, OrderByExpression<Table>>;

    template<typename T, typename Table>
    concept is_order_by_expression_or_none =
      is_order_by_expression<T, Table> || std::same_as<std::remove_cvref_t<T>, std::nullopt_t>;

    template<typename T, typename Tables>
    concept is_valid_order_by_expression =
      _is_order_by_expression<T> && tuple_contains_type<typename T::table_t, Tables>;  // TODO: Make list checker again.

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
        return OrderByExpression<typename C::table_t>(col.toString(), Order::Asc, nulls);
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
        return OrderByExpression<typename C::table_t>(col.toString(), Order::Desc, nulls);
    }

    template<_is_order_by_expression T>
    auto operator+(T lhs, T rhs)
    {
        for (auto& elem : rhs.elements) lhs.elements.emplace_back(elem);
        return std::forward<T>(lhs);
    }

}  // namespace sql
