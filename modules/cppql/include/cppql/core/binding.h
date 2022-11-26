#pragma once

////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <bit>
#include <concepts>
#include <cstdint>
#include <string>
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

    /**
     * \brief Wrapper around arbitrary binary data to bind to a statement.
     * Takes ownership of data and requires a destructor to deallocate it once the wrapper is no longer used.
     */
    struct Blob
    {
        /**
         * \brief Pointer to data.
         */
        const void* data;

        /**
         * \brief Size of data in bytes.
         */
        size_t size;

        /**
         * \brief Destructor to deallocate data.
         */
        void (*destructor)(void*);
    };

    /**
     * \brief Wrapper around arbitrary binary data to bind to a statement.
     * Does not take ownership. Data must remain valid for entire lifetime of statement,
     * or until other data is bound to the same parameter.
     */
    struct StaticBlob
    {
        const void* data;
        size_t      size;
    };

    /**
     * \brief Wrapper around arbitrary binary data to bind to a statement.
     * Does not take ownership. Data must remain valid during bind.
     */
    struct TransientBlob
    {
        const void* data;
        size_t      size;
    };

    /**
     * \brief Wrapper around text data to bind to a statement.
     * Takes ownership of data and requires a destructor to deallocate it once the wrapper is no longer used.
     */
    struct Text
    {
        const char* data;
        size_t      size;
        void (*destructor)(void*);
    };

    /**
     * \brief Wrapper around text data to bind to a statement.
     * Does not take ownership. Data must remain valid for entire lifetime of statement,
     * or until other data is bound to the same parameter.
     */
    struct StaticText
    {
        const char* data;
        size_t      size;
    };

    /**
     * \brief Wrapper around text data to bind to a statement.
     * Does not take ownership. Data must remain valid during bind.
     */
    struct TransientText
    {
        const char* data;
        size_t      size;
    };

    /**
     * \brief Create wrapper around single value. Wrapper takes ownership and will automatically deallocate.
     * \tparam T Value type.
     * \param value Value.
     * \return Blob.
     */
    template<typename T>
    Blob toBlob(T* value)
    {
        return Blob{.data = value, .size = sizeof(T), .destructor = [](void* p) { delete static_cast<T*>(p); }};
    }

    /**
     * \brief Create wrapper around single value. Wrapper does not take ownership.
     * \tparam T Value type.
     * \param value Value.
     * \return StaticBlob.
     */
    template<typename T>
    StaticBlob toStaticBlob(const T& value)
    {
        return StaticBlob{.data = &value, .size = sizeof(T)};
    }

    /**
     * \brief Create wrapper around vector of values. Wrapper does not take ownership.
     * \tparam T Value type.
     * \param value Vector of values.
     * \return StaticBlob.
     */
    template<typename T>
    StaticBlob toStaticBlob(const std::vector<T>& value)
    {
        return StaticBlob{.data = value.data(), .size = sizeof(T) * value.size()};
    }

    /**
     * \brief Create wrapper around values. Wrapper does not take ownership.
     * \tparam T Value type.
     * \param value Value.
     * \return TransientBlob.
     */
    template<typename T>
    TransientBlob toTransientBlob(const T& value)
    {
        return TransientBlob{.data = &value, .size = sizeof(T)};
    }

    /**
     * \brief Create wrapper around vector of values. Wrapper does not take ownership.
     * \tparam T Value type.
     * \param value Vector of values.
     * \return TransientBlob.
     */
    template<typename T>
    TransientBlob toTransientBlob(const std::vector<T>& value)
    {
        return TransientBlob{.data = value.data(), .size = sizeof(T) * value.size()};
    }

    /**
     * \brief Create wrapper around string. A copy is made. Wrapper takes ownership of copied data and will automatically deallocate.
     * \param value String.
     * \return Text.
     */
    inline Text toText(const std::string& value)
    {
        auto* data = new char[value.size() + 1];
        std::memcpy(data, value.data(), value.size() + 1);
        return Text{
          .data = data, .size = value.size(), .destructor = [](void* p) { delete[] static_cast<const char*>(p); }};
    }

    /**
     * \brief Create wrapper around string. A copy is made. Wrapper takes ownership of copied data and will automatically deallocate.
     * \param value Optional string.
     * \return Text.
     */
    inline Text toText(const std::optional<std::string>& value)
    {
        if (value) return toText(*value);
        return Text{.data = nullptr, .size = 0, .destructor = nullptr};
    }

    /**
     * \brief Create wrapper around string. Wrapper does not take ownership.
     * \param value String.
     * \return StaticText.
     */
    inline StaticText toStaticText(const std::string& value)
    {
        return StaticText{.data = value.data(), .size = value.size()};
    }

    /**
     * \brief Create wrapper around string. Wrapper does not take ownership.
     * \param value Optional string.
     * \return StaticText.
     */
    inline StaticText toStaticText(const std::optional<std::string>& value)
    {
        if (value) return toStaticText(*value);
        return StaticText{.data = nullptr, .size = 0};
    }

    /**
     * \brief Create wrapper around string. Wrapper does not take ownership.
     * \param value String.
     * \return TransientText.
     */
    inline TransientText toTransientText(const std::string& value)
    {
        return TransientText{.data = value.data(), .size = value.size()};
    }

    /**
     * \brief Create wrapper around string. Wrapper does not take ownership.
     * \param value Optional string.
     * \return TransientText.
     */
    inline TransientText toTransientText(const std::optional<std::string>& value)
    {
        if (value) return toTransientText(*value);
        return TransientText{.data = nullptr, .size = 0};
    }

    /**
     * \brief Create wrapper around string. A copy is made. Wrapper takes ownership of copied data and will automatically deallocate.
     * \param value String.
     * \return Text.
     */
    template<size_t N>
    Text toText(const char (&value)[N])
    {
        char* data = new char[N];
        std::memcpy(data, &value[0], N);
        return Text{.data = data, .size = N - 1, .destructor = [](void* p) { delete[] static_cast<const char*>(p); }};
    }

    ////////////////////////////////////////////////////////////////
    // Binding type traits.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Utility struct to determine what some type must be converted to in order to pass it to any of the sqlite bind functions.
     * value_t is original type of parameter.
     * return_t is type it must be converted to.
     * \tparam T Type.
     */
    template<typename T>
    struct bind_t
    {
    };

    // There are two integral bind functions that take either int32 or int64.
    // If parameter fits in 32 bits, use that version. Otherwise, use 64 bit version.
    template<std::integral T>
    struct bind_t<T>
    {
        using value_t  = T;
        using return_t = std::conditional_t<sizeof(T) <= sizeof(int32_t), int32_t, int64_t>;
    };

    // There is only a single floating point bind function that takes a double.
    template<std::floating_point T>
    struct bind_t<T>
    {
        using value_t  = T;
        using return_t = double;
    };

    // Nullptr is just nullptr.
    template<std::same_as<std::nullptr_t> T>
    struct bind_t<T>
    {
        using value_t  = nullptr_t;
        using return_t = nullptr_t;
    };

    // Wrappers must be returned by const ref.
    template<typename T>
    requires(std::same_as<Blob, T> || std::same_as<StaticBlob, T> || std::same_as<TransientBlob, T> ||
             std::same_as<Text, T> || std::same_as<StaticText, T> || std::same_as<TransientText, T>) struct bind_t<T>
    {
        using value_t  = T;
        using return_t = const T&;
    };

    // Unwrap optional.
    template<typename T>
    requires(is_optional_v<T>&& requires(T) { typename bind_t<typename T::value_type>::return_t; }) struct bind_t<T>
    {
        using value_t  = typename bind_t<typename T::value_type>::value_t;
        using return_t = typename bind_t<typename T::value_type>::return_t;
    };

    template<typename T>
    concept bindable = requires
    {
        typename bind_t<std::decay_t<T>>::return_t;
    };

    ////////////////////////////////////////////////////////////////
    // Binding class.
    ////////////////////////////////////////////////////////////////

    /**
     * \brief Utility class that holds an arbitrary value and converts it to a type that can be passed to one of the sqlite bind functions.
     * \tparam T Value type.
     */
    template<bindable T>
    class Binding
    {
    public:
        using value_t  = typename bind_t<T>::value_t;
        using return_t = typename bind_t<T>::return_t;

        Binding() = delete;

        explicit Binding(T value) : value(std::move(value)) {}

        Binding(const Binding&) = delete;

        Binding(Binding&&) = default;

        ~Binding() = default;

        Binding& operator=(const Binding&) = delete;

        Binding& operator=(Binding&&) = default;

        [[nodiscard]] bool holdsValue() const noexcept
        {
            if constexpr (is_optional_v<T>)
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
                return getValue();
        }

    private:
        [[nodiscard]] return_t getValue() const noexcept
        {
            if constexpr (is_optional_v<T>)
                return *value;
            else
                return value;
        }
        T value;
    };
}  // namespace sql