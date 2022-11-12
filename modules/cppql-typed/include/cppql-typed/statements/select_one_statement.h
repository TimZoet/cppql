#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/error/cppql_error.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/expressions/bind_parameters.h"
#include "cppql-typed/statements/select_statement.h"

namespace sql
{
    /**
     * \brief The SelectOneStatement class wraps around a Select instance. It uses the instance to return just one result row.
     * \tparam R Return type.
     * \tparam Cs Types of the columns to retrieve.
     */
    template<typename R, typename... Cs>
    requires(constructible_from<R, Cs...>) class SelectOneStatement
    {
    public:
        using select_t = SelectStatement<R, Cs...>;

        template<typename U, typename... Us>
        friend class TypedTable;

        SelectOneStatement() = delete;

        SelectOneStatement(const SelectOneStatement&) = delete;

        SelectOneStatement(SelectOneStatement&& other) noexcept : stmt(std::move(other.stmt)) {}

        explicit SelectOneStatement(select_t&& select) : stmt(std::move(select)) {}

        ~SelectOneStatement() = default;

        SelectOneStatement& operator=(const SelectOneStatement&) = delete;

        SelectOneStatement& operator=(SelectOneStatement&& other) noexcept
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

        /**
         * \brief Select statement.
         */
        select_t stmt;
    };
}  // namespace sql
