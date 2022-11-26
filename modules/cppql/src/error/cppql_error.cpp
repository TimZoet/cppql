#include "cppql/error/cppql_error.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>

namespace sql
{
    CppqlError::CppqlError(const std::string& msg, const std::source_location loc)
    {
        message = std::format("\"{}\" in {} at {}:{}", msg, loc.file_name(), loc.line(), loc.column());
    }

    const char* CppqlError::what() const noexcept { return message.c_str(); }
}  // namespace cppql
