#pragma once

namespace sql
{
    class Database;

    class Transaction
    {
    public:
        ////////////////////////////////////////////////////////////////
        // Types.
        ////////////////////////////////////////////////////////////////

        enum class Type
        {
            Deferred,
            Immediate,
            Exclusive
        };

        ////////////////////////////////////////////////////////////////
        // Constructors.
        ////////////////////////////////////////////////////////////////

        Transaction() = delete;

        Transaction(const Transaction&) = delete;

        Transaction(Transaction&&) = delete;

        Transaction(Database& db, Type type);

        Transaction& operator=(const Transaction&) = delete;

        Transaction& operator=(Transaction&&) = delete;

        ~Transaction() noexcept;

        ////////////////////////////////////////////////////////////////
        // Commit.
        ////////////////////////////////////////////////////////////////

        void commit();

        void rollback();

        ////////////////////////////////////////////////////////////////
        // Member variables.
        ////////////////////////////////////////////////////////////////
    private:
        Database* database;

        bool committed = false;
    };
}  // namespace sql