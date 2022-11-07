#pragma once
#if 0
////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <memory>
#include <optional>
#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/base_filter_expression.h"
#include "cppql-typed/expressions/bind_parameters.h"

namespace sql
{
    ////////////////////////////////////////////////////////////////
    // SingleFilterExpression class.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief The FilterExpression class is the base class for all
     * classes used in conjunction with query objects to generate
     * the WHERE <expr> part of a statement (without the WHERE).
     * \tparam T Table type.
     */
    template<typename T>
    class SingleFilterExpression : public BaseFilterExpression
    {
    public:
        using table_t = T;

        SingleFilterExpression() = default;

        SingleFilterExpression(const SingleFilterExpression&) = default;

        SingleFilterExpression(SingleFilterExpression&&) noexcept = default;

        ~SingleFilterExpression() noexcept override = default;

        SingleFilterExpression& operator=(const SingleFilterExpression&) = default;

        SingleFilterExpression& operator=(SingleFilterExpression&&) noexcept = default;

        [[nodiscard]] virtual std::unique_ptr<SingleFilterExpression<T>> cloneSingle() const = 0;
    };

    ////////////////////////////////////////////////////////////////
    // Type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    using SingleFilterExpressionPtr = std::unique_ptr<SingleFilterExpression<T>>;

    template<typename T, typename Table>
    concept is_single_filter_expression = std::derived_from<T, SingleFilterExpression<Table>>;

    template<typename T, typename Table>
    concept is_single_filter_expression_or_none =
      is_single_filter_expression<T, Table> || std::same_as<std::remove_cvref_t<T>, std::nullopt_t>;

    template<typename T>
    concept _is_single_filter_expression = std::derived_from<T, SingleFilterExpression<typename T::table_t>>;
}  // namespace sql
#endif