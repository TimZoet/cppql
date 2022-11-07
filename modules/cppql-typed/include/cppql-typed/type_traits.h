#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <tuple>

namespace sql
{
    template<size_t Count, size_t... Indices>
    concept in_column_range = sizeof...(Indices) == 0 || sizeof...(Indices) > 0 && ((Indices < Count) && ...);

    /**
     * \brief Get the type of a table column by index.
     * \tparam Index Column index.
     * \tparam T Table.
     */
    template<size_t Index, typename T>
    using col_t = std::tuple_element_t<Index, typename T::row_t>;

    template<typename T, typename... Ts>
    concept same_table = ((std::same_as<typename T::table_t, typename Ts::table_t>)&&...);

    template<typename R, typename T, size_t... Indices>
    concept constructible_from = std::constructible_from<R, get_column_return_t<col_t<Indices, T>>...>;

    /**
     * \brief Forward value to std::make_unique or return nullptr if it is of type std::nullopt_t.
     * \tparam T std::nullopt_t or type.
     * \param val std::nullopt or value.
     * \return nullptr or std::unique_ptr<T>.
     */
    template<typename T>
    auto optionalToPtr(T&& val)
    {
        if constexpr (std::same_as<std::remove_cvref_t<T>, std::nullopt_t>)
            return nullptr;
        else
            return std::make_unique<T>(std::forward<T>(val));
    }

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
    requires(!tuple_contains_type<T, Tuple>) struct tuple_unique<Tuple, std::tuple<T, Ts...>>
        : tuple_unique<tuple_cat_t<Tuple, T>, std::tuple<Ts...>>
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
    requires(tuple_contains_type<T, Superset>) struct tuple_subset<Superset, std::tuple<T, Ts...>>
        : tuple_subset<Superset, std::tuple<Ts...>>
    {
    };

    template<typename Superset>
    struct tuple_subset<Superset, std::tuple<>> : std::true_type
    {
    };

    template<typename Subset, typename Superset>
    concept tuple_is_subset = tuple_subset<Superset, Subset>::value;
}  // namespace sql