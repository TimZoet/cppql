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
    // TODO: T must be TypedTable, I a valid index, V must be bindable
    /**
     * \brief The ComparisonExpression class holds a column and a
     * fixed or dynamic value to compare it with.
     * \tparam T Table type.
     * \tparam V Value type.
     */
    template<typename T, size_t Index, typename V>
    class ComparisonExpression : public FilterExpression<std::tuple<T>>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class Operator
        {
            Eq,  // ==
            Ne,  // !=
            Lt,  // <
            Gt,  // >
            Le,  // <=
            Ge,  // >=
        };

        using value_t = V;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComparisonExpression() = delete;

        ComparisonExpression(const ComparisonExpression& other);

        ComparisonExpression(ComparisonExpression&& other) noexcept;

        ComparisonExpression(ColumnExpression<T, Index> col, V val, Operator o, bool colLhs);

        ComparisonExpression(ColumnExpression<T, Index> col, V* p, Operator o, bool colLhs);

        ~ComparisonExpression() override = default;

        ComparisonExpression& operator=(const ComparisonExpression& other);

        ComparisonExpression& operator=(ComparisonExpression&& other) noexcept;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string toString(int32_t& pIndex) override;

        void bind(Statement& stmt, BindParameters bind) const override;

    private:
        /**
         * \brief Column to compare.
         */
        ColumnExpression<T, Index> column;

        /**
         * \brief Fixed value.
         */
        V value;

        /**
         * \brief Dynamic value.
         */
        V* ptr = nullptr;

        // TODO: This can perhaps be turned into a template parameter.
        /**
         * \brief Comparison operator.
         */
        Operator op;

        // TODO: This can perhaps be turned into a template parameter.
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

    template<typename T, size_t Index, typename V>
    ComparisonExpression<T, Index, V>::ComparisonExpression(const ComparisonExpression& other) :
        column(other.column), value(other.value), ptr(other.ptr), op(other.op), lhs(other.lhs), index(other.index)
    {
    }

    template<typename T, size_t Index, typename V>
    ComparisonExpression<T, Index, V>::ComparisonExpression(ComparisonExpression&& other) noexcept :
        column(std::move(other.column)),
        value(other.value),
        ptr(other.ptr),
        op(other.op),
        lhs(other.lhs),
        index(other.index)
    {
    }

    template<typename T, size_t Index, typename V>
    ComparisonExpression<T, Index, V>::ComparisonExpression(ColumnExpression<T, Index> col,
                                                            V                          val,
                                                            const Operator             o,
                                                            const bool                 colLhs) :
        column(std::move(col)), value(val), op(o), lhs(colLhs)
    {
    }

    template<typename T, size_t Index, typename V>
    ComparisonExpression<T, Index, V>::ComparisonExpression(ColumnExpression<T, Index> col,
                                                            V*                         p,
                                                            const Operator             o,
                                                            const bool                 colLhs) :
        column(std::move(col)), value(), ptr(p), op(o), lhs(colLhs)
    {
    }

    template<typename T, size_t Index, typename V>
    ComparisonExpression<T, Index, V>& ComparisonExpression<T, Index, V>::operator=(const ComparisonExpression& other)
    {
        column = other.column;
        value  = other.value;
        ptr    = other.ptr;
        op     = other.op;
        lhs    = other.lhs;
        index  = other.index;
        return *this;
    }

    template<typename T, size_t Index, typename V>
    ComparisonExpression<T, Index, V>&
      ComparisonExpression<T, Index, V>::operator=(ComparisonExpression&& other) noexcept
    {
        column = std::move(other.column);
        value  = other.value;
        ptr    = other.ptr;
        op     = other.op;
        lhs    = other.lhs;
        index  = other.index;
        return *this;
    }

    template<typename T, size_t Index, typename V>
    std::string ComparisonExpression<T, Index, V>::toString(int32_t& pIndex)
    {
        // Store unincremented index value. Use incremented value in string, because sqlite parameter indices start at 1.
        index = pIndex++;

        // Get column and operator string.
        auto        col = column.toString(pIndex);
        std::string o;
        switch (op)
        {
        case Operator::Eq: o = "="; break;
        case Operator::Ne: o = "!="; break;
        case Operator::Lt: o = "<"; break;
        case Operator::Gt: o = ">"; break;
        case Operator::Le: o = "<="; break;
        case Operator::Ge: o = ">="; break;
        }

        // Format with column on LHS or RHS.
        return lhs ? std::format("{0} {1} ?{2}", std::move(col), std::move(o), pIndex) :
                     std::format("?{2} {1} {0}", std::move(col), std::move(o), pIndex);
    }

    template<typename T, size_t Index, typename V>
    void ComparisonExpression<T, Index, V>::bind(Statement& stmt, const BindParameters bind) const
    {
        Result res;
        if (any(bind & BindParameters::Fixed) && !ptr) res = stmt.bind(index + Statement::getFirstBindIndex(), value);
        if (!res) throw SqliteError(std::format("Failed to bind parameter."), res.code);
        if (any(bind & BindParameters::Dynamic) && ptr) res = stmt.bind(index + Statement::getFirstBindIndex(), *ptr);
        if (!res) throw SqliteError(std::format("Failed to bind parameter."), res.code);
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
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator==(ColumnExpression<T, Index>&& col, V val)
    {
        using C = ComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), std::move(val), C::Operator::Eq, true);
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
        using C = ComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Eq, false);
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
        using C = ComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Eq, true);
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
        using C = ComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Eq, false);
    }

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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator!=(ColumnExpression<T, Index> col, V val)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator!=(V val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator!=(ColumnExpression<T, Index> col, V* val)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator!=(V* val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<(ColumnExpression<T, Index> col, V val)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<(V val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<(ColumnExpression<T, Index> col, V* val)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<(V* val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>(ColumnExpression<T, Index> col, V val)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>(V val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>(ColumnExpression<T, Index> col, V* val)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>(V* val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<=(ColumnExpression<T, Index> col, V val)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<=(V val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<=(ColumnExpression<T, Index> col, V* val)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator<=(V* val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>=(ColumnExpression<T, Index> col, V val)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>=(V val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>=(ColumnExpression<T, Index> col, V* val)
    {
        using C = ComparisonExpression<T, Index, V>;
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
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index, typename V>
    requires(std::convertible_to<V, col_t<Index, T>>) auto operator>=(V* val, ColumnExpression<T, Index> col)
    {
        using C = ComparisonExpression<T, Index, V>;
        using D = ColumnExpression<T, Index>;
        return C(std::forward<D>(col), val, C::Operator::Ge, false);
    }
}  // namespace sql
