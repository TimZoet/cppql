#include "cppql_test/statement_step.h"

#include "sqlite3.h"

void StatementStep::operator()()
{
    {
        // Create table.
        const auto stmt = sql::Statement(*db, "CREATE TABLE myTable (col1 INTEGER);", true);
        compareTrue(stmt.isPrepared());
        compareTrue(stmt.step());
    }

    {
        // Insert.
        const auto stmt = sql::Statement(*db, "INSERT INTO myTable VALUES (10);", true);
        compareTrue(stmt.isPrepared());
        compareTrue(stmt.step());
        compareTrue(stmt.reset());
        compareTrue(stmt.step());
        // Since inserts do not return values, calling step without reset should also work.
        compareTrue(stmt.step());
    }

    {
        // Select.
        const auto stmt = sql::Statement(*db, "SELECT col1 FROM myTable;", true);
        compareTrue(stmt.isPrepared());
        compareEQ(stmt.step().code, SQLITE_ROW);
        compareEQ(stmt.step().code, SQLITE_ROW);
        compareEQ(stmt.step().code, SQLITE_ROW);
        compareEQ(stmt.step().code, SQLITE_DONE);
    }
}
