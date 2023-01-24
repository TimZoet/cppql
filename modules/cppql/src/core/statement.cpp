#include "cppql/core/statement.h"

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "sqlite3.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/database.h"

namespace sql
{
    ////////////////////////////////////////////////////////////////
    // Result.
    ////////////////////////////////////////////////////////////////

    int32_t Result::sqlite_row  = SQLITE_ROW;
    int32_t Result::sqlite_done = SQLITE_DONE;

    Result::Result(const int32_t c, const int32_t ext, const bool s) : code(c), extendedCode(ext), success(s) {}

    Result::operator bool() const noexcept { return success; }

    Result Result::operator|(const Result& rhs) const noexcept
    {
        // If both results are successful, return last one.
        if (*this && rhs) return rhs;
        // If both failed, return first failure.
        if (!*this && !rhs) return *this;
        // Return only failing result.
        return rhs ? *this : rhs;
    }

    Result& Result::operator|=(const Result& rhs) noexcept { return *this = *this | rhs; }

    Result Result::fromCode(const Database& db, const int32_t code, const bool success) noexcept
    {
        if (!success)
        {
            const int32_t extCode = sqlite3_extended_errcode(db.get());
            return {code, extCode, success};
        }
        return {code, SQLITE_OK, success};
    }

    ////////////////////////////////////////////////////////////////
    // Statement.
    ////////////////////////////////////////////////////////////////

    Statement::Statement(Database& database, std::string code, const bool prepare) : db(&database), sql(std::move(code))
    {
        if (prepare) this->prepare();
    }

    Statement::~Statement()
    {
        if (statement) sqlite3_finalize(statement);
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    sqlite3_stmt* Statement::get() const noexcept { return statement; }

    bool Statement::isPrepared() const noexcept { return statement != nullptr; }

    const std::string& Statement::getSql() const noexcept { return sql; }

    std::optional<Result> Statement::getResult() const noexcept { return prepareResult; }

    int32_t Statement::getFirstBindIndex() noexcept
    {
#ifdef CPPQL_BIND_ZERO_BASED_INDICES
        return 0;
#else
        return 1;
#endif
    }

    ////////////////////////////////////////////////////////////////
    // ...
    ////////////////////////////////////////////////////////////////

    Result Statement::prepare() noexcept
    {
        // If statement was already prepared, return generic error.
        if (statement) return Result::fromCode(*db, SQLITE_ERROR, false);

        // Try to prepare statement.
        const auto code =
          sqlite3_prepare_v2(db->db, getSql().c_str(), static_cast<int32_t>(getSql().size()), &statement, nullptr);
        prepareResult = Result::fromCode(*db, code, code == SQLITE_OK);
        return *prepareResult;
    }

    Result Statement::step() const noexcept
    {
        const auto code = sqlite3_step(statement);
        return Result::fromCode(*db, code, code == SQLITE_ROW || code == SQLITE_DONE);
    }

    Result Statement::reset() const noexcept
    {
        const auto code = sqlite3_reset(statement);
        return Result::fromCode(*db, code, code == SQLITE_OK);
    }

    ////////////////////////////////////////////////////////////////
    // Bindings.
    ////////////////////////////////////////////////////////////////

    Result Statement::bindInt(const int32_t index, const int32_t value) const noexcept
    {
#ifdef CPPQL_BIND_ZERO_BASED_INDICES
        const int32_t i = index + 1;
#else
        const int32_t i = index;
#endif

        const auto code = sqlite3_bind_int(statement, i, value);
        return Result::fromCode(*db, code, code == SQLITE_OK);
    }

    Result Statement::bindInt64(const int32_t index, const int64_t value) const noexcept
    {
#ifdef CPPQL_BIND_ZERO_BASED_INDICES
        const int32_t i = index + 1;
#else
        const int32_t i = index;
#endif

        const auto code = sqlite3_bind_int64(statement, i, value);
        return Result::fromCode(*db, code, code == SQLITE_OK);
    }

    Result Statement::bindFloat(const int32_t index, const float value) const noexcept
    {
#ifdef CPPQL_BIND_ZERO_BASED_INDICES
        const int32_t i = index + 1;
#else
        const int32_t i = index;
#endif

        const auto code = sqlite3_bind_double(statement, i, static_cast<double>(value));
        return Result::fromCode(*db, code, code == SQLITE_OK);
    }

    Result Statement::bindDouble(const int32_t index, const double value) const noexcept
    {
#ifdef CPPQL_BIND_ZERO_BASED_INDICES
        const int32_t i = index + 1;
#else
        const int32_t i = index;
#endif

        const auto code = sqlite3_bind_double(statement, i, value);
        return Result::fromCode(*db, code, code == SQLITE_OK);
    }

    Result Statement::bindNull(const int32_t index) const noexcept
    {
#ifdef CPPQL_BIND_ZERO_BASED_INDICES
        const int32_t i = index + 1;
#else
        const int32_t i = index;
#endif

        const auto code = sqlite3_bind_null(statement, i);
        return Result::fromCode(*db, code, code == SQLITE_OK);
    }

    Result Statement::bindBlob(const int32_t index,
                               const void*   data,
                               const size_t  size,
                               void (*destructor)(void*)) const noexcept
    {
#ifdef CPPQL_BIND_ZERO_BASED_INDICES
        const int32_t i = index + 1;
#else
        const int32_t i = index;
#endif

        const auto code = sqlite3_bind_blob64(statement, i, data, size, destructor);
        return Result::fromCode(*db, code, code == SQLITE_OK);
    }

    Result Statement::bindStaticBlob(const int32_t index, const void* data, const size_t size) const noexcept
    {
#ifdef CPPQL_BIND_ZERO_BASED_INDICES
        const int32_t i = index + 1;
#else
        const int32_t i = index;
#endif

        const auto code = sqlite3_bind_blob64(statement, i, data, size, SQLITE_STATIC);
        return Result::fromCode(*db, code, code == SQLITE_OK);
    }

    Result Statement::bindTransientBlob(const int32_t index, const void* data, const size_t size) const noexcept
    {
#ifdef CPPQL_BIND_ZERO_BASED_INDICES
        const int32_t i = index + 1;
#else
        const int32_t i = index;
#endif

        const auto code = sqlite3_bind_blob64(statement, i, data, size, SQLITE_TRANSIENT);
        return Result::fromCode(*db, code, code == SQLITE_OK);
    }

    Result Statement::bindText(const int32_t index,
                               const char*   data,
                               const size_t  size,
                               void (*destructor)(void*)) const noexcept
    {
#ifdef CPPQL_BIND_ZERO_BASED_INDICES
        const int32_t i = index + 1;
#else
        const int32_t i = index;
#endif

        const auto code = sqlite3_bind_text(statement, i, data, static_cast<int32_t>(size), destructor);
        return Result::fromCode(*db, code, code == SQLITE_OK);
    }

    Result Statement::bindStaticText(const int32_t index, const char* data, const size_t size) const noexcept
    {
#ifdef CPPQL_BIND_ZERO_BASED_INDICES
        const int32_t i = index + 1;
#else
        const int32_t i = index;
#endif

        const auto code = sqlite3_bind_text(statement, i, data, static_cast<int32_t>(size), SQLITE_STATIC);
        return Result::fromCode(*db, code, code == SQLITE_OK);
    }

    Result Statement::bindStaticText(const int32_t index, const std::string& data) const noexcept
    {
        return bindStaticText(index, data.data(), data.size());
    }

    Result Statement::bindTransientText(const int32_t index, const char* data, const size_t size) const noexcept
    {
#ifdef CPPQL_BIND_ZERO_BASED_INDICES
        const int32_t i = index + 1;
#else
        const int32_t i = index;
#endif

        const auto code = sqlite3_bind_text(statement, i, data, static_cast<int32_t>(size), SQLITE_TRANSIENT);
        return Result::fromCode(*db, code, code == SQLITE_OK);
    }

    Result Statement::bindTransientText(const int32_t index, const std::string& data) const noexcept
    {
        return bindTransientText(index, data.data(), data.size());
    }

    Result Statement::clearBindings() const noexcept
    {
        const auto code = sqlite3_clear_bindings(statement);
        return Result::fromCode(*db, code, code == SQLITE_OK);
    }

    ////////////////////////////////////////////////////////////////
    // Column getters.
    ////////////////////////////////////////////////////////////////

    Column::Type Statement::columnType(const int32_t index) const noexcept
    {
        switch (sqlite3_column_type(statement, index))
        {
        case SQLITE_BLOB: return Column::Type::Blob;
        case SQLITE_FLOAT: return Column::Type::Real;
        case SQLITE_INTEGER: return Column::Type::Int;
        case SQLITE_TEXT: return Column::Type::Text;
        default: return Column::Type::Null;
        }
    }

    void Statement::column(const int32_t index, int32_t& value) const noexcept
    {
        value = sqlite3_column_int(statement, index);
    }

    void Statement::column(const int32_t index, int64_t& value) const noexcept
    {
        value = sqlite3_column_int64(statement, index);
    }

    void Statement::column(const int32_t index, float& value) const noexcept
    {
        value = static_cast<float>(sqlite3_column_double(statement, index));
    }

    void Statement::column(const int32_t index, double& value) const noexcept
    {
        value = sqlite3_column_double(statement, index);
    }

    void Statement::column(const int32_t index, std::string& value) const
    {
        const auto* const data = sqlite3_column_text(statement, index);
        const auto        size = static_cast<size_t>(sqlite3_column_bytes(statement, index));
        value.resize(size);
        std::memcpy(value.data(), data, size);
    }

    std::pair<const void*, size_t> Statement::columnBlob(const int32_t index) const
    {
        const auto* data = sqlite3_column_blob(statement, index);
        auto        size = static_cast<size_t>(sqlite3_column_bytes(statement, index));
        return std::make_pair(data, size);
    }
}  // namespace sql