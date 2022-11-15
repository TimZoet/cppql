#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"
#include "cppql-core/error/sqlite_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/enums.h"
#include "cppql-typed/typed_table.h"

namespace sql
{
    /**
     * \brief The Count class wraps a SELECT COUNT(*) FROM <table> WHERE <expr> statement. When invoked, it returns the number of rows that match the expression.
     */
    class CountStatement
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        CountStatement() = delete;

        CountStatement(StatementPtr statement, BaseFilterExpressionPtr filterExpression) :
            stmt(std::move(statement)), exp(std::move(filterExpression))
        {
        }

        CountStatement(const CountStatement&) = delete;

        CountStatement(CountStatement&& other) noexcept = default;

        ~CountStatement() noexcept = default;

        CountStatement& operator=(const CountStatement&) = delete;

        CountStatement& operator=(CountStatement&& other) noexcept = default;

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
            if (any(b) && self.exp)  self.exp->bind(*self.stmt, b);
            return std::forward<Self>(self);
        }

        /**
         * \brief Run count statement. Optionally (re)binds parameters.
         * \param b Parameters to bind.
         * \return Number of rows.
         */
        row_id operator()(const BindParameters b) const
        {
            bind(b);

            // Run statement.
            if (const auto res = stmt->step(); !res)
                throw SqliteError(std::format("Failed to step through count statement."), res.code);

            // Retrieve number of rows.
            const auto rows = stmt->column<row_id>(0);

            // Reset statement.
            if (const auto res = stmt->reset(); !res)
                throw SqliteError(std::format("Failed to reset count statement."), res.code);

            return rows;
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
