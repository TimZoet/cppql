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

#include "cppql/core/statement.h"
#include "cppql/core/table.h"

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
        /**
         * \brief Which close function to call on the database handle on destruction.
         */
        enum class Close
        {
            // Do not call any close function.
            Off,
            //  Call sqlite3_close.
            V1,
            // Call sqlite3_close_v2.
            V2
        };

        /**
         * \brief Whether or not to call sqlite3_shutdown on destruction.
         */
        enum class Shutdown
        {
            // Do not call sqlite3_shutdown.
            Off,
            // Call sqlite3_shutdown.
            On
        };

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
         * \brief Create a new database. Internally calls sqlite3_open_v2.
         * \param file Path to database file.
         * \param flags Additional flags to pass to sqlite3_open_v2.
         * \return Database.
         */
        [[nodiscard]] static DatabasePtr create(const std::filesystem::path& file, int32_t flags = 0);

        /**
         * \brief Open an existing database. Internally calls sqlite3_open_v2.
         * \param file Path to database file.
         * \param flags Additional flags to pass to sqlite3_open_v2.
         * \return Database.
         */
        [[nodiscard]] static DatabasePtr open(const std::filesystem::path& file, int32_t flags = 0);

        /**
         * \brief Open an existing database or create one if it does not exist. Internally calls sqlite3_open_v2.
         * \param file Path to database file.
         * \param flags Additional flags to pass to sqlite3_open_v2.
         * \return Database and boolean indicating whether a new database was created.
         */
        [[nodiscard]] static std::pair<DatabasePtr, bool> openOrCreate(const std::filesystem::path& file,
                                                                       int32_t                      flags = 0);

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get handle to database connection.
         * \return Database connection handle.
         */
        [[nodiscard]] sqlite3* get() const noexcept;

        /**
         * \brief Get close function that is called on destruction.
         * \return Close.
         */
        [[nodiscard]] Close getClose() const noexcept;

        /**
         * \brief Get shutdown function that is called on destruction.
         * \return Shutdown.
         */
        [[nodiscard]] Shutdown getShutdown() const noexcept;

        [[nodiscard]] Table& getTable(const std::string& name);

        [[nodiscard]] int64_t getLastInsertRowId() const noexcept;

        [[nodiscard]] std::string getErrorMessage() const;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Set close function that is called on destruction.
         * \param value Close.
         */
        void setClose(Close value) noexcept;

        /**
         * \brief Set shutdown function that is called on destruction.
         * \param value Shutdown.
         */
        void setShutdown(Shutdown value) noexcept;

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

        /**
         * \brief Close method.
         */
        Close close = Close::V1;

        /**
         * \brief Shutdown method.
         */
#ifdef CPPQL_SHUTDOWN_DEFAULT_OFF
        Shutdown shutdown = Shutdown::Off;
#else
        Shutdown shutdown = Shutdown::On;
#endif

        std::unordered_map<std::string, TablePtr> tables;
    };
}  // namespace sql
