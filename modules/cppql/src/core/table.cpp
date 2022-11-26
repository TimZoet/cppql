#include "cppql/core/table.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <format>

////////////////////////////////////////////////////////////////
// External includes.
////////////////////////////////////////////////////////////////

#include "sqlite3.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/database.h"
#include "cppql/core/statement.h"
#include "cppql/error/sqlite_error.h"

namespace sql
{
    Table::Table(Database* database, std::string tableName) : db(database), name(std::move(tableName)) {}

    void Table::commit()
    {
        requireNotCommitted();

        // Make sure all table and column settings are legal.
        validate();

        // Create and run CREATE TABLE statement.
        const auto sql  = generateSql();
        const auto stmt = Statement(*db, sql, true);
        const auto res  = stmt.step();
        if (!res) throw SqliteError(std::format("Failed to commit table."), res.code);

        committed = true;
    }

    std::string Table::generateSql() const
    {
        // Count number of primary key columns. If there is one, the constraint is added immediately after the column.
        // Otherwise, all primary keys are concatenated.
        const auto pkCount =
          std::ranges::count_if(columns.begin(), columns.end(), [](const auto& col) { return col->isPrimaryKey(); });

        // Concatenate columnns and primary key.
        std::string cols;
        std::string pks;
        std::string fks;
        auto        firstCol = true;
        auto        firstPk  = true;
        for (const auto& col : columns)
        {
            cols += std::format("{} {} {} {} {} {} {}",
                                firstCol ? "" : ",",
                                col->getName(),
                                toString(col->getType()),
                                col->isPrimaryKey() && pkCount == 1 ? "PRIMARY KEY" : "",
                                col->isAutoIncrement() ? "AUTOINCREMENT" : "",
                                col->isNotNull() ? "NOT NULL" : "",
                                col->getDefaultValue().empty() ? "" : "DEFAULT " + col->getDefaultValue());

            // If column is one of multiple primary keys, add to string.
            if (pkCount > 1 && col->isPrimaryKey())
            {
                pks += (firstPk ? "" : ",") + col->getName();
                firstPk = false;
            }

            // If column is foreign key, add to string.
            if (col->isForeignKey())
            {
                fks += std::format(", FOREIGN KEY({}) REFERENCES {}({})",
                                   col->getName(),
                                   col->getForeignKey()->getTable().getName(),
                                   col->getForeignKey()->getName());
            }

            firstCol = false;
        }

        // Format primary key constraint.
        const auto pk = pkCount > 1 ? std::format(", PRIMARY KEY ({})", pks) : "";

        // Format full statement.
        auto sql = std::format("CREATE TABLE {} ({} {} {});", name, cols, pk, fks);
        return sql;
    }

    void Table::requireCommitted() const
    {
        if (!committed) throw CppqlError("Table was not yet committed.");
    }

    void Table::requireNotCommitted() const
    {
        if (committed) throw CppqlError("Table was already committed.");
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Database& Table::getDatabase() const noexcept { return *db; }

    const std::string& Table::getName() const noexcept { return name; }

    bool Table::isCommitted() const noexcept { return committed; }

    size_t Table::getColumnCount() const noexcept { return columns.size(); }

    Column& Table::getColumn(const std::string& columnName) const { return *columns[columnMap.at(columnName)]; }

    Column& Table::getColumn(const size_t index) const
    {
        return *columns[index];
    }

    ////////////////////////////////////////////////////////////////
    // Columns.
    ////////////////////////////////////////////////////////////////

    Column& Table::createColumn(const std::string& columnName, Column::Type type)
    {
        requireNotCommitted();

        // Check for duplicate name.
        const auto [it, emplaced] = columnMap.try_emplace(columnName, columns.size());
        if (!emplaced)
            throw CppqlError(
              std::format("Failed to create column {}. A column with this name already exists.", columnName));

        // Create a new column.
        auto col = std::make_unique<Column>(this, static_cast<int32_t>(it->second), columnName, type);
        return *columns.emplace_back(std::move(col));
    }

    Column& Table::createColumn(const std::string& columnName, Column& foreignKey)
    {
        requireNotCommitted();

        // Check for duplicate name.
        const auto [it, emplaced] = columnMap.try_emplace(columnName, columns.size());
        if (!emplaced)
            throw CppqlError(
              std::format("Failed to create column {}. A column with this name already exists.", columnName));

        // Create a new column.
        auto col = std::make_unique<Column>(this, static_cast<int32_t>(it->second), columnName, foreignKey);
        return *columns.emplace_back(std::move(col));
    }

    ////////////////////////////////////////////////////////////////
    // Private methods.
    ////////////////////////////////////////////////////////////////

    void Table::validate()
    {
        const auto pkCount =
          std::ranges::count_if(columns.begin(), columns.end(), [](const auto& col) { return col->isPrimaryKey(); });
        const auto autoIncCount =
          std::ranges::count_if(columns.begin(), columns.end(), [](const auto& col) { return col->isAutoIncrement(); });

        // Autoincrement is not allowed when there are more than 1 primary keys.
        if (pkCount > 1 && autoIncCount != 0)
            throw CppqlError(std::format(
              "Failed to validate table {}. here is an auto increment column and more than 1 primary key columns.",
              name));
    }

    void Table::readFromDb(
      std::unordered_map<std::string, std::vector<std::tuple<std::string, std::string, std::string>>>& foreignKeys)
    {
        // Create statement to select all columns.
        const auto select = db->createStatement("SELECT * FROM " + getName() + ";", true);

        // Create columns.
        const auto columnCount = sqlite3_column_count(select.get());
        for (auto i = 0; i < columnCount; i++)
        {
            // Get column name.
            const auto columnName = std::string(sqlite3_column_name(select.get(), i));

            // Get column metadata.
            const char* dt      = nullptr;
            const char* coll    = nullptr;
            auto        notNull = 0, primaryKey = 0, autoInc = 0;
            const auto  res = sqlite3_table_column_metadata(
              db->get(), nullptr, getName().c_str(), columnName.c_str(), &dt, &coll, &notNull, &primaryKey, &autoInc);
            if (res != SQLITE_OK) throw SqliteError(std::format("Could not retrieve column metadata."), res);

            // Determine column type. If returned data type string is null, column has a null type.
            auto columnType = Column::Type::Null;
            if (dt) fromString(std::string(dt), columnType);

            // Create column.
            auto& col = createColumn(columnName, columnType);
            col.setNotNull(notNull);
            col.setPrimaryKey(primaryKey);
            col.setAutoIncrement(autoInc);
        }

        const auto fks = db->createStatement("PRAGMA foreign_key_list(" + getName() + ");", true);
        while (fks.step().code == SQLITE_ROW)
        {
            const auto tableName    = fks.column<std::string>(2);
            const auto columnName   = fks.column<std::string>(3);
            const auto fkColumnName = fks.column<std::string>(4);

            foreignKeys[getName()].emplace_back(tableName, columnName, fkColumnName);
        }
    }

    void Table::resolveForeignKeys(
      std::unordered_map<std::string, std::vector<std::tuple<std::string, std::string, std::string>>>& foreignKeys,
      std::unordered_map<std::string, TablePtr>&                                                       tables)
    {
        for (const auto& fk : foreignKeys[getName()])
        {
            const auto& tableName    = std::get<0>(fk);
            const auto& columnName   = std::get<1>(fk);
            const auto& fkColumnName = std::get<2>(fk);

            // Try to get table.
            auto it = tables.find(tableName);
            if (it == tables.end())
                throw CppqlError(std::format(
                  "Could not resolve foreign key {}. Referenced table {} does not exist.", columnName, tableName));
            const auto& fkTable = it->second;

            // Try to get column.
            if (auto it2 = fkTable->columnMap.find(fkColumnName); it2 == fkTable->columnMap.end())
                throw CppqlError(std::format(
                  "Could not resolve foreign key {}. Referenced column {} does not exist.", columnName, fkColumnName));
            auto& fkColumn = *fkTable->columns[fkTable->columnMap[fkColumnName]];

            // Set foreign key.
            columns[columnMap[columnName]]->setForeignKey(fkColumn);
        }

        committed = true;
    }
}  // namespace sql