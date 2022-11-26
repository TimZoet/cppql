#include "cppql/error/sqlite_error.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>

namespace sql
{
    SqliteError::SqliteError(const std::string& msg, const int32_t code, const std::source_location loc)
    {
        message = std::format(
          "Sqlite error ({}): \"{}\" in {} at {}:{}", code, msg, loc.file_name(), loc.line(), loc.column());
    }

    const char* SqliteError::what() const noexcept { return message.c_str(); }
}  // namespace sql
