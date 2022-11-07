#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <format>
#include <string>
#include <tuple>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"
#include "common/static_assert.h"
#include "cppql-core/error/sqlite_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/type_traits.h"
#include "cppql-typed/expressions/bind_parameters.h"
#include "cppql-typed/expressions/column_expression.h"
#include "cppql-typed/expressions/filter_expression.h"

namespace sql
{
    enum class ColCompOperator
    {
        Eq,  // ==
        Ne,  // !=
        Lt,  // <
        Gt,  // >
        Le,  // <=
        Ge,  // >=
    };

    template<is_column_expression L, is_column_expression R, ColCompOperator Op>
    class ColumnComparisonExpression : public FilterExpression<std::tuple<typename L::table_t, typename R::table_t>>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ColumnComparisonExpression() = delete;

        ColumnComparisonExpression(const ColumnComparisonExpression& other);

        ColumnComparisonExpression(ColumnComparisonExpression&& other) noexcept;

        ColumnComparisonExpression(L lhs, R rhs);

        ~ColumnComparisonExpression() override = default;

        ColumnComparisonExpression& operator=(const ColumnComparisonExpression& other);

        ColumnComparisonExpression& operator=(ColumnComparisonExpression&& other) noexcept;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string toString(int32_t& pIndex) override;

        void bind(Statement& stmt, BindParameters bind) const override;

    private:
        L left;

        R right;
    };

    ////////////////////////////////////////////////////////////////
    // Implementation.
    ////////////////////////////////////////////////////////////////

    template<is_column_expression L, is_column_expression R, ColCompOperator Op>
    ColumnComparisonExpression<L, R, Op>::ColumnComparisonExpression(const ColumnComparisonExpression& other) :
        left(other.left), right(other.right)
    {
    }

    template<is_column_expression L, is_column_expression R, ColCompOperator Op>
    ColumnComparisonExpression<L, R, Op>::ColumnComparisonExpression(ColumnComparisonExpression&& other) noexcept :
        left(std::move(other.left)), right(std::move(other.right))
    {
    }

    template<is_column_expression L, is_column_expression R, ColCompOperator Op>
    ColumnComparisonExpression<L, R, Op>::ColumnComparisonExpression(L lhs, R rhs) :
        left(std::move(lhs)), right(std::move(rhs))
    {
    }

    template<is_column_expression L, is_column_expression R, ColCompOperator Op>
    ColumnComparisonExpression<L, R, Op>&
      ColumnComparisonExpression<L, R, Op>::operator=(const ColumnComparisonExpression& other)
    {
        left  = other.left;
        right = other.right;
        return *this;
    }

    template<is_column_expression L, is_column_expression R, ColCompOperator Op>
    ColumnComparisonExpression<L, R, Op>&
      ColumnComparisonExpression<L, R, Op>::operator=(ColumnComparisonExpression&& other) noexcept
    {
        left  = std::move(other.left);
        right = std::move(other.value);
        return *this;
    }

    template<is_column_expression L, is_column_expression R, ColCompOperator Op>
    std::string ColumnComparisonExpression<L, R, Op>::toString(int32_t& pIndex)
    {
        if constexpr (Op == ColCompOperator::Eq)
            return std::format("{0} = {1}", left.toString(pIndex), right.toString(pIndex));
        else if constexpr (Op == ColCompOperator::Ne)
            return std::format("{0} != {1}", left.toString(pIndex), right.toString(pIndex));
        else if constexpr (Op == ColCompOperator::Lt)
            return std::format("{0} < {1}", left.toString(pIndex), right.toString(pIndex));
        else if constexpr (Op == ColCompOperator::Gt)
            return std::format("{0} > {1}", left.toString(pIndex), right.toString(pIndex));
        else if constexpr (Op == ColCompOperator::Le)
            return std::format("{0} <= {1}", left.toString(pIndex), right.toString(pIndex));
        else if constexpr (Op == ColCompOperator::Ge)
            return std::format("{0} >= {1}", left.toString(pIndex), right.toString(pIndex));
        else
        {
            constexpr_static_assert();
            return {};
        }
    }

    template<is_column_expression L, is_column_expression R, ColCompOperator Op>
    void ColumnComparisonExpression<L, R, Op>::bind(Statement&, const BindParameters) const
    {
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
     * \return ColumnComparisonExpression object.
     */
    template<is_column_expression L, is_column_expression R>
    auto operator==(L&& lhs, R&& rhs)
    {
        return ColumnComparisonExpression<L, R, ColCompOperator::Eq>(std::forward<L>(lhs), std::forward<R>(rhs));
    }

#if 0
    ////////////////////////////////////////////////////////////////
    // !=
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column != fixed value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator!=(ColumnExpression<T, Index> col, V val)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), std::move(val), C::Operator::Ne, true);
    }

    /**
     * \brief Require fixed value != column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator!=(V val, ColumnExpression<T, Index> col)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Ne, false);
    }

    /**
     * \brief Require column != dynamic value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator!=(ColumnExpression<T, Index> col, V* val)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Ne, true);
    }

    /**
     * \brief Require dynamic value != column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator!=(V* val, ColumnExpression<T, Index> col)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Ne, false);
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
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<(ColumnExpression<T, Index> col, V val)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Lt, true);
    }

    /**
     * \brief Require fixed value < column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<(V val, ColumnExpression<T, Index> col)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Lt, false);
    }

    /**
     * \brief Require column < dynamic value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<(ColumnExpression<T, Index> col, V* val)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Lt, true);
    }

    /**
     * \brief Require dynamic value < column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<(V* val, ColumnExpression<T, Index> col)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Lt, false);
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
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>(ColumnExpression<T, Index> col, V val)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Gt, true);
    }

    /**
     * \brief Require fixed value > column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>(V val, ColumnExpression<T, Index> col)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Gt, false);
    }

    /**
     * \brief Require column > dynamic value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>(ColumnExpression<T, Index> col, V* val)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Gt, true);
    }

    /**
     * \brief Require dynamic value > column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>(V* val, ColumnExpression<T, Index> col)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Gt, false);
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
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<=(ColumnExpression<T, Index> col, V val)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Le, true);
    }

    /**
     * \brief Require fixed value <= column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<=(V val, ColumnExpression<T, Index> col)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Le, false);
    }

    /**
     * \brief Require column <= dynamic value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<=(ColumnExpression<T, Index> col, V* val)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Le, true);
    }

    /**
     * \brief Require dynamic value <= column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<=(V* val, ColumnExpression<T, Index> col)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Le, false);
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
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>=(ColumnExpression<T, Index> col, V val)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Ge, true);
    }

    /**
     * \brief Require fixed value >= column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>=(V val, ColumnExpression<T, Index> col)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Ge, false);
    }

    /**
     * \brief Require column >= dynamic value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>=(ColumnExpression<T, Index> col, V* val)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Ge, true);
    }

    /**
     * \brief Require dynamic value >= column.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \tparam V Value type.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ColumnComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>=(V* val, ColumnExpression<T, Index> col)
    {
        using C = ColumnComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Ge, false);
    }
#endif
}  // namespace sql
