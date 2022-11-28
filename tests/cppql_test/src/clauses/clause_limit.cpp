#include "cppql_test/clauses/clause_limit.h"

#include "cppql/include_all.h"

void ClauseLimit::operator()()
{
    compareEQ(sql::Limit<std::nullopt_t>::toString(), "");

    const auto limit = sql::Limit<std::true_type>(10, 32);
    compareEQ(limit.toString(), "LIMIT 10 OFFSET 32");
}
