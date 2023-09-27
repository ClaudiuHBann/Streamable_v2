#pragma once

#include "SizeFinder.h"
#include "Stream.h"

namespace hbann
{
class IStreamable;

class StreamWriter
{
  public:
    constexpr StreamWriter(Stream &aStream) noexcept : mStream(&aStream)
    {
    }

    constexpr StreamWriter(StreamWriter &&aStreamWriter) noexcept
    {
        *this = std::move(aStreamWriter);
    }

    template <typename Type, typename... Types> constexpr void WriteAll(const Type &aObject, const Types &...aObjects)
    {
        using TypeRaw = get_raw_t<Type>;

        Write<TypeRaw>(aObject);

        if constexpr (sizeof...(aObjects))
        {
            WriteAll(aObjects...);
        }
    }

    constexpr StreamWriter &operator=(StreamWriter &&aStreamWriter) noexcept
    {
        mStream = aStreamWriter.mStream;

        return *this;
    }

  private:
    Stream *mStream{};

    template <typename Type> inline void WriteObjectOfKnownSize(const Type &aObject)
    {
        static_assert(std::is_standard_layout_v<Type> && !std::is_pointer_v<Type>,
                      "Type is not an object of known size or it is a pointer!");

        const auto objectPtr = reinterpret_cast<const char *>(&aObject);
        mStream->Write(objectPtr, sizeof(aObject));
    }

    inline void WriteCount(const size_t aSize)
    {
        WriteObjectOfKnownSize((size_range)aSize);
    }

    template <typename Type> void WriteStreamable(Type &aStreamable)
    {
        static_assert(std::is_base_of_v<IStreamable, std::remove_pointer_t<Type>>,
                      "Type is not a streamable (pointer)!");

        size_t size{};
        std::string_view streamView{};
        if constexpr (std::is_pointer_v<Type>)
        {
            size = aStreamable->FindParseSize();
            streamView = aStreamable->ToStream().GetBuffer().view();
        }
        else
        {
            size = aStreamable.FindParseSize();
            streamView = aStreamable.ToStream().GetBuffer().view();
        }

        WriteCount(size); // we write the size in bytes of the stream
        mStream->Write(streamView.data(), streamView.size());
    }

    template <typename Type> constexpr void WriteRange(const Type &aRange)
    {
        static_assert(std::ranges::range<Type>, "Type is not a range!");

        using TypeValueType = typename Type::value_type;

        WriteCount(std::ranges::size(aRange));

        if constexpr (SizeFinder::FindRangeRank<Type>() > 1)
        {
            for (const auto &object : aRange)
            {
                WriteRange(object);
            }
        }
        else
        {
            if constexpr (std::ranges::contiguous_range<Type> && std::is_standard_layout_v<TypeValueType> &&
                          !std::is_pointer_v<TypeValueType>)
            {
                const auto rangePtr = reinterpret_cast<const char *>(std::ranges::data(aRange));
                const auto rangeSize = std::ranges::size(aRange) * sizeof(TypeValueType);
                mStream->Write(rangePtr, rangeSize);
            }
            else
            {
                for (const auto &object : aRange)
                {
                    Write(object);
                }
            }
        }
    }

    template <typename Type> constexpr void Write(const Type &aObject)
    {
        if constexpr (std::is_standard_layout_v<Type> && !std::is_pointer_v<Type>)
        {
            WriteObjectOfKnownSize(aObject);
        }
        else if constexpr (std::is_base_of_v<IStreamable, std::remove_pointer_t<Type>>)
        {
            WriteStreamable(aObject);
        }
        else if constexpr (std::ranges::range<Type>)
        {
            WriteRange(aObject);
        }
        else
        {
            static_assert(always_false<Type>, "Type is not accepted!");
        }
    }
};
} // namespace hbann
