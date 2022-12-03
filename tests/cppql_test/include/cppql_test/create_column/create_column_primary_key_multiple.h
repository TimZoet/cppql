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

class CreateColumnPrimaryKeyMultiple final
    : public bt::UnitTest<CreateColumnPrimaryKeyMultiple, bt::CompareMixin, bt::ExceptionMixin>,
      utils::DatabaseMember
{
public:
    CreateColumnPrimaryKeyMultiple() : DatabaseMember(false) {}

    void operator()() override;

    void create();

    void verify();
};
