#include "cppql/error/sqlite_error.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>

namespace sql
{
    SqliteError::SqliteError(const std::string&         msg,
                             const int32_t              code,
                             const int32_t              extendedCode,
                             const std::source_location loc) :
        errorCode(code), extendedErrorCode(extendedCode)
    {
        message = std::format("Sqlite error ({}, ext={}): \"{}\" in {} at {}:{}",
                              errorCode,
                              extendedErrorCode,
                              msg,
                              loc.file_name(),
                              loc.line(),
                              loc.column());
    }

    const char* SqliteError::what() const noexcept { return message.c_str(); }

    int32_t SqliteError::getErrorCode() const noexcept { return errorCode; }

    int32_t SqliteError::getExtendedErrorCode() const noexcept { return extendedErrorCode; }
}  // namespace sql
