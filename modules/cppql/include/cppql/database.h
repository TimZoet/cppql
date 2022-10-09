#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/statement.h"
#include "cppql/table.h"

struct sqlite3;

namespace sql
{
    class Table;
    class Database;
    using DatabasePtr = std::unique_ptr<Database>;
    using row_id      = int64_t;

    class Database
    {
    public:
        friend class Statement;

        Database() = delete;

        Database(const Database&) = delete;

        Database(Database&&) = default;

        explicit Database(sqlite3* database);

        ~Database();

        Database& operator=(const Database&) = delete;

        Database& operator=(Database&&) = default;

        ////////////////////////////////////////////////////////////////
        // Static open/create methods.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new database.
         * \param file Path to database file.
         * \return Database.
         */
        [[nodiscard]] static DatabasePtr create(const std::filesystem::path& file);

        /**
         * \brief Open an existing database.
         * \param file Path to database file.
         * \return Database.
         */
        [[nodiscard]] static DatabasePtr open(const std::filesystem::path& file);

        /**
         * \brief Open an existing database or create one if it does not exist.
         * \param file Path to database file.
         * \return Database and boolean indicating whether a new database was created.
         */
        [[nodiscard]] static std::pair<DatabasePtr, bool> openOrCreate(const std::filesystem::path& file);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get handle to database connection.
         * \return Database connection handle.
         */
        [[nodiscard]] sqlite3* get() const noexcept;

        [[nodiscard]] Table& getTable(const std::string& name);

        [[nodiscard]] int64_t getLastInsertRowId() const noexcept;

        [[nodiscard]] std::string getErrorMessage() const;

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        Statement createStatement(std::string code, bool prepare);

        Table& createTable(const std::string& name);

        Table& registerTable(const std::string& name);

        void dropTable(const std::string& name);

        /**
         * \brief Execute the VACUUM command.
         */
        void vacuum();

    private:
        void initializeTables();

        /**
         * \brief Handle to sqlite database connection.
         */
        sqlite3* db = nullptr;

        std::unordered_map<std::string, TablePtr> tables;
    };
}  // namespace sql
