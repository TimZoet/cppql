#include "cppql_test/aggregates/aggregate_total.h"

#include "cppql/include_all.h"

void AggregateTotal::operator()()
{
    compareEQ(sql::TotalAggregate<int64_t, false>::toString("col1"), "TOTAL(col1)");
    compareEQ(sql::TotalAggregate<float, false>::toString("col2"), "TOTAL(col2)");
    compareEQ(sql::TotalAggregate<int64_t, true>::toString("col3"), "TOTAL(DISTINCT col3)");
}
