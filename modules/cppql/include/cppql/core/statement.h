#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cstring>
#include <concepts>
#include <format>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/static_assert.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/binding.h"
#include "cppql/core/column.h"
#include "cppql/error/cppql_error.h"

struct sqlite3_stmt;

namespace sql
{
    class Database;
    class Statement;
    using StatementPtr = std::unique_ptr<Statement>;

    struct Result final
    {
        static int32_t sqlite_row;
        static int32_t sqlite_done;

        int32_t code         = 0;
        int32_t extendedCode = 0;
        bool    success      = true;

        Result() = default;

    private:
        Result(int32_t c, int32_t ext, bool s);

    public:
        Result(const Result&) = default;

        ~Result() = default;

        Result(Result&&) = default;

        Result& operator=(const Result&) = default;

        Result& operator=(Result&&) = default;

        [[nodiscard]] operator bool() const noexcept;

        [[nodiscard]] Result operator|(const Result& rhs) const noexcept;

        Result& operator|=(const Result& rhs) noexcept;

        static Result fromCode(const Database& db, int32_t code, bool success) noexcept;
    };

    class Statement
    {
    public:
        Statement() = delete;

        Statement(Database& database, std::string code, bool prepare);

        Statement(const Statement&) = delete;

        Statement(Statement&&) noexcept = default;

        ~Statement();

        Statement& operator=(const Statement&) = delete;

        Statement& operator=(Statement&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] sqlite3_stmt* get() const noexcept;

        [[nodiscard]] bool isPrepared() const noexcept;

        [[nodiscard]] const std::string& getSql() const noexcept;

        [[nodiscard]] std::optional<Result> getResult() const noexcept;

        [[nodiscard]] static int32_t getFirstBindIndex() noexcept;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Prepare statement. Internally calls sqlite3_prepare_v2.
         * \return Error code.
         */
        Result prepare() noexcept;

        /**
         * \brief Run statement. Internally calls sqlite3_step.
         * \return Error code.
         */
        [[nodiscard]] Result step() const noexcept;

        /**
         * \brief Reset statement. Internally calls sqlite3_reset.
         * \return Error code.
         */
        [[nodiscard]] Result reset() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Bindings.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Bind one or more values. The bind method that is used is determined automatically for each value.
         * \tparam T Type of first value.
         * \tparam Ts Type of next values.
         * \param index Index of first parameter.
         * \param value First value.
         * \param values Next values.
         * \return Union of all error codes.
         */
        template<bindable T, bindable... Ts>
        [[nodiscard]] Result bind(const int32_t index, T value, Ts... values) const noexcept
        {
            // Create Binding object.
            auto b         = Binding<T>(std::move(value));
            using return_t = typename decltype(b)::return_t;

            Result res;

            if (!b.holdsValue())
                res |= bindNull(index);
            else
            {
                // Call appropriate bind function.
                if constexpr (std::same_as<return_t, int32_t>)
                    res |= bindInt(index, b.get());
                else if constexpr (std::same_as<return_t, int64_t>)
                    res |= bindInt64(index, b.get());
                else if constexpr (std::same_as<return_t, double>)
                    res |= bindDouble(index, b.get());
                else if constexpr (std::same_as<return_t, std::nullptr_t>)
                    res |= bindNull(index);
                else if constexpr (std::same_as<return_t, const Text&>)
                {
                    const Text& text = b.get();
                    res |= bindText(index, text.data, text.size, text.destructor);
                }
                else if constexpr (std::same_as<return_t, const StaticText&>)
                {
                    const StaticText& text = b.get();
                    res |= bindStaticText(index, text.data, text.size);
                }
                else if constexpr (std::same_as<return_t, const TransientText&>)
                {
                    const TransientText& text = b.get();
                    res |= bindTransientText(index, text.data, text.size);
                }
                else if constexpr (std::same_as<return_t, const Blob&>)
                {
                    const Blob& blob = b.get();
                    res |= bindBlob(index, blob.data, blob.size, blob.destructor);
                }
                else if constexpr (std::same_as<return_t, const StaticBlob&>)
                {
                    const StaticBlob& blob = b.get();
                    res |= bindStaticBlob(index, blob.data, blob.size);
                }
                else if constexpr (std::same_as<return_t, const TransientBlob&>)
                {
                    const TransientBlob& blob = b.get();
                    res |= bindTransientBlob(index, blob.data, blob.size);
                }
#ifdef WIN32
                else { constexpr_static_assert<!std::same_as<return_t, return_t>>(); }
#else
                else { constexpr_static_assert(); }
#endif
            }

            // Recurse.
            if constexpr (sizeof...(Ts) > 0) res |= bind(index + 1, values...);

            return res;
        }

        /**
         * \brief Bind a 32-bit signed integer value. Internally calls sqlite3_bind_int.
         * \param index Parameter index.
         * \param value Value to bind.
         * \return ...
         */
        [[nodiscard]] Result bindInt(int32_t index, int32_t value) const noexcept;

        /**
         * \brief Bind a 64-bit signed integer value. Internally calls sqlite3_bind_int64.
         * \param index Parameter index.
         * \param value Value to bind.
         * \return ...
         */
        [[nodiscard]] Result bindInt64(int32_t index, int64_t value) const noexcept;

        /**
         * \brief Bind a float value. Internally calls sqlite3_bind_double.
         * \param index Parameter index.
         * \param value Value to bind.
         * \return ...
         */
        [[nodiscard]] Result bindFloat(int32_t index, float value) const noexcept;

        /**
         * \brief Bind a double value. Internally calls sqlite3_bind_double.
         * \param index Parameter index.
         * \param value Value to bind.
         * \return ...
         */
        [[nodiscard]] Result bindDouble(int32_t index, double value) const noexcept;

        /**
         * \brief Bind null. Internally calls sqlite3_bind_null.
         * \param index Parameter index.
         * \return ...
         */
        [[nodiscard]] Result bindNull(int32_t index) const noexcept;

        /**
         * \brief Bind blob. Ownership of data is passed to this function. Internally calls sqlite3_bind_blob64.
         * \param index Parameter index.
         * \param data Data to bind.
         * \param size Size of data in bytes.
         * \param destructor Function to dispose of data.
         * \return ...
         */
        [[nodiscard]] Result
          bindBlob(int32_t index, const void* data, size_t size, void (*destructor)(void*)) const noexcept;

        /**
         * \brief Bind blob. Data is assumed to exist for the entire lifetime of this statement. Internally calls sqlite3_bind_blob64 with SQLITE_STATIC.
         * \param index Parameter index.
         * \param data Data to bind.
         * \param size Size of data in bytes.
         * \return ...
         */
        [[nodiscard]] Result bindStaticBlob(int32_t index, const void* data, size_t size) const noexcept;

        /**
         * \brief Bind blob. Makes a copy of data. Internally calls sqlite3_bind_blob64 with SQLITE_TRANSIENT.
         * \param index Parameter index.
         * \param data Data to bind.
         * \param size Size of data in bytes.
         * \return ...
         */
        [[nodiscard]] Result bindTransientBlob(int32_t index, const void* data, size_t size) const noexcept;

        /**
         * \brief Bind UTF8 text. Ownership of data is passed to this function. Internally calls sqlite3_bind_text.
         * \param index Parameter index.
         * \param data Text to bind.
         * \param size Size of text in bytes.
         * \param destructor Function to dispose of text.
         * \return ...
         */
        [[nodiscard]] Result
          bindText(int32_t index, const char* data, size_t size, void (*destructor)(void*)) const noexcept;

        /**
         * \brief Bind UTF8 text. Data is assumed to exist for the entire lifetime of this statement. Internally calls sqlite3_bind_text with SQLITE_STATIC.
         * \param index Parameter index.
         * \param data Text to bind.
         * \param size Size of text in bytes.
         * \return ...
         */
        [[nodiscard]] Result bindStaticText(int32_t index, const char* data, size_t size) const noexcept;

        /**
         * \brief Bind UTF8 text. Data is assumed to exist for the entire lifetime of this statement. Internally calls sqlite3_bind_text with SQLITE_STATIC.
         * \param index Parameter index.
         * \param data Text to bind.
         * \return ...
         */
        [[nodiscard]] Result bindStaticText(int32_t index, const std::string& data) const noexcept;

        /**
         * \brief Bind UTF8 text. Makes a copy of data. Internally calls sqlite3_bind_text with SQLITE_TRANSIENT.
         * \param index Parameter index.
         * \param data Text to bind.
         * \param size Size of text in bytes.
         * \return ...
         */
        [[nodiscard]] Result bindTransientText(int32_t index, const char* data, size_t size) const noexcept;

        /**
         * \brief Bind UTF8 text. Makes a copy of data. Internally calls sqlite3_bind_text with SQLITE_TRANSIENT.
         * \param index Parameter index.
         * \param data Text to bind.
         * \return ...
         */
        [[nodiscard]] Result bindTransientText(int32_t index, const std::string& data) const noexcept;

        /**
         * \brief Clear all bindings on this statement. Internally calls sqlite3_clear_bindings.
         * \return ...
         */
        [[nodiscard]] Result clearBindings() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Column getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get the type of a colummn of the current result row. Internally calls sqlite3_column_type.
         * \param index Column index.
         * \return Column type.
         */
        [[nodiscard]] Column::Type columnType(int32_t index) const noexcept;

        /**
         * \brief Retrieve a column of the current result row. Delegates to appropriate column method based on return type.
         * \tparam T Return type.
         * \param index Column index.
         * \return Result value.
         */
        template<typename T>
        [[nodiscard]] get_column_return_t<T> column(int32_t index) const
        {
            using param_t  = get_column_parameter_t<T>;
            using return_t = get_column_return_t<T>;

            // Get column value.
            param_t value;
            column(index, value);

            // When retrieving uint32 a bitcast is needed.
            if constexpr (std::same_as<param_t, int32_t>)
            {
                if constexpr (std::same_as<return_t, uint32_t>)
                    return std::bit_cast<uint32_t>(value);
                else
                    return static_cast<return_t>(value);
            }
            // When retrieving uint64 a bitcast is needed.
            else if constexpr (std::same_as<param_t, int64_t>)
            {
                if constexpr (std::same_as<return_t, uint64_t>)
                    return std::bit_cast<uint64_t>(value);
                else
                    return static_cast<return_t>(value);
            }
            else { return static_cast<return_t>(value); }
        }

        /**
         * \brief Retrieve an integer column of the current result row. Internally calls sqlite3_column_int.
         * \param index Column index.
         * \param value Result value.
         */
        void column(int32_t index, int32_t& value) const noexcept;

        /**
         * \brief Retrieve an integer column of the current result row. Internally calls sqlite3_column_int64.
         * \param index Column index.
         * \param value Result value.
         */
        void column(int32_t index, int64_t& value) const noexcept;

        /**
         * \brief Retrieve a float column of the current result row. Internally calls sqlite3_column_double.
         * \param index Column index.
         * \param value Result value.
         */
        void column(int32_t index, float& value) const noexcept;

        /**
         * \brief Retrieve a double column of the current result row. Internally calls sqlite3_column_double.
         * \param index Column index.
         * \param value Result value.
         */
        void column(int32_t index, double& value) const noexcept;

        /**
         * \brief Retrieve a text column of the current result row. Internally calls sqlite3_column_text.
         * \param index Column index.
         * \param value Result value.
         */
        void column(int32_t index, std::string& value) const;

        /**
         * \brief Retrieve a blob column of the current result row as an object. If size of blob != sizeof(T), an exception is thrown. Internally calls sqlite3_column_blob.
         * \param index Column index.
         * \param value Object to which data is written.
         */
        template<typename T>
        void column(const int32_t index, T& value) const
        {
            // Get raw result data.
            const auto [data, size] = columnBlob(index);
            if (sizeof(T) != size)
                throw CppqlError(std::format("Size of data ({}) does not match size of object ({})", size, sizeof(T)));
            std::memcpy(&value, data, sizeof(T));
        }

        /**
         * \brief Retrieve a blob column of the current result row as a vector. If size of blob is not a multiple of sizeof(T), an exception is thrown. Internally calls sqlite3_column_blob.
         * \param index Column index.
         * \param values Vector to which data is written. Any data inside of vector is discarded/overwritten.
         */
        template<typename T>
        void column(const int32_t index, std::vector<T>& values) const
        {
            // Get raw result data.
            const auto [data, size] = columnBlob(index);
            if (size == 0) return;

            // Copy to vector. If size of blob is not a multiple of sizeof(T), excess data is discarded.
            if (size % sizeof(T))
                throw CppqlError(
                  std::format("Size of data ({}) is not a multiple of size of object ({})", size, sizeof(T)));
            values.resize(size / sizeof(T));
            std::memcpy(values.data(), data, size);
        }

    private:
        /**
         * \brief Retrieve a blob column of the current result row. Data is still owned by sqlite.
         * \param index Column index.
         * \return Pointer to data and number of bytes.
         */
        [[nodiscard]] std::pair<const void*, size_t> columnBlob(int32_t index) const;

        /**
         * \brief Database this statement operates on.
         */
        Database* db = nullptr;

        /**
         * \brief Handle to sqlite statement object.
         */
        sqlite3_stmt* statement = nullptr;

        /**
         * \brief Sql code that is being executed by this statement.
         */
        std::string sql;

        /**
         * \brief Result of preparing statement.
         */
        std::optional<Result> prepareResult;
    };
}  // namespace sql