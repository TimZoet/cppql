#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <string>
#include <tuple>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/enum_classes.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/error/sqlite_error.h"
#include "cppql/expressions/column_expression.h"
#include "cppql/typed/enums.h"

namespace sql
{
    /**
     * \brief The ComparisonExpression class holds a column and a fixed or dynamic value to compare it with.
     * \tparam C ColumnExpression type.
     * \tparam V Value type. Can be pointer.
     * \tparam Op Operator.
     * \tparam Lhs Boolean indicating column is on left hand side of comparison.
     */
    template<is_result_expression C, typename V, ComparisonOperator Op, bool Lhs>
    class ComparisonExpression
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using col_t = C;

        using value_t = V;

        using table_list_t = std::tuple<typename C::table_t>;

        using unique_table_list_t = table_list_t;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        ComparisonExpression() = delete;

        ComparisonExpression(const ComparisonExpression& other) = default;

        ComparisonExpression(ComparisonExpression&& other) noexcept = default;

        ComparisonExpression(col_t col, value_t val) : column(std::move(col)), value(std::move(val)) {}

        ~ComparisonExpression() noexcept = default;

        ComparisonExpression& operator=(const ComparisonExpression& other) = default;

        ComparisonExpression& operator=(ComparisonExpression&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool containsTables(const auto&... tables) const { return column.containsTables(tables...); }

        void generateIndices(int32_t& idx) { index = idx++; }

        /**
         * \brief Generate expression comparing a column to a fixed or dynamic value.
         * \return String with format "<col> <op> ?<index>" or "?<index> <op> <col>".
         */
        [[nodiscard]] std::string toString()
        {
            // Format with column on LHS or RHS.
            if constexpr (Lhs)
                return std::format("{0} {1} ?{2}", column.toString(), ComparisonOperatorType<Op>::str, index + 1);
            else
                return std::format("?{2} {1} {0}", column.toString(), ComparisonOperatorType<Op>::str, index + 1);
        }

        void bind(Statement& stmt, const BindParameters bind) const
        {
            if constexpr (std::is_pointer_v<value_t>)
            {
                if (any(bind & BindParameters::Dynamic))
                {
                    if (value)
                    {
                        const auto res = stmt.bind(index + Statement::getFirstBindIndex(), *value);
                        if (!res) throw SqliteError(std::format("Failed to bind dynamic parameter."), res.code);
                    }
                    else
                    {
                        const auto res = stmt.bind(index + Statement::getFirstBindIndex(), nullptr);
                        if (!res) throw SqliteError(std::format("Failed to bind dynamic parameter."), res.code);
                    }
                }
            }
            else
            {
                if (any(bind & BindParameters::Fixed))
                {
                    const auto res = stmt.bind(index + Statement::getFirstBindIndex(), value);
                    if (!res) throw SqliteError(std::format("Failed to bind fixed parameter."), res.code);
                }
            }
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Column to compare.
         */
        col_t column;

        /**
         * \brief Value.
         */
        value_t value;

        /**
         * \brief Index for parameter binding.
         */
        int32_t index = -1;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename C, typename V, ComparisonOperator Op, bool Lhs>
    struct _is_filter_expression<ComparisonExpression<C, V, Op, Lhs>> : std::true_type
    {
    };

    ////////////////////////////////////////////////////////////////
    // ==
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column == fixed value.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator==(C&& col, V&& val)
    {
        return ComparisonExpression<C, V, ComparisonOperator::Eq, true>(std::forward<C>(col), std::forward<V>(val));
    }

    /**
     * \brief Require fixed value == column.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator==(V&& val, C&& col)
    {
        return ComparisonExpression<C, V, ComparisonOperator::Eq, false>(std::forward<C>(col), std::forward<V>(val));
    }

    /**
     * \brief Require column == dynamic value.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param ptr Pointer to value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator==(C&& col, V* ptr)
    {
        return ComparisonExpression<C, V*, ComparisonOperator::Eq, true>(std::forward<C>(col), ptr);
    }

    /**
     * \brief Require dynamic value == column.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param ptr Pointer to value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator==(V* ptr, C&& col)
    {
        return ComparisonExpression<C, V*, ComparisonOperator::Eq, false>(std::forward<C>(col), ptr);
    }

    /**
     * \brief Require column == NULL.
     * \tparam C ColumnExpression type.
     * \param col ColumnExpression object.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C>
    auto operator==(C&& col, std::nullptr_t)
    {
        return ComparisonExpression<C, typename C::value_t*, ComparisonOperator::Eq, true>(std::forward<C>(col),
                                                                                           nullptr);
    }

    /**
     * \brief Require NULL == column.
     * \tparam C ColumnExpression type.
     * \param col ColumnExpression object.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator==(std::nullptr_t, C&& col)
    {
        return ComparisonExpression<C, typename C::value_t*, ComparisonOperator::Eq, false>(std::forward<C>(col),
                                                                                            nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // !=
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column != fixed value.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator!=(C&& col, V&& val)
    {
        return ComparisonExpression<C, V, ComparisonOperator::Ne, true>(std::forward<C>(col), std::forward<V>(val));
    }

    /**
     * \brief Require fixed value != column.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator!=(V&& val, C&& col)
    {
        return ComparisonExpression<C, V, ComparisonOperator::Ne, false>(std::forward<C>(col), std::forward<V>(val));
    }

    /**
     * \brief Require column != dynamic value.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param ptr Pointer to value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator!=(C&& col, V* ptr)
    {
        return ComparisonExpression<C, V*, ComparisonOperator::Ne, true>(std::forward<C>(col), ptr);
    }

    /**
     * \brief Require dynamic value != column.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param ptr Pointer to value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator!=(V* ptr, C&& col)
    {
        return ComparisonExpression<C, V*, ComparisonOperator::Ne, false>(std::forward<C>(col), ptr);
    }

    /**
     * \brief Require column != NULL.
     * \tparam C ColumnExpression type.
     * \param col ColumnExpression object.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C>
    auto operator!=(C&& col, std::nullptr_t)
    {
        return ComparisonExpression<C, typename C::value_t*, ComparisonOperator::Ne, true>(std::forward<C>(col),
                                                                                           nullptr);
    }

    /**
     * \brief Require NULL != column.
     * \tparam C ColumnExpression type.
     * \param col ColumnExpression object.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator!=(std::nullptr_t, C&& col)
    {
        return ComparisonExpression<C, typename C::value_t*, ComparisonOperator::Ne, false>(std::forward<C>(col),
                                                                                            nullptr);
    }

    ////////////////////////////////////////////////////////////////
    // <
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column < fixed value.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator<(C&& col, V&& val)
    {
        return ComparisonExpression<C, V, ComparisonOperator::Lt, true>(std::forward<C>(col), std::forward<V>(val));
    }

    /**
     * \brief Require fixed value < column.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator<(V&& val, C&& col)
    {
        return ComparisonExpression<C, V, ComparisonOperator::Lt, false>(std::forward<C>(col), std::forward<V>(val));
    }

    /**
     * \brief Require column < dynamic value.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param ptr Pointer to value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator<(C&& col, V* ptr)
    {
        return ComparisonExpression<C, V*, ComparisonOperator::Lt, true>(std::forward<C>(col), ptr);
    }

    /**
     * \brief Require dynamic value < column.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param ptr Pointer to value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator<(V* ptr, C&& col)
    {
        return ComparisonExpression<C, V*, ComparisonOperator::Lt, false>(std::forward<C>(col), ptr);
    }

    ////////////////////////////////////////////////////////////////
    // >
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column > fixed value.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator>(C&& col, V&& val)
    {
        return ComparisonExpression<C, V, ComparisonOperator::Gt, true>(std::forward<C>(col), std::forward<V>(val));
    }

    /**
     * \brief Require fixed value > column.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator>(V&& val, C&& col)
    {
        return ComparisonExpression<C, V, ComparisonOperator::Gt, false>(std::forward<C>(col), std::forward<V>(val));
    }

    /**
     * \brief Require column > dynamic value.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param ptr Pointer to value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator>(C&& col, V* ptr)
    {
        return ComparisonExpression<C, V*, ComparisonOperator::Gt, true>(std::forward<C>(col), ptr);
    }

    /**
     * \brief Require dynamic value > column.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param ptr Pointer to value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator>(V* ptr, C&& col)
    {
        return ComparisonExpression<C, V*, ComparisonOperator::Gt, false>(std::forward<C>(col), ptr);
    }

    ////////////////////////////////////////////////////////////////
    // <=
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column <= fixed value.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator<=(C&& col, V&& val)
    {
        return ComparisonExpression<C, V, ComparisonOperator::Le, true>(std::forward<C>(col), std::forward<V>(val));
    }

    /**
     * \brief Require fixed value <= column.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator<=(V&& val, C&& col)
    {
        return ComparisonExpression<C, V, ComparisonOperator::Le, false>(std::forward<C>(col), std::forward<V>(val));
    }

    /**
     * \brief Require column <= dynamic value.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param ptr Pointer to value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator<=(C&& col, V* ptr)
    {
        return ComparisonExpression<C, V*, ComparisonOperator::Le, true>(std::forward<C>(col), ptr);
    }

    /**
     * \brief Require dynamic value <= column.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param ptr Pointer to value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator<=(V* ptr, C&& col)
    {
        return ComparisonExpression<C, V*, ComparisonOperator::Le, false>(std::forward<C>(col), ptr);
    }

    ////////////////////////////////////////////////////////////////
    // >=
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column >= fixed value.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator>=(C&& col, V&& val)
    {
        return ComparisonExpression<C, V, ComparisonOperator::Ge, true>(std::forward<C>(col), std::forward<V>(val));
    }

    /**
     * \brief Require fixed value >= column.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param val Value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator>=(V&& val, C&& col)
    {
        return ComparisonExpression<C, V, ComparisonOperator::Ge, false>(std::forward<C>(col), std::forward<V>(val));
    }

    /**
     * \brief Require column >= dynamic value.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param ptr Pointer to value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator>=(C&& col, V* ptr)
    {
        return ComparisonExpression<C, V*, ComparisonOperator::Ge, true>(std::forward<C>(col), ptr);
    }

    /**
     * \brief Require dynamic value >= column.
     * \tparam C ColumnExpression type.
     * \tparam V Value type.
     * \param col ColumnExpression object.
     * \param ptr Pointer to value.
     * \return ComparisonExpression object.
     */
    template<is_result_expression C, is_convertible_to<C> V>
    auto operator>=(V* ptr, C&& col)
    {
        return ComparisonExpression<C, V*, ComparisonOperator::Ge, false>(std::forward<C>(col), ptr);
    }
}  // namespace sql
