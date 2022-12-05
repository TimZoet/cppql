#include "cppql_test/transaction.h"

#include "cppql/include_all.h"

void Transaction::operator()()
{
    // Create table.
    sql::Table* t;
    expectNoThrow([&] {
        t = &db->createTable("MyTable");
        t->createColumn("col1", sql::Column::Type::Int);
        t->commit();
    });
    const sql::TypedTable<int64_t> table(*t);

    auto count = table.count().compile();

    // Insert and commit.
    expectNoThrow([&] {
        auto trans  = db->beginTransaction(sql::Transaction::Type::Deferred);
        auto insert = table.insert().compile();
        insert(10);
        insert(20);
        insert(30);
        insert(40);
        trans.commit();
    });
    compareEQ(4, count());

    // Insert and rollback.
    expectNoThrow([&] {
        auto trans  = db->beginTransaction(sql::Transaction::Type::Deferred);
        auto insert = table.insert().compile();
        insert(10);
        insert(20);
        insert(30);
        insert(40);
        trans.rollback();
    });
    compareEQ(4, count());

    // Insert and cause automatic rollback.
    expectNoThrow([&] {
        auto trans  = db->beginTransaction(sql::Transaction::Type::Deferred);
        auto insert = table.insert().compile();
        insert(10);
        insert(20);
        insert(30);
        insert(40);
    });
    compareEQ(4, count());

    // Start 2 transactions at once.
    expectThrow([&] {
        auto trans0 = db->beginTransaction(sql::Transaction::Type::Deferred);
        auto trans1 = db->beginTransaction(sql::Transaction::Type::Deferred);
    });
}
