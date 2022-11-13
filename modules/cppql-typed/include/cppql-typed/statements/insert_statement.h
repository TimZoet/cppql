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
     * \brief The Insert class wraps a INSERT INTO <table> VALUES <vals> statement. When invoked, it adds a row to the table.
     * \tparam T Table type.
     * \tparam Indices 0-based indices of the columns to insert.
     */
    template<typename T, typename... Cols>
    class InsertStatement
    {
    public:
        /**
         * \brief Table type.
         */
        using table_t = T;

        static constexpr size_t column_count = sizeof...(Cols);

        InsertStatement() = default;

        explicit InsertStatement(StatementPtr statement) : stmt(std::move(statement)) {}

        InsertStatement(const InsertStatement&) = delete;

        InsertStatement(InsertStatement&& other) noexcept : stmt(std::move(other.stmt)) {}

        ~InsertStatement() = default;

        InsertStatement& operator=(const InsertStatement&) = delete;

        InsertStatement& operator=(InsertStatement&& other) noexcept
        {
            stmt = std::move(other.stmt);
            return *this;
        }

        /**
         * \brief Insert a row.
         * \tparam Cs Column types.
         * \param values Values.
         */
        template<bindable... Cs>
        requires(sizeof...(Cs) == column_count) void operator()(Cs... values)
        {
            if (const auto res = stmt->reset(); !res)
                throw SqliteError(std::format("Failed to reset insert statement."), res.code);

            // Only rebind if there are parameters.
            if constexpr (column_count > 0)
            {
                if (const auto res = stmt->bind(Statement::getFirstBindIndex(), std::move(values)...); !res)
                    throw SqliteError(std::format("Failed to bind parameters to insert statement."), res.code);
            }

            if (const auto res = stmt->step(); !res)
                throw SqliteError(std::format("Failed to step through insert statement."), res.code);
        }

        /**
         * \brief Insert a row.
         * \tparam Cs Column types.
         * \param values Values.
         */
        template<bindable... Cs>
        requires(sizeof...(Cs) == column_count) void operator()(const std::tuple<Cs...>& values)
        {
            // Call unpack function.
            this->operator()(values, std::index_sequence_for<Cs...>{});
        }

    private:
        template<typename Tuple, std::size_t... Is>
        void operator()(const Tuple& values, std::index_sequence<Is...>)
        {
            // Unpack tuple.
            this->operator()(std::get<Is>(values)...);
        }

        /**
         * \brief Pointer to statement.
         */
        StatementPtr stmt;
    };
}  // namespace sql
