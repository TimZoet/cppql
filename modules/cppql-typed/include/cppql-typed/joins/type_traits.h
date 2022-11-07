#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

namespace sql
{
    // TODO: Require L to be a join or TypedTable and R to a TypedTable. Require F to be a FilterExpression.
    template<typename L, typename R, typename F>
    class InnerJoin;

    template<typename T, typename... Ts>
    struct _is_join : std::false_type
    {
    };

    template<typename L, typename R, typename F>
    struct _is_join<InnerJoin<L, R, F>> : std::true_type
    {
    };

    template<typename T>
    concept is_join = _is_join<T>::value;

    
}  // namespace sql
