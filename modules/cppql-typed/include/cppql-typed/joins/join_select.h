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

#include "cppql-typed/expressions/column_expression.h"
#include "cppql-typed/expressions/filter_expression.h"
#include "cppql-typed/joins/type_traits.h"

namespace sql
{
    /*template<>
    std::string formatColumns(const std::initializer_list<size_t> columns)
    {
        std::string s;
        for (const auto index : columns) s += (s.empty() ? "" : ",") + table.getColumn(index).getName();
        return s;
    }*/

    template<typename J, is_column_expression C, is_column_expression... Cs>
    class JoinSelect
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using joint_t = J;
        using row_t   = std::tuple<C, Cs...>;
        joint_t join;
        row_t   columns;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        JoinSelect(const JoinSelect& other) : join(other.join) {}

        JoinSelect(JoinSelect&& other) noexcept : join(std::move(other.join)) {}

        JoinSelect(joint_t j, C c, Cs... cs) :
            join(std::move(j)), columns(std::make_tuple(std::move(c), std::move(cs)...))
        {
        }

        ~JoinSelect() = default;

        JoinSelect& operator=(const JoinSelect& other)
        {
            join = other.join;
            return *this;
        }

        JoinSelect& operator=(JoinSelect&& other) noexcept
        {
            join = std::move(other.join);
            return *this;
        }

        ////////////////////////////////////////////////////////////////
        // ...
        ////////////////////////////////////////////////////////////////

        template<typename Self>
        [[nodiscard]] std::string operator()(this Self&& self)
        {

            auto cols = [&self]<std::size_t I, std::size_t... Is>(std::index_sequence<I, Is...>, int32_t & index)
            {
                return std::get<I>(self.columns).toString(index) + (... + ("," + std::get<Is>(self.columns).toString(index)));
            };

            auto index = 0;
            auto sql   = std::format(
              "SELECT {0} FROM {1};", cols(std::index_sequence_for<C, Cs...>(), index), self.join.toString(index));

            return sql;
        }
    };

}  // namespace sql
