#include "cppql_test/aggregates/aggregate_average.h"

#include "cppql/include_all.h"

void AggregateAverage::operator()()
{
    compareEQ(sql::AverageAggregate<int64_t, false>::toString("col1"), "AVG(col1)");
    compareEQ(sql::AverageAggregate<float, false>::toString("col2"), "AVG(col2)");
    compareEQ(sql::AverageAggregate<int64_t, true>::toString("col3"), "AVG(DISTINCT col3)");
}
