#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <bit>
#include <concepts>
#include <cstdint>
#include <type_traits>
#include <vector>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "common/type_traits.h"

namespace sql
{
    ////////////////////////////////////////////////////////////////
    // Binding wrappers.
    ////////////////////////////////////////////////////////////////

    // TODO: Add docstrings to all of this.

    struct Blob
    {
        const void* data;
        size_t      size;
        void (*destructor)(void*);
    };

    struct StaticBlob
    {
        const void* data;
        size_t      size;
    };

    struct TransientBlob
    {
        const void* data;
        size_t      size;
    };

    struct Text
    {
        const char* data;
        size_t      size;
        void (*destructor)(void*);
    };

    struct StaticText
    {
        const char* data;
        size_t      size;
    };

    struct TransientText
    {
        const char* data;
        size_t      size;
    };

    template<typename T>
    Blob toBlob(T* value)
    {
        return Blob{.data = value, .size = sizeof(T), .destructor = [](void* p) { delete static_cast<T*>(p); }};
    }

    template<typename T>
    StaticBlob toStaticBlob(const T& value)
    {
        return StaticBlob{.data = &value, .size = sizeof(T)};
    }

    template<typename T>
    StaticBlob toStaticBlob(const std::vector<T>& value)
    {
        return StaticBlob{.data = value.data(), .size = sizeof(T) * value.size()};
    }

    template<typename T>
    TransientBlob toTransientBlob(const T& value)
    {
        return TransientBlob{.data = &value, .size = sizeof(T)};
    }

    template<typename T>
    TransientBlob toTransientBlob(const std::vector<T>& value)
    {
        return TransientBlob{.data = value.data(), .size = sizeof(T) * value.size()};
    }

    ////////////////////////////////////////////////////////////////
    // Binding type traits.
    ////////////////////////////////////////////////////////////////

    template<typename T>
    struct bind_t
    {
    };

    template<std::integral T>
    struct bind_t<T>
    {
        using value_t  = T;
        using return_t = std::conditional_t<sizeof(T) <= sizeof(int32_t), int32_t, int64_t>;
    };

    template<std::floating_point T>
    struct bind_t<T>
    {
        using value_t  = T;
        using return_t = double;
    };

    template<std::same_as<std::nullptr_t> T>
    struct bind_t<T>
    {
        using value_t  = T;
        using return_t = T;
    };

    template<typename T>
    requires(std::same_as<Blob, T> || std::same_as<StaticBlob, T> || std::same_as<TransientBlob, T> ||
             std::same_as<Text, T> || std::same_as<StaticText, T> || std::same_as<TransientText, T>) struct bind_t<T>
    {
        using value_t  = T;
        using return_t = const T&;
    };

    template<std::same_as<std::string> T>
    struct bind_t<T>
    {
        using value_t  = std::string;
        using return_t = const std::string&;
    };

    template<typename T>
    requires(is_optional_v<T>&& requires(T) { typename bind_t<typename T::value_type>::return_t; }) struct bind_t<T>
    {
        using value_t  = T;
        using return_t = typename bind_t<typename T::value_type>::return_t;
    };

    template<typename T>
    concept bindable = requires
    {
        typename bind_t<T>::return_t;
    };

    ////////////////////////////////////////////////////////////////
    // Binding class.
    ////////////////////////////////////////////////////////////////

    template<bindable T>
    class Binding
    {
    public:
        using value_t  = typename bind_t<T>::value_t;
        using return_t = typename bind_t<T>::return_t;

        Binding() = delete;

        explicit Binding(value_t value) : value(std::move(value)) {}

        Binding(const Binding&) = delete;

        Binding(Binding&&) = default;

        ~Binding() = default;

        Binding& operator=(const Binding&) = delete;

        Binding& operator=(Binding&&) = default;

        [[nodiscard]] bool holdsValue() const noexcept
        {
            if constexpr (is_optional_v<value_t>)
                return static_cast<bool>(value);
            else
                return true;
        }

        /**
         * \brief Get value that this object holds. Should not be called if holdsValue returned false.
         * \return Value.
         */
        [[nodiscard]] return_t get() const noexcept
        {
            if constexpr (std::same_as<return_t, int32_t>)
            {
                // Return as-is.
                if constexpr (std::same_as<value_t, int32_t>) return getValue();
                // Bitcast unsigned 32-bit int to signed 32-bit int.
                else if constexpr (std::same_as<value_t, uint32_t>)
                    return std::bit_cast<int32_t>(getValue());
                // Cast <32-bit (u|s)int to signed 32-bit int.
                else
                    return static_cast<int32_t>(getValue());
            }
            else if constexpr (std::same_as<return_t, int64_t>)
            {
                // Return as-is.
                if constexpr (std::same_as<value_t, int64_t>) return getValue();
                // Bitcast unsigned 64-bit int to signed 64-bit int.
                else if constexpr (std::same_as<value_t, uint64_t>)
                    return std::bit_cast<int64_t>(getValue());
            }
            else if constexpr (std::same_as<return_t, double>)
            {
                // Cast floating point to double.
                return static_cast<double>(getValue());
            }
            else
            {
                return getValue();
            }
        }

    private:
        [[nodiscard]] return_t getValue() const noexcept
        {
            if constexpr (is_optional_v<value_t>)
                return *value;
            else
                return value;
        }
        value_t value;
    };
}  // namespace sql