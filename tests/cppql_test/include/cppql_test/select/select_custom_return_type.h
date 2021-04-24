#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "bettertest/mixins/compare_mixin.h"
#include "bettertest/mixins/exception_mixin.h"
#include "bettertest/tests/unit_test.h"

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql_test/utils.h"

class SelectCustomReturnType : public bt::UnitTest<SelectCustomReturnType, bt::CompareMixin, bt::ExceptionMixin>,
                               utils::DatabaseMember
{
public:
    void operator()() override;
};