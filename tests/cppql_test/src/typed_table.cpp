#include "cppql_test/typed_table.h"

#include "cppql/ext/typed_table.h"

void TypedTable::operator()()
{
    auto& table = db->createTable("myTable");
    table.createColumn("col1", sql::Column::Type::Int);
    table.createColumn("col2", sql::Column::Type::Real);
    table.createColumn("col3", sql::Column::Type::Text);
    table.commit();

}
