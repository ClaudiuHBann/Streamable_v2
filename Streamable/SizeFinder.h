#pragma once

#include "Stream.h"

namespace hbann
{
class IStreamable;

class SizeFinder
{
  public:
    template <typename Type, typename... Types>
    [[nodiscard]] static constexpr size_t FindParseSize(Type &aObject, Types &...aObjects) noexcept
    {
        return FindObjectSize<get_raw_t<Type>>(aObject) + FindParseSize(aObjects...);
    }

    template <typename Type> [[nodiscard]] static constexpr size_t FindRangeRank() noexcept
    {
        using TypeRaw = get_raw_t<Type>;

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
    [[nodiscard]] static constexpr size_t GetRangeCount(const Range &aRange) noexcept
    {
        if constexpr (has_method_size<Range>)
        {
            return std::ranges::size(aRange);
        }
        else if constexpr (is_path<Range>)
        {
            return aRange.native().size();
        }
        else
        {
            static_assert(always_false<Range>, "Tried to get the range count from an unknown object!");
        }
    }

  private:
    template <typename Type> [[nodiscard]] static constexpr size_t FindObjectSize(Type &aObject) noexcept
    {
        if constexpr (is_std_lay_no_ptr<Type>)
        {
            return sizeof(Type);
        }
        else if constexpr (is_base_of_no_ptr<IStreamable, Type>)
        {
            if constexpr (is_pointer<Type>)
            {
                return static_cast<IStreamable *>(aObject)->FindParseSize();
            }
            else
            {
                return static_cast<IStreamable *>(&aObject)->FindParseSize();
            }
        }
        else if constexpr (std::ranges::range<Type>)
        {
            return FindRangeSize(aObject);
        }
        else
        {
            static_assert(always_false<Type>, "Type is not accepted!");
        }
    }

    template <typename Type> [[nodiscard]] static constexpr size_t FindRangeSize(Type &aRange) noexcept
    {
        using TypeValueType = typename Type::value_type;

        size_t size{};
        if constexpr (FindRangeRank<Type>() > 1)
        {
            size += sizeof(size_range);
            for (auto &object : aRange)
            {
                size += FindRangeSize(object);
            }
        }
        else
        {
            size += sizeof(size_range);
            if constexpr (is_range_std_lay<Type>)
            {
                size += GetRangeCount(aRange) * sizeof(TypeValueType);
            }
            else
            {
                for (auto &object : aRange)
                {
                    size += FindObjectSize(object);
                }
            }
        }

        return size;
    }

    static constexpr size_t FindParseSize() noexcept
    {
        return 0;
    }
};
} // namespace hbann
