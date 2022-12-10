#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>

namespace sql
{
    enum class ConflictClause
    {
        Rollback,
        Abort,
        Fail,
        Ignore,
        Replace
    };

    [[nodiscard]] inline std::string toString(const ConflictClause value)
    {
        switch (value)
        {
        case ConflictClause::Rollback: return "ON CONFLICT ROLLBACK";
        case ConflictClause::Abort: return "ON CONFLICT ABORT";
        case ConflictClause::Fail: return "ON CONFLICT FAIL";
        case ConflictClause::Ignore: return "ON CONFLICT IGNORE";
        case ConflictClause::Replace: return "ON CONFLICT REPLACE";
        }

        return "";
    }

    enum class Deferrable
    {
        NotDeferrable,
        InitiallyDeferred,
        InitiallyImmediate
    };

    [[nodiscard]] inline std::string toString(const Deferrable value)
    {
        switch (value)
        {
        case Deferrable::NotDeferrable: return "";
        case Deferrable::InitiallyDeferred: return "DEFERRABLE INITIALLY DEFERRED";
        case Deferrable::InitiallyImmediate: return "";
        }

        return "";
    }

    enum class ForeignKeyAction
    {
        SetNull,
        SetDefault,
        Cascade,
        Restrict,
        NoAction
    };

    [[nodiscard]] inline std::string toString(const ForeignKeyAction value)
    {
        switch (value)
        {
        case ForeignKeyAction::SetNull: return "SET NULL";
        case ForeignKeyAction::SetDefault: return "SET DEFAULT";
        case ForeignKeyAction::Cascade: return "CASCADE";
        case ForeignKeyAction::Restrict: return "RESTRICT";
        case ForeignKeyAction::NoAction: return "NO ACTION";
        }

        return "";
    }

    enum class BindParameters
    {
        // Don't bind any parameters.
        None = 0,
        // Bind fixed parameters only.
        Fixed = 1,
        // Bind dynamic parameters only.
        Dynamic = 2,
        // Bind fixed and dynamic parameters.
        All = Fixed | Dynamic
    };

    enum class ComparisonOperator
    {
        Eq,  // ==
        Ne,  // !=
        Lt,  // <
        Gt,  // >
        Le,  // <=
        Ge,  // >=
    };

    template<ComparisonOperator>
    struct ComparisonOperatorType
    {
    };

    template<>
    struct ComparisonOperatorType<ComparisonOperator::Eq>
    {
        inline static const std::string str = "=";
    };

    template<>
    struct ComparisonOperatorType<ComparisonOperator::Ne>
    {
        inline static const std::string str = "!=";
    };

    template<>
    struct ComparisonOperatorType<ComparisonOperator::Lt>
    {
        inline static const std::string str = "<";
    };

    template<>
    struct ComparisonOperatorType<ComparisonOperator::Gt>
    {
        inline static const std::string str = ">";
    };

    template<>
    struct ComparisonOperatorType<ComparisonOperator::Le>
    {
        inline static const std::string str = "<=";
    };

    template<>
    struct ComparisonOperatorType<ComparisonOperator::Ge>
    {
        inline static const std::string str = ">=";
    };

    enum class JoinType
    {
        Cross,
        Left,
        Right,
        Full,
        Inner,
        NaturalLeft,
        NaturalRight,
        NaturalFull,
        NaturalInner
    };

    enum class LogicalOperator
    {
        Or,
        And
    };

    enum class Order
    {
        Asc,
        Desc
    };

    enum class Nulls
    {
        None,
        First,
        Last
    };

    template<Order>
    struct OrderString
    {
    };

    template<>
    struct OrderString<Order::Asc>
    {
        inline static const std::string str = "ASC";
    };

    template<>
    struct OrderString<Order::Desc>
    {
        inline static const std::string str = "DESC";
    };

    [[nodiscard]] inline std::string toString(const Order value)
    {
        switch (value)
        {
        case Order::Asc: return "ASC";
        case Order::Desc: return "DESC";
        }

        return "";
    }

    template<Nulls>
    struct NullsString
    {
    };

    template<>
    struct NullsString<Nulls::None>
    {
        inline static const std::string str;
    };

    template<>
    struct NullsString<Nulls::First>
    {
        inline static const std::string str = "NULLS FIRST";
    };

    template<>
    struct NullsString<Nulls::Last>
    {
        inline static const std::string str = "NULLS LAST";
    };

    enum class UnionOperator
    {
        Union,
        UnionAll,
        Intersect,
        Except
    };

    template<UnionOperator>
    struct UnionString
    {
    };

    template<>
    struct UnionString<UnionOperator::Union>
    {
        inline static const std::string str = "UNION";
    };

    template<>
    struct UnionString<UnionOperator::UnionAll>
    {
        inline static const std::string str = "UNION ALL";
    };

    template<>
    struct UnionString<UnionOperator::Intersect>
    {
        inline static const std::string str = "INTERSECT";
    };

    template<>
    struct UnionString<UnionOperator::Except>
    {
        inline static const std::string str = "EXCEPT";
    };
}  // namespace sql
