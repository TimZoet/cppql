#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/typed/fwd.h"

namespace sql
{
    class CountStatement;

    class DeleteStatement;

    template<typename... Cs>
    class InsertStatement;

    template<typename R, typename... Cs>
        requires(constructible_from<R, Cs...>)
    class SelectStatement;

    template<typename R, typename... Cs>
        requires(constructible_from<R, Cs...>)
    class SelectOneStatement;

    template<typename... Cs>
    class UpdateStatement;
}  // namespace sql
