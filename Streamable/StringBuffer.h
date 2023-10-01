#pragma once

namespace hbann
{
class StringBuffer : public std::stringbuf
{
  public:
    enum class State : uint8_t
    {
        NONE = 0b00,
        WRITE = 0b01,
        READ = 0b10,
        BOTH = 0b11
    };

    inline StringBuffer(const State aState = State::NONE) : mState(aState)
    {
    }

    constexpr bool Can(const State aState) const noexcept
    {
        if (mState == aState)
        {
            return true;
        }

        return (uint8_t)mState & (uint8_t)aState;
    }

    inline void Reset() noexcept
    {
        setg(nullptr, nullptr, nullptr);
        setp(nullptr, nullptr);
    }

    inline StringBuffer &operator=(StringBuffer &&aStringBuffer) noexcept
    {
        swap(aStringBuffer);
        if (!mMovedInPlace)
        {
            aStringBuffer.Reset();
        }

        return *this;
    }

    inline StringBuffer &swap(StringBuffer &aStringBuffer) noexcept
    {
        if (this == std::addressof(aStringBuffer))
        {
            mMovedInPlace = true;
            return *this;
        }

        // we have just a stream for read and write so we set the pointers for both with the write ptrs
        const auto pbase = aStringBuffer.pbase();
        const auto size = static_cast<std::streamsize>(aStringBuffer.epptr() - pbase);
        setbuf(pbase, size);

        return *this;
    }

    ~StringBuffer() noexcept override
    {
        // when we allocate memory with setbuf we don't change the Allocate flag from the stringbuf
        // and there is not method for this so we need to clear the memory ourselves
        if (!mMovedInPlace)
        {
            Clear();
        }
    }

  protected:
    State mState = State::NONE;
    bool mMovedInPlace{}; // "moved" object in same object

    inline std::stringbuf *setbuf(char_type *aData, std::streamsize aSize) noexcept override
    {
        if (aSize <= 0)
        {
            return this;
        }

        if (aData)
        {
            const auto dataStart = aData;
            const auto dataEnd = aData + aSize;
            const auto dataNext = Can(State::WRITE) ? dataStart : dataEnd;

            setg(dataStart, dataStart, dataEnd);

            setp(dataStart, dataEnd);
            // sets the internal seekhigh ptr so that view() works properly
            pbump(static_cast<int>(dataNext - dataStart));
        }
        else
        {
            Clear();
            setbuf(new char_type[(size_t)aSize], aSize);
        }

        return this;
    }

  private:
    inline void Clear() noexcept
    {
        auto streamO(pbase());
        auto streamI(eback());
        if (!streamO && !streamI)
        {
            return;
        }

        // at least one pointer exists so delete at lease one
        // if the pointers were not the same delete the other one
        if (streamI == streamO)
        {
            delete streamI;
        }
        else if (streamI != streamO)
        {
            delete streamI;
            delete streamO;
        }

        // reset internal pointers
        Reset();
    }
};
} // namespace hbann

namespace std
{
[[nodiscard]] inline string to_string(const ::hbann::StringBuffer::State aState)
{
    switch (aState)
    {
    case ::hbann::StringBuffer::State::NONE:
        return "NONE";
    case ::hbann::StringBuffer::State::WRITE:
        return "WRITE";
    case ::hbann::StringBuffer::State::READ:
        return "READ";
    case ::hbann::StringBuffer::State::BOTH:
        return "BOTH";

    default:
        return "";
    }
}
} // namespace std
