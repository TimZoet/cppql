#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
#include <tuple>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/table.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/fwd.h"

namespace sql
{
    template<is_typed_table T, size_t Index>
        requires(is_valid_index<T, Index>)
    class ColumnExpression
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using table_t                 = T;
        using value_t                 = col_t<Index, T>;
        using table_list_t            = std::tuple<table_t>;
        using unique_table_list_t     = std::tuple<table_t>;
        static constexpr size_t index = Index;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ColumnExpression() = delete;

        ColumnExpression(const ColumnExpression&) = default;

        ColumnExpression(ColumnExpression&&) noexcept = default;

        explicit ColumnExpression(Table& t) : table(&t) {}

        ~ColumnExpression() noexcept = default;

        ColumnExpression& operator=(const ColumnExpression&) = default;

        ColumnExpression& operator=(ColumnExpression&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool containsTables(const auto&... tables) const { return ((&tables == table) || ...); }

        static void generateIndices(int32_t&) {}

        /**
         * \brief Get column name.
         * \return String with format "column-name".
         */
        [[nodiscard]] std::string name() const { return table->getColumn(Index).getName(); }

        /**
         * \brief Get column name.
         * \return String with format "table-name.column-name".
         */
        [[nodiscard]] std::string fullName() const
        {
            return std::format("{}.{}", table->getName(), table->getColumn(Index).getName());
        }

        static void bind(Statement&, BindParameters) {}

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        Table* table = nullptr;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T, size_t Index>
    struct _is_filter_expression<ColumnExpression<T, Index>> : std::true_type
    {
    };
}  // namespace sql
