#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <string>
#include <type_traits>

////////////////////////////////////////////////////////////////
// Current target includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/enums.h"

namespace sql
{
    template<JoinType J>
    struct JoinTypeWrapper
    {
        static constexpr JoinType type = J;
    };

    template<>
    struct JoinTypeWrapper<JoinType::Cross>
    {
        static constexpr JoinType       type    = JoinType::Cross;
        inline static const std::string name    = "CROSS JOIN";
        static constexpr bool           natural = false;
    };

    template<>
    struct JoinTypeWrapper<JoinType::Left>
    {
        static constexpr JoinType       type    = JoinType::Left;
        inline static const std::string name    = "LEFT JOIN";
        static constexpr bool           natural = false;
    };

    template<>
    struct JoinTypeWrapper<JoinType::Right>
    {
        static constexpr JoinType       type    = JoinType::Right;
        inline static const std::string name    = "RIGHT JOIN";
        static constexpr bool           natural = false;
    };

    template<>
    struct JoinTypeWrapper<JoinType::Full>
    {
        static constexpr JoinType       type    = JoinType::Full;
        inline static const std::string name    = "FULL JOIN";
        static constexpr bool           natural = false;
    };

    template<>
    struct JoinTypeWrapper<JoinType::Inner>
    {
        static constexpr JoinType       type    = JoinType::Inner;
        inline static const std::string name    = "INNER JOIN";
        static constexpr bool           natural = false;
    };

    template<>
    struct JoinTypeWrapper<JoinType::NaturalLeft>
    {
        static constexpr JoinType       type    = JoinType::NaturalLeft;
        inline static const std::string name    = "NATURAL LEFT JOIN";
        static constexpr bool           natural = true;
    };

    template<>
    struct JoinTypeWrapper<JoinType::NaturalRight>
    {
        static constexpr JoinType       type    = JoinType::NaturalRight;
        inline static const std::string name    = "NATURAL RIGHT JOIN";
        static constexpr bool           natural = true;
    };

    template<>
    struct JoinTypeWrapper<JoinType::NaturalFull>
    {
        static constexpr JoinType       type    = JoinType::NaturalFull;
        inline static const std::string name    = "NATURAL FULL JOIN";
        static constexpr bool           natural = true;
    };

    template<>
    struct JoinTypeWrapper<JoinType::NaturalInner>
    {
        static constexpr JoinType       type    = JoinType::NaturalInner;
        inline static const std::string name    = "NATURAL INNER JOIN";
        static constexpr bool           natural = true;
    };

    template<typename T>
    concept is_join_wrapper = std::same_as<std::remove_cvref_t<T>, JoinTypeWrapper<std::remove_cvref_t<T>::type>>;

    using CrossJoin        = JoinTypeWrapper<JoinType::Cross>;
    using LeftJoin         = JoinTypeWrapper<JoinType::Left>;
    using RightJoin        = JoinTypeWrapper<JoinType::Right>;
    using FullJoin         = JoinTypeWrapper<JoinType::Full>;
    using InnerJoin        = JoinTypeWrapper<JoinType::Inner>;
    using NaturalLeftJoin  = JoinTypeWrapper<JoinType::NaturalLeft>;
    using NaturalRightJoin = JoinTypeWrapper<JoinType::NaturalRight>;
    using NaturalFullJoin  = JoinTypeWrapper<JoinType::NaturalFull>;
    using NaturalInnerJoin = JoinTypeWrapper<JoinType::NaturalInner>;
}  // namespace sql