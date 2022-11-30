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
#include "common/type_traits.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/expressions/column_expression.h"
#include "cppql/typed/enums.h"

namespace sql
{
    template<is_result_expression L, is_result_expression R, ComparisonOperator Op>
    class ColumnComparisonExpression
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using left_t = L;

        using right_t = R;

        using table_list_t = std::tuple<typename left_t::table_t, typename right_t::table_t>;

        using unique_table_list_t = tuple_unique_t<table_list_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ColumnComparisonExpression() = delete;

        ColumnComparisonExpression(const ColumnComparisonExpression& other) = default;

        ColumnComparisonExpression(ColumnComparisonExpression&& other) noexcept = default;

        ColumnComparisonExpression(left_t lhs, right_t rhs) : left(std::move(lhs)), right(std::move(rhs)) {}

        ~ColumnComparisonExpression() noexcept = default;

        ColumnComparisonExpression& operator=(const ColumnComparisonExpression& other) = default;

        ColumnComparisonExpression& operator=(ColumnComparisonExpression&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool containsTables(const auto&... tables) const
        {
            return left.containsTables(tables...) || right.containsTables(tables...);
        }

        static void generateIndices(int32_t&) {}

        /**
         * \brief Generate expression comparing two columns.
         * \return String with format "<lhs> <op> <right>".
         */
        [[nodiscard]] std::string toString() const
        {
            return std::format("{0} {1} {2}", left.toString(), ComparisonOperatorType<Op>::str, right.toString());
        }

        static void bind(Statement&, BindParameters) {}

    private:
        left_t left;

        right_t right;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename L, typename R, ComparisonOperator Op>
    struct _is_filter_expression<ColumnComparisonExpression<L, R, Op>> : std::true_type
    {
    };

    ////////////////////////////////////////////////////////////////
    // Operators.
    ////////////////////////////////////////////////////////////////

    // TODO: There could be some additional checks here verifying that we're not comparing a column to itself.
    // However... that's pretty difficult to do, because two tables with the same columns would make that
    // impossible at compile time. Runtime check?

    /**
     * \brief Require lhs == rhs.
     * \tparam L Table type.
     * \tparam R Column index.
     * \param lhs Column object.
     * \param rhs Value.
     * \return ColumnComparisonExpression object.
     */
    template<is_result_expression L, is_result_expression R>
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
    template<is_result_expression L, is_result_expression R>
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
    template<is_result_expression L, is_result_expression R>
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
    template<is_result_expression L, is_result_expression R>
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
    template<is_result_expression L, is_result_expression R>
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
    template<is_result_expression L, is_result_expression R>
    auto operator>=(L&& lhs, R&& rhs)
    {
        return ColumnComparisonExpression<L, R, ComparisonOperator::Ge>(std::forward<L>(lhs), std::forward<R>(rhs));
    }
}  // namespace sql
