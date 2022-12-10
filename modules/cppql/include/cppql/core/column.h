#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <memory>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

//#include "common/static_assert.h"
//#include "common/type_traits.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/enums.h"

namespace sql
{
    class Column;
    using ColumnPtr = std::unique_ptr<Column>;
    class Table;

    class Column
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class Type
        {
            Text,
            Real,
            Blob,
            Int,
            Null
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Column() = delete;

        Column(Table* t, int32_t columnIndex, std::string columnName, Type columnType);

        Column(const Column&) = delete;

        Column(Column&&) = default;

        ~Column() = default;

        Column& operator=(const Column&) = delete;

        Column& operator=(Column&&) = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get table this column is in.
         * \return Table.
         */
        [[nodiscard]] Table& getTable() const noexcept;

        /**
         * \brief Get column index.
         * \return Column index.
         */
        [[nodiscard]] int32_t getIndex() const noexcept;

        /**
         * \brief Get column name.
         * \return Column name.
         */
        [[nodiscard]] const std::string& getName() const noexcept;

        /**
         * \brief Get column type.
         * \return Column type.
         */
        [[nodiscard]] Type getType() const noexcept;

        [[nodiscard]] bool isPrimaryKey() const noexcept;

        [[nodiscard]] ConflictClause getPrimaryKeyConflictClause() const noexcept;

        [[nodiscard]] bool isAutoIncrement() const noexcept;

        [[nodiscard]] bool isNotNull() const noexcept;

        [[nodiscard]] ConflictClause getNotNullConflictClause() const noexcept;

        [[nodiscard]] bool isUnique() const noexcept;

        [[nodiscard]] ConflictClause getUniqueConflictClause() const noexcept;

        [[nodiscard]] bool hasCheck() const noexcept;

        [[nodiscard]] const std::string& getCheck() const noexcept;

        [[nodiscard]] bool hasDefaultValue() const noexcept;

        [[nodiscard]] const std::string& getDefaultValue() const noexcept;

        [[nodiscard]] bool hasCollate() const noexcept;

        [[nodiscard]] const std::string& getCollate() const noexcept;

        [[nodiscard]] bool isForeignKey() const noexcept;

        [[nodiscard]] Column* getForeignKey() const noexcept;

        [[nodiscard]] ForeignKeyAction getForeignKeyDeleteAction() const noexcept;

        [[nodiscard]] ForeignKeyAction getForeignKeyUpdateAction() const noexcept;

        [[nodiscard]] Deferrable getForeignKeyDeferrable() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Add a primary key constraint. To create a multi-column primary key, add a constraint to each column.
         * \param autoInc Auto-increment.
         * \param conflict Conflict clause.
         * \return *this.
         */
        Column&
          primaryKey(bool autoInc = false, ConflictClause conflict = ConflictClause::Abort);

        /**
         * \brief Add a not null constraint.
         * \param conflict Conflict clause.
         * \return *this.
         */
        Column& notNull(ConflictClause conflict = ConflictClause::Abort);

        /**
         * \brief Add a unique constraint.
         * \param conflict Conflict clause.
         * \return *this.
         */
        Column& unique(ConflictClause conflict = ConflictClause::Abort);

        /**
         * \brief Add a check constraint.
         * \param expression Expression.
         * \return *this.
         */
        Column& check(std::string expression);

        /**
         * \brief Set default value. Note that to properly set default string and blob values you must include single quotes, e.g. column.setDefaultValue("'abc'").
         * \param value Default value.
         * \return *this.
         */
        Column& defaultValue(std::string value);

        /**
         * \brief Set default integer value.
         * \param value Integer value.
         * \return *this.
         */
        Column& defaultValue(int32_t value);

        /**
         * \brief Set default integer value.
         * \param value Integer value.
         * \return *this.
         */
        Column& defaultValue(int64_t value);

        /**
         * \brief Set default float value.
         * \param value Float value.
         * \return *this.
         */
        Column& defaultValue(float value);

        /**
         * \brief Set default float value.
         * \param value Float value.
         * \return *this.
         */
        Column& defaultValue(double value);

        /**
         * \brief Add collation sequence.
         * \param name Name.
         * \return *this.
         */
        Column& collate(std::string name);

        /**
         * \brief Add a foreign key constraint.
         * \param column Column.
         * \param deleteAction Delete action.
         * \param updateAction Update action.
         * \param deferrable Deferrable.
         * \return *this.
         */
        Column& foreignKey(Column&          column,
                           ForeignKeyAction deleteAction = ForeignKeyAction::NoAction,
                           ForeignKeyAction updateAction = ForeignKeyAction::NoAction,
                           Deferrable       deferrable   = Deferrable::InitiallyImmediate);

    private:
        Table*      table;
        int32_t     index;
        std::string name;
        Type        type;

        struct
        {
            bool           enabled  = false;
            ConflictClause conflict = ConflictClause::Abort;
            bool           autoInc  = false;
        } primaryKeyConstraint;

        struct
        {
            bool           enabled  = false;
            ConflictClause conflict = ConflictClause::Abort;
        } notNullConstraint;

        struct
        {
            bool           enabled  = false;
            ConflictClause conflict = ConflictClause::Abort;
        } uniqueConstraint;

        std::string checkConstraint;

        std::string defaultVal;

        std::string collationName;

        struct
        {
            Column*          foreignKey   = nullptr;
            ForeignKeyAction deleteAction = ForeignKeyAction::NoAction;
            ForeignKeyAction updateAction = ForeignKeyAction::NoAction;
            Deferrable       deferrable   = Deferrable::InitiallyImmediate;
        } foreignKeyConstraint;
    };

    ////////////////////////////////////////////////////////////////
    // Utlity functions.
    ////////////////////////////////////////////////////////////////

    std::string toString(Column::Type type);

    void fromString(const std::string& s, Column::Type& type);

    template<typename T>
    constexpr Column::Type toColumnType() noexcept
    {
        if constexpr (std::integral<T>)
            return Column::Type::Int;
        else if constexpr (std::floating_point<T>)
            return Column::Type::Real;
        else if constexpr (std::same_as<T, std::string>)
            return Column::Type::Text;
        else if constexpr (std::is_null_pointer_v<T>)
            return Column::Type::Null;
        else
            return Column::Type::Blob;
    }

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    // Generic value is returned directly.
    template<typename T>
    struct get_column_t
    {
        using parameter_t = T;
        using return_t    = T;
    };

    // Integers can only be retrieved as 32- or 64-bits signed integers.
    template<std::integral T>
    struct get_column_t<T>
    {
        using parameter_t = std::conditional_t<sizeof(T) <= sizeof(int32_t), int32_t, int64_t>;
        using return_t    = T;
    };

    // Floats and doubles can be retrieved and returned directly.
    template<std::floating_point T>
    struct get_column_t<T>
    {
        using parameter_t = T;
        using return_t    = T;
    };

    // Strings can be retrieved and returned directly.
    template<std::same_as<std::string> T>
    struct get_column_t<T>
    {
        using parameter_t = std::string;
        using return_t    = std::string;
    };

    // Vectors can be retrieved and returned directly.
    template<typename T>
    struct get_column_t<std::vector<T>>
    {
        using parameter_t = std::vector<T>;
        using return_t    = std::vector<T>;
    };

    /**
     * \brief Convert a type as it is passed to the column type list of a TypedTable to the type as it must be passed to the column methods of the Statement class.
     * \tparam T Column type.
     */
    template<typename T>
    using get_column_parameter_t = typename get_column_t<T>::parameter_t;

    /**
     * \brief Convert a type as it is passed to the column type list of a TypedTable to the type as it must be returned from the column methods of the Statement class.
     * \tparam T Column type.
     */
    template<typename T>
    using get_column_return_t = typename get_column_t<T>::return_t;
}  // namespace sql