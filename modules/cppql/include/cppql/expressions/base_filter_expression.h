#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/statement.h"
#include "cppql/typed/enums.h"

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
        // Bind.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Bind all parameters in this expression to the statement.
         * \param bind Parameters to bind.
         * \param stmt Statement object.
         */
        virtual void bind(Statement& stmt, BindParameters bind) const = 0;
    };

    using BaseFilterExpressionPtr = std::unique_ptr<BaseFilterExpression>;
}  // namespace sql
