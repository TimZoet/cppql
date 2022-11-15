#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <string>
#include <tuple>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/enums.h"
#include "cppql-typed/expressions/column_expression.h"
#include "cppql-typed/expressions/filter_expression.h"

namespace sql
{
    template<is_column_expression L, is_column_expression R, ComparisonOperator Op>
    class ColumnComparisonExpression final
        : public FilterExpression<std::tuple<typename L::table_t, typename R::table_t>>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using left_t = L;

        using right_t = R;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ColumnComparisonExpression() = delete;

        ColumnComparisonExpression(const ColumnComparisonExpression& other) = default;

        ColumnComparisonExpression(ColumnComparisonExpression&& other) noexcept = default;

        ColumnComparisonExpression(left_t lhs, right_t rhs) : left(std::move(lhs)), right(std::move(rhs)) {}

        ~ColumnComparisonExpression() noexcept override = default;

        ColumnComparisonExpression& operator=(const ColumnComparisonExpression& other) = default;

        ColumnComparisonExpression& operator=(ColumnComparisonExpression&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool containsTables(const auto&... tables) const
        {
            return left.containsTables(tables...) && right.containsTables(tables...);
        }

        void generateIndices(int32_t& idx) override
        {
            left.generateIndices(idx);
            right.generateIndices(idx);
        }

        /**
         * \brief Generate expression comparing two columns.
         * \return String with format "<lhs> <op> <right>".
         */
        [[nodiscard]] std::string toString() override
        {
            return std::format("{0} {1} {2}", left.toString(), ComparisonOperatorType<Op>::str, right.toString());
        }

        void bind(Statement&, BindParameters) const override {}

    private:
        left_t left;

        right_t right;
    };

    ////////////////////////////////////////////////////////////////
    // Operators.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require lhs == rhs.
     * \tparam L Table type.
     * \tparam R Column index.
     * \param lhs Column object.
     * \param rhs Value.
     * \return ColumnComparisonExpression object.
     */
    template<is_column_expression L, is_column_expression R>
    auto operator==(L&& lhs, R&& rhs)
    {
        return ColumnComparisonExpression<L, R, ComparisonOperator::Eq>(std::forward<L>(lhs), std::forward<R>(rhs));
    }

    /**
     * \brief Require lhs != rhs.
     * \tparam L Table type.
     * \tparam R Column index.
     * \param lhs Column object.
     * \param rhs Value.
     * \return ColumnComparisonExpression object.
     */
    template<is_column_expression L, is_column_expression R>
    auto operator!=(L&& lhs, R&& rhs)
    {
        return ColumnComparisonExpression<L, R, ComparisonOperator::Ne>(std::forward<L>(lhs), std::forward<R>(rhs));
    }

    /**
     * \brief Require lhs < rhs.
     * \tparam L Table type.
     * \tparam R Column index.
     * \param lhs Column object.
     * \param rhs Value.
     * \return ColumnComparisonExpression object.
     */
    template<is_column_expression L, is_column_expression R>
    auto operator<(L&& lhs, R&& rhs)
    {
        return ColumnComparisonExpression<L, R, ComparisonOperator::Lt>(std::forward<L>(lhs), std::forward<R>(rhs));
    }

    /**
     * \brief Require lhs > rhs.
     * \tparam L Table type.
     * \tparam R Column index.
     * \param lhs Column object.
     * \param rhs Value.
     * \return ColumnComparisonExpression object.
     */
    template<is_column_expression L, is_column_expression R>
    auto operator>(L&& lhs, R&& rhs)
    {
        return ColumnComparisonExpression<L, R, ComparisonOperator::Gt>(std::forward<L>(lhs), std::forward<R>(rhs));
    }

    /**
     * \brief Require lhs <= rhs.
     * \tparam L Table type.
     * \tparam R Column index.
     * \param lhs Column object.
     * \param rhs Value.
     * \return ColumnComparisonExpression object.
     */
    template<is_column_expression L, is_column_expression R>
    auto operator<=(L&& lhs, R&& rhs)
    {
        return ColumnComparisonExpression<L, R, ComparisonOperator::Le>(std::forward<L>(lhs), std::forward<R>(rhs));
    }

    /**
     * \brief Require lhs >= rhs.
     * \tparam L Table type.
     * \tparam R Column index.
     * \param lhs Column object.
     * \param rhs Value.
     * \return ColumnComparisonExpression object.
     */
    template<is_column_expression L, is_column_expression R>
    auto operator>=(L&& lhs, R&& rhs)
    {
        return ColumnComparisonExpression<L, R, ComparisonOperator::Ge>(std::forward<L>(lhs), std::forward<R>(rhs));
    }
}  // namespace sql
