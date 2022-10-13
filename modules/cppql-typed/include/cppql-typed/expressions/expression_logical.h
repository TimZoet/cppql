#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <format>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/expression.h"
#include "cppql-typed/expressions/expression_filter.h"

namespace sql
{
    ////////////////////////////////////////////////////////////////
    // LogicalExpression class.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief The LogicalExpression class holds two sub
     * FilterExpressions that are combined using either the AND or
     * OR operator.
     * \tparam T Table type.
     */
    template<typename T>
    class LogicalExpression final : public FilterExpression<T>
    {
    public:
        enum class Operator
        {
            Or,
            And
        };

        LogicalExpression() = delete;

        LogicalExpression(const LogicalExpression& other);

        LogicalExpression(LogicalExpression&& other) noexcept;

        LogicalExpression(FilterExpressionPtr<T> lhs, FilterExpressionPtr<T> rhs, Operator o);

        ~LogicalExpression() override = default;

        LogicalExpression& operator=(const LogicalExpression& other);

        LogicalExpression& operator=(LogicalExpression&& other) noexcept;

        [[nodiscard]] std::string toString(const Table& table, int32_t& pIndex) override;

        void bind(Statement& stmt) const override;

        [[nodiscard]] std::unique_ptr<FilterExpression<T>> clone() const override;

    private:
        /**
         * \brief Left hand side of expression.
         */
        FilterExpressionPtr<T> left;

        /**
         * \brief Right hand side of expression.
         */
        FilterExpressionPtr<T> right;

        /**
         * \brief Logical operator.
         */
        Operator op;
    };

    ////////////////////////////////////////////////////////////////
    // Implementation.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    LogicalExpression<T>::LogicalExpression(const LogicalExpression& other) :
        left(other.left->clone()), right(other.right->clone()), op(other.op)
    {
    }

    template<typename T>
    LogicalExpression<T>::LogicalExpression(LogicalExpression&& other) noexcept :
        left(std::move(other.left)), right(std::move(other.right)), op(other.op)
    {
    }

    template<typename T>
    LogicalExpression<T>::LogicalExpression(FilterExpressionPtr<T> lhs, FilterExpressionPtr<T> rhs, Operator o) :
        left(std::move(lhs)), right(std::move(rhs)), op(o)
    {
    }

    template<typename T>
    LogicalExpression<T>& LogicalExpression<T>::operator=(const LogicalExpression& other)
    {
        left  = other.left->clone();
        right = other.right->clone();
        op    = other.op;
        return *this;
    }

    template<typename T>
    LogicalExpression<T>& LogicalExpression<T>::operator=(LogicalExpression&& other) noexcept
    {
        left  = std::move(other.left);
        right = std::move(other.right);
        op    = other.op;
        return *this;
    }

    template<typename T>
    std::string LogicalExpression<T>::toString(const Table& table, int32_t& pIndex)
    {
        auto l = left->toString(table, pIndex);
        auto o = op == Operator::And ? std::string("AND") : std::string("OR");
        auto r = right->toString(table, pIndex);
        return std::format("({0} {1} {2})", std::move(l), std::move(o), std::move(r));
    }

    template<typename T>
    void LogicalExpression<T>::bind(Statement& stmt) const
    {
        left->bind(stmt);
        right->bind(stmt);
    }

    template<typename T>
    std::unique_ptr<FilterExpression<T>> LogicalExpression<T>::clone() const
    {
        return std::make_unique<LogicalExpression<T>>(*this);
    }

    ////////////////////////////////////////////////////////////////
    // AND (&&).
    ////////////////////////////////////////////////////////////////

    template<_is_filter_expression E1, _is_filter_expression E2>
    requires(same_table<E1, E2>) auto operator&&(E1&& lhs, E2&& rhs)
    {
        using L = LogicalExpression<typename E1::table_t>;
        return L(
          std::make_unique<E1>(std::forward<E1>(lhs)), std::make_unique<E2>(std::forward<E2>(rhs)), L::Operator::And);
    }

    ////////////////////////////////////////////////////////////////
    // OR (||).
    ////////////////////////////////////////////////////////////////

    template<_is_filter_expression E1, _is_filter_expression E2>
    requires(same_table<E1, E2>) auto operator||(E1&& lhs, E2&& rhs)
    {
        using L = LogicalExpression<typename E1::table_t>;
        return L(
          std::make_unique<E1>(std::forward<E1>(lhs)), std::make_unique<E2>(std::forward<E2>(rhs)), L::Operator::Or);
    }
}  // namespace sql
