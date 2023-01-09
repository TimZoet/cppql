#include "cppql/core/column.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <stdexcept>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/table.h"
#include "cppql/error/cppql_error.h"

namespace sql
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Column::Column(Table* t, const int32_t columnIndex, std::string columnName, const Type columnType) :
        table(t), index(columnIndex), name(std::move(columnName)), type(columnType)
    {
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Table& Column::getTable() const noexcept { return *table; }

    int32_t Column::getIndex() const noexcept { return index; }

    const std::string& Column::getName() const noexcept { return name; }

    Column::Type Column::getType() const noexcept { return type; }

    bool Column::isPrimaryKey() const noexcept { return primaryKeyConstraint.enabled; }

    ConflictClause Column::getPrimaryKeyConflictClause() const noexcept { return primaryKeyConstraint.conflict; }

    bool Column::isAutoIncrement() const noexcept { return primaryKeyConstraint.autoInc; }

    bool Column::isNotNull() const noexcept { return notNullConstraint.enabled; }

    ConflictClause Column::getNotNullConflictClause() const noexcept { return notNullConstraint.conflict; }

    bool Column::isUnique() const noexcept { return uniqueConstraint.enabled; }

    ConflictClause Column::getUniqueConflictClause() const noexcept { return uniqueConstraint.conflict; }

    bool Column::hasCheck() const noexcept { return !checkConstraint.empty(); }

    const std::string& Column::getCheck() const noexcept { return checkConstraint; }

    bool Column::hasDefaultValue() const noexcept { return !defaultVal.empty(); }

    const std::string& Column::getDefaultValue() const noexcept { return defaultVal; }

    bool Column::hasCollate() const noexcept { return !collationName.empty(); }

    const std::string& Column::getCollate() const noexcept { return collationName; }

    bool Column::isForeignKey() const noexcept { return foreignKeyConstraint.foreignKey != nullptr; }

    Column* Column::getForeignKey() const noexcept { return foreignKeyConstraint.foreignKey; }

    ForeignKeyAction Column::getForeignKeyDeleteAction() const noexcept { return foreignKeyConstraint.deleteAction; }

    ForeignKeyAction Column::getForeignKeyUpdateAction() const noexcept { return foreignKeyConstraint.updateAction; }

    Deferrable Column::getForeignKeyDeferrable() const noexcept { return foreignKeyConstraint.deferrable; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    Column& Column::primaryKey(const bool autoInc, const ConflictClause conflict)
    {
        table->requireNotCommitted();
        primaryKeyConstraint.enabled  = true;
        primaryKeyConstraint.conflict = conflict;
        primaryKeyConstraint.autoInc  = autoInc;
        return *this;
    }

    Column& Column::notNull(const ConflictClause conflict)
    {
        table->requireNotCommitted();
        notNullConstraint.enabled  = true;
        notNullConstraint.conflict = conflict;
        return *this;
    }

    Column& Column::unique(const ConflictClause conflict)
    {
        table->requireNotCommitted();
        uniqueConstraint.enabled  = true;
        uniqueConstraint.conflict = conflict;
        return *this;
    }

    Column& Column::check(std::string expression)
    {
        table->requireNotCommitted();
        checkConstraint = std::move(expression);
        return *this;
    }

    Column& Column::defaultValue(std::string value)
    {
        table->requireNotCommitted();
        defaultVal = std::move(value);
        return *this;
    }

    Column& Column::defaultValue(const int32_t value)
    {
        table->requireNotCommitted();
        defaultVal = std::format("{0:d}", value);
        return *this;
    }

    Column& Column::defaultValue(const int64_t value)
    {
        table->requireNotCommitted();
        defaultVal = std::format("{0:d}", value);
        return *this;
    }

    Column& Column::defaultValue(const float value)
    {
        table->requireNotCommitted();
        defaultVal = std::format("{0:f}", value);
        return *this;
    }

    Column& Column::defaultValue(const double value)
    {
        table->requireNotCommitted();
        defaultVal = std::format("{0:f}", value);
        return *this;
    }

    Column& Column::collate(std::string value)
    {
        table->requireNotCommitted();
        collationName = std::move(value);
        return *this;
    }

    Column& Column::foreignKey(Column&                column,
                               const ForeignKeyAction deleteAction,
                               const ForeignKeyAction updateAction,
                               const Deferrable       deferrable)
    {
        table->requireNotCommitted();
        if (&column.getTable() == table) throw CppqlError("Cannot set foreign key pointing to the same table.");
        foreignKeyConstraint.foreignKey   = &column;
        foreignKeyConstraint.deleteAction = deleteAction;
        foreignKeyConstraint.updateAction = updateAction;
        foreignKeyConstraint.deferrable   = deferrable;
        return *this;
    }

    std::string toString(const Column::Type type)
    {
        switch (type)
        {
        case Column::Type::Int: return "INTEGER";
        case Column::Type::Real: return "REAL";
        case Column::Type::Text: return "TEXT";
        case Column::Type::Blob: return "BLOB";
        case Column::Type::Null: return "NULL";
        }

        return "";
    }

    void fromString(const std::string& s, Column::Type& type)
    {
        if (s == "INTEGER")
            type = Column::Type::Int;
        else if (s == "REAL")
            type = Column::Type::Real;
        else if (s == "TEXT")
            type = Column::Type::Text;
        else if (s == "BLOB")
            type = Column::Type::Blob;
        else if (s == "NULL")
            type = Column::Type::Null;
        else
            throw CppqlError(std::format("Unknown type {}", s));
    }
}  // namespace sql