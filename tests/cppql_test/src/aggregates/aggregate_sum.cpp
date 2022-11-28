#include "cppql_test/aggregates/aggregate_sum.h"

#include "cppql/include_all.h"

void AggregateSum::operator()()
{
    compareEQ(sql::SumAggregate<int64_t, false>::toString("col1"), "SUM(col1)");
    compareEQ(sql::SumAggregate<float, false>::toString("col2"), "SUM(col2)");
    compareEQ(sql::SumAggregate<int64_t, true>::toString("col3"), "SUM(DISTINCT col3)");
}
