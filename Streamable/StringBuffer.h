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

    inline StringBuffer(const State aState) : mState(aState)
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

  protected:
    State mState;

    inline std::stringbuf *setbuf(char_type *aData, std::streamsize aSize) noexcept override
    {
        if (aSize <= 0)
        {
            return this;
        }

        if (aData)
        {
            auto dataStart = aData;
            auto dataEnd = aData + aSize;

            setg(dataStart, dataStart, dataEnd);
            setp(dataStart, Can(State::WRITE) ? dataStart : dataEnd, dataEnd);
        }
        else
        {
            Clear();
            setbuf(new char_type[aSize], aSize);
        }

        return this;
    }

  private:
    inline void Clear() noexcept
    {
        auto streamO(pbase()), streamI(eback());
        if (!streamO && !streamI)
        {
            return;
        }

        // at least one pointer exists so delete at lease one
        // if the pointers were not the same delete the other one
        delete streamI;
        if (streamI != streamO)
        {
            delete streamO;
        }

        // reset internal pointers
        setg(nullptr, nullptr, nullptr);
        setp(nullptr, nullptr, nullptr);
    }
};
} // namespace hbann