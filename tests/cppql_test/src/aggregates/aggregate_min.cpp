#include "cppql_test/aggregates/aggregate_min.h"

#include "cppql/include_all.h"

void AggregateMin::operator()()
{
    compareEQ(sql::MinAggregate<int64_t, false>::toString("col1"), "MIN(col1)");
    compareEQ(sql::MinAggregate<float, false>::toString("col2"), "MIN(col2)");
    compareEQ(sql::MinAggregate<int64_t, true>::toString("col3"), "MIN(DISTINCT col3)");
}
