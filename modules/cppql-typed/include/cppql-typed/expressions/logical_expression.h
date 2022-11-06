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
#include "cppql-typed/expressions/single_filter_expression.h"

namespace sql
{
    ////////////////////////////////////////////////////////////////
    // LogicalExpression class.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief The LogicalExpression class holds two sub
     * SingleFilterExpression that are combined using either the AND or
     * OR operator.
     * \tparam T Table type.
     */
    template<typename T>
    class LogicalExpression final : public SingleFilterExpression<T>
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

        LogicalExpression(SingleFilterExpression<T> lhs, SingleFilterExpression<T> rhs, Operator o);

        ~LogicalExpression() override = default;

        LogicalExpression& operator=(const LogicalExpression& other);

        LogicalExpression& operator=(LogicalExpression&& other) noexcept;

        [[nodiscard]] std::string toString(const Table& table, int32_t& pIndex) override;

        void bind(Statement& stmt, BindParameters bind) const override;

        [[nodiscard]] std::unique_ptr<SingleFilterExpression<T>> clone() const override;

    private:
        /**
         * \brief Left hand side of expression.
         */
        SingleFilterExpression<T> left;

        /**
         * \brief Right hand side of expression.
         */
        SingleFilterExpression<T> right;

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
    LogicalExpression<T>::LogicalExpression(SingleFilterExpression<T> lhs,
                                            SingleFilterExpression<T> rhs,
                                            const Operator            o) :
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
    void LogicalExpression<T>::bind(Statement& stmt, const BindParameters bind) const
    {
        left->bind(stmt, bind);
        right->bind(stmt, bind);
    }

    template<typename T>
    std::unique_ptr<SingleFilterExpression<T>> LogicalExpression<T>::clone() const
    {
        return std::make_unique<LogicalExpression<T>>(*this);
    }

    ////////////////////////////////////////////////////////////////
    // AND (&&).
    ////////////////////////////////////////////////////////////////

    template<_is_single_filter_expression E1, _is_single_filter_expression E2>
    requires(same_table<E1, E2>) auto operator&&(E1&& lhs, E2&& rhs)
    {
        using L = LogicalExpression<typename E1::table_t>;
        return L(
          std::make_unique<E1>(std::forward<E1>(lhs)), std::make_unique<E2>(std::forward<E2>(rhs)), L::Operator::And);
    }

    ////////////////////////////////////////////////////////////////
    // OR (||).
    ////////////////////////////////////////////////////////////////

    template<_is_single_filter_expression E1, _is_single_filter_expression E2>
    requires(same_table<E1, E2>) auto operator||(E1&& lhs, E2&& rhs)
    {
        using L = LogicalExpression<typename E1::table_t>;
        return L(
          std::make_unique<E1>(std::forward<E1>(lhs)), std::make_unique<E2>(std::forward<E2>(rhs)), L::Operator::Or);
    }
}  // namespace sql
