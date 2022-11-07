#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/bind_parameters.h"
#include "cppql-typed/expressions/filter_expression.h"

namespace sql
{
    template<is_filter_expression L, is_filter_expression R>
    class LogicalExpression : public FilterExpression<tuple_merge_t<typename L::table_list_t, typename R::table_list_t>>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class Operator
        {
            Or,
            And
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        LogicalExpression() = delete;

        LogicalExpression(const LogicalExpression& other);

        LogicalExpression(LogicalExpression&& other) noexcept;

        LogicalExpression(L lhs, R rhs, Operator o);

        ~LogicalExpression() override = default;

        LogicalExpression& operator=(const LogicalExpression& other);

        LogicalExpression& operator=(LogicalExpression&& other) noexcept;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string toString(int32_t& pIndex) override;

        void bind(Statement& stmt, BindParameters bind) const override;

    private:
        /**
         * \brief Left hand side of expression.
         */
        L left;

        /**
         * \brief Right hand side of expression.
         */
        R right;

        // TODO: This can perhaps be turned into a template parameter.
        /**
         * \brief Logical operator.
         */
        Operator op;
    };

    ////////////////////////////////////////////////////////////////
    // Implementation.
    ////////////////////////////////////////////////////////////////

    template<is_filter_expression L, is_filter_expression R>
    LogicalExpression<L, R>::LogicalExpression(const LogicalExpression& other) :
        left(other.left), right(other.right), op(other.op)
    {
    }

    template<is_filter_expression L, is_filter_expression R>
    LogicalExpression<L, R>::LogicalExpression(LogicalExpression&& other) noexcept :
        left(std::move(other.left)), right(std::move(other.right)), op(other.op)
    {
    }

    template<is_filter_expression L, is_filter_expression R>
    LogicalExpression<L, R>::LogicalExpression(L lhs, R rhs, const Operator o) :
        left(std::move(lhs)), right(std::move(rhs)), op(o)
    {
    }

    template<is_filter_expression L, is_filter_expression R>
    LogicalExpression<L, R>& LogicalExpression<L, R>::operator=(const LogicalExpression& other)
    {
        left  = other.left;
        right = other.right;
        op    = other.op;
        return *this;
    }

    template<is_filter_expression L, is_filter_expression R>
    LogicalExpression<L, R>& LogicalExpression<L, R>::operator=(LogicalExpression&& other) noexcept
    {
        left  = std::move(other.left);
        right = std::move(other.right);
        op    = other.op;
        return *this;
    }

    template<is_filter_expression L, is_filter_expression R>
    std::string LogicalExpression<L, R>::toString(int32_t& pIndex)
    {
        auto l = left.toString(pIndex);
        auto o = op == Operator::And ? std::string("AND") : std::string("OR");
        auto r = right.toString(pIndex);
        return std::format("({0} {1} {2})", std::move(l), std::move(o), std::move(r));
    }

    template<is_filter_expression L, is_filter_expression R>
    void LogicalExpression<L, R>::bind(Statement& stmt, const BindParameters bind) const
    {
        left.bind(stmt, bind);
        right.bind(stmt, bind);
    }

    ////////////////////////////////////////////////////////////////
    // AND (&&).
    ////////////////////////////////////////////////////////////////

    template<is_filter_expression L, is_filter_expression R>
    auto operator&&(L&& lhs, R&& rhs)
    {
        using t = LogicalExpression<L, R>;
        return t(std::forward<L>(lhs), std::forward<R>(rhs), t::Operator::And);
    }

    ////////////////////////////////////////////////////////////////
    // OR (||).
    ////////////////////////////////////////////////////////////////

    template<is_filter_expression L, is_filter_expression R>
    auto operator||(L&& lhs, R&& rhs)
    {
        using t = LogicalExpression<L, R>;
        return t(std::forward<L>(lhs), std::forward<R>(rhs), t::Operator::Or);
    }
}  // namespace sql
