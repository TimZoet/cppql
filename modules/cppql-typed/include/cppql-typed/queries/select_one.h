#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/queries/select.h"

namespace sql
{
    /**
     * \brief The SelectOne class wraps around a Select instance. It uses the instance to return just one result row.
     * \tparam T Table type.
     * \tparam R Return type.
     * \tparam Indices 0-based indices of the columns to retrieve. Duplicate values and reordering are allowed.
     */
    template<typename T, typename R, size_t... Indices>
    requires(constructible_from<R, T, Indices...>) class SelectOne
    {
    public:
        using select_t = Select<T, R, Indices...>;

        template<typename C, typename... Cs>
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
            if (it == stmt.end()) throw std::runtime_error("Select returned no result");

            // Get result.
            auto r = *it;

            // Check if there was only one result.
            if (++it != stmt.end()) throw std::runtime_error("More than one result returned");

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
