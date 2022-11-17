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

#include "cppql-typed/enums.h"
#include "cppql-typed/fwd.h"
#include "cppql-typed/typed_table.h"

namespace sql
{
    /**
     * \brief The SelectStatement class wraps a SELECT <cols> FROM <table> WHERE <expr> statement.
     * The statement is executed when calling the begin() method. Iterating over the
     * object will return all rows that matched the expression.
     * \tparam R Return type.
     * \tparam Cs Types of the columns to retrieve.
     */
    template<typename R, typename... Cs>
        requires(constructible_from<R, Cs...>)
    class SelectStatement
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

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
                auto res = stmt->step();
                code     = res.code;

                if (code != Result::sqlite_row && code != Result::sqlite_done)
                    throw SqliteError(std::format("Failed to step through select statement."), res.code);
                if (code == Result::sqlite_done)
                {
                    res = stmt->reset();
                    if (!res) throw SqliteError(std::format("Failed to reset select statement."), res.code);
                }

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

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        SelectStatement() = delete;

        SelectStatement(StatementPtr statement, BaseFilterExpressionPtr filterExpression) :
            stmt(std::move(statement)), exp(std::move(filterExpression))
        {
        }

        SelectStatement(const SelectStatement&) = delete;

        SelectStatement(SelectStatement&& other) noexcept = default;

        ~SelectStatement() noexcept = default;

        SelectStatement& operator=(const SelectStatement&) = delete;

        SelectStatement& operator=(SelectStatement&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        iterator begin()
        {
            // Reset statement.
            if (const auto res = stmt->reset(); !res)
                throw SqliteError(std::format("Failed to reset select statement."), res.code);

            return iterator(*stmt);
        }

        iterator end() { return iterator(); }

        /**
         * \brief Bind parameters.
         * \tparam Self Self type.
         * \param self Self.
         * \param b Parameters to bind.
         */
        template<typename Self>
        auto&& bind(this Self&& self, const BindParameters b)
        {
            if (any(b) && self.exp)
            {
                self.exp->bind(*self.stmt, b);
            }
            return std::forward<Self>(self);
        }

        //private:
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
