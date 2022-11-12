#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"
#include "cppql-core/table.h"
#include "cppql-core/error/sqlite_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/typed_table.h"
#include "cppql-typed/expressions/bind_parameters.h"

namespace sql
{
    /**
     * \brief The Delete class wraps a DELETE FROM <table> WHERE <expr> statement. When invoked, it removes all rows from the table that match the expression.
     * \tparam T Table type.
     */
    template<typename T>
    class DeleteStatement
    {
    public:
        /**
         * \brief Table type.
         */
        using table_t = T;

        DeleteStatement() = default;

        DeleteStatement(StatementPtr statement, BaseFilterExpressionPtr filterExpression) :
            stmt(std::move(statement)), exp(std::move(filterExpression))
        {
        }

        explicit DeleteStatement(StatementPtr statement) : stmt(std::move(statement)) {}

        DeleteStatement(const DeleteStatement&) = delete;

        DeleteStatement(DeleteStatement&& other) noexcept : stmt(std::move(other.stmt)), exp(std::move(other.exp)) {}

        ~DeleteStatement() = default;

        DeleteStatement& operator=(const DeleteStatement&) = delete;

        DeleteStatement& operator=(DeleteStatement&& other) noexcept
        {
            stmt = std::move(other.stmt);
            exp  = std::move(other.exp);
            return *this;
        }

        /**
         * \brief Run delete statement. Does not bind parameters.
         */
        void operator()() const { return this->operator()(BindParameters::None); }

        /**
         * \brief Run delete statement. Optionally binds parameters.
         * \param bind Parameters to bind.
         */
        void operator()(const BindParameters bind) const
        {
            // Bind parameters.
            if (any(bind) && exp) exp->bind(*stmt, bind);

            // Run statement.
            if (const auto res = stmt->step(); !res)
                throw SqliteError(std::format("Failed to step through delete statement."), res.code);

            // Reset statement.
            if (const auto res = stmt->reset(); !res)
                throw SqliteError(std::format("Failed to reset delete statement."), res.code);
        }

    private:
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
