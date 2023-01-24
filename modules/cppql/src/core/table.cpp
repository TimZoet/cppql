#include "cppql/core/table.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <algorithm>
#include <format>
#include <ranges>

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
        if (const auto res = stmt.step(); !res)
            throw SqliteError(std::format("Failed to commit table."), res.code, res.extendedCode);

        committed = true;
    }

    std::string Table::generateSql() const
    {
        // Count number of primary key columns. If there is one, the constraint is added immediately after the column.
        // Otherwise, all primary keys are concatenated.
        const auto pkCount =
          std::ranges::count_if(columns.begin(), columns.end(), [](const auto& col) { return col->isPrimaryKey(); });

        // Concatenate columnns.
        std::string cols;
        auto        firstCol = true;
        for (const auto& col : columns)
        {
            cols += std::format("{}{} {}", firstCol ? "" : ",", col->getName(), toString(col->getType()));
            if (col->isPrimaryKey() && pkCount == 1)
                cols += std::format(" PRIMARY KEY {} {}",
                                    toString(col->getPrimaryKeyConflictClause()),
                                    col->isAutoIncrement() ? "AUTOINCREMENT" : "");
            if (col->isNotNull()) cols += std::format(" NOT NULL {}", toString(col->getNotNullConflictClause()));
            if (col->isUnique()) cols += std::format(" UNIQUE {}", toString(col->getUniqueConflictClause()));
            if (col->hasCheck()) cols += std::format(" CHECK ({})", col->getCheck());
            if (col->hasDefaultValue()) cols += std::format(" DEFAULT ({})", col->getDefaultValue());
            if (col->hasCollate()) cols += std::format(" COLLATE {}", col->getCollate());
            if (col->isForeignKey())
            {
                cols += std::format(" REFERENCES {}({}){}{} {}",
                                    col->getForeignKey()->getTable().getName(),
                                    col->getForeignKey()->getName(),
                                    col->getForeignKeyDeleteAction() == ForeignKeyAction::NoAction ?
                                      "" :
                                      std::format(" ON DELETE {}", toString(col->getForeignKeyDeleteAction())),
                                    col->getForeignKeyUpdateAction() == ForeignKeyAction::NoAction ?
                                      "" :
                                      std::format(" ON UPDATE {}", toString(col->getForeignKeyUpdateAction())),
                                    toString(col->getForeignKeyDeferrable()));
            }

            firstCol = false;
        }

        // Format compound primary key.
        std::string pk;
        if (pkCount > 1)
        {
            const auto& first =
              (columns | std::views::filter([](const auto& c) { return c->isPrimaryKey(); }) | std::views::take(1))
                .front();
            pk = std::format(",PRIMARY KEY ({}) {}",
                             columns | std::views::filter([](const auto& c) { return c->isPrimaryKey(); }) |
                               std::views::transform([](const auto& c) { return c->getName(); }) |
                               std::views::join_with(',') | std::ranges::to<std::string>(),
                             toString(first->getPrimaryKeyConflictClause()));
        }

        std::string opts;
        if (options.withoutRowid) opts += "WITHOUT ROWID";
        if (options.strict) opts += options.withoutRowid ? ",STRICT" : "STRICT";

        // Format full statement.
        auto sql = std::format("CREATE TABLE {} ({} {}) {};", name, std::move(cols), std::move(pk), std::move(opts));
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

    bool Table::getWithoutRowid() const noexcept { return options.withoutRowid; }

    bool Table::getStrict() const noexcept { return options.strict; }

    bool Table::isCommitted() const noexcept { return committed; }

    size_t Table::getColumnCount() const noexcept { return columns.size(); }

    Column& Table::getColumn(const std::string& columnName) const { return *columns[columnMap.at(columnName)]; }

    Column& Table::getColumn(const size_t index) const { return *columns[index]; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    void Table::setWithoutRowid(const bool withoutRowid)
    {
        requireNotCommitted();
        options.withoutRowid = withoutRowid;
    }

    void Table::setStrict(const bool strict)
    {
        requireNotCommitted();
        options.strict = strict;
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

    ////////////////////////////////////////////////////////////////
    // Private methods.
    ////////////////////////////////////////////////////////////////

    void Table::validate()
    {
        const auto pkCount =
          std::ranges::count_if(columns.begin(), columns.end(), [](const auto& col) { return col->isPrimaryKey(); });

        // Autoincrement is not allowed when there are more than 1 primary keys.
        if (const auto autoIncCount = std::ranges::count_if(
              columns.begin(), columns.end(), [](const auto& col) { return col->isAutoIncrement(); });
            pkCount > 1 && autoIncCount != 0)
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
            if (const auto res = sqlite3_table_column_metadata(db->get(),
                                                               nullptr,
                                                               getName().c_str(),
                                                               columnName.c_str(),
                                                               &dt,
                                                               &coll,
                                                               &notNull,
                                                               &primaryKey,
                                                               &autoInc);
                res != SQLITE_OK)
                throw SqliteError(std::format("Could not retrieve column metadata."), res, SQLITE_OK);

            // Determine column type. If returned data type string is null, column has a null type.
            auto columnType = Column::Type::Null;
            if (dt) fromString(std::string(dt), columnType);

            // Create column.
            auto& col = createColumn(columnName, columnType);
            if (notNull) col.notNull();
            if (primaryKey) col.primaryKey(autoInc > 0, ConflictClause::Abort);
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
      const std::unordered_map<std::string, std::vector<std::tuple<std::string, std::string, std::string>>>&
                                                 foreignKeys,
      std::unordered_map<std::string, TablePtr>& tables)
    {
        if (const auto it = foreignKeys.find(getName()); it == foreignKeys.end()) return;

        for (const auto& fk : foreignKeys.at(getName()))
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
            columns[columnMap[columnName]]->foreignKey(fkColumn);
        }

        committed = true;
    }
}  // namespace sql