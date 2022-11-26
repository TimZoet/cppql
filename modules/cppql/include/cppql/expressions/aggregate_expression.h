#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <string>
#include <tuple>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/typed/enums.h"
#include "cppql/typed/fwd.h"

namespace sql
{
    /**
     * \brief The AggregateExpression class holds a column and an aggregate function type.
     * \tparam C ColumnExpression type.
     * \tparam A Aggregate function type.
     */
    template<is_column_expression C, is_aggregate_function A>
    class AggregateExpression
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using col_t = C;

        using aggregate_t = A;

        using value_t = typename aggregate_t::value_t;

        using table_t = typename C::table_t;

        using table_list_t = std::tuple<table_t>;

        using unique_table_list_t = table_list_t;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        AggregateExpression() = delete;

        AggregateExpression(const AggregateExpression& other) = default;

        AggregateExpression(AggregateExpression&& other) noexcept = default;

        explicit AggregateExpression(col_t col) : column(std::move(col)) {}

        ~AggregateExpression() noexcept = default;

        AggregateExpression& operator=(const AggregateExpression& other) = default;

        AggregateExpression& operator=(AggregateExpression&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool containsTables(const auto&... tables) const { return column.containsTables(tables...); }

        void generateIndices(int32_t& idx) { column.generateIndices(idx); }

        /**
         * \brief Generate aggregate expression on a column.
         * \return String with format "<func>(<col>)".
         */
        [[nodiscard]] std::string toString() const { return A::toString(column.fullName()); }

        static void bind(Statement&, const BindParameters) {}

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Column.
         */
        col_t column;
    };

    template<typename C, typename A>
    struct _is_filter_expression<AggregateExpression<C, A>> : std::true_type
    {
    };
}  // namespace sql
