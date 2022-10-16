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

#include "cppql-typed/type_traits.h"
#include "cppql-typed/expressions/expression_column.h"
#include "cppql-typed/expressions/expression_filter.h"

namespace sql
{
    ////////////////////////////////////////////////////////////////
    // ComparisonExpression class.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief The ComparisonExpression class holds a column and a
     * fixed or dynamic value to compare it with.
     * \tparam T Table type.
     * \tparam V Value type.
     */
    template<typename T, typename V>
    class ComparisonExpression final : public FilterExpression<T>
    {
    public:
        enum class Operator
        {
            Eq,  // ==
            Lt,  // <
            Gt,  // >
            Le,  // <=
            Ge,  // >=
        };

        using value_t = V;

        ComparisonExpression() = delete;

        ComparisonExpression(const ComparisonExpression& other);

        ComparisonExpression(ComparisonExpression&& other) noexcept;

        ComparisonExpression(BaseColumnExpressionPtr<T> col, V val, Operator o, bool colLhs);

        ComparisonExpression(BaseColumnExpressionPtr<T> col, V* p, Operator o, bool colLhs);

        ~ComparisonExpression() override = default;

        ComparisonExpression& operator=(const ComparisonExpression& other);

        ComparisonExpression& operator=(ComparisonExpression&& other) noexcept;

        [[nodiscard]] std::string toString(const Table& table, int32_t& pIndex) override;

        void bind(Statement& stmt) const override;

        [[nodiscard]] std::unique_ptr<FilterExpression<T>> clone() const override;

    private:
        /**
         * \brief Column to compare.
         */
        BaseColumnExpressionPtr<T> column;

        /**
         * \brief Fixed value.
         */
        V value;

        /**
         * \brief Dynamic value.
         */
        V* ptr = nullptr;

        /**
         * \brief Comparison operator.
         */
        Operator op;

        /**
         * \brief Indicates column should be placed on left hand side of comparison.
         */
        bool lhs;

        /**
         * \brief Index for parameter binding.
         */
        int32_t index = -1;
    };

    ////////////////////////////////////////////////////////////////
    // Implementation.
    ////////////////////////////////////////////////////////////////

    template<typename T, typename V>
    ComparisonExpression<T, V>::ComparisonExpression(const ComparisonExpression& other) :
        column(other.column->clone()),
        value(other.value),
        ptr(other.ptr),
        op(other.op),
        lhs(other.lhs),
        index(other.index)
    {
    }

    template<typename T, typename V>
    ComparisonExpression<T, V>::ComparisonExpression(ComparisonExpression&& other) noexcept :
        column(std::move(other.column)),
        value(other.value),
        ptr(other.ptr),
        op(other.op),
        lhs(other.lhs),
        index(other.index)
    {
    }

    template<typename T, typename V>
    ComparisonExpression<T, V>::ComparisonExpression(BaseColumnExpressionPtr<T> col,
                                                     V                          val,
                                                     const Operator             o,
                                                     const bool                 colLhs) :
        column(std::move(col)), value(val), op(o), lhs(colLhs)
    {
    }

    template<typename T, typename V>
    ComparisonExpression<T, V>::ComparisonExpression(BaseColumnExpressionPtr<T> col,
                                                     V*                         p,
                                                     const Operator             o,
                                                     const bool                 colLhs) :
        column(std::move(col)), value(), ptr(p), op(o), lhs(colLhs)
    {
    }

    template<typename T, typename V>
    ComparisonExpression<T, V>& ComparisonExpression<T, V>::operator=(const ComparisonExpression& other)
    {
        column = other.column->clone();
        value  = other.value;
        ptr    = other.ptr;
        op     = other.op;
        lhs    = other.lhs;
        index  = other.index;
        return *this;
    }

    template<typename T, typename V>
    ComparisonExpression<T, V>& ComparisonExpression<T, V>::operator=(ComparisonExpression&& other) noexcept
    {
        column = std::move(other.column);
        value  = other.value;
        ptr    = other.ptr;
        op     = other.op;
        lhs    = other.lhs;
        index  = other.index;
        return *this;
    }

    template<typename T, typename V>
    std::string ComparisonExpression<T, V>::toString(const Table& table, int32_t& pIndex)
    {
        // Store unincremented index value. Use incremented value in string, because sqlite parameter indices start at 1.
        index = pIndex++;

        // Get column and operator string.
        auto        col = column->toString(table);
        std::string o;
        switch (op)
        {
        case Operator::Eq: o = "="; break;
        case Operator::Lt: o = "<"; break;
        case Operator::Gt: o = ">"; break;
        case Operator::Le: o = "<="; break;
        case Operator::Ge: o = ">="; break;
        }

        // Format with column on LHS or RHS.
        return lhs ? std::format("{0} {1} ?{2}", std::move(col), std::move(o), pIndex) :
                     std::format("?{2} {1} {0}", std::move(col), std::move(o), pIndex);
    }

    template<typename T, typename V>
    void ComparisonExpression<T, V>::bind(Statement& stmt) const
    {
        // If pointer is not null, bind dynamic value. Otherwise bind fixed value.
        const auto res = ptr ? stmt.bind(index + Statement::getFirstBindIndex(), *ptr) :
                               stmt.bind(index + Statement::getFirstBindIndex(), value);
        if (!res) throw std::runtime_error("");
    }

    template<typename T, typename V>
    std::unique_ptr<FilterExpression<T>> ComparisonExpression<T, V>::clone() const
    {
        return std::make_unique<ComparisonExpression<T, V>>(*this);
    }

    ////////////////////////////////////////////////////////////////
    // ==
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column == fixed value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator==(ColumnExpression<T, Index> col, V val)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), std::move(val), C::Operator::Eq, true);
    }

    /**
     * \brief Require fixed value == column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator==(V val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Eq, false);
    }

    /**
     * \brief Require column == dynamic value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator==(ColumnExpression<T, Index> col, V* val)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Eq, true);
    }

    /**
     * \brief Require dynamic value == column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator==(V* val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Eq, false);
    }

    ////////////////////////////////////////////////////////////////
    // <
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column < fixed value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<(ColumnExpression<T, Index> col, V val)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Lt, true);
    }

    /**
     * \brief Require fixed value < column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<(V val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Lt, false);
    }

    /**
     * \brief Require column < dynamic value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<(ColumnExpression<T, Index> col, V* val)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Lt, true);
    }

    /**
     * \brief Require dynamic value < column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<(V* val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Lt, false);
    }

    ////////////////////////////////////////////////////////////////
    // >
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column > fixed value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>(ColumnExpression<T, Index> col, V val)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Gt, true);
    }

    /**
     * \brief Require fixed value > column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>(V val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Gt, false);
    }

    /**
     * \brief Require column > dynamic value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>(ColumnExpression<T, Index> col, V* val)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Gt, true);
    }

    /**
     * \brief Require dynamic value > column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>(V* val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Gt, false);
    }

    ////////////////////////////////////////////////////////////////
    // <=
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column <= fixed value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<=(ColumnExpression<T, Index> col, V val)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Le, true);
    }

    /**
     * \brief Require fixed value <= column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<=(V val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Le, false);
    }

    /**
     * \brief Require column <= dynamic value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<=(ColumnExpression<T, Index> col, V* val)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Le, true);
    }

    /**
     * \brief Require dynamic value <= column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<=(V* val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Le, false);
    }

    ////////////////////////////////////////////////////////////////
    // >=
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column >= fixed value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>=(ColumnExpression<T, Index> col, V val)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Ge, true);
    }

    /**
     * \brief Require fixed value >= column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>=(V val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Ge, false);
    }

    /**
     * \brief Require column >= dynamic value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>=(ColumnExpression<T, Index> col, V* val)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Ge, true);
    }

    /**
     * \brief Require dynamic value >= column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>=(V* val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val, C::Operator::Ge, false);
    }
}  // namespace sql
