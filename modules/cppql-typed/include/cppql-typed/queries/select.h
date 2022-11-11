#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <iterator>
#include <tuple>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"
#include "cppql-core/error/sqlite_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/typed_table.h"
#include "cppql-typed/expressions/bind_parameters.h"

namespace sql
{
    /**
     * \brief The Select class wraps a SELECT <cols> FROM <table> WHERE <expr> statement.
     * The statement is executed when calling the begin() method. Iterating over the
     * object will return all rows that matched the expression.
     * \tparam R Return type.
     * \tparam Cs Types of the columns to retrieve.
     */
    template<typename R, typename... Cs>
    requires(constructible_from<R, Cs...>) class Select
    {
    public:
        /**
         * \brief Row return type.
         */
        using return_t = R;

        class iterator
        {
            Statement* stmt;
            int32_t    code = 100;

        public:
            using difference_type   = long;
            using value_type        = long;
            using pointer           = const return_t*;
            using reference         = return_t;
            using iterator_category = std::input_iterator_tag;

            iterator() : stmt(nullptr) {}

            explicit iterator(Statement& statement) : stmt(&statement) { this->operator++(); }

            iterator& operator++()
            {
                const auto res = stmt->step();
                code           = res.code;

                if (code != Result::sqlite_row && code != Result::sqlite_done)
                    throw SqliteError(std::format("Failed to step through select statement."), res.code);

                return *this;
            }

            iterator operator++(int)
            {
                const iterator retval = *this;
                ++*this;
                return retval;
            }

            bool operator==(iterator) const { return code != Result::sqlite_row; }

            bool operator!=(iterator other) const { return !(*this == other); }

            reference operator*() const
            {
                using return_tuple_t = std::tuple<get_column_return_t<Cs>...>;

                auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    // std::tuple_element_t<Is...> gets the return type of each selected column.
                    // stmt->column(Is...) gets each column value.
                    return return_t(std::move(stmt->column<std::tuple_element_t<Is, return_tuple_t>>(Is))...);
                };

                // Call f with 0, 1, sizeof...(Indices) - 1.
                return f(std::index_sequence_for<Cs...>());
            }
        };

        Select() = default;

        Select(StatementPtr statement, BaseFilterExpressionPtr filterExpression) :
            stmt(std::move(statement)), exp(std::move(filterExpression))
        {
        }

        explicit Select(StatementPtr statement) : stmt(std::move(statement)) {}

        Select(const Select&) = delete;

        Select(Select&& other) noexcept : stmt(std::move(other.stmt)), exp(std::move(other.exp)) {}

        ~Select() = default;

        Select& operator=(const Select&) = delete;

        Select& operator=(Select&& other) noexcept
        {
            stmt = std::move(other.stmt);
            exp  = std::move(other.exp);
            return *this;
        }

        iterator begin()
        {
            // Reset statement.
            if (const auto res = stmt->reset(); !res)
                throw SqliteError(std::format("Failed to reset select statement."), res.code);

            return iterator(*stmt);
        }

        iterator end() { return iterator(); }

        /**
         * \brief Bind parameters and return reference to this object.
         * \param bind Parameters to bind.
         * \return *this.
         */
        Select& operator()(const BindParameters bind)
        {
            // Reset statement.
            if (const auto res = stmt->reset(); !res)
                throw SqliteError(std::format("Failed to reset select statement."), res.code);

            // (Re)bind parameters.
            if (any(bind) && exp) exp->bind(*stmt, bind);

            return *this;
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
