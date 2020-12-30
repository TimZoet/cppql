#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/mixins/compare_mixin.h"
#include "bettertest/mixins/exception_mixin.h"
#include "bettertest/tests/unit_test.h"

namespace test
{
    class CreateDatabase : public bt::UnitTest<CreateDatabase, bt::CompareMixin, bt::ExceptionMixin>
    {
    public:
        void operator()() override;
    };
}  // namespace test
