#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/type_traits.h"
#include "cppql-typed/expressions/column_expression.h"

namespace sql
{
    template<typename T, typename F>
    class CountQuery;

    template<typename T, typename F, typename O, typename L>
    class DeleteQuery;

    template<typename T, is_column_expression... Cs>
    class InsertQuery;

    template<typename R,
        typename J,
        typename F,
        typename O,
        typename L,
        is_column_expression C,
        is_column_expression... Cs>
        requires(constructible_from<R, typename C::value_t, typename Cs::value_t...>)
    class SelectQuery;

    template<typename T, typename F, typename O, typename L, is_column_expression C, is_column_expression... Cs>
    class UpdateQuery;
}  // namespace sql
