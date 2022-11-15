#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/table.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/base_filter_expression.h"
#include "cppql-typed/type_traits.h"

namespace sql
{
    template<typename T>
    class FilterExpression : public BaseFilterExpression
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using table_list_t                 = T;
        using unique_table_list_t          = tuple_unique_t<table_list_t>;
        static constexpr auto count        = std::tuple_size_v<table_list_t>;
        static constexpr auto unique_count = std::tuple_size_v<unique_table_list_t>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        FilterExpression() = default;

        FilterExpression(const FilterExpression&) = default;

        FilterExpression(FilterExpression&&) noexcept = default;

        ~FilterExpression() noexcept override = default;

        FilterExpression& operator=(const FilterExpression&) = default;

        FilterExpression& operator=(FilterExpression&&) noexcept = default;
    };

    // TODO: Rework these.

    template<typename T>
    concept is_filter_expression =
      std::derived_from<std::remove_cvref_t<T>, FilterExpression<typename std::remove_cvref_t<T>::table_list_t>>;

    template<typename T, typename Table>
    concept is_single_filter_expression =
      T::unique_count == 1 && tuple_contains_type<Table, typename T::unique_table_list_t>;

    template<typename T, typename Table>
    concept is_single_filter_expression_or_none =
      is_single_filter_expression<T, Table> || std::same_as<std::remove_cvref_t<T>, std::nullopt_t>;

    template<typename F, typename Tables>
    concept is_valid_filter_expression =
      is_filter_expression<F> && tuple_is_subset<typename std::remove_cvref_t<F>::unique_table_list_t, Tables>;
}  // namespace sql
