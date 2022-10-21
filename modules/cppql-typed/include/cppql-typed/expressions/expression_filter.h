#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"

namespace sql
{
    /**
     * \brief Specifies whether to bind fixed and/or dynamic parameters.
     */
    enum class BindParameters
    {
        // Don't bind any parameters.
        None = 0,
        // Bind fixed parameters only.
        Fixed = 1,
        // Bind dynamic parameters only.
        Dynamic = 2,
        // Bind fixed and dynamic parameters.
        All = Fixed | Dynamic
    };

    ////////////////////////////////////////////////////////////////
    // FilterExpression class.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief The FilterExpression class is the base class for all
     * classes used in conjunction with query objects to generate
     * the WHERE <expr> part of a statement (without the WHERE).
     * \tparam T Table type.
     */
    template<typename T>
    class FilterExpression
    {
    public:
        using table_t = T;

        FilterExpression() = default;

        FilterExpression(const FilterExpression&) = default;

        FilterExpression(FilterExpression&&) noexcept = default;

        virtual ~FilterExpression() = default;

        FilterExpression& operator=(const FilterExpression&) = default;

        FilterExpression& operator=(FilterExpression&&) noexcept = default;

        /**
         * \brief Generate SQL.
         * \param table Table.
         * \param pIndex Reference to counter that is incremented for each parameter.
         * \return SQL code.
         */
        [[nodiscard]] virtual std::string toString(const Table& table, int32_t& pIndex) = 0;

        /**
         * \brief Bind all parameters in this expression to the statement.
         * \param bind Parameters to bind.
         * \param stmt Statement object.
         */
        virtual void bind(Statement& stmt, BindParameters bind) const = 0;

        [[nodiscard]] virtual std::unique_ptr<FilterExpression<T>> clone() const = 0;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    using FilterExpressionPtr = std::unique_ptr<FilterExpression<T>>;

    template<typename T, typename Table>
    concept is_filter_expression = std::derived_from<T, FilterExpression<Table>>;

    template<typename T>
    concept _is_filter_expression = std::derived_from<T, FilterExpression<typename T::table_t>>;
}  // namespace sql
