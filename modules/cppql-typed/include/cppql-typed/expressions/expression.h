#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/table.h"

namespace sql
{
    // TODO: Move everything here elsewhere?

    template<typename T, typename... Ts>
    concept same_table = ((std::same_as<typename T::table_t, typename Ts::table_t>)&&...);

    /**
     * \brief Get the type of a table column by index.
     * \tparam Index Column index.
     * \tparam T Table.
     */
    template<size_t Index, typename T>
    using col_t = std::tuple_element_t<Index, typename T::row_t>;
}  // namespace sql
