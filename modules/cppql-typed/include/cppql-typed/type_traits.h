#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <tuple>

namespace sql
{
    /**
     * \brief Get the type of a table column by index.
     * \tparam Index Column index.
     * \tparam T Table.
     */
    template<size_t Index, typename T>
    using col_t = std::tuple_element_t<Index, typename T::row_t>;

    template<typename R, typename... Cs>
    concept constructible_from = std::constructible_from<R, get_column_return_t<Cs>...>;

    // TODO: Move some of these type traits into common library.

    ////////////////////////////////////////////////////////////////

    template<typename Tuple0, typename Tuple1>
    struct tuple_merge
    {
        using type = Tuple0;
    };

    template<typename Tuple0>
    struct tuple_merge<Tuple0, std::tuple<>>
    {
        using type = Tuple0;
    };

    template<typename Tuple0, typename T, typename... Ts>
    struct tuple_merge<Tuple0, std::tuple<T, Ts...>> : tuple_merge<tuple_cat_t<Tuple0, T>, std::tuple<Ts...>>
    {
    };

    template<typename Tuple0, typename Tuple1>
    using tuple_merge_t = typename tuple_merge<Tuple0, Tuple1>::type;

    ////////////////////////////////////////////////////////////////

    template<typename T, typename Tuple>
    struct tuple_has_type;

    template<typename T>
    struct tuple_has_type<T, std::tuple<>> : std::false_type
    {
    };

    template<typename T, typename U, typename... Ts>
    struct tuple_has_type<T, std::tuple<U, Ts...>> : tuple_has_type<T, std::tuple<Ts...>>
    {
    };

    template<typename T, typename... Ts>
    struct tuple_has_type<T, std::tuple<T, Ts...>> : std::true_type
    {
    };

    template<typename T, typename Tuple>
    concept tuple_contains_type = tuple_has_type<T, Tuple>::value;

    ////////////////////////////////////////////////////////////////

    template<typename Tuple, typename... Ts>
    struct tuple_unique
    {
        using type = Tuple;
    };

    template<typename Tuple, typename T, typename... Ts>
    struct tuple_unique<Tuple, std::tuple<T, Ts...>> : tuple_unique<Tuple, std::tuple<Ts...>>
    {
    };

    template<typename Tuple, typename T, typename... Ts>
        requires(!tuple_contains_type<T, Tuple>)
    struct tuple_unique<Tuple, std::tuple<T, Ts...>> : tuple_unique<tuple_cat_t<Tuple, T>, std::tuple<Ts...>>
    {
    };

    template<typename Tuple>
    struct tuple_unique<Tuple, std::tuple<>> : tuple_unique<Tuple>
    {
    };

    template<typename Tuple>
    using tuple_unique_t = typename tuple_unique<std::tuple<>, Tuple>::type;

    ////////////////////////////////////////////////////////////////

    template<typename Superset, typename... Ts>
    struct tuple_subset : std::false_type
    {
    };

    template<>
    struct tuple_subset<std::tuple<>, std::tuple<>> : std::true_type
    {
    };

    template<typename Superset, typename T, typename... Ts>
    struct tuple_subset<Superset, std::tuple<T, Ts...>> : std::false_type
    {
    };

    template<typename Superset, typename T, typename... Ts>
        requires(tuple_contains_type<T, Superset>)
    struct tuple_subset<Superset, std::tuple<T, Ts...>> : tuple_subset<Superset, std::tuple<Ts...>>
    {
    };

    template<typename Superset>
    struct tuple_subset<Superset, std::tuple<>> : std::true_type
    {
    };

    template<typename Subset, typename Superset>
    concept tuple_is_subset = tuple_subset<Superset, Subset>::value;

    ////////////////////////////////////////////////////////////////

    template<typename Tuple, int64_t Index>
    struct tuple_index_wrapped
    {
        static constexpr size_t value = static_cast<size_t>(Index);
    };

    template<typename Tuple, int64_t Index>
        requires(Index < 0)
    struct tuple_index_wrapped<Tuple, Index>
    {
        static constexpr size_t value = static_cast<size_t>(static_cast<int64_t>(std::tuple_size_v<Tuple>) + Index);
    };

    template<typename Tuple, int64_t I>
    inline constexpr size_t tuple_index_wrapped_v = tuple_index_wrapped<Tuple, I>::value;

    template<typename Tuple, int64_t I>
    using tuple_element_wrapped_t = std::tuple_element_t<tuple_index_wrapped_v<Tuple, I>, Tuple>;

    template<typename Tuple, int64_t...>
    struct tuple_swizzle
    {
        using type = std::tuple<>;
    };

    template<typename Tuple, int64_t I, int64_t... Is>
    struct tuple_swizzle<Tuple, I, Is...>
    {
        using type =
          tuple_merge_t<std::tuple<tuple_element_wrapped_t<Tuple, I>>, typename tuple_swizzle<Tuple, Is...>::type>;
    };

    template<typename Tuple, int64_t I, int64_t... Is>
    using tuple_swizzle_t = typename tuple_swizzle<Tuple, I, Is...>::type;

}  // namespace sql