#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"
#include "cppql-core/table.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/queries/select.h"

namespace sql
{
    // TODO: Constrain indices to be valid columns in table.
    // Also prevent duplicates.
    /**
     * \brief The Insert class wraps a INSERT INTO <table> VALUES <vals> statement. When invoked, it adds a row to the table.
     * \tparam T Table type.
     */
    template<typename T, size_t... Indices>
    class Insert
    {
    public:
        /**
         * \brief Table type.
         */
        using table_t = T;

        Insert() = default;

        Insert(StatementPtr statement) : stmt(std::move(statement)) {}

        Insert(const Insert&) = delete;

        Insert(Insert&& other) noexcept : stmt(std::move(other.stmt)) {}

        ~Insert() = default;

        Insert& operator=(const Insert&) = delete;

        Insert& operator=(Insert&& other) noexcept
        {
            stmt = std::move(other.stmt);
            return *this;
        }

        // TODO: Constrain types to bindable types? Or let lower level handle that?
        // TODO: Constrain types to match column types? Or leave user free to insert other stuff.

        /**
         * \brief Insert a row.
         * \tparam Cs Column types.
         * \param values Values.
         */
        template<typename... Cs>
        requires(sizeof...(Cs) == sizeof...(Indices)) void operator()(Cs... values)
        {
            if (!stmt->reset()) throw std::runtime_error("");
            // Only rebind if there are parameters.
            if constexpr (sizeof...(Indices) > 0)
            {
                if (!stmt->bind(Statement::getFirstBindIndex(), std::move(values)...)) throw std::runtime_error("");
            }
            if (!stmt->step()) throw std::runtime_error("");
        }

        /**
         * \brief Insert a row.
         * \tparam Cs Column types.
         * \param values Values.
         */
        template<typename... Cs>
        requires(sizeof...(Cs) == sizeof...(Indices)) void operator()(const std::tuple<Cs...>& values)
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
