#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <concepts>
#include <format>
#include <memory>
#include <string>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "cppql-typed/type_traits.h"
#include "cppql-typed/expressions/expression_column.h"
#include "cppql-typed/expressions/expression_filter.h"

namespace sql
{
    ////////////////////////////////////////////////////////////////
    // LikeExpression class.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief The LikeExpression class holds a string column and a
     * fixed or dynamic value to use for pattern matching.
     * \tparam T Table type.
     */
    template<typename T>
    class LikeExpression final : public FilterExpression<T>
    {
    public:
        LikeExpression() = delete;

        LikeExpression(const LikeExpression& other);

        LikeExpression(LikeExpression&& other) noexcept;

        LikeExpression(BaseColumnExpressionPtr<T> col, std::string val);

        LikeExpression(BaseColumnExpressionPtr<T> col, std::string* p);

        ~LikeExpression() override = default;

        LikeExpression& operator=(const LikeExpression& other);

        LikeExpression& operator=(LikeExpression&& other) noexcept;

        [[nodiscard]] std::string toString(const Table& table, int32_t& pIndex) override;

        void bind(Statement& stmt) const override;

        [[nodiscard]] std::unique_ptr<FilterExpression<T>> clone() const override;

    private:
        /**
         * \brief Column to compare.
         */
        BaseColumnExpressionPtr<T> column;

        /**
         * \brief Fixed value.
         */
        std::string value;

        /**
         * \brief Dynamic value.
         */
        std::string* ptr = nullptr;

        /**
         * \brief Index for parameter binding.
         */
        int32_t index = -1;
    };

    ////////////////////////////////////////////////////////////////
    // Implementation.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    LikeExpression<T>::LikeExpression(const LikeExpression& other) :
        column(other.column->clone()), value(other.value), ptr(other.ptr), index(other.index)
    {
    }

    template<typename T>
    LikeExpression<T>::LikeExpression(LikeExpression&& other) noexcept :
        column(std::move(other.column)), value(other.value), ptr(other.ptr), index(other.index)
    {
    }

    template<typename T>
    LikeExpression<T>::LikeExpression(BaseColumnExpressionPtr<T> col, std::string val) :
        column(std::move(col)), value(std::move(val))
    {
    }

    template<typename T>
    LikeExpression<T>::LikeExpression(BaseColumnExpressionPtr<T> col, std::string* p) :
        column(std::move(col)), ptr(p)
    {
    }

    template<typename T>
    LikeExpression<T>& LikeExpression<T>::operator=(const LikeExpression& other)
    {
        column = other.column->clone();
        value  = other.value;
        ptr    = other.ptr;
        index  = other.index;
        return *this;
    }

    template<typename T>
    LikeExpression<T>& LikeExpression<T>::operator=(LikeExpression&& other) noexcept
    {
        column = std::move(other.column);
        value  = other.value;
        ptr    = other.ptr;
        index  = other.index;
        return *this;
    }

    template<typename T>
    std::string LikeExpression<T>::toString(const Table& table, int32_t& pIndex)
    {
        // Store unincremented index value. Use incremented value in string, because sqlite parameter indices start at 1.
        index    = pIndex++;
        auto col = column->toString(table);
        return std::format("{0} LIKE ?{1}", column->toString(table), pIndex);
    }

    template<typename T>
    void LikeExpression<T>::bind(Statement& stmt) const
    {
        // If pointer is not null, bind dynamic value. Otherwise bind fixed value.
        const auto res = ptr ? stmt.bind(index + Statement::getFirstBindIndex(), *ptr) :
                               stmt.bind(index + Statement::getFirstBindIndex(), value);
        if (!res) throw std::runtime_error("");
    }

    template<typename T>
    std::unique_ptr<FilterExpression<T>> LikeExpression<T>::clone() const
    {
        return std::make_unique<LikeExpression<T>>(*this);
    }

    ////////////////////////////////////////////////////////////////
    // like()
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Require column LIKE fixed value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \param col Column object.
     * \param val Value.
     * \return LikeExpression object.
     */
    template<typename T, size_t Index>
    requires(std::same_as<std::string, col_t<Index, T>>) auto like(ColumnExpression<T, Index> col, std::string val)
    {
        using C = LikeExpression<T>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), std::move(val));
    }
    
    /**
     * \brief Require column LIKE dynamic value.
     * \tparam T Table type.
     * \tparam Index Column index.
     * \param col Column object.
     * \param val Pointer to value.
     * \return ComparisonExpression object.
     */
    template<typename T, size_t Index>
    requires(std::same_as<std::string, col_t<Index, T>>) auto like(ColumnExpression<T, Index> col, std::string* val)
    {
        using C = LikeExpression<T>;
        using D = ColumnExpression<T, Index>;
        return C(std::make_unique<D>(std::move(col)), val);
    }
}  // namespace sql