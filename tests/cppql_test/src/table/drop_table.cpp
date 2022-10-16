#include "cppql_test/table/drop_table.h"

void DropTable::operator()()
{
    auto& table1 = db->createTable("table1");
    table1.createColumn<float>("col");
    table1.commit();
    auto& table2 = db->createTable("table2");
    table2.createColumn<float>("col");
    table2.commit();

    expectNoThrow([this] { db->dropTable("table1"); });
    expectNoThrow([this] { db->dropTable("table2"); });
    expectThrow([this] { db->dropTable("table2"); });
    expectThrow([this] { db->dropTable("table2"); });
}
