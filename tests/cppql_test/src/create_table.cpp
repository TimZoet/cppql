#include "cppql_test/create_table.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

namespace test
{
    struct Foo
    {
    };

    void CreateTable::operator()()
    {
        // Create new database and create tables.
        testNew();

        // Open previously created database and verify tables.
        testExisting();
    }

    void CreateTable::testNew()
    {
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "db.db";
        std::filesystem::remove(dbPath);
        auto db = sql::Database::create(dbPath);

        // Create tables. Last create should throw due to duplicate name.
        sql::Table *table1, *table2, *table3, *table4;
        expectNoThrow([&db, &table1]() { table1 = &db->createTable("table1"); });
        expectNoThrow([&db, &table2]() { table2 = &db->createTable("table2"); });
        expectNoThrow([&db, &table3]() { table3 = &db->createTable("table3"); });
        expectNoThrow([&db, &table4]() { table4 = &db->createTable("table4"); });
        expectThrow([&db]() { auto& x = db->createTable("table1"); });

        // Verify names and uncommitted.
        compareEQ(table1->getName(), "table1");
        compareEQ(table2->getName(), "table2");
        compareEQ(table3->getName(), "table3");
        compareFalse(table1->isCommitted());
        compareFalse(table2->isCommitted());
        compareFalse(table3->isCommitted());

        // Create new columns of every type.
        expectNoThrow([&table1]() { table1->createColumn("col1", sql::Column::Type::Int); });
        expectNoThrow([&table1]() { table1->createColumn("col2", sql::Column::Type::Real); });
        expectNoThrow([&table1]() { table1->createColumn("col3", sql::Column::Type::Text); });
        expectNoThrow([&table1]() { table1->createColumn("col4", sql::Column::Type::Blob); });
        expectNoThrow([&table1]() { table1->createColumn("col5", sql::Column::Type::Null); });

        // Second create should throw due to duplicate name.
        expectNoThrow([&table2]() { table2->createColumn("col1", sql::Column::Type::Int); });
        expectThrow([&table2]() { table2->createColumn("col1", sql::Column::Type::Int); });

        // Create columns using template method.
        auto& col1  = table3->createColumn<int32_t>("col1");
        auto& col2  = table3->createColumn<uint8_t>("col2");
        auto& col3  = table3->createColumn<size_t>("col3");
        auto& col4  = table3->createColumn<float>("col4");
        auto& col5  = table3->createColumn<double>("col5");
        auto& col6  = table3->createColumn<float*>("col6");
        auto& col7  = table3->createColumn<void*>("col7");
        auto& col8  = table3->createColumn<Foo*>("col8");
        auto& col9  = table3->createColumn<std::string>("col9");
        auto& col10 = table3->createColumn<std::nullptr_t>("col10");
        compareEQ(col1.getType(), sql::Column::Type::Int);
        compareEQ(col2.getType(), sql::Column::Type::Int);
        compareEQ(col3.getType(), sql::Column::Type::Int);
        compareEQ(col4.getType(), sql::Column::Type::Real);
        compareEQ(col5.getType(), sql::Column::Type::Real);
        compareEQ(col6.getType(), sql::Column::Type::Blob);
        compareEQ(col7.getType(), sql::Column::Type::Blob);
        compareEQ(col8.getType(), sql::Column::Type::Blob);
        compareEQ(col9.getType(), sql::Column::Type::Text);
        compareEQ(col10.getType(), sql::Column::Type::Null);

        // Commit tables. Last commit should throw because there are no columns.
        expectNoThrow([&table1]() { table1->commit(); });
        expectNoThrow([&table2]() { table2->commit(); });
        expectNoThrow([&table3]() { table3->commit(); });
        expectThrow([&table4]() { table4->commit(); });
        compareTrue(table1->isCommitted());
        compareTrue(table2->isCommitted());
        compareTrue(table3->isCommitted());
        compareFalse(table4->isCommitted());

        // Another commit should throw.
        expectThrow([&table1]() { table1->commit(); });
    }

    void CreateTable::testExisting()
    {
        const auto cwd    = std::filesystem::current_path();
        const auto dbPath = cwd / "db.db";
        auto       db     = sql::Database::open(dbPath);

        // Try to get tables.
        sql::Table* table1;
        expectNoThrow([&db, &table1]() { table1 = &db->getTable("table1"); });
        expectNoThrow([&db]() { auto& t = db->getTable("table2"); });
        expectNoThrow([&db]() { auto& t = db->getTable("table3"); });
        expectThrow([&db]() { auto& t = db->getTable("table4"); });

        // Try to get columns.
        const auto& cols = table1->getColumns();
        compareEQ(cols.size(), 5);
        const auto col1 = cols.find("col1");
        const auto col2 = cols.find("col2");
        const auto col3 = cols.find("col3");
        const auto col4 = cols.find("col4");
        const auto col5 = cols.find("col5");
        compareNE(col1, cols.end());
        compareNE(col2, cols.end());
        compareNE(col3, cols.end());
        compareNE(col4, cols.end());
        compareNE(col5, cols.end());

        // Check column names.
        compareEQ(col1->second->getName(), "col1");
        compareEQ(col2->second->getName(), "col2");
        compareEQ(col3->second->getName(), "col3");
        compareEQ(col4->second->getName(), "col4");
        compareEQ(col5->second->getName(), "col5");

        // Check column types.
        compareEQ(col1->second->getType(), sql::Column::Type::Int);
        compareEQ(col2->second->getType(), sql::Column::Type::Real);
        compareEQ(col3->second->getType(), sql::Column::Type::Text);
        compareEQ(col4->second->getType(), sql::Column::Type::Blob);
        compareEQ(col5->second->getType(), sql::Column::Type::Null);

        std::filesystem::remove(dbPath);
    }
}  // namespace test
