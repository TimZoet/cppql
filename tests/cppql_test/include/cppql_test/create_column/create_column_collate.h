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

class CreateColumnCollate final : public bt::UnitTest<CreateColumnCollate, bt::CompareMixin, bt::ExceptionMixin>,
                                  utils::DatabaseMember
{
public:
    CreateColumnCollate() : DatabaseMember(false) {}

    void operator()() override;

    void create();

    void verify();
};
