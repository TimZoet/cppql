#include "cppql_test/aggregates/aggregate_count.h"

#include "cppql/include_all.h"

void AggregateCount::operator()()
{
    compareEQ(sql::CountAggregate<int64_t, false>::toString("col1"), "COUNT(col1)");
    compareEQ(sql::CountAggregate<float, false>::toString("col2"), "COUNT(col2)");
    compareEQ(sql::CountAggregate<int64_t, true>::toString("col3"), "COUNT(DISTINCT col3)");
}
