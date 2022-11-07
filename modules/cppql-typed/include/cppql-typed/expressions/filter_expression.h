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

#include "cppql-typed/expressions/bind_parameters.h"
#include "cppql-typed/type_traits.h"

namespace sql
{
    class BaseFilterExpression
    {
    public:

        virtual ~BaseFilterExpression() = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Generate SQL.
         * \param pIndex Reference to counter that is incremented for each parameter.
         * \return SQL code.
         */
        [[nodiscard]] virtual std::string toString(int32_t& pIndex) = 0;

        /**
         * \brief Bind all parameters in this expression to the statement.
         * \param bind Parameters to bind.
         * \param stmt Statement object.
         */
        virtual void bind(Statement& stmt, BindParameters bind) const = 0;
    };

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

        explicit FilterExpression(Table& t) noexcept : table(&t) {}

        virtual ~FilterExpression() = default;

        FilterExpression& operator=(const FilterExpression&) = default;

        FilterExpression& operator=(FilterExpression&&) noexcept = default;

    protected:
        Table* table = nullptr;
    };

    using BaseFilterExpressionPtr = std::unique_ptr<BaseFilterExpression>;

    template<typename T>
    concept is_filter_expression =
      std::derived_from<T, FilterExpression<typename T::table_list_t>>;

    template<typename T, typename Table>
    concept is_single_filter_expression =
      T::unique_count == 1 && tuple_contains_type<Table, typename T::unique_table_list_t>;

    template<typename T, typename Table>
    concept is_single_filter_expression_or_none =
      is_single_filter_expression<T, Table> || std::same_as<std::remove_cvref_t<T>, std::nullopt_t>;

    template<typename F, typename Tables>
    concept is_valid_filter_expression = is_filter_expression<F> && tuple_is_subset<typename F::unique_table_list_t, Tables>;
}  // namespace sql
