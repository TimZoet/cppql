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
#include "cppql-core/error/cppql_error.h"
#include "cppql-core/error/sqlite_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/type_traits.h"
#include "cppql-typed/clauses/columns.h"
#include "cppql-typed/expressions/bind_parameters.h"
#include "cppql-typed/expressions/column_comparison_expression.h"
#include "cppql-typed/expressions/column_expression.h"
#include "cppql-typed/expressions/comparison_expression.h"
#include "cppql-typed/expressions/like_expression.h"
#include "cppql-typed/expressions/limit_expression.h"
#include "cppql-typed/expressions/logical_expression.h"
#include "cppql-typed/expressions/order_by_expression.h"
#include "cppql-typed/expressions/single_filter_expression.h"

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

    template<typename R, typename... Cs>
    requires(constructible_from<R, Cs...>) class Select;

    template<typename R, typename... Cs>
    requires(constructible_from<R, Cs...>) class SelectOne;

    template<typename T, size_t... Indices>
    class Update;

    template<typename R, typename J, typename F, typename O, is_column_expression C, is_column_expression... Cs> requires (constructible_from_or_none<R, typename C::value_t, typename Cs::value_t...>)
    class ComplexSelect;

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
            if (column_count != table->getColumnCount()) throw CppqlError("Invalid number of columns.");
        }

        TypedTable(const TypedTable&) = default;

        TypedTable(TypedTable&&) noexcept = default;

        ~TypedTable() noexcept = default;

        TypedTable& operator=(const TypedTable&) = default;

        TypedTable& operator=(TypedTable&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        Table& getTable() noexcept { return *table; }

        [[nodiscard]] const Table& getTable() const noexcept { return *table; }

        [[nodiscard]] std::string toString(int32_t&)
        {
            return table->getName();
        }

        ////////////////////////////////////////////////////////////////
        // Columns.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get a column expression by index.
         * \tparam Index Column index.
         * \return Column expression.
         */
        template<size_t Index>
        requires(in_column_range<column_count, Index>)
          [[nodiscard]] ColumnExpression<table_t, Index> col() const noexcept
        {
            return ColumnExpression<table_t, Index>(*table);
        }

        ////////////////////////////////////////////////////////////////
        // Insert.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create an INSERT INTO query. Creates a callable object that will insert the passed values for each specified column and default values for the rest.
         * \tparam Indices Indices of the columns for which to explicitly bind values. Leave empty to insert all default values.
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
         * \brief Create a DELETE FROM query. Creates a callable object that will delete all rows that match the filter expression.
         * \tparam F SingleFilterExpression type or std::nullopt_t.
         * \tparam O OrderByExpression type or std::nullopt_t.
         * \tparam L LimitExpression type or std::nullopt_t.
         * \param filterExpression Expression to filter rows that are deleted. If std::nullopt, all rows are deleted.
         * \param orderByExpression Expression to order results by. If std::nullopt, results are not ordered.
         * \param limitExpression Expression to limit results by. If std::nullopt, results are not limited.
         * \param bind Parameters to bind.
         * \return Delete object.
         */
        template<is_single_filter_expression_or_none<table_t> F,
                 is_order_by_expression_or_none<table_t>      O,
                 is_limit_expression_or_none                  L>
        [[nodiscard]] Delete<table_t>
          del(F&& filterExpression, O&& orderByExpression, L&& limitExpression, BindParameters bind)
        {
            return delImpl(optionalToPtr(std::forward<F>(filterExpression)),
                           std::forward<O>(orderByExpression),
                           std::forward<L>(limitExpression),
                           bind);
        }

        ////////////////////////////////////////////////////////////////
        // Select.
        ////////////////////////////////////////////////////////////////

        template<size_t... Indices>
        [[nodiscard]] auto select()
        {
            if constexpr (sizeof...(Indices))
                return ComplexSelect<std::nullopt_t, table_t, std::nullopt_t, std::nullopt_t, ColumnExpression<table_t, Indices>...>(*this, Columns<ColumnExpression<table_t, Indices>...>(std::make_tuple(col<Indices>()...)));
            else
            {
                const auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return select<Is...>();
                };

                return f(std::index_sequence_for<C, Cs...>{});
            }
        }

        template<typename R, size_t... Indices>
        [[nodiscard]] auto select()
        {
            if constexpr (sizeof...(Indices))
                return ComplexSelect<R, table_t, std::nullopt_t, std::nullopt_t, ColumnExpression<table_t, Indices>...>(*this, Columns<ColumnExpression<table_t, Indices>...>(std::make_tuple(col<Indices>()...)));
            else
            {
                const auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return select<R, Is...>();
                };

                return f(std::index_sequence_for<C, Cs...>{});
            }
        }

        ////////////////////////////////////////////////////////////////
        // Count.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create a COUNT query. Creates a callable object that returns the number of rows that match the filter expression.
         * \tparam F SingleFilterExpression type.
         * \param filterExpression Expression to filter results by.
         * \param bind Parameters to bind.
         * \return Count object.
         */
        template<is_single_filter_expression_or_none<table_t> F>
        [[nodiscard]] auto count(F&& filterExpression, BindParameters bind)
        {
            return countImpl(optionalToPtr(std::forward<F>(filterExpression)), bind);
        }

        ////////////////////////////////////////////////////////////////
        // Update.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Create an UPDATE query. Creates a callable object that will update the specified columns of matching rows with the passed values.
         * \tparam Indices Indices of the columns to update.
         * \tparam F SingleFilterExpression type or std::nullopt_t.
         * \tparam O OrderByExpression type or std::nullopt_t.
         * \tparam L LimitExpression type or std::nullopt_t.
         * \param filterExpression Expression to filter the rows that will be updated. If std::nullopt, rows are not filtered.
         * \param orderByExpression Expression to order rows by. If std::nullopt, rows are not ordered.
         * \param limitExpression Expression to limit rows by. If std::nullopt, rows are not limited.
         * \param bind Parameters to bind.
         * \return Update object.
         */
        template<size_t... Indices,
                 is_single_filter_expression_or_none<table_t> F,
                 is_order_by_expression_or_none<table_t>      O,
                 is_limit_expression_or_none                  L>
        requires(in_column_range<column_count, Indices...>) [[nodiscard]] auto update(F&&            filterExpression,
                                                                                      O&&            orderByExpression,
                                                                                      L&&            limitExpression,
                                                                                      BindParameters bind)
        {
            // TODO: In all of these filters, there could technically be expressions referencing columns from different table instances that just so happen to have the same column types.
            // That should proably result in a runtime error.
            if constexpr (sizeof...(Indices))
            {
                return updateImpl<Indices...>(optionalToPtr(std::forward<F>(filterExpression)),
                                              std::forward<O>(orderByExpression),
                                              std::forward<L>(limitExpression),
                                              bind);
            }
            else
            {
                const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return update<Is...>(std::forward<F>(filterExpression),
                                         std::forward<O>(orderByExpression),
                                         std::forward<L>(limitExpression),
                                         bind);
                };
                return f(std::index_sequence_for<C, Cs...>{});
            }
        }

    private:
        template<size_t Index, typename U, typename... Us>
        void validate()
        {
            if (table->getColumn(Index).getType() != toColumnType<U>()) throw CppqlError("Invalid column type.");

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
            if (!stmt->isPrepared())
                throw SqliteError(std::format("Failed to prepare statement \"{}\"", stmt->getSql()),
                                  stmt->getResult()->code);

            // Construct Insert.
            return Insert<table_t, Indices...>(std::move(stmt));
        }

        [[nodiscard]] auto delImpl(BaseFilterExpressionPtr                         fExpr,
                                   const std::optional<OrderByExpression<table_t>> oExpr,
                                   const std::optional<LimitExpression>            lExpr,
                                   const BindParameters                            bind)
        {
            // Generate format arguments.
            auto        index   = 0;
            std::string e       = fExpr ? "WHERE " + fExpr->toString(index) : "";
            std::string orderBy = oExpr ? oExpr->toString() : "";
            std::string limit   = lExpr ? lExpr->toString() : "";

            // Format SQL statement.
            auto sql = std::format(
              "DELETE FROM {0} {1} {2} {3};", table->getName(), std::move(e), std::move(orderBy), std::move(limit));

            // Create and prepare statement.
            auto stmt = std::make_unique<Statement>(table->getDatabase(), std::move(sql), true);
            if (!stmt->isPrepared())
                throw SqliteError(std::format("Failed to prepare statement \"{}\"", stmt->getSql()),
                                  stmt->getResult()->code);

            // Bind parameters.
            if (fExpr && any(bind)) fExpr->bind(*stmt, bind);

            // Construct Delete.
            return Delete<table_t>(std::move(stmt), std::move(fExpr));
        }

        [[nodiscard]] auto countImpl(BaseFilterExpressionPtr fExpr, BindParameters bind)
        {
            std::string sql;

            // Format SQL statement.
            if (fExpr)
            {
                auto index = 0;
                sql = std::format("SELECT COUNT(*) FROM {0} WHERE {1};", table->getName(), fExpr->toString(index));
            }
            else
                sql = std::format("SELECT COUNT(*) FROM {0};", table->getName());

            // Create and prepare statement.
            auto stmt = std::make_unique<Statement>(table->getDatabase(), std::move(sql), true);
            if (!stmt->isPrepared())
                throw SqliteError(std::format("Failed to prepare statement \"{}\"", stmt->getSql()),
                                  stmt->getResult()->code);

            // Bind parameters.
            if (fExpr && any(bind)) fExpr->bind(*stmt, bind);

            // Construct Delete.
            return Count<table_t>(std::move(stmt), std::move(fExpr));
        }

        template<size_t... Indices>
        [[nodiscard]] auto updateImpl(BaseFilterExpressionPtr                         fExpr,
                                      const std::optional<OrderByExpression<table_t>> oExpr,
                                      const std::optional<LimitExpression>            lExpr,
                                      BindParameters                                  bind)
        {
            // Construct an UPDATE <table> SET (<cols>) = (<vals>) WHERE <filter> ORDER BY <expr> LIMIT <expr> OFFSET <expr> query.

            // Generate format arguments.
            auto        index    = static_cast<int32_t>(sizeof...(Indices));
            std::string e        = fExpr ? "WHERE " + fExpr->toString(index) : "";
            std::string orderBy  = oExpr ? oExpr->toString() : "";
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
            if (!stmt->isPrepared())
                throw SqliteError(std::format("Failed to prepare statement \"{}\"", stmt->getSql()),
                                  stmt->getResult()->code);

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

#if 0
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
#endif
}  // namespace sql