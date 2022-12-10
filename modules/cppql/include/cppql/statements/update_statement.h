#pragma once

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/enums.h"
#include "cppql/core/statement.h"
#include "cppql/error/sqlite_error.h"
#include "cppql/typed/typed_table.h"

namespace sql
{
    /**
     * \brief The UpdateStatement class manages a prepared statement for updating existing rows in a table. It can be
     * constructed using a UpdateQuery.
     * \tparam Cols Types of the columns to update.
     */
    template<typename... Cols>
    class UpdateStatement
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        static constexpr size_t column_count = sizeof...(Cols);

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        UpdateStatement() = delete;

        UpdateStatement(StatementPtr statement, BaseFilterExpressionPtr filterExpression) :
            stmt(std::move(statement)), exp(std::move(filterExpression))
        {
        }

        UpdateStatement(const UpdateStatement&) = delete;

        UpdateStatement(UpdateStatement&& other) noexcept = default;

        ~UpdateStatement() noexcept = default;

        UpdateStatement& operator=(const UpdateStatement&) = delete;

        UpdateStatement& operator=(UpdateStatement&& other) noexcept = default;

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
            if (any(b) && self.exp) self.exp->bind(*self.stmt, b);
            return std::forward<Self>(self);
        }

        /**
         * \brief Update table.
         * \tparam Cs Column types.
         * \param values Values.
         */
        template<bindable... Cs>
            requires(sizeof...(Cs) == column_count)
        void operator()(Cs&&... values)
        {
            // Bind value parameters.
            if (const auto res = stmt->bind(Statement::getFirstBindIndex(), std::forward<Cs>(values)...); !res)
                throw SqliteError(std::format("Failed to bind parameters to update statement."), res.code);

            if (const auto res = stmt->step(); !res)
            {
                static_cast<void>(stmt->reset());
                throw SqliteError(std::format("Failed to step through update statement."), res.code);
            }

            if (const auto res = stmt->reset(); !res)
                throw SqliteError(std::format("Failed to reset update statement."), res.code);
        }

        /**
         * \brief Update table.
         * \tparam Cs Column types.
         * \param values Values.
         */
        template<bindable... Cs>
            requires(sizeof...(Cs) == column_count)
        void operator()(std::tuple<Cs...>&& values)
        {
            const auto unpack = [this]<std::size_t... Is>(std::index_sequence<Is...>, auto&& vals)
            {
                this->operator()(std::get<Is>(vals)...);
            };

            return unpack(std::index_sequence_for<Cs...>{}, std::forward<std::tuple<Cs...>>(values));
        }

    private:
        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////

        /**
         * \brief Pointer to statement.
         */
        StatementPtr stmt;

        /**
         * \brief Pointer to filter expression.
         */
        BaseFilterExpressionPtr exp;
    };
}  // namespace sql
