#pragma once

namespace sql
{
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
