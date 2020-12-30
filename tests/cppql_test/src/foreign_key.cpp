#include "cppql_test/foreign_key.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

namespace test
{
    void ForeignKey::operator()()
    {
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / ".db";
        std::filesystem::remove(dbPath);

        auto db = sql::Database::create(dbPath);

        // Create 1st table.
        auto& table1 = db->createTable("table1");
        auto& idCol  = table1.createColumn("id", sql::Column::Type::Int);
        idCol.setAutoIncrement(true);
        idCol.setPrimaryKey(true);

        // Create 2nd table with reference to 1st.
        auto& table2 = db->createTable("table2");
        auto& refCol = table2.createColumn("ref", idCol);

        // Commit tables. Committing 2nd first should throw.
        expectThrow([&table2]() { table2.commit(); });
        expectNoThrow([&table1]() { table1.commit(); });
        expectNoThrow([&table2]() { table2.commit(); });

        std::filesystem::remove(dbPath);
    }
}  // namespace test
