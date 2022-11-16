#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <format>
#include <optional>
#include <string>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/column_expression.h"

namespace sql
{
    // TODO: Restrict, if possible.
    template<typename L, typename R>
    class OrderByExpression;

    template<typename T>
    concept is_order_by_expression =
      std::same_as<std::remove_cvref_t<T>,
                   OrderByExpression<typename std::remove_cvref_t<T>::left_t, typename std::decay_t<T>::right_t>>;

    template<typename T, typename Tables>
    concept is_valid_order_by_expression =
      is_order_by_expression<T> && tuple_is_subset<typename std::remove_cvref_t<T>::unique_table_list_t, Tables>;

    template<typename T>
    struct get_order_table_list
    {
        using table_list_t = std::tuple<>;
    };

    template<is_order_by_expression T>
    struct get_order_table_list<T>
    {
        using table_list_t = typename T::table_list_t;
    };

    template<is_column_expression T>
    struct get_order_table_list<T>
    {
        using table_list_t = std::tuple<typename T::table_t>;
    };

    template<typename T>
    using get_order_table_list_t = typename get_order_table_list<T>::table_list_t;

    template<typename L, typename R>
    class OrderByExpression
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using left_t              = L;
        using right_t             = R;
        using table_list_t        = tuple_merge_t<get_order_table_list_t<left_t>, get_order_table_list_t<right_t>>;
        using unique_table_list_t = tuple_unique_t<table_list_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        OrderByExpression() = delete;

        OrderByExpression(const OrderByExpression&) = default;

        OrderByExpression(OrderByExpression&&) noexcept = default;

        OrderByExpression(left_t l, right_t r) :
            left(std::move(l)), right(std::move(r)), order(Order::Asc), nulls(Nulls::None)
        {
        }

        OrderByExpression(left_t l, const Order o, const Nulls n) :
            left(std::move(l)), right(std::nullopt), order(o), nulls(n)
        {
        }

        ~OrderByExpression() noexcept = default;

        OrderByExpression& operator=(const OrderByExpression&) = default;

        OrderByExpression& operator=(OrderByExpression&&) noexcept = default;

        [[nodiscard]] bool containsTables(const auto&... tables) const
        {
            if constexpr (is_column_expression<left_t>)
                return left.containsTables(tables...);
            else
                return left.containsTables(tables...) && right.containsTables(tables...);
        }

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate columns of ORDER BY clause.
         * \return String with format "<col> <order> [<nulls>],...,<col> <order> [<nulls>]".
         */
        [[nodiscard]] std::string toString() const
        {
            if constexpr (is_column_expression<left_t>)
            {
                return std::format("{0} {1} {2}",
                                   left.fullName(),
                                   order == Order::Asc ? OrderString<Order::Asc>::str : OrderString<Order::Desc>::str,
                                   nulls == Nulls::None  ? NullsString<Nulls::None>::str :
                                   nulls == Nulls::First ? NullsString<Nulls::First>::str :
                                                           NullsString<Nulls::Last>::str);
            }
            else { return std::format("{0}, {1}", left.toString(), right.toString()); }
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        left_t  left;
        right_t right;
        Order   order;
        Nulls   nulls;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename L, typename R>
    struct _is_filter_expression<OrderByExpression<L, R>> : std::true_type
    {
    };

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
        return OrderByExpression<C, std::nullopt_t>(std::forward<C>(col), Order::Asc, nulls);
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
        return OrderByExpression<C, std::nullopt_t>(std::forward<C>(col), Order::Desc, nulls);
    }

    template<is_order_by_expression L, is_order_by_expression R>
    auto operator+(L&& left, R&& right)
    {
        return OrderByExpression<std::decay_t<L>, std::decay_t<R>>(std::forward<L>(left), std::forward<R>(right));
    }

}  // namespace sql
