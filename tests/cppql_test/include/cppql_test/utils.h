#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-core/database.h"

namespace utils
{
    class DatabaseMember
    {
    public:
        DatabaseMember();

        virtual ~DatabaseMember() noexcept;

        void reopen();

    protected:
        sql::DatabasePtr db;
    };
}