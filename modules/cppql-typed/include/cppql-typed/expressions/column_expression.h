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

#include "cppql-typed/expressions/filter_expression.h"

namespace sql
{
    // TODO: T must be TypedTable, I a valid index
    template<typename T, size_t Index>
    class ColumnExpression : public FilterExpression<std::tuple<T>>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using table_t                 = T;
        static constexpr size_t index = Index;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ColumnExpression() = default;

        ColumnExpression(const ColumnExpression&) = default;

        ColumnExpression(ColumnExpression&&) noexcept = default;

        explicit ColumnExpression(Table& t) : FilterExpression<std::tuple<T>>(t) {}

        ~ColumnExpression() override = default;

        ColumnExpression& operator=(const ColumnExpression&) = default;

        ColumnExpression& operator=(ColumnExpression&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string toString(int32_t&) override
        {
            return std::format("{}.{}", this->table->getName(), this->table->getColumn(Index).getName());
        }

        void bind(Statement&, BindParameters) const override {}
    };

    template<typename T>
    concept is_column_expression = std::same_as<std::decay_t<T>, ColumnExpression<typename T::table_t, T::index>>;

    template<typename C, typename Tables>
    concept is_valid_column_expression = is_column_expression<C> && tuple_contains_type<typename C::table_t, Tables>;
}  // namespace sql
