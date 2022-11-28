#include "cppql_test/aggregates/aggregate_max.h"

#include "cppql/include_all.h"

void AggregateMax::operator()()
{
    compareEQ(sql::MaxAggregate<int64_t, false>::toString("col1"), "MAX(col1)");
    compareEQ(sql::MaxAggregate<float, false>::toString("col2"), "MAX(col2)");
    compareEQ(sql::MaxAggregate<int64_t, true>::toString("col3"), "MAX(DISTINCT col3)");
}
