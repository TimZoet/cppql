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

class CreateColumnBlob final : public bt::UnitTest<CreateColumnBlob, bt::CompareMixin, bt::ExceptionMixin>,
                               utils::DatabaseMember
{
public:
    CreateColumnBlob() : DatabaseMember(false) {}

    void operator()() override;

    void create();

    void verify();
};
