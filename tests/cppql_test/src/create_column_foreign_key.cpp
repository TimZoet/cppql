#include "cppql_test/create_column_foreign_key.h"

void CreateColumnForeignKey::operator()()
{
    // Create database and table(s).
    create();
    // Open database and verify contents.
    reopen();
    verify();
}

void CreateColumnForeignKey::create()
{
    // Create 1st table.
    sql::Table* table1;
    expectNoThrow([&table1, this] { table1 = &db->createTable("table1"); });
    sql::Column* idCol;
    expectNoThrow([&table1, &idCol]() { idCol = &table1->createColumn("id", sql::Column::Type::Int); });
    expectNoThrow([&idCol]() { idCol->setAutoIncrement(true).setPrimaryKey(true).setNotNull(true); });

    // Create 2nd table with reference to 1st.
    sql::Table* table2;
    expectNoThrow([&table2, this] { table2 = &db->createTable("table2"); });
    sql::Column* refCol;
    expectNoThrow([&idCol, &table2, &refCol]() { refCol = &table2->createColumn("ref", *idCol); });

    // Check column types.
    compareEQ(idCol->getType(), sql::Column::Type::Int);
    compareTrue(idCol->isAutoIncrement());
    compareTrue(idCol->isPrimaryKey());
    compareTrue(idCol->isNotNull());
    compareEQ(refCol->getType(), sql::Column::Type::Int);
    compareTrue(refCol->isForeignKey());
    compareEQ(refCol->getForeignKey(), idCol);

    // Commit tables. Committing 2nd first should throw.
    expectThrow([&table2]() { table2->commit(); });
    expectNoThrow([&table1]() { table1->commit(); });
    expectNoThrow([&table2]() { table2->commit(); });
    compareTrue(table1->isCommitted());
    compareTrue(table2->isCommitted());
}

void CreateColumnForeignKey::verify()
{
    // Try to get tables.
    sql::Table *table1, *table2;
    expectNoThrow([&table1, this]() { table1 = &db->getTable("table1"); });
    expectNoThrow([&table2, this]() { table2 = &db->getTable("table2"); });

    // Check column types.
    const auto& cols1 = table1->getColumns();
    compareEQ(cols1.find("id")->second->getType(), sql::Column::Type::Int);
    const auto& cols2 = table2->getColumns();
    compareEQ(cols2.find("ref")->second->getType(), sql::Column::Type::Int);
    compareTrue(cols2.find("ref")->second->isForeignKey());
    compareEQ(cols2.find("ref")->second->getForeignKey(), cols1.find("id")->second.get());
}
