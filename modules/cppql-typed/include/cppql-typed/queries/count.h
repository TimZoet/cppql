#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"
#include "cppql-core/error/sqlite_error.h"

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

        explicit Count(StatementPtr statement) : stmt(std::move(statement)) {}

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
        row_id operator()() const { return this->operator()(BindParameters::None); }

        /**
         * \brief Run count statement. Optionally binds parameters.
         * \param bind Parameters to bind.
         * \return Number of rows.
         */
        row_id operator()(const BindParameters bind) const
        {
            // Bind parameters.
            if (any(bind) && exp) exp->bind(*stmt, bind);

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
