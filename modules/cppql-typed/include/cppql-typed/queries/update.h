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
     * \brief The Update class wraps an UPDATE <table> SET <cols> = <vals> statement. When invoked, it updates the rows in the table.
     * \tparam T Table type.
     * \tparam Indices 0-based indices of the columns to update.
     */
    template<typename T, size_t... Indices>
    class Update
    {
    public:
        /**
         * \brief Table type.
         */
        using table_t = T;

        Update() = default;

        Update(StatementPtr statement, FilterExpressionPtr<T> filterExpression) :
            stmt(std::move(statement)), exp(std::move(filterExpression))
        {
        }

        Update(const Update&) = delete;

        Update(Update&& other) noexcept : stmt(std::move(other.stmt)), exp(std::move(other.exp)) {}

        ~Update() = default;

        Update& operator=(const Update&) = delete;

        Update& operator=(Update&& other) noexcept
        {
            stmt = std::move(other.stmt);
            exp  = std::move(other.exp);
            return *this;
        }

        // TODO: Constrain types to bindable types? Or let lower level handle that?
        // TODO: Constrain types to match column types? Or leave user free to insert other stuff.

        /**
         * \brief Update table.
         * \tparam Cs Column types.
         * \param values Values.
         */
        template<typename... Cs>
        requires(sizeof...(Cs) == sizeof...(Indices)) void operator()(Cs&&... values)
        {
            this->operator()(false, std::forward<Cs>(values)...);
        }

        /**
         * \brief Update table.
         * \tparam Cs Column types.
         * \param bind Parameters to bind.
         * \param values Values.
         */
        template<typename... Cs>
        requires(sizeof...(Cs) == sizeof...(Indices)) void operator()(BindParameters bind, Cs... values)
        {
            if (!stmt->reset()) throw std::runtime_error("");

            // Bind value parameters.
            if (!stmt->bind(Statement::getFirstBindIndex(), std::move(values)...)) throw std::runtime_error("");

            // (Re)bind filter expression parameters.
            if (any(bind) && exp) exp->bind(*stmt, bind);

            if (!stmt->step()) throw std::runtime_error("");
        }

        /**
         * \brief Update table.
         * \tparam Cs Column types.
         * \param values Values.
         */
        template<typename... Cs>
        requires(sizeof...(Cs) == sizeof...(Indices)) void operator()(const std::tuple<Cs...>& values)
        {
            this->operator()(false, values);
        }

        /**
         * \brief Update table.
         * \tparam Cs Column types.
         * \param bind Parameters to bind.
         * \param values Values.
         */
        template<typename... Cs>
        requires(sizeof...(Cs) == sizeof...(Indices)) void operator()(BindParameters bind, const std::tuple<Cs...>& values)
        {
            // Call unpack function.
            this->operator()(bind, values, std::index_sequence_for<Cs...>{});
        }

    private:
        template<typename Tuple, std::size_t... Is>
        void operator()(BindParameters bind, const Tuple& values, std::index_sequence<Is...>)
        {
            // Unpack tuple.
            this->operator()(bind, std::get<Is>(values)...);
        }

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
