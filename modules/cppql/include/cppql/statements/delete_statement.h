#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/statement.h"
#include "cppql/core/table.h"
#include "cppql/error/sqlite_error.h"
#include "cppql/typed/enums.h"
#include "cppql/typed/typed_table.h"

namespace sql
{
    /**
     * \brief The DeleteStatement class manages a prepared statement for removing rows from a table. It can be
     * constructed using a DeleteQuery.
     */
    class DeleteStatement
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        DeleteStatement() = delete;

        DeleteStatement(StatementPtr statement, BaseFilterExpressionPtr filterExpression) :
            stmt(std::move(statement)), exp(std::move(filterExpression))
        {
        }

        DeleteStatement(const DeleteStatement&) = delete;

        DeleteStatement(DeleteStatement&& other) noexcept = default;

        ~DeleteStatement() noexcept = default;

        DeleteStatement& operator=(const DeleteStatement&) = delete;

        DeleteStatement& operator=(DeleteStatement&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Bind parameters.
         * \tparam Self Self type.
         * \param self Self.
         * \param b Parameters to bind.
         */
        template<typename Self>
        auto&& bind(this Self&& self, const BindParameters b)
        {
            if (any(b) && self.exp) self.exp->bind(*self.stmt, b);
            return std::forward<Self>(self);
        }

        /**
         * \brief Run delete statement.
         */
        void operator()() const
        {
            // Run statement.
            if (const auto res = stmt->step(); !res)
            {
                static_cast<void>(stmt->reset());
                throw SqliteError(std::format("Failed to step through delete statement."), res.code);
            }

            // Reset statement.
            if (const auto res = stmt->reset(); !res)
                throw SqliteError(std::format("Failed to reset delete statement."), res.code);
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Pointer to statement.
         */
        StatementPtr stmt;

        /**
         * \brief Pointer to filter expression.
         */
        BaseFilterExpressionPtr exp;
    };
}  // namespace sql
