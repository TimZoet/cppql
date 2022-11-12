#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/statement.h"
#include "cppql-core/table.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/filter_expression.h"

namespace sql
{
    /**
     * \brief 
     * \tparam T std::tuple of FilterExpression types.
     */
    template<typename T>
    class FilterExpressionList : public BaseFilterExpression
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

        [[nodiscard]] std::string toString(int32_t&) noexcept override
        {
            return {};
        }

        void bind(Statement& stmt, BindParameters bind) const override
        {
            auto b = [&stmt, &bind]<std::size_t... Is>(std::index_sequence<Is...>, const auto& tuple)
            {
                (std::get<Is>(tuple).bind(stmt, bind), ...);
            };
            
            b(std::make_index_sequence<std::tuple_size_v<T>>{}, filters);
        }

    private:
        T filters;
    };
}  // namespace sql
