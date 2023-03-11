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

        [[nodiscard]] int32_t getErrorCode() const noexcept;

        [[nodiscard]] int32_t getExtendedErrorCode() const noexcept;

    private:
        std::string message;
        int32_t     errorCode;
        int32_t     extendedErrorCode;
    };
}  // namespace sql
