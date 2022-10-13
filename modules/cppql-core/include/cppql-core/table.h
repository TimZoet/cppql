#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/column.h"

namespace sql
{
    class Database;
    class Table;
    using TablePtr = std::unique_ptr<Table>;

    class Table
    {
        friend class Database;

    public:
        Table() = delete;

        Table(Database* database, std::string tableName);

        /**
         * \brief Run the CREATE TABLE statement to commit this table to the database.
         */
        void commit();

        [[nodiscard]] std::string generateSql() const;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] Database& getDatabase() const noexcept;

        [[nodiscard]] const std::string& getName() const noexcept;

        [[nodiscard]] bool isCommitted() const noexcept;

        [[nodiscard]] size_t getColumnCount() const noexcept;

        [[nodiscard]] Column& getColumn(const std::string& columnName) const;

        [[nodiscard]] Column& getColumn(size_t index) const;

        ////////////////////////////////////////////////////////////////
        // Columns.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a new column.
         * \param columnName New column name.
         * \param type New column type.
         * \return Column.
         */
        Column& createColumn(const std::string& columnName, sql::Column::Type type);

        /**
         * \brief Create a new foreign key column.
         * \param columnName New column name.
         * \param foreignKey Column from another table.
         * \return Column.
         */
        Column& createColumn(const std::string& columnName, Column& foreignKey);

        template<typename T>
        Column& createColumn(const std::string& columnName)
        {
            return createColumn(columnName, toColumnType<T>());
        }

    private:
        void validate();

        void readFromDb(
          std::unordered_map<std::string, std::vector<std::tuple<std::string, std::string, std::string>>>& foreignKeys);

        void resolveForeignKeys(
          std::unordered_map<std::string, std::vector<std::tuple<std::string, std::string, std::string>>>& foreignKeys,
          std::unordered_map<std::string, TablePtr>&                                                       tables);

        /**
         * \brief Database this table is in.
         */
        Database* db;

        /**
         * \brief Table name.
         */
        std::string name;

        bool committed = false;


        std::vector<ColumnPtr>        columns;
        std::map<std::string, size_t> columnMap;
    };
}  // namespace sql