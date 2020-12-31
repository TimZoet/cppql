#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/mixins/compare_mixin.h"
#include "bettertest/mixins/exception_mixin.h"
#include "bettertest/tests/unit_test.h"

namespace test
{
    class CreateColumnBlob : public bt::UnitTest<CreateColumnBlob, bt::CompareMixin, bt::ExceptionMixin>
    {
    public:
        void operator()() override;

        void create();

        void verify();
    };
}  // namespace test
