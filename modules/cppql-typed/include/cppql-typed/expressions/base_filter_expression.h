#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/enums.h"

namespace sql
{
    class BaseFilterExpression
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        BaseFilterExpression() = default;

        BaseFilterExpression(const BaseFilterExpression&) = default;

        BaseFilterExpression(BaseFilterExpression&&) noexcept = default;

        virtual ~BaseFilterExpression() noexcept = default;

        BaseFilterExpression& operator=(const BaseFilterExpression&) = default;

        BaseFilterExpression& operator=(BaseFilterExpression&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        // TODO: To what extent do generateIndices and toString need to be a virtual base method? Seems like this class is only used virtually by statements to bind parameters.
        // Perhaps the inheritance for these filter expressions can be removed entirely. Instead, queries can construct a templated FilterExpression that holds a copy of the expressions.
        /**
         * \brief Generate indices for all parameters this expression holds.
         * \param idx Reference to counter that is incremented for each parameter.
         */
        virtual void generateIndices(int32_t& idx) = 0;
        
        /**
         * \brief Generate SQL.
         * \return SQL code.
         */
        [[nodiscard]] virtual std::string toString() = 0;

        /**
         * \brief Bind all parameters in this expression to the statement.
         * \param bind Parameters to bind.
         * \param stmt Statement object.
         */
        virtual void bind(Statement& stmt, BindParameters bind) const = 0;
    };

    using BaseFilterExpressionPtr = std::unique_ptr<BaseFilterExpression>;
}  // namespace sql
