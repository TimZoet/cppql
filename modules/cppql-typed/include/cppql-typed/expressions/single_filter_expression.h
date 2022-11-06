#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <optional>
#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/bind_parameters.h"

namespace sql
{
    ////////////////////////////////////////////////////////////////
    // SingleFilterExpression class.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief The FilterExpression class is the base class for all
     * classes used in conjunction with query objects to generate
     * the WHERE <expr> part of a statement (without the WHERE).
     * \tparam T Table type.
     */
    template<typename T>
    class SingleFilterExpression
    {
    public:
        using table_t = T;

        SingleFilterExpression() = default;

        SingleFilterExpression(const SingleFilterExpression&) = default;

        SingleFilterExpression(SingleFilterExpression&&) noexcept = default;

        virtual ~SingleFilterExpression() = default;

        SingleFilterExpression& operator=(const SingleFilterExpression&) = default;

        SingleFilterExpression& operator=(SingleFilterExpression&&) noexcept = default;

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

        [[nodiscard]] virtual std::unique_ptr<SingleFilterExpression<T>> clone() const = 0;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    using SingleFilterExpressionPtr = std::unique_ptr<SingleFilterExpression<T>>;

    template<typename T, typename Table>
    concept is_single_filter_expression = std::derived_from<T, SingleFilterExpression<Table>>;

    template<typename T, typename Table>
    concept is_single_filter_expression_or_none =
      is_single_filter_expression<T, Table> || std::same_as<std::remove_cvref_t<T>, std::nullopt_t>;

    template<typename T>
    concept _is_single_filter_expression = std::derived_from<T, SingleFilterExpression<typename T::table_t>>;
}  // namespace sql
