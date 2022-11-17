#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/error/sqlite_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/fwd.h"
#include "cppql-typed/expressions/column_expression.h"

namespace sql
{
    template<is_column_expression C, bool Dynamic>
    class LikeExpression
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using col_t = C;

        using value_t = std::conditional_t<Dynamic, std::string*, std::string>;

        using table_list_t = std::tuple<typename C::table_t>;

        using unique_table_list_t = table_list_t;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        LikeExpression() = delete;

        LikeExpression(const LikeExpression& other) = default;

        LikeExpression(LikeExpression&& other) noexcept = default;

        LikeExpression(col_t col, value_t val) : column(std::move(col)), value(std::move(val)) {}

        ~LikeExpression() noexcept = default;

        LikeExpression& operator=(const LikeExpression& other) = default;

        LikeExpression& operator=(LikeExpression&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool containsTables(const auto&... tables) const { return column.containsTables(tables...); }

        void generateIndices(int32_t& idx)
        {
            index = idx++;
            column.generateIndices(idx);
        }

        /**
         * \brief Generate expression doing a pattern match on a column.
         * \return String with format "<col> LIKE ?<index>".
         */
        [[nodiscard]] std::string toString() { return std::format("{0} LIKE ?{1}", column.fullName(), index + 1); }

        void bind(Statement& stmt, const BindParameters bind) const
        {
            if constexpr (Dynamic)
            {
                if (any(bind & BindParameters::Dynamic))
                {
                    if (value)
                    {
                        const auto res = stmt.bind(index + Statement::getFirstBindIndex(), sql::toText(*value));
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
                    const auto res = stmt.bind(index + Statement::getFirstBindIndex(), sql::toText(value));
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

    template<typename T, bool Dynamic>
    struct _is_filter_expression<LikeExpression<T, Dynamic>> : std::true_type
    {
    };

    ////////////////////////////////////////////////////////////////
    // like()
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column LIKE fixed value.
     * \tparam C ColumnExpression type.
     * \param col Column object.
     * \param val Value.
     * \return LikeExpression object.
     */
    template<is_column_expression C>
    auto like(C&& col, std::string val)
    {
        return LikeExpression<C, false>(std::forward<C>(col), std::move(val));
    }

    /**
     * \brief Require column LIKE dynamic value.
     * \tparam C ColumnExpression type.
     * \param col Column object.
     * \param val Value.
     * \return LikeExpression object.
     */
    template<is_column_expression C>
    auto like(C&& col, std::string* val)
    {
        return LikeExpression<C, true>(std::forward<C>(col), val);
    }
}  // namespace sql
