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

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/table.h"
#include "cppql/typed/join_type.h"
#include "cppql/clauses/columns.h"
#include "cppql/clauses/on.h"
#include "cppql/clauses/using.h"
#include "cppql/expressions/filter_expression.h"
#include "cppql/queries/select_query.h"

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

    /**
     * \brief The Join class can be used to construct a sql::SelectQuery that performs a join on two or more tables.
     * The join type and conditions can all be specified. It is constructed using the join method of the TypedTable
     * and can be extended using the same method on the resulting join query.
     * \tparam J JoinTypeWrapper.
     * \tparam L Join or TypedTable table. Left hand side of join.
     * \tparam R TypedTable type. Right hand side of join.
     * \tparam F Filter expression type (or std::nullopt_t if not yet initialized). Is used to generate the ON clause.
     * \tparam Cols List of ColumnExpression types (or empty if not yet initialized). Are used to generate the USING clause.
     */
    template<is_join_wrapper              J,
             is_join_or_typed_table       L,
             is_typed_table               R,
             is_filter_expression_or_none F,
             is_column_expression... Cols>
    class Join
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool recursive       = is_join<L>;
        using join_t                          = J;
        using left_t                          = std::conditional_t<recursive, L, const Table*>;
        using table_t                         = R;
        using filter_t                        = On<F>;
        using using_t                         = Using<Cols...>;
        using table_list_t                    = tuple_cat_t<get_table_list_t<L>, R>;
        using left_table_t                    = tuple_element_wrapped_t<table_list_t, -2>;
        using filter_list_t                   = lazy_filter_list_t<recursive, left_t, F>;
        static constexpr bool has_filter_list = std::tuple_size_v<filter_list_t> > 0;

        left_t       left;
        const Table* right;
        filter_t     filter;
        using_t      usingCols;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Join() = delete;

        Join(const Join& other) = default;

        Join(Join&& other) noexcept = default;

        Join(const Table& l, const Table& r) : left(&l), right(&r) {}

        Join(left_t l, const Table& r, filter_t f) : left(std::move(l)), right(&r), filter(std::move(f)) {}

        Join(left_t l, const Table& r, using_t u) : left(std::move(l)), right(&r), usingCols(std::move(u)) {}

        Join(left_t l, const Table& r) : left(std::move(l)), right(&r) {}

        ~Join() noexcept = default;

        Join& operator=(const Join& other) = default;

        Join& operator=(Join&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Getters.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] const Table& getTable() const noexcept { return *right; }

        [[nodiscard]] bool containsTables(const auto& expr, const auto&... tables) const
        {
            if constexpr (recursive)
                return left.containsTables(expr, *right, tables...);
            else
                return expr.containsTables(*left, *right, tables...);
        }

        ////////////////////////////////////////////////////////////////
        // Join,
        ////////////////////////////////////////////////////////////////

        template<typename Self, is_join_wrapper J2, is_typed_table T>
        auto join(this Self&& self, J2&&, T&& table)
        {
            return Join<std::remove_cvref_t<J2>, std::remove_cvref_t<Self>, std::remove_cvref_t<T>, std::nullopt_t>(
              std::forward<Self>(self), std::forward<T>(table).getTable());
        }

        template<typename Self, is_valid_filter_expression<table_list_t> F2>
            requires(!filter_t::valid && !using_t::valid && !join_t::natural)
        auto on(this Self&& self, F2&& filter)
        {
            if (!self.containsTables(filter))
                throw CppqlError(
                  std::format("Cannot apply filter to join because the expression contains a table not in the query."));

            return Join<join_t, L, R, std::remove_cvref_t<F2>>(
              std::forward<Self>(self).left, *self.right, On<std::remove_cvref_t<F2>>(std::forward<F2>(filter)));
        }

        template<typename Self, is_column_expression C, is_column_expression... Cs>
            requires(!filter_t::valid && !using_t::valid && !join_t::natural &&
                     std::same_as<std::tuple<C, Cs...>,
                                  tuple_unique_t<std::tuple<C, Cs...>>> &&  // Checks for a unique set of columns.
                     ((std::same_as<typename C::table_t, left_table_t> &&
                       ... &&  // Checks all columns come exclusively from the left or right table.
                       std::same_as<typename Cs::table_t, left_table_t>) ||
                      (std::same_as<typename C::table_t, table_t> && ... &&
                       std::same_as<typename Cs::table_t, table_t>)))
        auto usings(this Self&& self, C&& c, Cs&&... cs)
        {
            if ((!self.containsTables(c) || ... || !self.containsTables(cs)))
                throw CppqlError(
                  std::format("Cannot apply filter to join because the expression contains a table not in the query."));

            return Join<join_t, L, R, std::nullopt_t, std::remove_cvref_t<C>, std::remove_cvref_t<Cs>...>(
              std::forward<Self>(self).left,
              *self.right,
              Using<std::remove_cvref_t<C>, std::remove_cvref_t<Cs>...>(std::forward<C>(c), std::forward<Cs>(cs)...));
        }

        ////////////////////////////////////////////////////////////////
        // Select.
        ////////////////////////////////////////////////////////////////

        template<typename Self,
                 is_valid_result_expression<table_list_t> C,
                 is_valid_result_expression<table_list_t>... Cs>
        auto select(this Self&& self, C&& c, Cs&&... cs)
        {
            return std::forward<Self>(self)
              .template selectAs<
                std::tuple<typename std::remove_cvref_t<C>::value_t, typename std::remove_cvref_t<Cs>::value_t...>,
                Self,
                C,
                Cs...>(std::forward<C>(c), std::forward<Cs>(cs)...);
        }

        template<typename Return,
                 typename Self,
                 is_valid_result_expression<table_list_t> C,
                 is_valid_result_expression<table_list_t>... Cs>
        auto selectAs(this Self&& self, C&& c, Cs&&... cs)
        {
            if ((!self.containsTables(c) || ... || !self.containsTables(cs)))
                throw CppqlError(std::format("Cannot apply select to join because at least one of the columns contains "
                                             "a table not in the query."));

            return SelectQuery<Return,
                               std::remove_cvref_t<Self>,
                               std::nullopt_t,
                               std::nullopt_t,
                               std::nullopt_t,
                               std::nullopt_t,
                               std::nullopt_t,
                               std::nullopt_t,
                               std::remove_cvref_t<C>,
                               std::remove_cvref_t<Cs>...>(
              std::forward<Self>(self),
              Columns<std::remove_cvref_t<C>, std::remove_cvref_t<Cs>...>(std::forward<C>(c), std::forward<Cs>(cs)...));
        }

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        /*template<typename Self, typename... Ts>
        [[nodiscard]] auto getFilters(this Self&& self, Ts&&... filters)
        {
            if constexpr (recursive)
            {
                if constexpr (filter_t::valid)
                    return std::forward<Self>(self).left.getFilters(std::forward<Self>(self).filter.filter,
                                                                    std::forward<Ts>(filters)...);
                else
                    return std::forward<Self>(self).left.getFilters(std::forward<Ts>(filters)...);
            }
            else
            {
                if constexpr (filter_t::valid)
                    return std::make_unique<FilterExpression<F, std::remove_cvref_t<Ts>...>>(
                      std::forward<Self>(self).filter.filter, std::forward<Ts>(filters)...);
                else if constexpr (sizeof...(filters) > 0)
                    return std::make_unique<FilterExpression<std::remove_cvref_t<Ts>...>>(std::forward<Ts>(filters)...);
                else
                    return nullptr;
            }
        }*/
        template<typename Self, typename... Ts>
        [[nodiscard]] auto getFilters(this Self&& self, Ts&&... filters)
        {
            if constexpr (recursive)
            {
                if constexpr (filter_t::valid)
                    return std::forward<Self>(self).left.getFilters(std::forward<Self>(self).filter.filter,
                                                                    std::forward<Ts>(filters)...);
                else
                    return std::forward<Self>(self).left.getFilters(std::forward<Ts>(filters)...);
            }
            else
            {
                if constexpr (filter_t::valid)
                    return std::make_tuple(std::forward<Self>(self).filter.filter, std::forward<Ts>(filters)...);
                else if constexpr (sizeof...(filters) > 0)
                    return std::make_tuple(std::forward<Ts>(filters)...);
                else
                    return std::tuple<>{};
            }
        }

        void generateIndices(int32_t& idx)
        {
            if constexpr (recursive) left.generateIndices(idx);

            filter.generateIndices(idx);
        }

        [[nodiscard]] std::string toString()
        {
            if constexpr (recursive)
            {
                // Left contains more joins that must be stringified.
                return std::format("{0} {1} {2} {3}{4}",
                                   left.toString(),
                                   join_t::name,
                                   right->getName(),
                                   filter.toString(),
                                   usingCols.toString());
            }
            else
            {
                // Left-most side of the join sequence. Left and right are both tables.
                return std::format("{0} {1} {2} {3}{4}",
                                   left->getName(),
                                   join_t::name,
                                   right->getName(),
                                   filter.toString(),
                                   usingCols.toString());
            }
        }
    };
}  // namespace sql
