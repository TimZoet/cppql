#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <string>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/enums.h"
#include "cppql-typed/expressions/filter_expression.h"

namespace sql
{
    template<is_filter_expression L, is_filter_expression R, LogicalOperator Op>
    class LogicalExpression final
        : public FilterExpression<tuple_merge_t<typename L::table_list_t, typename R::table_list_t>>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        LogicalExpression() = delete;

        LogicalExpression(const LogicalExpression& other) = default;

        LogicalExpression(LogicalExpression&& other) noexcept = default;

        LogicalExpression(L lhs, R rhs) : left(std::move(lhs)), right(std::move(rhs)) {}

        ~LogicalExpression() override = default;

        LogicalExpression& operator=(const LogicalExpression& other) = default;

        LogicalExpression& operator=(LogicalExpression&& other) noexcept = default;

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
         * \brief Generate expression checking left- and right-hand sides.
         * \return String with format "(<lhs> <op> <rhs>)".
         */
        [[nodiscard]] std::string toString() override
        {
            auto l = left.toString();
            auto o = Op == LogicalOperator::And ? std::string("AND") : std::string("OR");
            auto r = right.toString();
            return std::format("({0} {1} {2})", std::move(l), std::move(o), std::move(r));
        }

        void bind(Statement& stmt, BindParameters bind) const override
        {
            left.bind(stmt, bind);
            right.bind(stmt, bind);
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Left hand side of expression.
         */
        L left;

        /**
         * \brief Right hand side of expression.
         */
        R right;
    };

    ////////////////////////////////////////////////////////////////
    // AND (&&).
    ////////////////////////////////////////////////////////////////

    template<is_filter_expression L, is_filter_expression R>
    auto operator&&(L&& lhs, R&& rhs)
    {
        return LogicalExpression<L, R, LogicalOperator::And>(std::forward<L>(lhs), std::forward<R>(rhs));
    }

    ////////////////////////////////////////////////////////////////
    // OR (||).
    ////////////////////////////////////////////////////////////////

    template<is_filter_expression L, is_filter_expression R>
    auto operator||(L&& lhs, R&& rhs)
    {
        return LogicalExpression<L, R, LogicalOperator::Or>(std::forward<L>(lhs), std::forward<R>(rhs));
    }
}  // namespace sql
