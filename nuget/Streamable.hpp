/*
    Copyright (c) 2024 Claudiu HBann
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#pragma once

namespace hbann
{
class Converter;
class IStreamable;
class Size;
class SizeFinder;
class Stream;
class StreamReader;
class StreamWriter;
} // namespace hbann

// native
#ifdef _WIN32

#ifndef NOIME
#define NOIME
#endif // !NOIME

#ifndef NOMCX
#define NOMCX
#endif // !NOMCX

#ifndef NOMINMAX
#define NOMINMAX
#endif // !NOMINMAX

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // !WIN32_LEAN_AND_MEAN

#include <Windows.h>

#else
#warning "Platform does not support encoding UTF16 strings to save memory!"
#endif

// std
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
#define EXPAND(x) x
#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, MACRO, ...) MACRO

// clang-format off
#define PASTE(...) EXPAND(GET_MACRO(__VA_ARGS__, PASTE9, PASTE8, PASTE7, PASTE6, PASTE5, PASTE4, PASTE3, PASTE2, PASTE1)(__VA_ARGS__))

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

#define TS_BASE(base) base::ToStream();
#define TS_BASES(...) EXPAND(PASTE(TS_BASE, __VA_ARGS__))

#define FS_BASE(base) base::FromStream();
#define FS_BASES(...) EXPAND(PASTE(FS_BASE, __VA_ARGS__))

#define STATIC_ASSERT_HAS_ISTREAMABLE_BASE(...)                                                                        \
    static_assert(::hbann::are_derived_from_istreamable<__VA_ARGS__>, "The class must inherit a streamable!");

#define STATIC_ASSERT_DONT_PASS_ISTREAMABLE_AS_BASE(...)                                                               \
    static_assert(!::hbann::are_same_as_istreamable<__VA_ARGS__>, "The class ::hbann::IStreamable should not be a "    \
                                                                  "base!");

#define STREAMABLE_RESET_ACCESS_MODIFIER private:

#define STREAMABLE_DEFINE_TO_STREAM_BASES(...)                                                                         \
  protected:                                                                                                           \
    void ToStreamBases() override                                                                                      \
    {                                                                                                                  \
        TS_BASES(__VA_ARGS__);                                                                                         \
    }                                                                                                                  \
                                                                                                                       \
    STREAMABLE_RESET_ACCESS_MODIFIER

#define STREAMABLE_DEFINE_FROM_STREAM_BASES(...)                                                                       \
  protected:                                                                                                           \
    void FromStreamBases() override                                                                                    \
    {                                                                                                                  \
        FS_BASES(__VA_ARGS__);                                                                                         \
    }                                                                                                                  \
                                                                                                                       \
    STREAMABLE_RESET_ACCESS_MODIFIER

#define STREAMABLE_DEFINE_BASE(...)                                                                                    \
    STATIC_ASSERT_HAS_ISTREAMABLE_BASE(__VA_ARGS__)                                                                    \
    STATIC_ASSERT_DONT_PASS_ISTREAMABLE_AS_BASE(__VA_ARGS__)                                                           \
                                                                                                                       \
    STREAMABLE_DEFINE_TO_STREAM_BASES(__VA_ARGS__)                                                                     \
    STREAMABLE_DEFINE_FROM_STREAM_BASES(__VA_ARGS__)                                                                   \
                                                                                                                       \
    STREAMABLE_RESET_ACCESS_MODIFIER

#define STREAMABLE_DEFINE_FROM_STREAM(className, ...)                                                                  \
  protected:                                                                                                           \
    void FromStream() override                                                                                         \
    {                                                                                                                  \
        className::FromStreamBases();                                                                                  \
                                                                                                                       \
        mStreamReader.ReadAll(__VA_ARGS__);                                                                            \
    }                                                                                                                  \
                                                                                                                       \
    STREAMABLE_RESET_ACCESS_MODIFIER

#define STREAMABLE_DEFINE_TO_STREAM(className, ...)                                                                    \
  protected:                                                                                                           \
    void ToStream() override                                                                                           \
    {                                                                                                                  \
        className::ToStreamBases();                                                                                    \
                                                                                                                       \
        mStreamWriter.WriteAll(__VA_ARGS__);                                                                           \
    }                                                                                                                  \
                                                                                                                       \
    STREAMABLE_RESET_ACCESS_MODIFIER

#define STREAMABLE_DEFINE_INTRUSIVE                                                                                    \
  private:                                                                                                             \
    friend class ::hbann::StreamReader;                                                                                \
    friend class ::hbann::StreamWriter;

#define STREAMABLE_DEFINE(className, ...)                                                                              \
    STREAMABLE_DEFINE_INTRUSIVE                                                                                        \
                                                                                                                       \
    STREAMABLE_DEFINE_TO_STREAM(className, __VA_ARGS__)                                                                \
    STREAMABLE_DEFINE_FROM_STREAM(className, __VA_ARGS__)                                                              \
                                                                                                                       \
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

template <typename... Types>
concept are_derived_from_istreamable = (std::derived_from<Types, IStreamable> && ...);

template <typename... Types>
concept are_same_as_istreamable = (std::is_same_v<Types, IStreamable> || ...);

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

[[nodiscard]] consteval bool static_equal(const char *aString1, const char *aString2) noexcept
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

class Converter
{
  public:
    template <typename Type> [[nodiscard]] static constexpr auto Encode(const Type &aString)
    {
        static_assert(is_utf16string<Type>, "Type must be a UTF16 string!");

#ifdef _WIN32
        const auto requiredSize = WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<LPCWCH>(aString.data()),
                                                      static_cast<int>(aString.size()), nullptr, 0, nullptr, nullptr);
        std::string str(requiredSize, 0);

        WideCharToMultiByte(CP_UTF8, 0, reinterpret_cast<LPCWCH>(aString.data()), static_cast<int>(aString.size()),
                            str.data(), requiredSize, nullptr, nullptr);
        return str;
#else
        return std::string_view{reinterpret_cast<const std::string::value_type *>(aString.data()),
                                aString.size() * sizeof(typename Type::value_type)};
#endif
    }

    template <typename Type> [[nodiscard]] static constexpr auto Decode(const std::span<const uint8_t> aString)
    {
        static_assert(is_utf16string<Type>, "Type must be a UTF16 string!");

#ifdef _WIN32
        const auto requiredSize = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCCH>(aString.data()),
                                                      static_cast<int>(aString.size()), nullptr, 0);
        Type str(requiredSize, 0);

        MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<LPCCH>(aString.data()), static_cast<int>(aString.size()),
                            str.data(), requiredSize);
        return str;
#else
        using TypeValueType = typename Type::value_type;
        return std::basic_string_view<TypeValueType>{reinterpret_cast<const TypeValueType *>(aString.data()),
                                                     aString.size() / sizeof(TypeValueType)};
#endif
    }
};

/*
    Format: first 3 bits + the actual size at last

    The first 3 bits represent how many bytes are necessary to represent the actual size with those 3 bits and they are
   written at the most left side of the 1-8 bytes.

    The actual size is written to the left most side of the 1-8 bytes.
*/
class Size
{
  public:
    using size_max = size_t; // size_t / (4/8)
    using span = std::span<const uint8_t>;

    [[nodiscard]] static constexpr auto FindRequiredBytes(const uint8_t aSize) noexcept
    {
        if constexpr (SIZE_MAX_IN_BYTES == 4)
        {
            return static_cast<size_max>(aSize >> 6);
        }
        else
        {
            return static_cast<size_max>(aSize >> 5);
        }
    }

    [[nodiscard]] static constexpr size_max FindRequiredBytes(const size_max aSize) noexcept
    {
        size_max requiredBits{1};
        if (aSize)
        {
            // add the bits required to represent the size
            requiredBits += static_cast<size_max>(std::log2(aSize));
        }

        // add the bits required to represent the required bytes to store the final value
        if constexpr (SIZE_MAX_IN_BYTES == 4)
        {
            requiredBits += 2;
        }
        else
        {
            requiredBits += 3;
        }

        // add 7 bits to round the final value up
        return (requiredBits + 7) / 8;
    }

    [[nodiscard]] static inline auto MakeSize(const size_max aSize) noexcept
    {
        thread_local uint8_t SIZE_AS_CHARS[SIZE_MAX_IN_BYTES]{};
        auto &SIZE = *reinterpret_cast<size_max *>(SIZE_AS_CHARS);

        const auto requiredBytes = FindRequiredBytes(aSize);
        auto SIZE_AS_CHARS_START = SIZE_AS_CHARS + (SIZE_MAX_IN_BYTES - requiredBytes);

        // write the size itself
        SIZE = ToBigEndian(aSize);
        // write the 3 bits representing the bytes required
        if constexpr (SIZE_MAX_IN_BYTES == 4)
        {
            *SIZE_AS_CHARS_START |= requiredBytes << 6;
        }
        else
        {
            *SIZE_AS_CHARS_START |= requiredBytes << 5;
        }

        // return the last 'requiredBytes' from SIZE
        return span{SIZE_AS_CHARS_START, requiredBytes};
    }

    [[nodiscard]] static inline auto MakeSize(const span aSize) noexcept
    {
        uint8_t SIZE_AS_CHARS[SIZE_MAX_IN_BYTES]{};
        auto &SIZE = *reinterpret_cast<size_max *>(SIZE_AS_CHARS);

        size_max requiredBytes{};
        if constexpr (SIZE_MAX_IN_BYTES == 4)
        {
            requiredBytes = static_cast<size_max>(aSize.front() >> 6);
        }
        else
        {
            requiredBytes = static_cast<size_max>(aSize.front() >> 5);
        }

        auto SIZE_AS_CHARS_START = SIZE_AS_CHARS + (SIZE_MAX_IN_BYTES - requiredBytes);
        // copy only the resizeable size
        std::memcpy(SIZE_AS_CHARS_START, aSize.data(), requiredBytes);

        // clear the required bytes
        if constexpr (SIZE_MAX_IN_BYTES == 4)
        {
            *SIZE_AS_CHARS_START &= 0b00111111;
        }
        else
        {
            *SIZE_AS_CHARS_START &= 0b00011111;
        }

        return ToBigEndian(SIZE);
    }

  private:
    static inline constexpr auto SIZE_MAX_IN_BYTES = sizeof(size_max);

    [[nodiscard]] static constexpr size_max ToBigEndian(const size_max aSize) noexcept
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            if constexpr (SIZE_MAX_IN_BYTES == 4)
            {
                return ((aSize >> 24) & 0x000000FF) | ((aSize >> 8) & 0x0000FF00) | ((aSize << 8) & 0x00FF0000) |
                       ((aSize << 24) & 0xFF000000);
            }
            else
            {
                return ((aSize & 0xFF00000000000000) >> 56) | ((aSize & 0x00FF000000000000) >> 40) |
                       ((aSize & 0x0000FF0000000000) >> 24) | ((aSize & 0x000000FF00000000) >> 8) |
                       ((aSize & 0x00000000FF000000) << 8) | ((aSize & 0x0000000000FF0000) << 24) |
                       ((aSize & 0x000000000000FF00) << 40) | ((aSize & 0x00000000000000FF) << 56);
            }
        }
        else
        {
            return aSize;
        }
    }
};

class Stream
{
    friend class StreamReader;

  public:
    using vector = std::vector<uint8_t>;
    using span = std::span<const uint8_t>;
    using stream = std::variant<vector, span>;

    constexpr Stream() noexcept : mStream(vector())
    {
    }

    constexpr Stream(const Stream &) noexcept
    {
    }

    constexpr Stream(const span aSpan) noexcept : mStream(aSpan)
    {
    }

    constexpr Stream(Stream &&aStream) noexcept
    {
        *this = std::move(aStream);
    }

    constexpr vector &&Release() noexcept
    {
        return std::move(GetStream());
    }

    template <typename FunctionSeek>
    constexpr decltype(auto) Peek(FunctionSeek &&aFunctionSeek, const Size::size_max aOffset = 0)
    {
        const auto readIndex = mReadIndex;
        mReadIndex += aOffset;
        aFunctionSeek(readIndex);
        mReadIndex = readIndex;

        return *this;
    }

    constexpr decltype(auto) Reserve(const Size::size_max aSize)
    {
        GetStream().reserve(aSize);
        return *this;
    }

    [[nodiscard]] constexpr auto View() noexcept
    {
        const auto spen = std::get_if<span>(&mStream);
        return spen ? *spen : GetStream();
    }

    [[nodiscard]] constexpr auto Read(Size::size_max aSize) noexcept
    {
        const auto view = View();

        // clamp read count
        if (mReadIndex + aSize > view.size())
        {
            aSize = view.size() - mReadIndex;
        }

        mReadIndex += aSize;
        return span{view.data() + (mReadIndex - aSize), aSize};
    }

    [[nodiscard]] constexpr auto Current() noexcept
    {
        return View()[mReadIndex];
    }

    constexpr decltype(auto) Write(const span aSpan)
    {
        GetStream().insert(GetStream().end(), aSpan.data(), aSpan.data() + aSpan.size());
        return *this;
    }

    constexpr Stream &operator=(const Stream &) noexcept
    {
        return *this;
    }

    constexpr Stream &operator=(Stream &&aStream) noexcept
    {
        mStream = std::move(aStream.mStream);
        mReadIndex = aStream.mReadIndex;

        return *this;
    }

    constexpr decltype(auto) Clear() noexcept
    {
        GetStream().clear();
        return *this;
    }

  private:
    stream mStream{};
    Size::size_max mReadIndex{};

    constexpr vector &GetStream() noexcept
    {
        // if crashed here --> it's read only (span)
        return std::get<vector>(mStream);
    }
};

class SizeFinder
{
  public:
    template <typename Type> [[nodiscard]] static consteval Size::size_max FindRangeRank() noexcept
    {
        using TypeRaw = std::remove_cvref_t<Type>;

        if constexpr (std::ranges::range<TypeRaw>)
        {
            return 1 + FindRangeRank<typename TypeRaw::value_type>();
        }
        else
        {
            return 0;
        }
    }

    template <std::ranges::range Range>
    [[nodiscard]] static constexpr Size::size_max GetRangeCount(const Range &aRange) noexcept
    {
        using RangeRaw = std::remove_cvref_t<Range>;

        if constexpr (has_method_size<RangeRaw>)
        {
            return std::ranges::size(aRange);
        }
        else if constexpr (is_path<RangeRaw>)
        {
            return aRange.native().size();
        }
        else
        {
            static_assert(always_false<RangeRaw>, "Tried to get the range count from an unknown object!");
        }
    }
};

class StreamReader
{
  public:
    constexpr StreamReader(Stream &aStream) noexcept : mStream(&aStream)
    {
    }

    constexpr StreamReader(const StreamReader &aStreamReader) noexcept : mStream(aStreamReader.mStream)
    {
    }

    constexpr StreamReader(StreamReader &&aStreamReader) noexcept
    {
        *this = std::move(aStreamReader);
    }

    template <typename Type, typename... Types> constexpr void ReadAll(Type &aObject, Types &...aObjects)
    {
        using TypeRaw = std::remove_cvref_t<Type>;

        Read<TypeRaw>(aObject);

        if constexpr (sizeof...(aObjects))
        {
            ReadAll<Types...>(aObjects...);
        }
    }

    constexpr void ReadAll()
    {
    }

    template <typename FunctionSeek>
    constexpr decltype(auto) Peek(FunctionSeek &&aFunctionSeek, const Size::size_max aOffset = 0)
    {
        mStream->Peek(std::forward<FunctionSeek>(aFunctionSeek), aOffset);
        return *this;
    }

    constexpr StreamReader &operator=(const StreamReader &aStreamReader) noexcept
    {
        mStream = aStreamReader.mStream;
        return *this;
    }

    constexpr StreamReader &operator=(StreamReader &&aStreamReader) noexcept
    {
        mStream = aStreamReader.mStream;
        return *this;
    }

  private:
    Stream *mStream{};

    template <typename Type> constexpr decltype(auto) Read(Type &aObject)
    {
        if constexpr (is_optional_v<Type>)
        {
            return ReadOptional(aObject);
        }
        else if constexpr (is_variant_v<Type>)
        {
            return ReadVariant(aObject);
        }
        else if constexpr (is_tuple_v<Type>)
        {
            std::apply([&](auto &&...aArgs) { ReadAll(aArgs...); }, aObject);
            return *this;
        }
        else if constexpr (is_pair_v<Type>)
        {
            // we remove the constness of map's pair's key so we can use the already implemented Read branches
            auto &first = const_cast<std::remove_const_t<typename Type::first_type> &>(aObject.first);
            return ReadAll(first, aObject.second);
        }
        else if constexpr (std::ranges::range<Type>)
        {
            aObject = std::move(ReadRange<Type>());
            return *this;
        }
        else if constexpr (std::derived_from<Type, IStreamable>)
        {
            return ReadStreamable(aObject);
        }
        else if constexpr (is_any_pointer<Type>)
        {
            return ReadPointer(aObject);
        }
        else if constexpr (is_standard_layout_no_pointer<Type>)
        {
            return ReadObjectOfKnownSize(aObject);
        }
        else
        {
            static_assert(always_false<Type>, "Type is not accepted!");
        }
    }

    template <typename Type> constexpr decltype(auto) ReadOptional(Type &aOpt)
    {
        static_assert(is_optional_v<Type>, "Type is not an optional!");

        using TypeValueType = typename Type::value_type;

        if (ReadCount())
        {
            TypeValueType obj{};
            Read(obj);
            aOpt = std::move(obj);
        }

        return *this;
    }

    template <typename Type> constexpr decltype(auto) ReadVariant(Type &aVariant)
    {
        static_assert(is_variant_v<Type>, "Type is not a variant!");

        aVariant = variant_from_index<Type>(ReadCount());
        std::visit([&](auto &&aArg) { Read(aArg); }, aVariant);

        return *this;
    }

    template <typename Type> constexpr decltype(auto) ReadPointer(Type &aPointer)
    {
        static_assert(is_any_pointer<Type>, "Type is not a smart/raw pointer!");

        if constexpr (is_unique_ptr_v<Type>)
        {
            aPointer = std::make_unique<typename Type::element_type>();
        }
        else if constexpr (is_shared_ptr_v<Type>)
        {
            aPointer = std::make_shared<typename Type::element_type>();
        }
        else if constexpr (!is_derived_from_pointer<Type, IStreamable>)
        {
            // raw pointers MUST be allocated only for non streamables
            aPointer = new std::remove_pointer_t<Type>;
        }

        // we treat pointers to streamable in a special way
        if constexpr (is_derived_from_pointer<Type, IStreamable>)
        {
            return ReadStreamablePtr(aPointer);
        }
        else
        {
            return Read(*aPointer);
        }
    }

    template <typename Type> constexpr decltype(auto) ReadStreamable(Type &aStreamable)
    {
        static_assert(std::derived_from<Type, IStreamable>, "Type is not a streamable!");

        aStreamable.Deserialize(mStream->Read(ReadCount()), false); // read streamable size in bytes
        return *this;
    }

    template <typename Type> constexpr decltype(auto) ReadStreamablePtr(Type &aStreamablePtr)
    {
        static_assert(is_derived_from_pointer<Type, IStreamable>, "Type is not a streamable smart/raw pointer!");

        using TypeNoPtr = std::conditional_t<std::is_pointer_v<Type>, std::remove_pointer_t<Type>,
                                             typename std::pointer_traits<Type>::element_type>;

        // we cannot use the has_method_find_derived_streamable concept because we must use the context of the
        // StreamReader that is a friend of streamables
        static_assert(
            requires(StreamReader &aStreamReader) {
                {
                    TypeNoPtr::FindDerivedStreamable(aStreamReader)
                } -> std::convertible_to<IStreamable *>;
            }, "Type doesn't have a public/protected method 'static "
               "IStreamable* FindDerivedStreamable(StreamReader &)' !");

        Peek([&](auto) {
            Stream stream(mStream->Read(ReadCount())); // read streamable size in bytes
            StreamReader streamReader(stream);

            // TODO: we let the user read n objects after wich we read again... fix it
            if constexpr (is_smart_pointer<Type>)
            {
                aStreamablePtr.reset(static_cast<TypeNoPtr *>(TypeNoPtr::FindDerivedStreamable(streamReader)));
            }
            else
            {
                aStreamablePtr = dynamic_cast<TypeNoPtr *>(TypeNoPtr::FindDerivedStreamable(streamReader));
            }
        });

        aStreamablePtr->Deserialize(mStream->Read(ReadCount()), false);
        return *this;
    }

    template <typename Type> [[nodiscard]] constexpr Type ReadRange()
    {
        static_assert(std::ranges::range<Type>, "Type is not a range!");

        using TypeValueType = typename Type::value_type;

        Type range{};
        const auto count = ReadCount();

        if constexpr (SizeFinder::FindRangeRank<Type>() > 1)
        {
            for (size_t i = 0; i < count; i++)
            {
                range.insert(std::ranges::cend(range), ReadRange<TypeValueType>());
            }
        }
        else
        {
            ReadRangeRank1(range, count);
        }

        return range;
    }

    template <typename Type> constexpr decltype(auto) ReadPath(Type &aRange, const Size::size_max aCount)
    {
        static_assert(is_path<Type>, "Type is not a path!");

        using TypeStringType = typename Type::string_type;

        TypeStringType pathNative{};
        ReadRangeStandardLayout(pathNative, aCount);
        aRange.assign(pathNative);

        return *this;
    }

    template <typename Type> constexpr StreamReader &ReadRangeStandardLayout(Type &aRange, const Size::size_max aCount)
    {
        static_assert(is_range_standard_layout<Type>, "Type is not a standard layout range!");

        using TypeValueType = typename Type::value_type;

        if constexpr (is_utf16string<Type>)
        {
            aRange.assign(Converter::Decode<Type>(mStream->Read(aCount)));
        }
        else if constexpr (is_path<Type>)
        {
            ReadPath(aRange, aCount);
        }
        else
        {
            const auto rangeView = mStream->Read(aCount * sizeof(TypeValueType));
            const auto rangePtr = reinterpret_cast<const TypeValueType *>(rangeView.data());
            aRange.assign(rangePtr, rangePtr + rangeView.size() / sizeof(TypeValueType));
        }

        return *this;
    }

    template <typename Type> constexpr decltype(auto) ReadRangeRank1(Type &aRange, const Size::size_max aCount)
    {
        static_assert(std::ranges::range<Type>, "Type is not a range!");

        using TypeValueType = typename Type::value_type;

        if constexpr (is_range_standard_layout<Type>)
        {
            ReadRangeStandardLayout(aRange, aCount);
        }
        else
        {
            for (size_t i = 0; i < aCount; i++)
            {
                TypeValueType object{};
                Read(object);
                aRange.insert(std::ranges::cend(aRange), std::move(object));
            }
        }

        return *this;
    }

    template <typename Type> constexpr decltype(auto) ReadObjectOfKnownSize(Type &aObject)
    {
        static_assert(is_standard_layout_no_pointer<Type>, "Type is not an object of known size or it is a pointer!");

        const auto view = mStream->Read(sizeof(Type));
        aObject = *reinterpret_cast<const Type *>(view.data());

        return *this;
    }

    inline Size::size_max ReadCount() noexcept
    {
        const auto size = Size::FindRequiredBytes(mStream->Current());
        return Size::MakeSize(mStream->Read(size));
    }
};

class StreamWriter
{
  public:
    constexpr StreamWriter(Stream &aStream) noexcept : mStream(&aStream)
    {
    }

    constexpr StreamWriter(const StreamWriter &aStreamWriter) noexcept : mStream(aStreamWriter.mStream)
    {
    }

    constexpr StreamWriter(StreamWriter &&aStreamWriter) noexcept
    {
        *this = std::move(aStreamWriter);
    }

    template <typename Type, typename... Types> constexpr void WriteAll(Type &aObject, Types &...aObjects)
    {
        using TypeRaw = std::remove_cvref_t<Type>;

        Write<TypeRaw>(aObject);

        if constexpr (sizeof...(aObjects))
        {
            WriteAll<Types...>(aObjects...);
        }
    }

    constexpr void WriteAll()
    {
    }

    constexpr StreamWriter &operator=(const StreamWriter &aStreamWriter) noexcept
    {
        mStream = aStreamWriter.mStream;
        return *this;
    }

    constexpr StreamWriter &operator=(StreamWriter &&aStreamWriter) noexcept
    {
        mStream = aStreamWriter.mStream;
        return *this;
    }

  private:
    Stream *mStream{};

    template <typename Type> constexpr decltype(auto) WriteObjectOfKnownSize(Type &aObject)
    {
        static_assert(is_standard_layout_no_pointer<Type>, "Type is not an object of known size or it is a pointer!");

        const auto objectPtr = reinterpret_cast<uint8_t *>(&aObject);
        mStream->Write({objectPtr, sizeof(aObject)});

        return *this;
    }

    inline decltype(auto) WriteCount(const Size::size_max aSize)
    {
        mStream->Write(Size::MakeSize(aSize));
        return *this;
    }

    template <typename Type> constexpr decltype(auto) WriteStreamable(Type &aStreamable)
    {
        static_assert(std::derived_from<Type, IStreamable>, "Type is not a streamable!");

        auto stream(std::move(aStreamable.Serialize()));
        const auto streamView = stream.View();

        // we write the size in bytes of the stream
        WriteCount(streamView.size());
        return mStream->Write(streamView);
    }

    template <typename Type> constexpr decltype(auto) WriteRange(Type &aRange)
    {
        static_assert(std::ranges::range<Type>, "Type is not a range!");

        if constexpr (SizeFinder::FindRangeRank<Type>() > 1)
        {
            WriteCount(SizeFinder::GetRangeCount(aRange));
            for (auto &object : aRange)
            {
                WriteRange(object);
            }
        }
        else
        {
            WriteRangeRank1(aRange);
        }

        return *this;
    }

    template <typename Type> constexpr decltype(auto) WriteRangeStandardLayout(const Type &aRange)
    {
        static_assert(is_range_standard_layout<Type>, "Type is not a standard layout range!");

        using TypeValueType = typename Type::value_type;

        if constexpr (is_utf16string<Type>)
        {
            WriteRangeStandardLayout(Converter::Encode(aRange));
        }
        else if constexpr (is_path<Type>)
        {
            WriteRangeStandardLayout(aRange.native());
        }
        else
        {
            WriteCount(SizeFinder::GetRangeCount(aRange));

            const auto rangePtr = reinterpret_cast<const uint8_t *>(std::ranges::data(aRange));
            const auto rangeSize = SizeFinder::GetRangeCount(aRange) * sizeof(TypeValueType);
            mStream->Write({rangePtr, rangeSize});
        }

        return *this;
    }

    template <typename Type> constexpr decltype(auto) WriteRangeRank1(Type &aRange)
    {
        static_assert(SizeFinder::FindRangeRank<Type>() == 1, "Type is not a rank 1 range!");

        if constexpr (is_range_standard_layout<Type>)
        {
            WriteRangeStandardLayout(aRange);
        }
        else
        {
            WriteCount(SizeFinder::GetRangeCount(aRange));
            for (auto &object : aRange)
            {
                Write(object);
            }
        }

        return *this;
    }

    template <typename Type> constexpr decltype(auto) WriteVariant(Type &aVariant)
    {
        static_assert(is_variant_v<Type>, "Type is not a variant!");

        WriteCount(aVariant.index());
        std::visit([&](auto &&aArg) { Write(aArg); }, aVariant);

        return *this;
    }

    template <typename Type> constexpr decltype(auto) WriteOptional(Type &aOpt)
    {
        static_assert(is_optional_v<Type>, "Type is not an optional!");

        WriteCount(aOpt.has_value());
        if (aOpt.has_value())
        {
            Write(*aOpt);
        }

        return *this;
    }

    template <typename Type> constexpr decltype(auto) Write(Type &aObject)
    {
        if constexpr (is_optional_v<Type>)
        {
            return WriteOptional(aObject);
        }
        else if constexpr (is_variant_v<Type>)
        {
            return WriteVariant(aObject);
        }
        else if constexpr (is_tuple_v<Type>)
        {
            std::apply([&](auto &&...aArgs) { WriteAll(aArgs...); }, aObject);
            return *this;
        }
        else if constexpr (is_pair_v<Type>)
        {
            // we remove the constness of map's pair's key so we can use the already implemented Write branches
            auto &first = const_cast<std::remove_const_t<typename Type::first_type> &>(aObject.first);
            return WriteAll(first, aObject.second);
        }
        else if constexpr (std::ranges::range<Type>)
        {
            return WriteRange(aObject);
        }
        else if constexpr (std::derived_from<Type, IStreamable>)
        {
            return WriteStreamable(aObject);
        }
        else if constexpr (is_any_pointer<Type>)
        {
            return Write(*aObject);
        }
        else if constexpr (is_standard_layout_no_pointer<Type>)
        {
            return WriteObjectOfKnownSize(aObject);
        }
        else
        {
            static_assert(always_false<Type>, "Type is not accepted!");
        }
    }
};

class IStreamable
{
    friend class StreamWriter;
    friend class StreamReader;

    Stream mStream;

  public:
    [[nodiscard]] constexpr decltype(auto) Serialize()
    {
        Swap(Stream());
        ToStream();
        return Release();
    }

    constexpr void Deserialize(Stream &&aStream, const bool aClear = true)
    {
        Swap(std::move(aStream));
        FromStream();

        if (aClear)
        {
            mStream.Clear();
        }
    }

  protected:
    StreamWriter mStreamWriter;
    StreamReader mStreamReader;

    constexpr IStreamable() noexcept : mStreamWriter(mStream), mStreamReader(mStream)
    {
    }

    virtual void ToStream() = 0;
    virtual void FromStream() = 0;

    virtual void ToStreamBases()
    {
    }

    virtual void FromStreamBases()
    {
    }

    [[nodiscard]] constexpr Stream &&Release() noexcept
    {
        return std::move(mStream);
    }

    constexpr Stream &Swap(Stream &&aStream) noexcept
    {
        mStream = std::move(aStream);
        mStreamWriter = StreamWriter(mStream);
        mStreamReader = StreamReader(mStream);

        return mStream;
    }

    constexpr decltype(auto) Reserve(const Size::size_max aSize)
    {
        return mStream.Reserve(aSize);
    }

    constexpr IStreamable(const IStreamable &aIStreamable) noexcept : IStreamable()
    {
        *this = aIStreamable;
    }

    constexpr IStreamable(IStreamable &&aIStreamable) noexcept : IStreamable()
    {
        *this = std::move(aIStreamable);
    }

    constexpr IStreamable &operator=(const IStreamable &) noexcept
    {
        return *this;
    }

    constexpr IStreamable &operator=(IStreamable &&aIStreamable) noexcept
    {
        Swap(std::move(aIStreamable.mStream));
        return *this;
    }
};
} // namespace hbann
