#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/typed_table.h"

namespace sql
{
    /**
     * \brief The Count class wraps a SELECT COUNT(*) FROM <table> WHERE <expr> statement. When invoked, it returns the number of rows that match the expression.
     * \tparam T Table type.
     */
    template<typename T>
    class Count
    {
    public:
        /**
         * \brief Table type.
         */
        using table_t = T;

        Count() = default;

        Count(StatementPtr statement, FilterExpressionPtr<T> filterExpression) :
            stmt(std::move(statement)), exp(std::move(filterExpression))
        {
        }

        Count(StatementPtr statement) : stmt(std::move(statement)) {}

        Count(const Count&) = delete;

        Count(Count&& other) noexcept : stmt(std::move(other.stmt)), exp(std::move(other.exp)) {}

        ~Count() = default;

        Count& operator=(const Count&) = delete;

        Count& operator=(Count&& other) noexcept
        {
            stmt = std::move(other.stmt);
            exp  = std::move(other.exp);
            return *this;
        }

        /**
         * \brief Run count statement. Does not bind parameters.
         * \return Number of rows.
         */
        row_id operator()() const
        {
            // Run statement.
            if (!stmt->step()) throw std::runtime_error("");

            // Reset statement.
            if (!stmt->reset()) throw std::runtime_error("");

            // Return number of rows.
            return stmt->column<row_id>(0);
        }

        /**
         * \brief Run count statement. Optionally binds parameters.
         * \param bind If true, (re)bind parameters.
         * \return Number of rows.
         */
        row_id operator()(const bool bind) const
        {
            // Bind parameters.
            if (bind && exp) exp->bind(*stmt);

            // Run statement.
            if (!stmt->step()) throw std::runtime_error("");

            // Retrieve number of rows.
            const auto rows = stmt->column<row_id>(0);

            // Reset statement.
            if (!stmt->reset()) throw std::runtime_error("");

            return rows;
        }

    private:
        /**
         * \brief Pointer to statement.
         */
        StatementPtr stmt;

        /**
         * \brief Pointer to filter expression.
         */
        FilterExpressionPtr<T> exp;
    };
}  // namespace sql
