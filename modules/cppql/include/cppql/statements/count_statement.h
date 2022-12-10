#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/enums.h"
#include "cppql/core/statement.h"
#include "cppql/error/sqlite_error.h"
#include "cppql/typed/typed_table.h"

namespace sql
{
    /**
     * \brief The CountStatement class manages a prepared statement for retrieving the number of rows in a table. It
     * can be constructed using a CountQuery.
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
            if (any(b) && self.exp) self.exp->bind(*self.stmt, b);
            return std::forward<Self>(self);
        }

        /**
         * \brief Run count statement.
         * \return Number of rows.
         */
        row_id operator()() const
        {
            // Run statement.
            if (const auto res = stmt->step(); !res)
            {
                static_cast<void>(stmt->reset());
                throw SqliteError(std::format("Failed to step through count statement."), res.code);
            }

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
