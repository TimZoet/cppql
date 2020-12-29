#include "cppql_test/test01.h"

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

namespace test
{
    void Test01::operator()()
    {
        const auto cwd = std::filesystem::current_path();

        expectNoThrow([cwd]() { auto db = sql::Database::create(cwd / ".db"); });
        expectNoThrow([cwd]() { auto db = sql::Database::open(cwd / ".db"); });
        compareEQ(0, 0);
    }
}  // namespace test
