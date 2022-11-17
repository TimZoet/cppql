#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/fwd.h"

namespace sql
{
    template<typename T>
    concept is_true_type_or_none = std::same_as<T, std::true_type> || std::same_as<T, std::nullopt_t>;

    template<is_typed_table T, is_filter_expression_or_none F>
    class CountQuery;

    template<is_typed_table T, is_filter_expression_or_none F, is_order_by_expression_or_none O, is_true_type_or_none L>
    class DeleteQuery;

    template<is_typed_table T, is_column_expression... Cs>
    class InsertQuery;

    template<typename R,
             is_join_or_typed_table         J,
             is_filter_expression_or_none   F,
             is_order_by_expression_or_none O,
             is_true_type_or_none           L,
             is_column_expression           C,
             is_column_expression... Cs>
        requires(constructible_from<R, typename C::value_t, typename Cs::value_t...>)
    class SelectQuery;

    template<is_typed_table                 T,
             is_filter_expression_or_none   F,
             is_order_by_expression_or_none O,
             is_true_type_or_none           L,
             is_column_expression           C,
             is_column_expression... Cs>
    class UpdateQuery;
}  // namespace sql
