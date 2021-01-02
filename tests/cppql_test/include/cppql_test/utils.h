#pragma once

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql/database.h"

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