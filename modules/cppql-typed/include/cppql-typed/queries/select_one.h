#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/error/cppql_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/bind_parameters.h"
#include "cppql-typed/queries/select.h"

namespace sql
{
    /**
     * \brief The SelectOne class wraps around a Select instance. It uses the instance to return just one result row.
     * \tparam R Return type.
     * \tparam Cs Types of the columns to retrieve.
     */
    template<typename R, typename... Cs>
    requires(constructible_from<R, Cs...>) class SelectOne
    {
    public:
        using select_t = Select<R, Cs...>;

        template<typename U, typename... Us>
        friend class TypedTable;

        SelectOne() = delete;

        SelectOne(const SelectOne&) = delete;

        SelectOne(SelectOne&& other) noexcept : stmt(std::move(other.stmt)) {}

        ~SelectOne() = default;

        SelectOne& operator=(const SelectOne&) = delete;

        SelectOne& operator=(SelectOne&& other) noexcept
        {
            stmt = std::move(other.stmt);
            return *this;
        }

        /**
         * \brief Execute statement.
         * \param bind Parameters to bind.
         * \return Result row.
         */
        typename select_t::return_t operator()(const BindParameters bind)
        {
            // Reset statement and possibly rebind parameters.
            if (any(bind)) stmt(bind);

            // Get iterator to execute statement.
            auto it = stmt.begin();

            // Check if there is a result.
            if (it == stmt.end()) throw CppqlError("Select returned no result.");

            // Get result.
            auto r = *it;

            // Check if there was only one result.
            if (++it != stmt.end()) throw CppqlError("More than one result returned.");

            return r;
        }

    private:
        explicit SelectOne(select_t&& select) : stmt(std::move(select)) {}

        /**
         * \brief Select statement.
         */
        select_t stmt;
    };
}  // namespace sql
