#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <string>

namespace sql
{
    struct LimitExpression
    {
        int64_t limit  = 0;
        int64_t offset = 0;

        [[nodiscard]] std::string toString() const { return std::format("LIMIT {0} OFFSET {1}", limit, offset); }
    };
}  // namespace sql
