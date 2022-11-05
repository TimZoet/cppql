#include "cppql-core/column.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <stdexcept>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/table.h"
#include "cppql-core/error/cppql_error.h"

namespace sql
{
    ////////////////////////////////////////////////////////////////
    // Constructors.
    ////////////////////////////////////////////////////////////////

    Column::Column(Table* t, const int32_t columnIndex, std::string columnName, const Type columnType) :
        table(t), index(columnIndex), name(std::move(columnName)), type(columnType)
    {
    }

    Column::Column(Table* t, const int32_t columnIndex, std::string columnName, Column& fk) :
        table(t), index(columnIndex), name(std::move(columnName)), type(Type::Int)
    {
        setForeignKey(fk);
    }

    ////////////////////////////////////////////////////////////////
    // Getters.
    ////////////////////////////////////////////////////////////////

    Table& Column::getTable() const noexcept { return *table; }

    int32_t Column::getIndex() const noexcept { return index; }

    const std::string& Column::getName() const noexcept { return name; }

    Column::Type Column::getType() const noexcept { return type; }

    bool Column::isPrimaryKey() const noexcept { return primaryKey; }

    bool Column::isForeignKey() const noexcept { return foreignKey != nullptr; }

    bool Column::isNotNull() const noexcept { return notNull; }

    bool Column::isAutoIncrement() const noexcept { return autoIncrement; }

    ////////////////////////////////////////////////////////////////
    // Setters.
    ////////////////////////////////////////////////////////////////

    Column& Column::setPrimaryKey(const bool value)
    {
        table->requireNotCommitted();
        primaryKey = value;
        return *this;
    }

    Column& Column::setNotNull(const bool value)
    {
        table->requireNotCommitted();
        notNull = value;
        return *this;
    }

    Column& Column::setAutoIncrement(const bool value)
    {
        table->requireNotCommitted();
        autoIncrement = value;
        return *this;
    }

    Column* Column::getForeignKey() const noexcept { return foreignKey; }

    const std::string& Column::getDefaultValue() const noexcept { return defaultValue; }

    Column& Column::setForeignKey(Column& column)
    {
        table->requireNotCommitted();
        if (&column.getTable() == table) throw CppqlError("Cannot set foreign key pointing to the same table.");
        if (column.getType() != Type::Int) throw CppqlError("Cannot set foreign key pointing to non-integer column type.");
        this->foreignKey = &column;
        return *this;
    }

    Column& Column::setDefaultValue(std::string value)
    {
        table->requireNotCommitted();
        defaultValue = std::move(value);
        return *this;
    }

    Column& Column::setDefaultValue(int32_t value)
    {
        table->requireNotCommitted();
        defaultValue = std::format("{0:d}", value);
        return *this;
    }

    Column& Column::setDefaultValue(int64_t value)
    {
        table->requireNotCommitted();
        defaultValue = std::format("{0:d}", value);
        return *this;
    }

    Column& Column::setDefaultValue(float value)
    {
        table->requireNotCommitted();
        defaultValue = std::format("{0:f}", value);
        return *this;
    }

    Column& Column::setDefaultValue(double value)
    {
        table->requireNotCommitted();
        defaultValue = std::format("{0:f}", value);
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