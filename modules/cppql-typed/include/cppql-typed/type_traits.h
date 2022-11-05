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
}  // namespace sql