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

#include "cppql-typed/join_type.h"
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

    // TODO: Move these to separate locations?

    template<typename T>
    class CountStatement;

    template<typename T>
    class DeleteStatement;

    template<typename T, typename... Cs>
    class InsertStatement;

    template<typename R, typename... Cs>
        requires(constructible_from<R, Cs...>)
    class SelectStatement;

    template<typename R, typename... Cs>
        requires(constructible_from<R, Cs...>)
    class SelectOneStatement;

    template<typename T, typename... Cs>
    class UpdateStatement;



    template<typename T, typename F>
    class CountQuery;

    template<typename T, typename F, typename O, typename L>
    class DeleteQuery;

    template<typename T, is_column_expression... Cs>
    class InsertQuery;

    template<typename R,
             typename J,
             typename F,
             typename O,
             typename L,
             is_column_expression C,
             is_column_expression... Cs>
        requires(constructible_from_or_none<R, typename C::value_t, typename Cs::value_t...>)
    class SelectQuery;

    template<typename T, typename F, typename O, typename L, is_column_expression C, is_column_expression... Cs>
    class UpdateQuery;

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

        // TODO: Try to delete int& param.
        [[nodiscard]] std::string toString(int32_t&) { return table->getName(); }

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

        template<size_t... Indices>
        [[nodiscard]] auto insert() const
        {
            return InsertQuery<table_t, ColumnExpression<table_t, Indices>...>(
              *table, Columns(std::make_tuple(col<Indices>()...)));
        }

        [[nodiscard]] auto insert() const
        {
            const auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>) { return insert<Is...>(); };
            return f(std::index_sequence_for<C, Cs...>{});
        }

        ////////////////////////////////////////////////////////////////
        // Delete.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] auto del() const
        {
            return DeleteQuery<table_t, std::nullopt_t, std::nullopt_t, std::nullopt_t>(*table);
        }

        ////////////////////////////////////////////////////////////////
        // Select.
        ////////////////////////////////////////////////////////////////

        template<size_t... Indices>
        [[nodiscard]] auto select()
        {
            if constexpr (sizeof...(Indices))
                return SelectQuery<std::nullopt_t,
                                   table_t,
                                   std::nullopt_t,
                                   std::nullopt_t,
                                   std::nullopt_t,
                                   ColumnExpression<table_t, Indices>...>(
                  *this, Columns<ColumnExpression<table_t, Indices>...>(std::make_tuple(col<Indices>()...)));
            else
            {
                const auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>) { return select<Is...>(); };

                return f(std::index_sequence_for<C, Cs...>{});
            }
        }

        template<typename R, size_t... Indices>
        [[nodiscard]] auto select()
        {
            if constexpr (sizeof...(Indices))
                return SelectQuery<R,
                                   table_t,
                                   std::nullopt_t,
                                   std::nullopt_t,
                                   std::nullopt_t,
                                   ColumnExpression<table_t, Indices>...>(
                  *this, Columns<ColumnExpression<table_t, Indices>...>(std::make_tuple(col<Indices>()...)));
            else
            {
                const auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>) { return select<R, Is...>(); };

                return f(std::index_sequence_for<C, Cs...>{});
            }
        }

        ////////////////////////////////////////////////////////////////
        // Count.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] auto count() const { return CountQuery<table_t, std::nullopt_t>(*table); }

        ////////////////////////////////////////////////////////////////
        // Update.
        ////////////////////////////////////////////////////////////////

        template<size_t... Indices>
        [[nodiscard]] auto update() const
        {
            if constexpr (sizeof...(Indices))
                return UpdateQuery<table_t,

                                   std::nullopt_t,
                                   std::nullopt_t,
                                   std::nullopt_t,
                                   ColumnExpression<table_t, Indices>...>(
                  *table, Columns<ColumnExpression<table_t, Indices>...>(std::make_tuple(col<Indices>()...)));
            else
            {
                const auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>) { return update<Is...>(); };

                return f(std::index_sequence_for<C, Cs...>{});
            }
        }

        ////////////////////////////////////////////////////////////////
        // Join.
        ////////////////////////////////////////////////////////////////

        // TODO: Require J to be JoinWrapper and R to be TypedTable.
        template<typename J, typename R>
        [[nodiscard]] auto join(J&&, R& rhs) const
        {
            return Join<J, table_t, R, std::nullopt_t>(*table, rhs.getTable());
        }

    private:
        template<size_t Index, typename U, typename... Us>
        void validate()
        {
            if (table->getColumn(Index).getType() != toColumnType<U>()) throw CppqlError("Invalid column type.");

            // Recurse.
            if constexpr (sizeof...(Us) > 0) validate<Index + 1, Us...>();
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