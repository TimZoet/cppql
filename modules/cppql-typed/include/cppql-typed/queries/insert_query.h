#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <format>
#include <string>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/clauses/columns.h"
#include "cppql-typed/expressions/column_expression.h"
#include "cppql-typed/statements/insert_statement.h"

namespace sql
{
    template<is_typed_table T, is_column_expression... Cs>
    class InsertQuery
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        using table_t   = T;
        using columns_t = Columns<Cs...>;

        Table*    table;
        columns_t columns;

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        InsertQuery() = delete;

        InsertQuery(const InsertQuery& other) = default;

        InsertQuery(InsertQuery&& other) noexcept = default;

        InsertQuery(Table& t, columns_t cs) : table(&t), columns(std::move(cs)) {}

        ~InsertQuery() noexcept = default;

        InsertQuery& operator=(const InsertQuery& other) = default;

        InsertQuery& operator=(InsertQuery&& other) noexcept = default;

        ////////////////////////////////////////////////////////////////
        // Generate.
        ////////////////////////////////////////////////////////////////

        [[nodiscard]] std::string toString() const
        {
            if constexpr (columns_t::size == 0)
            {
                return std::format("INSERT INTO {0} DEFAULT VALUES", table->getName());
            }
            else
            {
                std::string vals = "?1";
                for (size_t i = 1; i < columns_t::size; i++) vals += std::format(",?{0}", i + 1);
                return std::format(
                  "INSERT INTO {0} ({1}) VALUES ({2})", table->getName(), columns.toString(), std::move(vals));
            }
        }

        /**
         * \brief Generate InsertStatement object. Generates and compiles SQL code.
         * \tparam Self Self type.
         * \param self Self.
         * \return InsertStatement.
         */
        template<typename Self>
        [[nodiscard]] auto compile(this Self&& self)
        {
            // Construct statement. Note: This generates the bind indices of all filter expressions
            // and should therefore happen before the BaseFilterExpressionPtr construction below.
            auto stmt = std::make_unique<Statement>(self.table->getDatabase(), self.toString(), true);
            if (!stmt->isPrepared())
                throw SqliteError(std::format("Failed to prepare statement \"{}\"", stmt->getSql()),
                                  stmt->getResult()->code);

            return InsertStatement<Cs...>(std::move(stmt));
        }
    };
}  // namespace sql
