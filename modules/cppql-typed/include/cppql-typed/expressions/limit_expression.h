#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <optional>
#include <string>

namespace sql
{
    // TODO: Discard this whole file and type once Limit clause is used everywhere.
    struct LimitExpression
    {
        int64_t limit  = 0;
        int64_t offset = 0;

        [[nodiscard]] std::string toString() const { return std::format("LIMIT {0} OFFSET {1}", limit, offset); }
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    concept is_limit_expression = std::same_as<T, LimitExpression>;

    template<typename T>
    concept is_limit_expression_or_none =
      is_limit_expression<T> || std::same_as<std::remove_cvref_t<T>, std::nullopt_t>;
}  // namespace sql
