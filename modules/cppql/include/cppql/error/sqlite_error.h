#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <exception>
#include <string>
#include <source_location>

namespace sql
{
    class SqliteError final : public std::exception
    {
    public:
        SqliteError(const std::string&   msg,
                    int32_t              code,
                    int32_t              extendedCode,
                    std::source_location loc = std::source_location::current());

        [[nodiscard]] const char* what() const noexcept override;

    private:
        std::string message;
    };
}  // namespace sql
