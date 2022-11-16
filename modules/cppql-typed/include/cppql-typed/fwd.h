#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/type_traits.h"

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

    template<typename F, typename Tables>
    concept is_valid_filter_expression =
      is_filter_expression<F> && tuple_is_subset<typename std::remove_cvref_t<F>::unique_table_list_t, Tables>;

    ////////////////////////////////////////////////////////////////
    // ColumnExpression.
    ////////////////////////////////////////////////////////////////

    template<is_typed_table T, size_t Index>
        requires(is_valid_index<T, Index>)
    class ColumnExpression;

    /**
     * \brief Check if a type is a ColumnExpression.
     * \tparam C Type.
     */
    template<typename C>
    concept is_column_expression =
      std::same_as<std::remove_cvref_t<C>, ColumnExpression<typename C::table_t, C::index>>;

    /**
     * \brief Check if a type is a ColumnExpression for a TypedTable in the provided list of types.
     * \tparam C Type.
     * \tparam Tables Tuple containing a list of TypedTables.
     */
    template<typename C, typename Tables>
    concept is_valid_column_expression = is_column_expression<C> && tuple_contains_type<typename C::table_t, Tables>;

    /**
     * \brief Check if a type is convertible to the value type of a ColumnExpression.
     * \tparam T Type.
     * \tparam C ColumnExpression type.
     */
    template<typename T, typename C>
    concept is_convertible_to = is_column_expression<C> && std::convertible_to<T, typename C::value_t>;

    ////////////////////////////////////////////////////////////////
    // Join.
    ////////////////////////////////////////////////////////////////

    // TODO: Require J to be a JoinTypeWrapper, L to be a join or TypedTable and R to a TypedTable. Require F to be a FilterExpression.
    template<typename J, typename L, typename R, typename F, typename... Cs>
    class Join;

    template<typename J, typename... Ts>
    struct _is_join : std::false_type
    {
    };

    template<typename J, typename L, typename R, typename F, typename... Cs>
    struct _is_join<Join<J, L, R, F, Cs...>> : std::true_type
    {
    };

    template<typename T>
    concept is_join = _is_join<T>::value;
}  // namespace sql
