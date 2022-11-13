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

#include "cppql-typed/complex_select.h"
#include "cppql-typed/join_type.h"
#include "cppql-typed/clauses/columns.h"
#include "cppql-typed/clauses/on.h"
#include "cppql-typed/clauses/using.h"
#include "cppql-typed/expressions/filter_expression.h"

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

    template<typename L>
    struct lazy_filter_list<true, L, std::nullopt_t>
    {
        using type = typename L::filter_list_t;
    };

    template<typename L, typename F>
    struct lazy_filter_list<false, L, F>
    {
        using type = std::tuple<F>;
    };

    template<typename L>
    struct lazy_filter_list<false, L, std::nullopt_t>
    {
        using type = std::tuple<>;
    };

    template<bool B, typename L, typename F>
    using lazy_filter_list_t = typename lazy_filter_list<B, L, F>::type;

    ////////////////////////////////////////////////////////////////



    template<typename J, typename L, typename R, typename F, typename... Cs>
    class Join
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool recursive       = is_join<L>;
        using left_t                          = std::conditional_t<recursive, L, Table*>;
        using table_t                         = R;
        using filter_t                        = On<F>;
        using using_t                         = Using<Cs...>;
        using table_list_t                    = tuple_cat_t<get_table_list_t<L>, R>;
        using filter_list_t                   = lazy_filter_list_t<recursive, left_t, F>;
        static constexpr bool has_filter_list = std::tuple_size_v<filter_list_t> > 0;

        left_t   left;
        Table*   right;
        filter_t filter;
        using_t  usingCols;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Join() = delete;

        Join(const Join& other) = default;

        Join(Join&& other) noexcept = default;

        Join(Table& l, Table& r) : left(&l), right(&r), filter(std::nullopt), usingCols(std::nullopt) {}

        Join(left_t l, Table& r, filter_t f) :
            left(std::move(l)), right(&r), filter(std::move(f)), usingCols(std::nullopt)
        {
        }

        Join(left_t l, Table& r, using_t u) :
            left(std::move(l)), right(&r), filter(std::nullopt), usingCols(std::move(u))
        {
        }

        Join(left_t l, Table& r) : left(std::move(l)), right(&r), filter(std::nullopt), usingCols(std::nullopt) {}

        ~Join() noexcept = default;

        Join& operator=(const Join& other) = default;

        Join& operator=(Join&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        Table& getTable() noexcept { return *right; }

        [[nodiscard]] const Table& getTable() const noexcept { return *right; }

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        // TODO: Require J2 to be JoinWrapper, T to be a TypedTable.
        template<typename Self, typename J2, typename T>
        auto join(this Self&& self, J2&&, T& table)
        {
            // TODO: Check table instances in columns match tables in joins.
            return Join<J2, Self, T, std::nullopt_t>(std::forward<Self>(self), table.getTable());
        }

        template<typename Self, is_valid_filter_expression<table_list_t> F2>
            requires(!filter_t::valid && !using_t::valid && !J::natural)
        auto on(this Self&& self, F2&& filter)
        {
            // TODO: Check table instances in filter match tables in joins.
            return Join<J, L, R, std::decay_t<F2>>(
              std::forward<Self>(self).left, *self.right, On<std::decay_t<F2>>(std::forward<F2>(filter)));
        }

        template<typename Self, is_column_expression C, is_column_expression... Cs>
            requires(!filter_t::valid && !using_t::valid && !J::natural)
        auto usings(this Self&& self, C&& c, Cs&&... cs)
        {
            // TODO: Detect duplicates.
            // TODO: Check column types in left typedtable.
            // TODO: Runtime check column names are in left and right table.
            return Join<J, L, R, std::nullopt_t, std::decay_t<C>, std::decay_t<Cs>...>(
              std::forward<Self>(self).left,
              *self.right,
              Using<std::decay_t<C>, std::decay_t<Cs>...>(std::forward<C>(c), std::forward<Cs>(cs)...));
        }

        template<typename Self,
                 is_valid_column_expression<table_list_t> C,
                 is_valid_column_expression<table_list_t>... Cs>
        auto select(this Self&& self, C&& c, Cs&&... cs)
        {
            // TODO: Check table instances in columns match tables in joins.

            return ComplexSelect<std::nullopt_t,
                                 std::decay_t<Self>,
                                 std::nullopt_t,
                                 std::nullopt_t,
                                 std::decay_t<C>,
                                 std::decay_t<Cs>...>(
              std::forward<Self>(self),
              Columns<C, Cs...>(std::make_tuple(std::forward<C>(c), std::forward<Cs>(cs)...)));
        }

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        template<typename T = filter_list_t>
            requires(std::tuple_size_v<T> > 0)
        [[nodiscard]] filter_list_t getFilters()
        {
            if constexpr (recursive)
            {
                if constexpr (left_t::has_filter_list)
                    return std::tuple_cat(left.getFilters(), std::make_tuple(filter.filter));
                else
                    return std::make_tuple(filter.filter);
            }
            else
                return std::make_tuple(filter.filter);
        }

        [[nodiscard]] std::string toString(int32_t& pIndex)
        {
            if constexpr (recursive)
            {
                // Left contains more joins that must be stringified.
                return std::format("{0} {1} {2} {3}{4}",
                                   left.toString(pIndex),
                                   J::name,
                                   right->getName(),
                                   filter.toString(pIndex),
                                   usingCols.toString());
            }
            else
            {
                // Left-most side of the join sequence. Left and right are both tables.
                return std::format("{0} {1} {2} {3}{4}",
                                   left->getName(),
                                   J::name,
                                   right->getName(),
                                   filter.toString(pIndex),
                                   usingCols.toString());
            }
        }
    };
}  // namespace sql
