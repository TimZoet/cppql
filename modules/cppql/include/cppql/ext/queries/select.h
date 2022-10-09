#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <initializer_list>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/statement.h"
#include "cppql/table.h"
#include "cppql/ext/typed_table.h"

namespace sql::ext
{
    /**
     * \brief The Select class wraps a SELECT <cols> FROM <table> WHERE <expr> statement. The statement is executed when calling the begin() method. Iterating over the object will return all rows that matched the expression.
     * \tparam T Table type.
     * \tparam R Return type.
     * \tparam Indices 0-based indices of the columns to retrieve. Duplicate values and reordering are allowed.
     */
    template<typename T, typename R, size_t... Indices>
    class Select
    {
    public:
        /**
         * \brief Table type.
         */
        using table_t = T;

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
                // TODO: Catch errors.
                const auto res = stmt->step();
                code           = res.code;

                // if (code != SQLITE_ROW && code != SQLITE_DONE) throw std::runtime_error("");

                return *this;
            }

            iterator operator++(int)
            {
                const iterator retval = *this;
                ++*this;
                return retval;
            }

            // TODO: Use sqlite constant instead of hardcoded number.
            bool operator==(iterator) const { return code != 100; }

            bool operator!=(iterator other) const { return !(*this == other); }

            reference operator*() const
            {
                using return_tuple_t = std::tuple<get_column_return_t<col_t<Indices, T>>...>;

                auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    // std::tuple_element_t<Is...> gets the return type of each selected column.
                    // stmt->column(Is...) gets each column value.
                    return return_t(std::move(stmt->column<std::tuple_element_t<Is, return_tuple_t>>(Is))...);
                };

                // Call f with 0, 1, sizeof...(Indices) - 1.
                return f(std::make_index_sequence<sizeof...(Indices)>{});
            }
        };

        Select() = default;

        Select(StatementPtr statement, FilterExpressionPtr<T> filterExpression) :
            stmt(std::move(statement)), exp(std::move(filterExpression))
        {
        }

        Select(StatementPtr statement) : stmt(std::move(statement)) {}

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
            if (!stmt->reset()) throw std::runtime_error("");

            return iterator(*stmt);
        }

        iterator end() { return iterator(); }

        /**
         * \brief Bind parameters and return reference to this object.
         * \param bind If true, (re)bind parameters.
         * \return *this.
         */
        Select& operator()(const bool bind)
        {
            // Reset statement.
            if (!stmt->reset()) throw std::runtime_error("");

            // (Re)bind parameters.
            if (bind && exp) exp->bind(*stmt);

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
        FilterExpressionPtr<T> exp;
    };
}  // namespace sql::ext
