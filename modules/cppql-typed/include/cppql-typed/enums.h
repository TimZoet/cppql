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
    struct ComparisonOperatorType{};

    template<>
    struct ComparisonOperatorType<ComparisonOperator::Eq> { inline static const std::string str = "="; };

    template<>
    struct ComparisonOperatorType<ComparisonOperator::Ne> { inline static const std::string str = "!="; };

    template<>
    struct ComparisonOperatorType<ComparisonOperator::Lt> { inline static const std::string str = "<"; };

    template<>
    struct ComparisonOperatorType<ComparisonOperator::Gt> { inline static const std::string str = ">"; };

    template<>
    struct ComparisonOperatorType<ComparisonOperator::Le> { inline static const std::string str = "<="; };

    template<>
    struct ComparisonOperatorType<ComparisonOperator::Ge> { inline static const std::string str = ">="; };

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
}  // namespace sql
