#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <format>
#include <optional>
#include <stdexcept>
#include <tuple>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/column.h"
#include "cppql-core/database.h"
#include "cppql-core/table.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/type_traits.h"
#include "cppql-typed/expressions/expression_column.h"
#include "cppql-typed/expressions/expression_comparison.h"
#include "cppql-typed/expressions/expression_filter.h"
#include "cppql-typed/expressions/expression_like.h"
#include "cppql-typed/expressions/expression_limit.h"
#include "cppql-typed/expressions/expression_logical.h"
#include "cppql-typed/expressions/expression_order_by.h"

namespace sql
{
    inline std::string formatColumns(const Table& table, const std::initializer_list<size_t> columns)
    {
        std::string s;
        for (const auto index : columns) s += (s.empty() ? "" : ",") + table.getColumn(index).getName();
        return s;
    }

    ////////////////////////////////////////////////////////////////
    // Forward declarations.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    class Count;

    template<typename T>
    class Delete;

    template<typename T, size_t... Indices>
    class Insert;

    template<typename T, typename R, size_t... Indices>
    class Select;

    template<typename T, typename R, size_t... Indices>
    class SelectOne;

    template<typename T, size_t... Indices>
    class Update;

    ////////////////////////////////////////////////////////////////
    // TypedTable class.
    ////////////////////////////////////////////////////////////////

    template<typename C, typename... Cs>
    class TypedTable
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Type of this table.
         */
        using table_t = TypedTable<C, Cs...>;

        /**
         * \brief Column types as tuple.
         */
        using row_t = std::tuple<C, Cs...>;

        /**
         * \brief Number of columns.
         */
        static constexpr size_t column_count = 1 + sizeof...(Cs);

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        TypedTable() = default;

        explicit TypedTable(Table& t) : table(&t)
        {
            validate<0, C, Cs...>();
            if (column_count != table->getColumnCount()) throw std::runtime_error("Invalid number of columns");
        }

        TypedTable(const TypedTable&) = default;

        TypedTable(TypedTable&&) = default;

        ~TypedTable() = default;

        TypedTable& operator=(const TypedTable&) = default;

        TypedTable& operator=(TypedTable&&) = default;

        ////////////////////////////////////////////////////////////////
        // Columns.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get a column expression by index.
         * \tparam Index Column index.
         * \return Column expression.
         */
        template<size_t Index>
        requires(in_column_range<column_count, Index>) ColumnExpression<table_t, Index> col()
        const noexcept { return ColumnExpression<table_t, Index>(); }

        ////////////////////////////////////////////////////////////////
        // Insert.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create an INSERT INTO query. Creates a callable object that will insert the passed values for each specified column and default values for the rest.
         * \tparam Indices Indices of the columns for which to explicitly bind values.
         * \return Insert object.
         */
        template<size_t... Indices>
        [[nodiscard]] auto insert()
        {
            return insertImpl<Indices...>();
        }

        /**
         * \brief Create an INSERT INTO query. Creates a callable object that will insert the passed values.
         * \return Insert object.
         */
        [[nodiscard]] auto insert()
        {
            const auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>) { return insertImpl<Is...>(); };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        ////////////////////////////////////////////////////////////////
        // Delete.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a DELETE FROM query. Creates a callable object that will delete all rows.
         * \return Delete object.
         */
        [[nodiscard]] Delete<table_t> del() { return delImpl(nullptr, {}, {}, BindParameters::None); }

        /**
         * \brief Create a DELETE FROM query. Creates a callable object that will delete all rows.
         * \tparam O OrderByExpression type.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \return Delete object.
         */
        template<is_order_by_expression<table_t> O>
        [[nodiscard]] Delete<table_t> del(O&& orderByExpression, LimitExpression limitExpression)
        {
            return delImpl(nullptr, std::forward<O>(orderByExpression), limitExpression, BindParameters::None);
        }

        /**
         * \brief Create a DELETE FROM query. Creates a callable object that will delete all rows that match the filter expression.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter rows that are deleted.
         * \param bind Parameters to bind.
         * \return Delete object.
         */
        template<is_filter_expression<table_t> F>
        [[nodiscard]] Delete<table_t> del(F&& filterExpression, BindParameters bind)
        {
            return delImpl(std::make_unique<F>(std::forward<F>(filterExpression)), {}, {}, bind);
        }

        /**
         * \brief Create a DELETE FROM query. Creates a callable object that will delete all rows that match the filter expression.
         * \tparam F FilterExpression type.
         * \tparam O OrderByExpression type.
         * \param filterExpression Expression to filter rows that are deleted.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \param bind Parameters to bind.
         * \return Delete object.
         */
        template<is_filter_expression<table_t> F, is_order_by_expression<table_t> O>
        [[nodiscard]] Delete<table_t>
          del(F&& filterExpression, O&& orderByExpression, LimitExpression limitExpression, BindParameters bind)
        {
            return delImpl(std::make_unique<F>(std::forward<F>(filterExpression)),
                           std::forward<O>(orderByExpression),
                           limitExpression,
                           bind);
        }

        // TODO: For all Select(One/All) methods with en explicit return type, constrain R to object that is constructible from selected column types.
        // TODO: Replace boolean bind parameter by an enum.

        ////////////////////////////////////////////////////////////////
        // Select.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as tuples.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<is_filter_expression<table_t> F>
        [[nodiscard]] auto select(F&& filterExpression, BindParameters bind)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return select<Is...>(std::forward<F>(filterExpression), {}, bind);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as tuples.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param limitExpression Expression to limit results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<is_filter_expression<table_t> F>
        [[nodiscard]] auto select(F&& filterExpression, LimitExpression limitExpression, BindParameters bind)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return select<Is...>(std::forward<F>(filterExpression), limitExpression, bind);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as tuples.
         * \tparam F FilterExpression type.
         * \tparam O OrderByExpression type.
         * \param filterExpression Expression to filter results by.
         * \param orderByExpression Expression to order results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<is_filter_expression<table_t> F, is_order_by_expression<table_t> O>
        [[nodiscard]] auto select(F&& filterExpression, O&& orderByExpression, BindParameters bind)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return select<Is...>(std::forward<F>(filterExpression), std::forward<O>(orderByExpression), bind);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as tuples.
         * \tparam F FilterExpression type.
         * \tparam O OrderByExpression type.
         * \param filterExpression Expression to filter results by.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<is_filter_expression<table_t> F, is_order_by_expression<table_t> O>
        [[nodiscard]] auto
          select(F&& filterExpression, O&& orderByExpression, LimitExpression limitExpression, BindParameters bind)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return select<Is...>(
                  std::forward<F>(filterExpression), std::forward<O>(orderByExpression), limitExpression, bind);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as tuples.
         * \tparam Indices Indices of the columns to select.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<size_t... Indices, is_filter_expression<table_t> F>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto select(F&& filterExpression, BindParameters bind)
        {
            using return_t = std::tuple<get_column_return_t<col_t<Indices, table_t>>...>;
            return selectImpl<return_t, Indices...>(
              std::make_unique<F>(std::forward<F>(filterExpression)), {}, {}, bind);
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as tuples.
         * \tparam Indices Indices of the columns to select.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param limitExpression Expression to limit results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<size_t... Indices, is_filter_expression<table_t> F>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto select(F&& filterExpression, LimitExpression limitExpression, BindParameters bind)
        {
            using return_t = std::tuple<get_column_return_t<col_t<Indices, table_t>>...>;
            return selectImpl<return_t, Indices...>(
              std::make_unique<F>(std::forward<F>(filterExpression)), {}, limitExpression, bind);
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as tuples.
         * \tparam Indices Indices of the columns to select.
         * \tparam F FilterExpression type.
         * \tparam O OrderByExpression type.
         * \param filterExpression Expression to filter results by.
         * \param orderByExpression Expression to order results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<size_t... Indices, is_filter_expression<table_t> F, is_order_by_expression<table_t> O>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto select(F&& filterExpression, O&& orderByExpression, BindParameters bind)
        {
            using return_t = std::tuple<get_column_return_t<col_t<Indices, table_t>>...>;
            return selectImpl<return_t, Indices...>(
              std::make_unique<F>(std::forward<F>(filterExpression)), std::forward<O>(orderByExpression), {}, bind);
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as tuples.
         * \tparam Indices Indices of the columns to select.
         * \tparam F FilterExpression type.
         * \tparam O OrderByExpression type.
         * \param filterExpression Expression to filter results by.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<size_t... Indices, is_filter_expression<table_t> F, is_order_by_expression<table_t> O>
        requires(in_column_range<column_count, Indices...>) [[nodiscard]] auto select(F&&             filterExpression,
                                                                                      O&&             orderByExpression,
                                                                                      LimitExpression limitExpression,
                                                                                      BindParameters  bind)
        {
            using return_t = std::tuple<get_column_return_t<col_t<Indices, table_t>>...>;
            return selectImpl<return_t, Indices...>(std::make_unique<F>(std::forward<F>(filterExpression)),
                                                    std::forward<O>(orderByExpression),
                                                    limitExpression,
                                                    bind);
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<typename R, is_filter_expression<table_t> F>
        [[nodiscard]] auto select(F&& filterExpression, BindParameters bind)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return select<R, Is...>(std::forward<F>(filterExpression), bind);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param limitExpression Expression to limit results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<typename R, is_filter_expression<table_t> F>
        [[nodiscard]] auto select(F&& filterExpression, LimitExpression limitExpression, BindParameters bind)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return select<R, Is...>(std::forward<F>(filterExpression), limitExpression, bind);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam F FilterExpression type.
         * \tparam O OrderByExpression type.
         * \param filterExpression Expression to filter results by.
         * \param orderByExpression Expression to order results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<typename R, is_filter_expression<table_t> F, is_order_by_expression<table_t> O>
        [[nodiscard]] auto select(F&& filterExpression, O&& orderByExpression, BindParameters bind)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return select<R, Is...>(std::forward<F>(filterExpression), std::forward<O>(orderByExpression), bind);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam F FilterExpression type.
         * \tparam O OrderByExpression type.
         * \param filterExpression Expression to filter results by.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<typename R, is_filter_expression<table_t> F, is_order_by_expression<table_t> O>
        [[nodiscard]] auto
          select(F&& filterExpression, O&& orderByExpression, LimitExpression limitExpression, BindParameters bind)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return select<R, Is...>(
                  std::forward<F>(filterExpression), std::forward<O>(orderByExpression), limitExpression, bind);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam Indices Indices of the columns to select.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<typename R, size_t... Indices, is_filter_expression<table_t> F>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto select(F&& filterExpression, BindParameters bind)
        {
            return selectImpl<R, Indices...>(std::make_unique<F>(std::forward<F>(filterExpression)), {}, {}, bind);
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam Indices Indices of the columns to select.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param limitExpression Expression to limit results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<typename R, size_t... Indices, is_filter_expression<table_t> F>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto select(F&& filterExpression, LimitExpression limitExpression, BindParameters bind)
        {
            return selectImpl<R, Indices...>(
              std::make_unique<F>(std::forward<F>(filterExpression)), {}, limitExpression, bind);
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam Indices Indices of the columns to select.
         * \tparam F FilterExpression type.
         * \tparam O OrderByExpression type.
         * \param filterExpression Expression to filter results by.
         * \param orderByExpression Expression to order results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<typename R, size_t... Indices, is_filter_expression<table_t> F, is_order_by_expression<table_t> O>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto select(F&& filterExpression, O&& orderByExpression, BindParameters bind)
        {
            return selectImpl<R, Indices...>(
              std::make_unique<F>(std::forward<F>(filterExpression)), std::forward<O>(orderByExpression), {}, bind);
        }

        /**
         * \brief Create a SELECT query. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam Indices Indices of the columns to select.
         * \tparam F FilterExpression type.
         * \tparam O OrderByExpression type.
         * \param filterExpression Expression to filter results by.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \param bind Parameters to bind.
         * \return Select object.
         */
        template<typename R, size_t... Indices, is_filter_expression<table_t> F, is_order_by_expression<table_t> O>
        requires(in_column_range<column_count, Indices...>) [[nodiscard]] auto select(F&&             filterExpression,
                                                                                      O&&             orderByExpression,
                                                                                      LimitExpression limitExpression,
                                                                                      BindParameters  bind)
        {
            return selectImpl<R, Indices...>(std::make_unique<F>(std::forward<F>(filterExpression)),
                                             std::forward<O>(orderByExpression),
                                             limitExpression,
                                             bind);
        }

        ////////////////////////////////////////////////////////////////
        // SelectOne.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a SELECT query. Creates a callable object that returns a single row as a tuple. Will throw if there are 0 or more than 1 results.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param bind Parameters to bind.
         * \return SelectOne object.
         */
        template<is_filter_expression<table_t> F>
        [[nodiscard]] auto selectOne(F&& filterExpression, BindParameters bind)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return selectOne<Is...>(std::forward<F>(filterExpression), bind);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query. Creates a callable object that returns a single row as a tuple. Will throw if there are 0 or more than 1 results.
         * \tparam Indices Indices of the columns to select.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param bind Parameters to bind.
         * \return SelectOne object.
         */
        template<size_t... Indices, is_filter_expression<table_t> F>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto selectOne(F&& filterExpression, BindParameters bind)
        {
            using return_t = std::tuple<get_column_return_t<col_t<Indices, table_t>>...>;
            return selectOne<return_t, Indices...>(std::forward<F>(filterExpression), bind);
        }

        /**
         * \brief Create a SELECT query. Creates a callable object that returns a single row as an object of type R. Will throw if there are 0 or more than 1 results.
         * \tparam R Row return type.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param bind Parameters to bind.
         * \return SelectOne object.
         */
        template<typename R, is_filter_expression<table_t> F>
        [[nodiscard]] auto selectOne(F&& filterExpression, BindParameters bind)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return selectOne<R, Is...>(std::forward<F>(filterExpression), bind);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query. Creates a callable object that returns a single row as an object of type R. Will throw if there are 0 or more than 1 results.
         * \tparam R Row return type.
         * \tparam Indices Indices of the columns to select.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param bind Parameters to bind.
         * \return SelectOne object.
         */
        template<typename R, size_t... Indices, is_filter_expression<table_t> F>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto selectOne(F&& filterExpression, BindParameters bind)
        {
            return SelectOne<table_t, R, Indices...>(select<R, Indices...>(std::forward<F>(filterExpression), bind));
        }

        ////////////////////////////////////////////////////////////////
        // SelectAll.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as tuples.
         * \return Select object.
         */
        auto selectAll()
        {
            const auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>) { return selectAll<Is...>(); };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as tuples.
         * \param limitExpression Expression to limit results by.
         * \return Select object.
         */
        auto selectAll(LimitExpression limitExpression)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return selectAll<Is...>(limitExpression);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as tuples.
         * \tparam O OrderByExpression type.
         * \param orderByExpression Expression to order results by.
         * \return Select object.
         */
        template<is_order_by_expression<table_t> O>
        [[nodiscard]] auto selectAll(O&& orderByExpression)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return selectAll<Is...>(std::forward<O>(orderByExpression));
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as tuples.
         * \tparam O OrderByExpression type.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \return Select object.
         */
        template<is_order_by_expression<table_t> O>
        [[nodiscard]] auto selectAll(O&& orderByExpression, LimitExpression limitExpression)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return selectAll<Is...>(std::forward<O>(orderByExpression), limitExpression);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as tuples.
         * \tparam Indices Indices of the columns to select.
         * \return Select object.
         */
        template<size_t... Indices>
        requires(in_column_range<column_count, Indices...>) [[nodiscard]] auto selectAll()
        {
            using return_t = std::tuple<get_column_return_t<col_t<Indices, table_t>>...>;
            return selectImpl<return_t, Indices...>(nullptr, {}, {}, BindParameters::None);
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as tuples.
         * \tparam Indices Indices of the columns to select.
         * \param limitExpression Expression to limit results by.
         * \return Select object.
         */
        template<size_t... Indices>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto selectAll(LimitExpression limitExpression)
        {
            using return_t = std::tuple<get_column_return_t<col_t<Indices, table_t>>...>;
            return selectImpl<return_t, Indices...>(nullptr, {}, limitExpression, BindParameters::None);
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as tuples.
         * \tparam Indices Indices of the columns to select.
         * \tparam O OrderByExpression type.
         * \param orderByExpression Expression to order results by.
         * \return Select object.
         */
        template<size_t... Indices, is_order_by_expression<table_t> O>
        requires(in_column_range<column_count, Indices...>) [[nodiscard]] auto selectAll(O&& orderByExpression)
        {
            using return_t = std::tuple<get_column_return_t<col_t<Indices, table_t>>...>;
            return selectImpl<return_t, Indices...>(
              nullptr, std::forward<O>(orderByExpression), {}, BindParameters::None);
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as tuples.
         * \tparam Indices Indices of the columns to select.
         * \tparam O OrderByExpression type.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \return Select object.
         */
        template<size_t... Indices, is_order_by_expression<table_t> O>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto selectAll(O&& orderByExpression, LimitExpression limitExpression)
        {
            using return_t = std::tuple<get_column_return_t<col_t<Indices, table_t>>...>;
            return selectImpl<return_t, Indices...>(
              nullptr, std::forward<O>(orderByExpression), limitExpression, BindParameters::None);
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \return Select object.
         */
        template<typename R>
        auto selectAll()
        {
            const auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>) { return selectAll<R, Is...>(); };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \param limitExpression Expression to limit results by.
         * \return Select object.
         */
        template<typename R>
        auto selectAll(LimitExpression limitExpression)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return selectAll<R, Is...>(limitExpression);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam O OrderByExpression type.
         * \param orderByExpression Expression to order results by.
         * \return Select object.
         */
        template<typename R, is_order_by_expression<table_t> O>
        [[nodiscard]] auto selectAll(O&& orderByExpression)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return selectAll<R, Is...>(std::forward<O>(orderByExpression));
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam O OrderByExpression type.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \return Select object.
         */
        template<typename R, is_order_by_expression<table_t> O>
        [[nodiscard]] auto selectAll(O&& orderByExpression, LimitExpression limitExpression)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return selectAll<R, Is...>(std::forward<O>(orderByExpression), limitExpression);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam Indices Indices of the columns to select.
         * \return Select object.
         */
        template<typename R, size_t... Indices>
        requires(in_column_range<column_count, Indices...>) [[nodiscard]] auto selectAll()
        {
            return selectImpl<R, Indices...>(nullptr, {}, {}, BindParameters::None);
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam Indices Indices of the columns to select.
         * \param limitExpression Expression to limit results by.
         * \return Select object.
         */
        template<typename R, size_t... Indices>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto selectAll(LimitExpression limitExpression)
        {
            return selectImpl<R, Indices...>(nullptr, {}, limitExpression, BindParameters::None);
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam Indices Indices of the columns to select.
         * \tparam O OrderByExpression type.
         * \param orderByExpression Expression to order results by.
         * \return Select object.
         */
        template<typename R, size_t... Indices, is_order_by_expression<table_t> O>
        requires(in_column_range<column_count, Indices...>) [[nodiscard]] auto selectAll(O&& orderByExpression)
        {
            return selectImpl<R, Indices...>(nullptr, std::forward<O>(orderByExpression), {}, BindParameters::None);
        }

        /**
         * \brief Create a SELECT query without filter expression to select all rows. Creates an iterable object that returns rows as objects of type R.
         * \tparam R Row return type.
         * \tparam Indices Indices of the columns to select.
         * \tparam O OrderByExpression type.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \return Select object.
         */
        template<typename R, size_t... Indices, is_order_by_expression<table_t> O>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto selectAll(O&& orderByExpression, LimitExpression limitExpression)
        {
            return selectImpl<R, Indices...>(
              nullptr, std::forward<O>(orderByExpression), limitExpression, BindParameters::None);
        }

        ////////////////////////////////////////////////////////////////
        // Count.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a COUNT query. Creates a callable object that returns the number of rows that match the filter expression.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param bind Parameters to bind.
         * \return Count object.
         */
        template<is_filter_expression<table_t> F>
        [[nodiscard]] auto count(F&& filterExpression, BindParameters bind)
        {
            return countImpl(std::make_unique<F>(std::forward<F>(filterExpression)), bind);
        }

        /**
         * \brief Create a COUNT query. Creates a callable object that returns the total number of rows.
         * \return Count object.
         */
        [[nodiscard]] auto countAll() { return countImpl(nullptr, BindParameters::None); }

        ////////////////////////////////////////////////////////////////
        // Update.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create an UPDATE query. Creates a callable object that will update the specified columns of matching rows with the passed values.
         * \return Update object.
         */
        [[nodiscard]] auto update()
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>) { return update<Is...>(); };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create an UPDATE query. Creates a callable object that will update the specified columns of matching rows with the passed values.
         * \tparam O OrderByExpression type.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \return Update object.
         */
        template<is_order_by_expression<table_t> O>
        [[nodiscard]] auto update(O&& orderByExpression, LimitExpression limitExpression)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return update<Is...>(std::forward<O>(orderByExpression), limitExpression);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create an UPDATE query. Creates a callable object that will update the specified columns of matching rows with the passed values.
         * \tparam Indices Indices of the columns to update.
         * \return Update object.
         */
        template<size_t... Indices>
        requires(in_column_range<column_count, Indices...>) [[nodiscard]] auto update()
        {
            return updateImpl<Indices...>(nullptr, {}, {}, BindParameters::None);
        }

        /**
         * \brief Create an UPDATE query. Creates a callable object that will update the specified columns of matching rows with the passed values.
         * \tparam Indices Indices of the columns to update.
         * \tparam O OrderByExpression type.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \return Update object.
         */
        template<size_t... Indices, is_order_by_expression<table_t> O>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto update(O&& orderByExpression, LimitExpression limitExpression)
        {
            return updateImpl<Indices...>(
              nullptr, std::forward<O>(orderByExpression), limitExpression, BindParameters::None);
        }

        /**
         * \brief Create an UPDATE query. Creates a callable object that will update the specified columns of matching rows with the passed values.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter the rows that will be updated.
         * \param bind Parameters to bind.
         * \return Update object.
         */
        template<is_filter_expression<table_t> F>
        [[nodiscard]] auto update(F&& filterExpression, BindParameters bind)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return update<Is...>(std::forward<F>(filterExpression), bind);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create an UPDATE query. Creates a callable object that will update the specified columns of matching rows with the passed values.
         * \tparam F FilterExpression type.
         * \tparam O OrderByExpression type.
         * \param filterExpression Expression to filter the rows that will be updated.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \param bind Parameters to bind.
         * \return Update object.
         */
        template<is_filter_expression<table_t> F, is_order_by_expression<table_t> O>
        [[nodiscard]] auto
          update(F&& filterExpression, O&& orderByExpression, LimitExpression limitExpression, BindParameters bind)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                return update<Is...>(
                  std::forward<F>(filterExpression), std::forward<O>(orderByExpression), limitExpression, bind);
            };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        /**
         * \brief Create an UPDATE query. Creates a callable object that will update the specified columns of matching rows with the passed values.
         * \tparam Indices Indices of the columns to update.
         * \tparam F FilterExpression type.
         * \param filterExpression Expression to filter the rows that will be updated.
         * \param bind Parameters to bind.
         * \return Update object.
         */
        template<size_t... Indices, is_filter_expression<table_t> F>
        requires(in_column_range<column_count, Indices...>)
          [[nodiscard]] auto update(F&& filterExpression, BindParameters bind)
        {
            return updateImpl<Indices...>(std::make_unique<F>(std::forward<F>(filterExpression)), {}, {}, bind);
        }

        /**
         * \brief Create an UPDATE query. Creates a callable object that will update the specified columns of matching rows with the passed values.
         * \tparam Indices Indices of the columns to update.
         * \tparam F FilterExpression type.
         * \tparam O OrderByExpression type.
         * \param filterExpression Expression to filter the rows that will be updated.
         * \param orderByExpression Expression to order results by.
         * \param limitExpression Expression to limit results by.
         * \param bind Parameters to bind.
         * \return Update object.
         */
        template<size_t... Indices, is_filter_expression<table_t> F, is_order_by_expression<table_t> O>
        requires(in_column_range<column_count, Indices...>) [[nodiscard]] auto update(F&&             filterExpression,
                                                                                      O&&             orderByExpression,
                                                                                      LimitExpression limitExpression,
                                                                                      BindParameters  bind)
        {
            return updateImpl<Indices...>(std::make_unique<F>(std::forward<F>(filterExpression)),
                                          std::forward<O>(orderByExpression),
                                          limitExpression,
                                          bind);
        }

    private:
        template<size_t Index, typename U, typename... Us>
        void validate()
        {
            if (table->getColumn(Index).getType() != toColumnType<U>()) throw std::runtime_error("Invalid type");

            // Recurse.
            if constexpr (sizeof...(Us) > 0) validate<Index + 1, Us...>();
        }

        template<size_t... Indices>
        [[nodiscard]] auto insertImpl()
        {
            std::string sql;

            // If there are no indices, create statement to insert all default values.
            if constexpr (sizeof...(Indices) == 0)
            {
                sql = std::format("INSERT INTO {0} DEFAULT VALUES", table->getName());
            }
            // Otherwise, construct an INSERT INTO <table> (<cols>) VALUES <vals> query.
            else
            {
                // Generate format arguments.
                std::string colNames = formatColumns(*table, {Indices...});
                std::string cs       = "?";
                for (size_t i = 1; i < sizeof...(Indices); i++) cs += ",?";

                // Format SQL statement.
                sql = std::format(
                  "INSERT INTO {0} ({1}) VALUES ({2});", table->getName(), std::move(colNames), std::move(cs));
            }

            // Create and prepare statement.
            auto stmt = std::make_unique<Statement>(table->getDatabase(), std::move(sql), true);
            if (!stmt->isPrepared()) throw std::runtime_error("");

            // Construct Insert.
            return Insert<table_t, Indices...>(std::move(stmt));
        }

        [[nodiscard]] auto delImpl(FilterExpressionPtr<table_t>                    fExpr,
                                   const std::optional<OrderByExpression<table_t>> oExpr,
                                   const std::optional<LimitExpression>            lExpr,
                                   const BindParameters                            bind)
        {
            // Generate format arguments.
            auto        index   = 0;
            std::string e       = fExpr ? "WHERE " + fExpr->toString(*table, index) : "";
            std::string orderBy = oExpr ? oExpr->toString(*table) : "";
            std::string limit   = lExpr ? lExpr->toString() : "";

            // Format SQL statement.
            auto sql = std::format(
              "DELETE FROM {0} {1} {2} {3};", table->getName(), std::move(e), std::move(orderBy), std::move(limit));

            // Create and prepare statement.
            auto stmt = std::make_unique<Statement>(table->getDatabase(), std::move(sql), true);
            if (!stmt->isPrepared()) throw std::runtime_error("");

            // Bind parameters.
            if (fExpr && any(bind)) fExpr->bind(*stmt, bind);

            // Construct Delete.
            return Delete<table_t>(std::move(stmt), std::move(fExpr));
        }

        template<typename R, size_t... Indices>
        [[nodiscard]] auto selectImpl(FilterExpressionPtr<table_t>                    fExpr,
                                      const std::optional<OrderByExpression<table_t>> oExpr,
                                      const std::optional<LimitExpression>            lExpr,
                                      BindParameters                                  bind)
        {
            // Generate format arguments.
            auto        index   = 0;
            std::string cs      = formatColumns(*table, {Indices...});
            std::string e       = fExpr ? "WHERE " + fExpr->toString(*table, index) : "";
            std::string orderBy = oExpr ? oExpr->toString(*table) : "";
            std::string limit   = lExpr ? lExpr->toString() : "";

            // Format SQL statement.
            auto sql = std::format("SELECT {0} FROM {1} {2} {3} {4};",
                                   std::move(cs),
                                   table->getName(),
                                   std::move(e),
                                   std::move(orderBy),
                                   std::move(limit));

            // Create and prepare statement.
            auto stmt = std::make_unique<Statement>(table->getDatabase(), std::move(sql), true);
            if (!stmt->isPrepared()) throw std::runtime_error("");

            // Bind parameters.
            if (fExpr && any(bind)) fExpr->bind(*stmt, bind);

            // Construct Select.
            return Select<table_t, R, Indices...>(std::move(stmt), std::move(fExpr));
        }

        [[nodiscard]] auto countImpl(FilterExpressionPtr<table_t> fExpr, BindParameters bind)
        {
            std::string sql;

            // Format SQL statement.
            if (fExpr)
            {
                auto index = 0;
                sql =
                  std::format("SELECT COUNT(*) FROM {0} WHERE {1};", table->getName(), fExpr->toString(*table, index));
            }
            else
                sql = std::format("SELECT COUNT(*) FROM {0};", table->getName());

            // Create and prepare statement.
            auto stmt = std::make_unique<Statement>(table->getDatabase(), std::move(sql), true);
            if (!stmt->isPrepared()) throw std::runtime_error("");

            // Bind parameters.
            if (fExpr && any(bind)) fExpr->bind(*stmt, bind);

            // Construct Delete.
            return Count<table_t>(std::move(stmt), std::move(fExpr));
        }

        template<size_t... Indices>
        [[nodiscard]] auto updateImpl(FilterExpressionPtr<table_t>                    fExpr,
                                      const std::optional<OrderByExpression<table_t>> oExpr,
                                      const std::optional<LimitExpression>            lExpr,
                                      BindParameters                                  bind)
        {
            // Construct an UPDATE <table> SET (<cols>) = (<vals>) WHERE <filter> ORDER BY <expr> LIMIT <expr> OFFSET <expr> query.

            // Generate format arguments.
            auto        index    = static_cast<int32_t>(sizeof...(Indices));
            std::string e        = fExpr ? "WHERE " + fExpr->toString(*table, index) : "";
            std::string orderBy  = oExpr ? oExpr->toString(*table) : "";
            std::string limit    = lExpr ? lExpr->toString() : "";
            std::string colNames = formatColumns(*table, {Indices...});
            std::string cs       = "?1";
            for (size_t i = 1; i < sizeof...(Indices); i++) cs += std::format(",?{0}", i + 1);

            // Format SQL statement.
            std::string sql = std::format("UPDATE {0} SET ({1}) = ({2}) {3} {4} {5};",
                                          table->getName(),
                                          std::move(colNames),
                                          std::move(cs),
                                          std::move(e),
                                          std::move(orderBy),
                                          std::move(limit));

            // Create and prepare statement.
            auto stmt = std::make_unique<Statement>(table->getDatabase(), std::move(sql), true);
            if (!stmt->isPrepared()) throw std::runtime_error("");

            // Bind parameters.
            if (fExpr && any(bind)) fExpr->bind(*stmt, bind);

            // Construct Update.
            return Update<table_t, Indices...>(std::move(stmt), std::move(fExpr));
        }

        Table* table;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename...>
    struct tuple_to_table
    {
    };

    template<template<typename, typename...> class Tuple, typename... Cs>
    struct tuple_to_table<Tuple<Cs...>>
    {
        using type = TypedTable<Cs...>;
    };

    /**
     * \brief Convert a std::tuple to a TypedTable.
     * \tparam T std::tuple type.
     */
    template<typename T>
    using tuple_to_table_t = typename tuple_to_table<T>::type;

    /**
     * \brief Get the return type of a call to the insert method of the given table class with the given parameters.
     * \tparam T Table.
     */
    template<typename T>
    using insert_t = decltype(std::declval<T>().insert());

    /**
     * \brief Get the return type of a call to the update method of the given table class with the given parameters.
     * \tparam T Table.
     */
    template<typename T>
    using update_t = decltype(std::declval<T>().update());

    /**
     * \brief Get the return type of a call to the select method of the given table class with the given parameters.
     * \tparam T Table.
     * \tparam R Row return type.
     * \tparam Indices Indices of the columns to select.
     */
    template<typename T, typename R, size_t... Indices>
    using select_t = decltype(std::declval<T>().template select<R, Indices...>(std::declval<LogicalExpression<T>>(),
                                                                               std::declval<BindParameters>()));

    /**
     * \brief Get the return type of a call to the selectOne method of the given table class with the given parameters.
     * \tparam T Table.
     * \tparam R Row return type.
     * \tparam Indices Indices of the columns to select.
     */
    template<typename T, typename R, size_t... Indices>
    using select_one_t = decltype(std::declval<T>().template selectOne<R, Indices...>(
      std::declval<LogicalExpression<T>>(), std::declval<BindParameters>()));

    /**
     * \brief Get the return type of a call to the selectOne method of the given table class with the given parameters.
     * \tparam T Table.
     * \tparam R Row return type.
     */
    template<typename T, typename R>
    using select_one2_t = decltype(std::declval<T>().template selectOne<R>(std::declval<LogicalExpression<T>>(),
                                                                           std::declval<BindParameters>()));

    /**
     * \brief Get the return type of a call to the selectOne method of the given table class with the given parameters.
     * \tparam T Table.
     */
    template<typename T>
    using select_one3_t =
      decltype(std::declval<T>().selectOne(std::declval<LogicalExpression<T>>(), std::declval<BindParameters>()));

    /**
     * \brief Get the return type of a call to the delete method of the given table class with the given parameters.
     * \tparam T Table.
     */
    template<typename T>
    using delete_t =
      decltype(std::declval<T>().del(std::declval<LogicalExpression<T>>(), std::declval<BindParameters>()));

    /**
     * \brief Get the return type of a call to the count method of the given table class with the given parameters.
     * \tparam T Table.
     */
    template<typename T>
    using count_t = decltype(std::declval<T>().countAll());

    /**
     * \brief Get the return type of a call to the selectAll method of the given table class with the given parameters.
     * \tparam T Table.
     * \tparam R Row return type.
     * \tparam Indices Indices of the columns to select.
     */
    template<typename T, typename R, size_t... Indices>
    using select_all_t = decltype(std::declval<T>().template selectAll<R, Indices...>());
}  // namespace sql