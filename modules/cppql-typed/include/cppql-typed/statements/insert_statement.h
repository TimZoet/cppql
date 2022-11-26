#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <utility>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"
#include "cppql-core/error/sqlite_error.h"

namespace sql
{
    /**
     * \brief The InsertStatement class manages a prepared statement for inserting new rows into a table. It can be
     * constructed using a InsertQuery.
     * \tparam Cols Types of the columns to insert.
     */
    template<typename... Cols>
    class InsertStatement
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr size_t column_count = sizeof...(Cols);

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        InsertStatement() = delete;

        explicit InsertStatement(StatementPtr statement) : stmt(std::move(statement)) {}

        InsertStatement(const InsertStatement&) = delete;

        InsertStatement(InsertStatement&& other) noexcept = default;

        ~InsertStatement() noexcept = default;

        InsertStatement& operator=(const InsertStatement&) = delete;

        InsertStatement& operator=(InsertStatement&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Insert a row.
         * \tparam Cs Column types.
         * \param values Values.
         */
        template<bindable... Cs>
            requires(sizeof...(Cs) == column_count)
        void operator()(Cs&&... values)
        {
            // Only rebind if there are parameters.
            if constexpr (column_count > 0)
            {
                if (const auto res = stmt->bind(Statement::getFirstBindIndex(), std::forward<Cs>(values)...); !res)
                    throw SqliteError(std::format("Failed to bind parameters to insert statement."), res.code);
            }

            if (const auto res = stmt->step(); !res)
            {
                static_cast<void>(stmt->reset());
                throw SqliteError(std::format("Failed to step through insert statement."), res.code);
            }

            if (const auto res = stmt->reset(); !res)
                throw SqliteError(std::format("Failed to reset insert statement."), res.code);
        }

        /**
         * \brief Insert a row.
         * \tparam Cs Column types.
         * \param values Values.
         */
        template<bindable... Cs>
            requires(sizeof...(Cs) == column_count)
        void operator()(std::tuple<Cs...> values)
        {
            const auto unpack = [this]<std::size_t... Is>(std::index_sequence<Is...>, auto vals)
            {
                this->operator()(std::get<Is>(vals)...);
            };

            return unpack(std::index_sequence_for<Cs...>{}, std::move(values));
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Pointer to statement.
         */
        StatementPtr stmt;
    };
}  // namespace sql
