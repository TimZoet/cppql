#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/column.h"
#include "cppql/typed/join_type.h"

namespace sql
{
    ////////////////////////////////////////////////////////////////
    // TypedTable.
    ////////////////////////////////////////////////////////////////

    template<typename C, typename... Cs>
    class TypedTable;

    template<typename...>
    struct _is_typed_table : std::false_type
    {
    };

    template<typename C, typename... Cs>
    struct _is_typed_table<TypedTable<C, Cs...>> : std::true_type
    {
    };

    template<typename T>
    concept is_typed_table = _is_typed_table<std::remove_cvref_t<T>>::value;

    template<typename T, size_t I>
    concept is_valid_index = is_typed_table<T> && I < T::column_count;

    /**
     * \brief Get the type of a table column by index.
     * \tparam Index Column index.
     * \tparam T Table.
     */
    template<size_t Index, typename T>
    using col_t = std::tuple_element_t<Index, typename T::row_t>;

    ////////////////////////////////////////////////////////////////
    // FilterExpression.
    ////////////////////////////////////////////////////////////////

    template<typename... T>
    struct _is_filter_expression : std::false_type
    {
    };

    template<typename T>
    inline constexpr bool is_filter_expression_v = _is_filter_expression<T>::value;

    template<typename T>
    concept is_filter_expression = is_filter_expression_v<T>;

    template<typename T>
    concept is_filter_expression_or_none = std::same_as<std::nullopt_t, T> || is_filter_expression<T>;

    template<typename F, typename Tables>
    concept is_valid_filter_expression =
      is_filter_expression<F> && tuple_is_subset<typename std::remove_cvref_t<F>::unique_table_list_t, Tables>;

    ////////////////////////////////////////////////////////////////
    // ColumnExpression.
    ////////////////////////////////////////////////////////////////

    template<is_typed_table T, size_t Index>
        requires(is_valid_index<T, Index>)
    class ColumnExpression;

    template<typename...>
    struct _is_column_expression : std::false_type
    {
    };

    template<typename T, size_t Index>
    struct _is_column_expression<ColumnExpression<T, Index>> : std::true_type
    {
    };

    /**
     * \brief Check if a type is a ColumnExpression.
     * \tparam C Type.
     */
    template<typename C>
    concept is_column_expression = _is_column_expression<std::remove_cvref_t<C>>::value;

    /**
     * \brief Check if a type is a ColumnExpression for a TypedTable in the provided list of types.
     * \tparam C Type.
     * \tparam Tables Tuple containing a list of TypedTables.
     */
    template<typename C, typename Tables>
    concept is_valid_column_expression =
      is_column_expression<C> && tuple_contains_type<typename std::remove_cvref_t<C>::table_t, Tables>;

    template<typename R, typename... Cs>
    concept constructible_from = std::constructible_from<R, get_column_return_t<Cs>...>;

    ////////////////////////////////////////////////////////////////
    // AggregateExpression.
    ////////////////////////////////////////////////////////////////

    template<typename A>
    concept is_aggregate_function = requires {
                                        {
                                            A::toString(std::declval<std::string>())
                                            } -> std::same_as<std::string>;
                                    };

    template<is_column_expression C, is_aggregate_function A>
    class AggregateExpression;

    template<typename...>
    struct _is_aggregate_expression : std::false_type
    {
    };

    template<typename C, typename A>
    struct _is_aggregate_expression<AggregateExpression<C, A>> : std::true_type
    {
    };

    /**
     * \brief Check if a type is an AggregateExpression.
     * \tparam T Type.
     */
    template<typename T>
    concept is_aggregate_expression = _is_aggregate_expression<std::remove_cvref_t<T>>::value;

    /**
     * \brief Check if a type is a AggregateExpression for a TypedTable in the provided list of types.
     * \tparam T Type.
     * \tparam Tables Tuple containing a list of TypedTables.
     */
    template<typename T, typename Tables>
    concept is_valid_aggregate_expression =
      is_aggregate_expression<T> && tuple_contains_type<typename std::remove_cvref_t<T>::table_t, Tables>;

    ////////////////////////////////////////////////////////////////
    // Result.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    concept is_result_expression = is_column_expression<T> || is_aggregate_expression<T>;

    template<typename T, typename Tables>
    concept is_valid_result_expression =
      is_valid_column_expression<T, Tables> || is_valid_aggregate_expression<T, Tables>;

    /**
     * \brief Check if a type is convertible to the value type of a ColumnExpression/AggregateExpression.
     * \tparam T Type.
     * \tparam C ColumnExpression type.
     */
    template<typename T, typename C>
    concept is_convertible_to = is_result_expression<C> && std::convertible_to<T, typename C::value_t>;

    ////////////////////////////////////////////////////////////////
    // Join.
    ////////////////////////////////////////////////////////////////

    template<typename J, typename... Ts>
    struct _is_join : std::false_type
    {
    };

    template<typename T>
    concept is_join = _is_join<T>::value;

    template<typename T>
    concept is_join_or_typed_table = is_join<T> || is_typed_table<T>;

    template<is_join_wrapper              J,
             is_join_or_typed_table       L,
             is_typed_table               R,
             is_filter_expression_or_none F,
             is_column_expression... Cs>
    class Join;

    template<typename J, typename L, typename R, typename F, typename... Cs>
    struct _is_join<Join<J, L, R, F, Cs...>> : std::true_type
    {
    };
}  // namespace sql
