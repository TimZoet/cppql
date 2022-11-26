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

        Column(Table* t, int32_t columnIndex, std::string columnName, Column& fk);

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

        /**
         * \brief Returns whether column is (part of) the primary key.
         * \return True if part of the primary key.
         */
        [[nodiscard]] bool isPrimaryKey() const noexcept;

        /**
         * \brief Returns whether column is a foreign key.
         * \return True if column is a foreign key.
         */
        [[nodiscard]] bool isForeignKey() const noexcept;

        /**
         * \brief Returns whether column has a NOT NULL constraint.
         * \return True if it has a constraint.
         */
        [[nodiscard]] bool isNotNull() const noexcept;

        /**
         * \brief Returns whether column has AUTOINCREMENT enabled.
         * \return True if enabled.
         */
        [[nodiscard]] bool isAutoIncrement() const noexcept;

        /**
         * \brief Get other column referenced by this column.
         * \return Pointer to referenced column, or null if this column is not a foreign key.
         */
        [[nodiscard]] Column* getForeignKey() const noexcept;

        /**
         * \brief Get default value.
         * \return Default value.
         */
        [[nodiscard]] const std::string& getDefaultValue() const noexcept;

        ////////////////////////////////////////////////////////////////
        // Setters.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Make the column (part of) the primary key.
         * \param value If true, makes column primary key. If false, disables primary key.
         * \return *this.
         */
        Column& setPrimaryKey(bool value);

        Column& setNotNull(bool value);

        Column& setAutoIncrement(bool value);

        Column& setForeignKey(Column& column);

        /**
         * \brief Set default value for this column. Note that to properly set default string and blob values you must include single quotes, e.g. column.setDefaultValue("'abc'").
         * \param value Default value.
         * \return *this.
         */
        Column& setDefaultValue(std::string value);

        Column& setDefaultValue(int32_t value);

        Column& setDefaultValue(int64_t value);

        Column& setDefaultValue(float value);

        Column& setDefaultValue(double value);

    private:
        Table*      table;
        int32_t     index;
        std::string name;
        Type        type;
        Column*     foreignKey    = nullptr;
        bool        primaryKey    = false;
        bool        notNull       = false;
        bool        autoIncrement = false;
        std::string defaultValue;
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