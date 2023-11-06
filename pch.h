/*
    Copyright (c) 2023 Claudiu HBann

    See LICENSE for the full terms of the MIT License.
*/

#pragma once

namespace hbann
{
class IStreamable;
class StreamReader;
} // namespace hbann

// std
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#include <bit>
#include <cmath>
#include <codecvt>
#include <cstring>
#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <variant>
#include <vector>

// Streamable
constexpr auto STREAMABLE_INTERFACE_NAME = "IStreamable";

#define STREAMABLE_DEFINE_FROM_STREAM(baseClass, ...)                                                                  \
  public:                                                                                                              \
    constexpr void FromStream() override                                                                               \
    {                                                                                                                  \
        if constexpr (!::hbann::static_equal(#baseClass, STREAMABLE_INTERFACE_NAME))                                   \
        {                                                                                                              \
            baseClass::FromStream();                                                                                   \
        }                                                                                                              \
                                                                                                                       \
        mStreamReader.ReadAll(__VA_ARGS__);                                                                            \
    }

#define STREAMABLE_DEFINE_TO_STREAM(baseClass, ...)                                                                    \
  public:                                                                                                              \
    constexpr void ToStream() override                                                                                 \
    {                                                                                                                  \
        if constexpr (!::hbann::static_equal(#baseClass, STREAMABLE_INTERFACE_NAME))                                   \
        {                                                                                                              \
            baseClass::ToStream();                                                                                     \
        }                                                                                                              \
        else                                                                                                           \
        {                                                                                                              \
            Reserve(FindParseSize());                                                                                  \
        }                                                                                                              \
                                                                                                                       \
        mStreamWriter.WriteAll(__VA_ARGS__);                                                                           \
    }

#define STREAMABLE_DEFINE_FIND_PARSE_SIZE(baseClass, ...)                                                              \
  protected:                                                                                                           \
    [[nodiscard]] constexpr ::hbann::Size::size_max FindParseSize() override                                           \
    {                                                                                                                  \
        ::hbann::Size::size_max size{};                                                                                \
        if constexpr (!::hbann::static_equal(#baseClass, STREAMABLE_INTERFACE_NAME))                                   \
        {                                                                                                              \
            size += baseClass::FindParseSize();                                                                        \
        }                                                                                                              \
                                                                                                                       \
        size += ::hbann::SizeFinder::FindParseSize(__VA_ARGS__);                                                       \
                                                                                                                       \
        return size;                                                                                                   \
    }

#define STREAMABLE_DEFINE_INTRUSIVE                                                                                    \
  private:                                                                                                             \
    friend class ::hbann::StreamReader;                                                                                \
    friend class ::hbann::StreamWriter;

#define STATIC_ASSERT_HAS_ISTREAMABLE_BASE(baseClass)                                                                  \
    static_assert(std::is_base_of_v<::hbann::IStreamable, baseClass>, "The class must inherit a streamable!");

#define STREAMABLE_DEFINE(baseClass, ...)                                                                              \
    STATIC_ASSERT_HAS_ISTREAMABLE_BASE(baseClass)                                                                      \
    STREAMABLE_DEFINE_INTRUSIVE                                                                                        \
    STREAMABLE_DEFINE_TO_STREAM(baseClass, __VA_ARGS__)                                                                \
    STREAMABLE_DEFINE_FROM_STREAM(baseClass, __VA_ARGS__)                                                              \
    STREAMABLE_DEFINE_FIND_PARSE_SIZE(baseClass, __VA_ARGS__)

namespace hbann
{
template <typename> constexpr auto always_false = false;

namespace impl
{
template <typename> struct is_tuple : std::false_type
{
};

template <typename... Types> struct is_tuple<std::tuple<Types...>> : std::true_type
{
};

template <typename> struct is_pair : std::false_type
{
};

template <typename TypeFirst, typename TypeSecond> struct is_pair<std::pair<TypeFirst, TypeSecond>> : std::true_type
{
};

template <typename> struct is_variant : std::false_type
{
};

template <typename... Types> struct is_variant<std::variant<Types...>> : std::true_type
{
};
} // namespace impl

template <typename Type> constexpr bool is_tuple_v = impl::is_tuple<Type>::value;
template <typename Type> constexpr bool is_pair_v = impl::is_pair<Type>::value;
template <typename Type> constexpr bool is_variant_v = impl::is_variant<Type>::value;

template <typename Container>
concept has_method_reserve =
    std::ranges::contiguous_range<Container> && requires(Container &aContainer) { aContainer.reserve(size_t(0)); };

template <typename Type>
concept is_pointer_unique = std::is_same_v<std::remove_cvref_t<Type>, std::unique_ptr<typename Type::value_type>>;

template <typename Type>
concept is_pointer_shared = std::is_same_v<std::remove_cvref_t<Type>, std::shared_ptr<typename Type::value_type>>;

template <typename Type>
concept is_pointer = std::is_pointer_v<Type> || is_pointer_unique<Type> || is_pointer_shared<Type>;

template <typename Type>
concept is_std_lay_no_ptr = std::is_standard_layout_v<Type> && !is_pointer<Type>;

template <typename Base, typename Derived>
concept is_base_of_no_ptr = std::is_base_of_v<Base, std::remove_pointer_t<Derived>>;

template <typename Type>
concept is_path = std::is_same_v<std::remove_cvref_t<Type>, std::filesystem::path>;

template <typename Container>
concept is_range_std_lay =
    (std::ranges::contiguous_range<Container> && is_std_lay_no_ptr<typename Container::value_type>) ||
    is_path<Container>;

template <typename Container>
concept has_method_size = requires(Container &aContainer) { std::ranges::size(aContainer); };

template <typename Class>
concept has_method_find_derived_streamable = requires(StreamReader &aStreamReader) {
    {
        Class::FindDerivedStreamable(aStreamReader)
    } -> std::convertible_to<IStreamable *>;
};

constexpr bool static_equal(const char *aString1, const char *aString2) noexcept
{
    return *aString1 == *aString2 && (!*aString1 || static_equal(aString1 + 1, aString2 + 1));
}

template <typename Type, std::size_t I = 0> Type variant_from_index(const std::size_t aIndex)
{
    static_assert(is_variant_v<Type>, "Type is not a variant!");

    if constexpr (I < std::variant_size_v<Type>)
    {
        return aIndex ? variant_from_index<Type, I + 1>(aIndex - 1) : Type{std::in_place_index<I>};
    }
    else
    {
        throw std::out_of_range("Out of bounds variant index!");
    }
}
} // namespace hbann

/*
    TODO:
         - objects that have std_lay should come before like std::pair or std::tuple
         - check SizeFinder for for incorrect use of the Size::findrequired bytes and etc...
         - can FindDerivedStreamable be protected or even private?
         - make the user choose the data type for the stream
         - make the tostream and fromstream private or protected
         - can Streamable call the intermediate class's FindDerivedStreamable automatically?
         - when reserving size for wstrings that have been encoded we reserve more (worst case x2)
         - use monostate for the std::variant
         - FindRangeSize should not check for contiguous range when finding size of a range
         - when finding derived class from base class pointer, add a tuple representing the types that can be read and
        make the user access the objects by index so can't read a bad object
         - add separated examples
         - add tests for converter
         - add support for unique_ptr and shared_ptr
         - add hbann::Size tests

    WARNING:
         - when adding a new feature try to implement the serialize and deserialize algorithms with SizeFinder for the
   object and, if possible, the reserve method
*/
