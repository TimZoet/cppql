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

        DatabaseMember(const DatabaseMember&) = delete;

        DatabaseMember(DatabaseMember&&) = delete;

        DatabaseMember& operator=(const DatabaseMember&) = delete;

        DatabaseMember& operator=(DatabaseMember&&) = delete;

        virtual ~DatabaseMember() noexcept;

        void reopen();

    protected:
        sql::DatabasePtr db;
    };
}