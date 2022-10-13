#include "cppql_test/database/database_vacuum.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/database.h"

#include "cppql-typed/typed_table.h"
#include "cppql-typed/queries/insert.h"

using namespace std::string_literals;

void DatabaseVacuum::operator()()
{
    // Generate some data.
    sql::Table* t;
    expectNoThrow([&t, this]() {
        t = &db->createTable("myTable");
        t->createColumn("col1", sql::Column::Type::Int);
        t->createColumn("col2", sql::Column::Type::Real);
        t->createColumn("col3", sql::Column::Type::Text);
        t->commit();
    });
    sql::TypedTable<int64_t, float, std::string> table(*t);
    auto                                              insert = table.insert();
    expectNoThrow([&insert]() { insert(10, 20.0f, "abc"s); });
    expectNoThrow([&insert]() { insert(20, 40.5f, "def"s); });
    expectNoThrow([&insert]() { insert(30, 80.2f, "ghij"s); });
    expectNoThrow([&insert]() { insert(40, 133.3f, "gh\0ij"s); });

    // Run VACUUM command.
    expectNoThrow([&] { db->vacuum(); });
}
