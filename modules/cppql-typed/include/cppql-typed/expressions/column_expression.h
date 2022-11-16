#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
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
#include "cppql-typed/expressions/filter_expression.h"

namespace sql
{
    template<is_typed_table T, size_t Index>
        requires(is_valid_index<T, Index>)
    class ColumnExpression final : public FilterExpression<std::tuple<T>>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using table_t                 = T;
        using value_t                 = col_t<Index, T>;
        static constexpr size_t index = Index;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ColumnExpression() = delete;

        ColumnExpression(const ColumnExpression&) = default;

        ColumnExpression(ColumnExpression&&) noexcept = default;

        explicit ColumnExpression(Table& t) : FilterExpression<std::tuple<T>>(), table(&t) {}

        ~ColumnExpression() noexcept override = default;

        ColumnExpression& operator=(const ColumnExpression&) = default;

        ColumnExpression& operator=(ColumnExpression&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool containsTables(const auto&... tables) const { return ((&tables == table) || ...); }

        void generateIndices(int32_t&) override {}

        // TODO: Get rid of toString and only use name/fullName.
        [[nodiscard]] std::string toString() override { return fullName(); }

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

        void bind(Statement&, BindParameters) const override {}

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        Table* table = nullptr;
    };
}  // namespace sql
