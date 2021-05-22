#include "cppql_test/statement_prepare.h"

#include "sqlite3.h"

void StatementPrepare::operator()()
{
    // Valid statements.

    {
        // Create unprepared statement.
        auto stmt = sql::Statement(*db, "CREATE TABLE myTable (col1 INTEGER);", false);
        compareFalse(stmt.isPrepared());
        // Prepare statement.
        compareEQ(stmt.prepare().code, SQLITE_OK);
        compareTrue(stmt.isPrepared());
        // Another prepare should fail.
        compareEQ(stmt.prepare().code, SQLITE_ERROR);
        compareTrue(stmt.isPrepared());
    }

    {
        // Create and immediately prepare statement.
        auto stmt = sql::Statement(*db, "CREATE TABLE myTable (col1 INTEGER);", true);
        compareTrue(stmt.isPrepared());
        // Another prepare should be fail.
        compareEQ(stmt.prepare().code, SQLITE_ERROR);
        compareTrue(stmt.isPrepared());
    }

    // Invalid statements.

    {
        // Create unprepared statement.
        auto stmt = sql::Statement(*db, "serfgrtgfewaferf", false);
        compareFalse(stmt.isPrepared());
        // Prepare should fail.
        compareNE(stmt.prepare().code, SQLITE_OK);
        compareFalse(stmt.isPrepared());
    }

    {
        // Create and immediately prepare statement, which should fail.
        auto stmt = sql::Statement(*db, "serfgrtgfewaferf", true);
        compareFalse(stmt.isPrepared());
        compareNE(stmt.prepare().code, SQLITE_OK);
        compareFalse(stmt.isPrepared());
    }
}
