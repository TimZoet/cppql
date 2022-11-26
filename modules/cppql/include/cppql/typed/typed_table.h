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
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/column.h"
#include "cppql/core/database.h"
#include "cppql/core/table.h"
#include "cppql/error/cppql_error.h"
#include "cppql/error/sqlite_error.h"
#include "cppql/typed/fwd.h"
#include "cppql/typed/join_type.h"
#include "cppql/clauses/columns.h"
#include "cppql/expressions/column_comparison_expression.h"
#include "cppql/expressions/column_expression.h"
#include "cppql/expressions/comparison_expression.h"
#include "cppql/expressions/like_expression.h"
#include "cppql/expressions/logical_expression.h"
#include "cppql/expressions/order_by_expression.h"
#include "cppql/queries/fwd.h"

namespace sql
{
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

        [[nodiscard]] std::string toString() const { return table->getName(); }

        ////////////////////////////////////////////////////////////////
        // Columns.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Get a column expression by index.
         * \tparam Index Column index.
         * \return Column expression.
         */
        template<size_t Index>
            requires(Index < column_count)
        [[nodiscard]] auto col() const noexcept
        {
            return ColumnExpression<table_t, Index>(*table);
        }

        ////////////////////////////////////////////////////////////////
        // Insert.
        ////////////////////////////////////////////////////////////////

        // TODO: There is no detection of duplicate columns here yet. Same for update.

        template<is_valid_column_expression<std::tuple<table_t>> Col,
                 is_valid_column_expression<std::tuple<table_t>>... Cols>
        [[nodiscard]] auto insert(Col&& c, Cols&&... cs)
        {
            return InsertQuery<table_t, Col, Cols...>(*table, Columns(std::forward<Col>(c), std::forward<Cols>(cs)...));
        }

        template<size_t... Indices>
            requires((Indices < column_count) && ...)
        [[nodiscard]] auto insert() const
        {
            return InsertQuery<table_t, ColumnExpression<table_t, Indices>...>(*table, Columns(col<Indices>()...));
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

        template<is_valid_result_expression<std::tuple<table_t>> Col,
                 is_valid_result_expression<std::tuple<table_t>>... Cols>
        [[nodiscard]] auto select(Col&& c, Cols&&... cs)
        {
            return selectAs<std::tuple<typename Col::value_t, typename Cols::value_t...>>(std::forward<Col>(c),
                                                                                          std::forward<Cols>(cs)...);
        }

        template<typename R,
                 is_valid_result_expression<std::tuple<table_t>> Col,
                 is_valid_result_expression<std::tuple<table_t>>... Cols>
            requires(constructible_from<R, typename Col::value_t, typename Cols::value_t...>)
        [[nodiscard]] auto selectAs(Col&& c, Cols&&... cs)
        {
            return SelectQuery<R,
                               table_t,
                               std::nullopt_t,
                               std::nullopt_t,
                               std::nullopt_t,
                               std::nullopt_t,
                               std::nullopt_t,
                               std::nullopt_t,
                               std::remove_cvref_t<Col>,
                               std::remove_cvref_t<Cols>...>(
              *this,
              Columns<std::remove_cvref_t<Col>, std::remove_cvref_t<Cols>...>(std::forward<Col>(c),
                                                                              std::forward<Cols>(cs)...));
        }

        template<size_t... Indices>
            requires((Indices < column_count) && ...)
        [[nodiscard]] auto select()
        {
            if constexpr (sizeof...(Indices))
                return selectAs<std::tuple<col_t<Indices, table_t>...>, Indices...>();
            else
            {
                const auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>)
                {
                    return selectAs<row_t>(col<Is>()...);
                };

                return f(std::index_sequence_for<C, Cs...>{});
            }
        }

        template<typename R, size_t... Indices>
            requires(((Indices < column_count) && ...) &&
                     constructible_from<R, std::tuple_element_t<Indices, row_t>...>)
        [[nodiscard]] auto selectAs()
        {
            if constexpr (sizeof...(Indices))
                return selectAs<R>(col<Indices>()...);
            else
            {
                const auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>) { return selectAs<R, Is...>(); };

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

        template<is_valid_column_expression<std::tuple<table_t>> Col,
                 is_valid_column_expression<std::tuple<table_t>>... Cols>
        [[nodiscard]] auto update(Col&& c, Cols&&... cs)
        {
            return UpdateQuery<table_t, std::nullopt_t, std::nullopt_t, std::nullopt_t, Col, Cols...>(
              *table, Columns(std::forward<Col>(c), std::forward<Cols>(cs)...));
        }

        template<size_t... Indices>
            requires((Indices < column_count) && ...)
        [[nodiscard]] auto update() const
        {
            if constexpr (sizeof...(Indices))
                return UpdateQuery<table_t,
                                   std::nullopt_t,
                                   std::nullopt_t,
                                   std::nullopt_t,
                                   ColumnExpression<table_t, Indices>...>(
                  *table, Columns<ColumnExpression<table_t, Indices>...>(col<Indices>()...));
            else
            {
                const auto f = [this]<std::size_t... Is>(std::index_sequence<Is...>) { return update<Is...>(); };

                return f(std::index_sequence_for<C, Cs...>{});
            }
        }

        ////////////////////////////////////////////////////////////////
        // Join.
        ////////////////////////////////////////////////////////////////

        template<is_join_wrapper J, is_typed_table R>
        [[nodiscard]] auto join(J&&, R&& rhs) const
        {
            return Join<std::remove_cvref_t<J>, table_t, std::remove_cvref_t<R>, std::nullopt_t>(
              *table, std::forward<R>(rhs).getTable());
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