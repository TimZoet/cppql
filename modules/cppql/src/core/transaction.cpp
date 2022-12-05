#include "cppql/core/transaction.h"

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <cassert>
#include <format>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql/core/database.h"
#include "cppql/core/statement.h"
#include "cppql/error/sqlite_error.h"

namespace sql
{
    Transaction::Transaction(Database& db, const Type type) : database(&db)
    {
        std::string t;
        switch (type)
        {
        case Type::Deferred: t = "DEFERRED"; break;
        case Type::Immediate: t = "IMMEDIATE"; break;
        case Type::Exclusive: t = "EXCLUSIVE"; break;
        }

        const auto stmt = database->createStatement(std::format("BEGIN {} TRANSACTION;", std::move(t)), true);
        if (const auto res = stmt.step(); !res) throw SqliteError("Failed to begin transaction.", res.code);
    }

    Transaction::~Transaction() noexcept
    {
        if (!committed) rollback();
    }

    void Transaction::commit()
    {
        assert(!committed);

        committed       = true;
        const auto stmt = database->createStatement("COMMIT TRANSACTION;", true);
        if (const auto res = stmt.step(); !res) throw SqliteError("Failed to commit transaction.", res.code);
    }

    void Transaction::rollback()
    {
        assert(!committed);

        committed       = true;
        const auto stmt = database->createStatement("ROLLBACK TRANSACTION;", true);
        if (const auto res = stmt.step(); !res) throw SqliteError("Failed to rollback transaction.", res.code);
    }

}  // namespace sql