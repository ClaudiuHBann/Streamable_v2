#pragma once

#include "IStreamable.h"
#include "SizeFinder.h"
#include "Stream.h"

namespace hbann
{
class StreamWriter
{
  public:
    constexpr StreamWriter(Stream &aStream) noexcept : mStream(aStream)
    {
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

  private:
    Stream &mStream;

    template <typename Type> inline void WriteObjectOfKnownSize(const Type &aObject)
    {
        static_assert(std::is_standard_layout_v<Type> && !std::is_pointer_v<Type>,
                      "Type is not an object of known size or it is a pointer!");

        const auto objectPtr = reinterpret_cast<const char *>(&aObject);
        mStream.Write(objectPtr, sizeof(aObject));
    }

    inline void WriteStreamable(IStreamable &aStreamable)
    {
        WriteObjectOfKnownSize(aStreamable.FindParseSize());

        const auto stream = aStreamable.ToStream();
        mStream.Write(stream.data(), (size_range)stream.size());
    }

    template <typename Type> constexpr void WriteRange(const Type &aRange)
    {
        static_assert(std::ranges::range<Type>, "Type is not a range!");

        WriteObjectOfKnownSize((size_range)std::ranges::size(aRange));

        if constexpr (SizeFinder::FindRangeRank<Type>() > 1)
        {
            for (const auto &aObject : aRange)
            {
                WriteRange(aObject);
            }
        }
        else
        {
            for (const auto &aObject : aRange)
            {
                Write(aObject);
            }
        }
    }

    template <typename Type> constexpr void Write(const Type &aObject)
    {
        if constexpr (std::is_standard_layout_v<Type> && !std::is_pointer_v<Type>)
        {
            WriteObjectOfKnownSize(aObject);
        }
        else if constexpr (std::is_base_of_v<IStreamable, Type>)
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