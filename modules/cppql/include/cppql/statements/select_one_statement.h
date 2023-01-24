#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/enums.h"
#include "cppql/error/cppql_error.h"
#include "cppql/typed/fwd.h"
#include "cppql/statements/select_statement.h"

namespace sql
{
    /**
     * \brief The SelectOneStatement class wraps around a Select instance. It uses the instance to return just one
     * result row.
     * \tparam R Return type.
     * \tparam Cs Types of the columns to retrieve.
     */
    template<typename R, typename... Cs>
        requires(constructible_from<R, Cs...>)
    class SelectOneStatement
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using select_t = SelectStatement<R, Cs...>;

        /**
         * \brief Row return type.
         */
        using return_t = R;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        SelectOneStatement() = delete;

        explicit SelectOneStatement(select_t&& select) : stmt(std::move(select)) {}

        SelectOneStatement(const SelectOneStatement&) = delete;

        SelectOneStatement(SelectOneStatement&& other) noexcept = default;

        ~SelectOneStatement() noexcept = default;

        SelectOneStatement& operator=(const SelectOneStatement&) = delete;

        SelectOneStatement& operator=(SelectOneStatement&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Run.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Bind parameters.
         * \tparam Self Self type.
         * \param self Self.
         * \param b Parameters to bind.
         */
        template<typename Self>
        auto&& bind(this Self&& self, const BindParameters b)
        {
            self.stmt.bind(b);
            return std::forward<Self>(self);
        }

        /**
         * \brief Execute statement. Will throw if not exactly one row was returned.
         * \return Result row.
         */
        return_t operator()()
        {
            // Get iterator to execute statement.
            auto it = stmt.begin();

            // Check if there is a result.
            if (it == stmt.end())
            {
                stmt.reset();
                throw CppqlError("Select returned no result.");
            }

            // Get result.
            auto r = *it;

            // Check if there was only one result.
            if (++it != stmt.end())
            {
                stmt.reset();
                throw CppqlError("More than one result returned.");
            }

            return r;
        }

        void clearBindings() const
        {
            stmt.clearBindings();
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Select statement.
         */
        select_t stmt;
    };
}  // namespace sql
