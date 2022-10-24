#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"
#include "cppql-core/table.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/typed_table.h"

namespace sql
{
    /**
     * \brief The Delete class wraps a DELETE FROM <table> WHERE <expr> statement. When invoked, it removes all rows from the table that match the expression.
     * \tparam T Table type.
     */
    template<typename T>
    class Delete
    {
    public:
        /**
         * \brief Table type.
         */
        using table_t = T;

        Delete() = default;

        Delete(StatementPtr statement, FilterExpressionPtr<T> filterExpression) :
            stmt(std::move(statement)), exp(std::move(filterExpression))
        {
        }

        explicit Delete(StatementPtr statement) : stmt(std::move(statement)) {}

        Delete(const Delete&) = delete;

        Delete(Delete&& other) noexcept : stmt(std::move(other.stmt)), exp(std::move(other.exp)) {}

        ~Delete() = default;

        Delete& operator=(const Delete&) = delete;

        Delete& operator=(Delete&& other) noexcept
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
            if (!stmt->step()) throw std::runtime_error("");

            // Reset statement.
            if (!stmt->reset()) throw std::runtime_error("");
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
