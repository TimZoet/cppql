#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/table.h"

namespace sql
{
    template<typename T>
    class BaseColumnExpression
    {
    public:
        using table_t = T;

        BaseColumnExpression() = default;

        BaseColumnExpression(const BaseColumnExpression&) = default;

        BaseColumnExpression(BaseColumnExpression&&) noexcept = default;

        virtual ~BaseColumnExpression() = default;

        BaseColumnExpression& operator=(const BaseColumnExpression&) = default;

        BaseColumnExpression& operator=(BaseColumnExpression&&) noexcept = default;

        [[nodiscard]] virtual std::string toString(const Table& table) const = 0;

        [[nodiscard]] virtual std::unique_ptr<BaseColumnExpression<T>> clone() const = 0;
    };

    template<typename T, size_t Index>
    class ColumnExpression final : public BaseColumnExpression<T>
    {
    public:
        static constexpr size_t index = Index;

        ColumnExpression() = default;

        ColumnExpression(const ColumnExpression&) = default;

        ColumnExpression(ColumnExpression&&) noexcept = default;

        ~ColumnExpression() override = default;

        ColumnExpression& operator=(const ColumnExpression&) = default;

        ColumnExpression& operator=(ColumnExpression&&) noexcept = default;

        [[nodiscard]] std::string toString(const Table& table) const override
        {
            return table.getColumn(Index).getName();
        }

        [[nodiscard]] std::unique_ptr<BaseColumnExpression<T>> clone() const override
        {
            return std::make_unique<ColumnExpression<T, Index>>(*this);
        }
    };

    template<typename T>
    concept is_column_expression = std::derived_from<T, BaseColumnExpression<typename T::table_t>>;

    template<typename T>
    using BaseColumnExpressionPtr = std::unique_ptr<BaseColumnExpression<T>>;
}  // namespace sql
