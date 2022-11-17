#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"
#include "cppql-core/column.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/join_type.h"

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

    template<typename R, typename... Cs>
    concept constructible_from = std::constructible_from<R, get_column_return_t<Cs>...>;

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

    template<is_join_wrapper J, is_join_or_typed_table L, is_typed_table R, is_filter_expression_or_none F, is_column_expression... Cs>
    class Join;

    template<typename J, typename L, typename R, typename F, typename... Cs>
    struct _is_join<Join<J, L, R, F, Cs...>> : std::true_type
    {
    };
}  // namespace sql
