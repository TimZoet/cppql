#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <string>
#include <tuple>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/enums.h"
#include "cppql-typed/queries/fwd.h"

namespace sql
{
    template<typename...>
    class Union
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid = false;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Union() = default;

        Union(const Union& other) = default;

        Union(Union&& other) noexcept = default;

        ~Union() noexcept = default;

        Union& operator=(const Union& other) = default;

        Union& operator=(Union&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        static void generateIndices(int32_t&) {}

        [[nodiscard]] static std::string toString() { return {}; }
    };

    template<is_select_query... Qs>
    class Union<Qs...>
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr bool valid = true;
        using query_t               = std::tuple<Qs...>;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Union() = delete;

        Union(const Union& other) = default;

        Union(Union&& other) noexcept = default;

        explicit Union(const UnionOperator op, auto q) : query(std::make_tuple<Qs...>(std::move(q)))
        {
            std::get<0>(query).unionOp = op;
        }

        // Appends a new query to a previous tuple of queries.
        Union(const UnionOperator op, auto prev, auto q) :
            query(std::tuple_cat(std::move(prev), std::make_tuple(std::move(q))))
        {
            std::get<tuple_index_wrapped_v<query_t, -1>>(query).unionOp = op;
        }

        ~Union() noexcept = default;

        Union& operator=(const Union& other) = default;

        Union& operator=(Union&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        void generateIndices(int32_t& idx)
        {
            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                (std::get<Is>(query).generateIndices(idx), ...);
            };
            f(std::index_sequence_for<Qs...>{});
        }

        /**
         * \brief Generate UNION clause with queries.
         * \return String with format "<union-0> <query-0> .... <union-N> <query-N>".
         */
        [[nodiscard]] std::string toString()
        {
            std::string s;

            const auto opstring = [](const UnionOperator op) -> std::string {
                switch (op)
                {
                case UnionOperator::Union: return UnionString<UnionOperator::Union>::str;
                case UnionOperator::UnionAll: return UnionString<UnionOperator::UnionAll>::str;
                case UnionOperator::Intersect: return UnionString<UnionOperator::Intersect>::str;
                case UnionOperator::Except: return UnionString<UnionOperator::Except>::str;
                }
                return "";
            };

            const auto f = [&]<std::size_t... Is>(std::index_sequence<Is...>)
            {
                ((s += opstring(std::get<Is>(query).unionOp) + " " + std::get<Is>(query).toString()), ...);
            };
            f(std::index_sequence_for<Qs...>{});

            return s;
        }

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        query_t query;
    };

    template<typename Q, typename...>
    struct merge_unions
    {
        using type = Union<Q>;
    };

    template<typename Q, typename... Qs>
    struct merge_unions<Q, std::tuple<Qs...>>
    {
        using type = Union<Qs..., Q>;
    };

    template<typename Q, typename Prev>
    using merge_unions_t = typename merge_unions<Q, Prev>::type;
}  // namespace sql
