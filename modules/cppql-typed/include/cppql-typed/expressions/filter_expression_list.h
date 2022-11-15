#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/filter_expression.h"

namespace sql
{
    /**
     * \brief Holds a list of FilterExpression types. Can be used to bind all their parameters at once.
     * \tparam T std::tuple of FilterExpression types.
     */
    template<typename T>
    class FilterExpressionList final : public BaseFilterExpression
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        FilterExpressionList() = default;

        FilterExpressionList(const FilterExpressionList&) = default;

        FilterExpressionList(FilterExpressionList&&) noexcept = default;

        explicit FilterExpressionList(T f) noexcept : filters(f) {}

        ~FilterExpressionList() noexcept override = default;

        FilterExpressionList& operator=(const FilterExpressionList&) = default;

        FilterExpressionList& operator=(FilterExpressionList&&) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] bool containsTables(const auto&... tables) const
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>) -> bool
            {
               return  (std::get<Is>(filters).containsTables(tables...) && ...);
            };

            return f(std::make_index_sequence<std::tuple_size_v<T>>{}, filters);
        }

        void generateIndices(int32_t&) override {}

        [[nodiscard]] std::string toString() noexcept override
        {
            return {};
        }

        void bind(Statement& stmt, const BindParameters bind) const override
        {
            const auto b = [&stmt, &bind]<std::size_t... Is>(std::index_sequence<Is...>, const auto& tuple)
            {
                (std::get<Is>(tuple).bind(stmt, bind), ...);
            };
            
            b(std::make_index_sequence<std::tuple_size_v<T>>{}, filters);
        }

    private:
        T filters;
    };
}  // namespace sql
