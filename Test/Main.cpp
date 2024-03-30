#include "pch.h"

inline constexpr auto LICENSE = R"(/*
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
*/)";

inline constexpr auto FILES = {"Converter.h", "Size.h", "Stream.h", "SizeFinder.h", "StreamReader.h", "StreamWriter.h"};

inline constexpr auto PATH_INPUT = R"(C:\Users\Claudiu HBann\Desktop\Projects\C++\Streamable\Streamable\)";
inline constexpr auto PATH_OUTPUT = R"(C:\Users\Claudiu HBann\Desktop\Projects\C++\Streamable\nuget\Streamable.hpp)";

inline constexpr const char NS_START[] = "namespace hbann\n{";
inline constexpr const char NS_END[] = "} // namespace hbann";
inline constexpr const char PRAGMA_ONCE[] = "#pragma once";
inline constexpr const char INCLUDE_STREAMABLE_FWD[] = R"(#include "StreamableFWD.h")";

static inline auto ReadAllText(const std::filesystem::path &aPath)
{
    return (std::ostringstream() << std::ifstream(aPath).rdbuf()).str();
}

static auto ReadFWD()
{
    auto text = ReadAllText(PATH_INPUT + R"(..\StreamableFWD.h)"s);
    text = text.substr(text.find(PRAGMA_ONCE + "\n"s) + sizeof(PRAGMA_ONCE) + 1);
    return text;
}

static auto ReadPCH()
{
    auto text = ReadAllText(PATH_INPUT + R"(..\pch.h)"s);
    text = text.substr(text.find(PRAGMA_ONCE));
    text = text.substr(0, text.rfind(NS_END));

    auto start = text.begin() + text.find(INCLUDE_STREAMABLE_FWD);
    text = text.replace(start, start + sizeof(INCLUDE_STREAMABLE_FWD), ReadFWD());

    return text;
}

static auto ReadIStreamable()
{
    auto text = ReadAllText(PATH_INPUT + R"(IStreamable.h)"s);
    text = text.substr(text.find(NS_START) + sizeof(NS_START));
    return text;
}

int main()
{
    std::string streamable;
    streamable += LICENSE + "\n\n"s + ReadPCH() + '\n';

    for (const std::string file : FILES)
    {
        auto text = ReadAllText(PATH_INPUT + file);
        text = text.substr(text.find(NS_START) + sizeof(NS_START));
        text = text.substr(0, text.rfind(NS_END));

        streamable += text + '\n';
    }

    streamable += ReadIStreamable();

    std::ofstream(PATH_OUTPUT) << streamable;
    return 0;
}
