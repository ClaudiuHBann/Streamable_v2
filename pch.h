/*
    Copyright (c) 2024 Claudiu HBann

    See LICENSE for the full terms of the MIT License.
*/

#pragma once

namespace hbann
{
class IStreamable;
class StreamReader;
} // namespace hbann

#ifdef _WIN32

#define NOIME
#define NOMCX
#define NOMINMAX
#define NOSERVICE
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#else
#warning "Platform does not support encoding UTF16 strings to save memory!"
#endif

#include <bit>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <variant>
#include <vector>

// Streamable
inline constexpr auto STREAMABLE_INTERFACE_NAME = "IStreamable";

#define EXPAND(x) x
#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, MACRO, ...) MACRO

// clang-format off
#define PASTE(...) EXPAND(GET_MACRO(__VA_ARGS__, PASTE9, PASTE8, PASTE7, PASTE6, PASTE5, PASTE4, PASTE3, PASTE2, PASTE1)(__VA_ARGS__))
// Macro Magik Mate :D
#define PASTE1(func, v1) func(v1)
#define PASTE2(func, v1, v2) PASTE1(func, v1) PASTE1(func, v2)
#define PASTE3(func, v1, v2, v3) PASTE1(func, v1) PASTE2(func, v2, v3)
#define PASTE4(func, v1, v2, v3, v4) PASTE1(func, v1) PASTE3(func, v2, v3, v4)
#define PASTE5(func, v1, v2, v3, v4, v5) PASTE1(func, v1) PASTE4(func, v2, v3, v4, v5)
#define PASTE6(func, v1, v2, v3, v4, v5, v6) PASTE1(func, v1) PASTE5(func, v2, v3, v4, v5, v6)
#define PASTE7(func, v1, v2, v3, v4, v5, v6, v7) PASTE1(func, v1) PASTE6(func, v2, v3, v4, v5, v6, v7)
#define PASTE8(func, v1, v2, v3, v4, v5, v6, v7, v8) PASTE1(func, v1) PASTE7(func, v2, v3, v4, v5, v6, v7, v8)
#define PASTE9(func, v1, v2, v3, v4, v5, v6, v7, v8, v9) PASTE1(func, v1) PASTE8(func, v2, v3, v4, v5, v6, v7, v8, v9)
// clang-format on

#define TO_STREAM(baseClass)                                                                                           \
    if constexpr (!::hbann::static_equal(#baseClass, STREAMABLE_INTERFACE_NAME))                                       \
    {                                                                                                                  \
        baseClass::ToStream();                                                                                         \
    }
#define TO_STREAM_ALL(...) EXPAND(PASTE(TO_STREAM, __VA_ARGS__))

#define FROM_STREAM(baseClass)                                                                                         \
    if constexpr (!::hbann::static_equal(#baseClass, STREAMABLE_INTERFACE_NAME))                                       \
    {                                                                                                                  \
        baseClass::FromStream();                                                                                       \
    }
#define FROM_STREAM_ALL(...) EXPAND(PASTE(FROM_STREAM, __VA_ARGS__))

// IStreamable by default because it must not be empty and does nothing
#define STREAMABLE_DEFINE_BASE(...) IStreamable, __VA_ARGS__

#define STREAMABLE_RESET_ACCESS_MODIFIER private:

#define STREAMABLE_DEFINE_FROM_STREAM(baseClasses, ...)                                                                \
  protected:                                                                                                           \
    constexpr void FromStream() override                                                                               \
    {                                                                                                                  \
        FROM_STREAM_ALL(baseClasses);                                                                                  \
                                                                                                                       \
        mStreamReader.ReadAll(__VA_ARGS__);                                                                            \
    }                                                                                                                  \
                                                                                                                       \
    STREAMABLE_RESET_ACCESS_MODIFIER

#define STREAMABLE_DEFINE_TO_STREAM(baseClasses, ...)                                                                  \
  protected:                                                                                                           \
    constexpr void ToStream() override                                                                                 \
    {                                                                                                                  \
        TO_STREAM_ALL(baseClasses);                                                                                    \
                                                                                                                       \
        mStreamWriter.WriteAll(__VA_ARGS__);                                                                           \
    }                                                                                                                  \
                                                                                                                       \
    STREAMABLE_RESET_ACCESS_MODIFIER

#define STREAMABLE_DEFINE_INTRUSIVE                                                                                    \
  private:                                                                                                             \
    friend class ::hbann::StreamReader;                                                                                \
    friend class ::hbann::StreamWriter;

#define STATIC_ASSERT_HAS_ISTREAMABLE_BASE(baseClasses)                                                                \
    static_assert(::hbann::are_derived_from<::hbann::IStreamable, baseClasses>, "The class must inherit a "            \
                                                                                "streamable!");

#define STREAMABLE_DEFINE(className, baseClasses, ...)                                                                 \
    STATIC_ASSERT_HAS_ISTREAMABLE_BASE(baseClasses)                                                                    \
    STREAMABLE_DEFINE_INTRUSIVE                                                                                        \
    STREAMABLE_DEFINE_TO_STREAM(baseClasses, __VA_ARGS__)                                                              \
    STREAMABLE_DEFINE_FROM_STREAM(baseClasses, __VA_ARGS__)                                                            \
    STREAMABLE_RESET_ACCESS_MODIFIER

namespace hbann
{
namespace detail
{
template <typename> struct is_pair : std::false_type
{
};
template <typename TypeFirst, typename TypeSecond> struct is_pair<std::pair<TypeFirst, TypeSecond>> : std::true_type
{
};

template <typename> struct is_tuple : std::false_type
{
};
template <typename... Types> struct is_tuple<std::tuple<Types...>> : std::true_type
{
};

template <typename> struct is_variant : std::false_type
{
};
template <typename... Types> struct is_variant<std::variant<Types...>> : std::true_type
{
};

template <typename> struct is_unique_ptr : std::false_type
{
};
template <typename Type> struct is_unique_ptr<std::unique_ptr<Type>> : std::true_type
{
};

template <typename> struct is_optional : std::false_type
{
};
template <typename Type> struct is_optional<std::optional<Type>> : std::true_type
{
};

template <typename> struct is_shared_ptr : std::false_type
{
};
template <typename Type> struct is_shared_ptr<std::shared_ptr<Type>> : std::true_type
{
};

template <typename> struct is_basic_string : std::false_type
{
};
template <typename... Types> struct is_basic_string<std::basic_string<Types...>> : std::true_type
{
};
} // namespace detail

template <typename Type> inline constexpr bool is_pair_v = detail::is_pair<Type>::value;
template <typename Type> inline constexpr bool is_tuple_v = detail::is_tuple<Type>::value;
template <typename Type> inline constexpr bool is_variant_v = detail::is_variant<Type>::value;
template <typename Type> inline constexpr bool is_optional_v = detail::is_optional<Type>::value;
template <typename Type> inline constexpr bool is_unique_ptr_v = detail::is_unique_ptr<Type>::value;
template <typename Type> inline constexpr bool is_shared_ptr_v = detail::is_shared_ptr<Type>::value;
template <typename Type> inline constexpr bool is_basic_string_v = detail::is_basic_string<Type>::value;

template <typename> inline constexpr auto always_false = false;

template <typename Base, typename... Derived>
concept are_derived_from = (std::derived_from<Derived, Base> && ...);

template <typename Type>
concept is_wstring = std::is_same_v<typename Type::value_type, std::wstring::value_type> && is_basic_string_v<Type>;

template <typename Type>
concept is_u16string = std::is_same_v<typename Type::value_type, std::u16string::value_type> && is_basic_string_v<Type>;

template <typename Type>
concept is_utf16string = is_u16string<Type> || (sizeof(std::wstring::value_type) == 2 && is_wstring<Type>);

template <typename Type>
concept is_smart_pointer = is_shared_ptr_v<Type> || is_unique_ptr_v<Type>;

template <typename Type>
concept is_any_pointer = std::is_pointer_v<Type> || is_smart_pointer<Type>;

template <typename Derived, typename Base>
concept is_derived_from_pointer =
    std::is_pointer_v<Derived> && std::derived_from<std::remove_pointer_t<Derived>, Base> ||
    (is_smart_pointer<Derived> && std::derived_from<typename Derived::element_type, Base>);

template <typename Type>
concept is_standard_layout_no_pointer = std::is_standard_layout_v<Type> && !is_any_pointer<Type>;

template <typename Type>
concept is_path = std::is_same_v<Type, std::filesystem::path>;

template <typename Container>
concept is_range_standard_layout =
    (std::ranges::contiguous_range<Container> && is_standard_layout_no_pointer<typename Container::value_type>) ||
    is_path<Container>;

template <typename Container>
concept has_method_size = requires(Container &aContainer) { std::ranges::size(aContainer); };

[[nodiscard]] constexpr bool static_equal(const char *aString1, const char *aString2) noexcept
{
    return *aString1 == *aString2 && (!*aString1 || static_equal(aString1 + 1, aString2 + 1));
}

template <typename Type, std::size_t vIndex = 0>
[[nodiscard]] constexpr Type variant_from_index(const std::size_t aIndex)
{
    static_assert(is_variant_v<Type>, "Type is not a variant!");

    if constexpr (vIndex < std::variant_size_v<Type>)
    {
        return aIndex ? variant_from_index<Type, vIndex + 1>(aIndex - 1) : Type{std::in_place_index<vIndex>};
    }
    else
    {
        throw std::out_of_range("Out of bounds variant index!");
    }
}
} // namespace hbann

/*
    TODO:
         - add separated examples
         - refactor tests

    FEATURES:
         - instead of reading object to jump over the value, create a jump method

    UX:
         - when finding derived class from base class pointer, add a tuple representing the types that can be read
   and make the user access the objects by index so can't read a bad object
*/
