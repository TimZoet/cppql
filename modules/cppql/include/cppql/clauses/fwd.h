#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

namespace sql
{
    ////////////////////////////////////////////////////////////////
    // GroupBy.
    ////////////////////////////////////////////////////////////////

    template<typename...>
    class GroupBy;

    template<typename...>
    struct _is_group_by : std::false_type
    {
    };

    template<typename... Cs>
    struct _is_group_by<GroupBy<Cs...>> : std::conditional_t<GroupBy<Cs...>::valid, std::true_type, std::false_type>
    {
    };

    template<typename T>
    concept is_group_by = _is_group_by<T>::value;

    template<typename T>
    concept is_group_by_or_none = std::same_as<T, std::nullopt_t> || _is_group_by<T>::value;

    ////////////////////////////////////////////////////////////////
    // Union.
    ////////////////////////////////////////////////////////////////

    template<typename...>
    class Union;

    template<typename...>
    struct _is_union : std::false_type
    {
    };

    template<typename... Qs>
    struct _is_union<Union<Qs...>> : std::conditional_t<Union<Qs...>::valid, std::true_type, std::false_type>
    {
    };

    template<typename T>
    concept is_union = _is_union<T>::value;

    template<typename T>
    concept is_union_or_none = std::same_as<T, std::nullopt_t> || _is_union<T>::value;

    template<typename Q, typename...>
    struct merge_unions
    {
        using type = Union<Q>;
    };

    template<typename Q, typename... Qs>
    struct merge_unions<Q, std::tuple<Qs...>>
    {
        using type = Union<Qs..., Q>;
    };

    template<typename Q, typename Prev>
    using merge_unions_t = typename merge_unions<Q, Prev>::type;
}  // namespace sql
