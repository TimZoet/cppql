#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <tuple>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"
#include "cppql-core/table.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/clauses/columns.h"
#include "cppql-typed/expressions/filter_expression.h"
#include "cppql-typed/joins/type_traits.h"
#include "cppql-typed/queries/complex_select.h"

namespace sql
{
    ////////////////////////////////////////////////////////////////

    template<typename T>
    struct get_table_list
    {
        using table_list_t = std::tuple<T>;
    };

    template<typename T>
        requires(is_join<T>)
    struct get_table_list<T>
    {
        using table_list_t = typename T::table_list_t;
    };

    template<typename T>
    using get_table_list_t = typename get_table_list<T>::table_list_t;

    ////////////////////////////////////////////////////////////////

    template<bool B, typename L, typename F>
    struct lazy_filter_list
    {
    };

    template<typename L, typename F>
    struct lazy_filter_list<true, L, F>
    {
        using type = tuple_cat_t<typename L::filter_list_t, F>;
    };

    template<typename L, typename F>
    struct lazy_filter_list<false, L, F>
    {
        using type = std::tuple<F>;
    };

    template<bool B, typename L, typename F>
    using lazy_filter_list_t = typename lazy_filter_list<B, L, F>::type;

    ////////////////////////////////////////////////////////////////

    template<typename L, typename R, typename F>
    class InnerJoin
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool recursive = is_join<L>;
        using left_t = std::conditional_t<recursive, L, Table*>;
        using table_t = R;
        using table_list_t = tuple_cat_t<get_table_list_t<L>, R>;
        using filter_list_t = lazy_filter_list_t<recursive, left_t, F>;

        left_t left;
        Table* right;
        F      filter;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        InnerJoin(const InnerJoin& other) : left(other.left), right(other.right), filter(other.filter) {}

        InnerJoin(InnerJoin&& other) noexcept :
            left(std::move(other.left)), right(other.right), filter(std::move(other.filter))
        {
        }

        InnerJoin(Table& l, Table& r, F f) : left(&l), right(&r), filter(std::move(f)) {}

        InnerJoin(L l, Table& r, F f) : left(std::move(l)), right(&r), filter(std::move(f)) {}

        ~InnerJoin() = default;

        InnerJoin& operator=(const InnerJoin& other)
        {
            left = other.left;
            right = other.right;
            filter = other.filter;
            return *this;
        }

        InnerJoin& operator=(InnerJoin&& other) noexcept
        {
            left = std::move(other.left);
            right = other.right;
            filter = std::move(other.filter);
            return *this;
        }

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        // TODO: Require T to be a TypedTable.
        template<typename Self, typename T, is_valid_filter_expression<tuple_cat_t<table_list_t, T>> F2>
        auto innerJoin(this Self&& self, T& table, F2&& filter)
        {
            // TODO: Check table instances in columns match tables in joins.
            return InnerJoin<Self, T, F2>(std::forward<Self>(self), table.getTable(), std::forward<F2>(filter));
        }

        template<typename Self,
            is_valid_column_expression<table_list_t> C,
            is_valid_column_expression<table_list_t>... Cs>
        auto select(this Self&& self, C&& c, Cs&&... cs)
        {
            // TODO: Check table instances in columns match tables in joins.

            return ComplexSelect<std::decay_t<Self>, std::nullopt_t, std::nullopt_t, std::decay_t<C>, std::decay_t<Cs>...>(
                std::forward<Self>(self),
                Columns<C, Cs...>(std::make_tuple(std::forward<C>(c), std::forward<Cs>(cs)...))
            );
        }

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] filter_list_t getFilters()
        {
            if constexpr (recursive)
                return std::tuple_cat(left.getFilters(), std::make_tuple(filter));
            else
                return std::make_tuple(filter);
        }

        [[nodiscard]] std::string toString(int32_t& pIndex)
        {
            if constexpr (recursive)
            {
                // Left contains more joins that must be stringified.
                return std::format(
                    "{0} INNER JOIN {1} ON {2}", left.toString(pIndex), right->getName(), filter.toString(pIndex));
            }
            else
            {
                // Left-most side of the join sequence. Left and right are both tables.
                return std::format(
                    "{0} INNER JOIN {1} ON {2}", left->getName(), right->getName(), filter.toString(pIndex));
            }
        }
    };

    // TODO: Require L and R to be TypedTables.
    template<typename L, typename R, is_valid_filter_expression<std::tuple<L, R>> F>
    InnerJoin<L, R, F> innerJoin(L& lhs, R& rhs, F&& filter)
    {
        return InnerJoin<L, R, F>(lhs.getTable(), rhs.getTable(), std::forward<F>(filter));
    }
}  // namespace sql
