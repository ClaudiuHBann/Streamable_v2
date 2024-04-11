
# Streamable

Fastest, Smallest and Simplest (De)Serializer for C++20 or newer.

## Table of Contents

- [Installation](#installation)
- [Features](#features)
- [Usage](#usage)
- [Benchmark](#benchmark)
- [Downsides and Limitations](#downsides-and-limitations)
- [Examples](#examples)

## Installation

To use this library, simply download and include the header file `Streamable.hpp` into your project or use NuGet to install it.

## Features

- **fast** - fast^2, memory-- and easy++ compared to [MsgPack](https://msgpack.org/)
- **easy-to-use** - inherit a class and use a macro :D
- **single-header** - just copy paste the file into your project
- **has no dependencies** - uses the `C++20` standard library and OS native API for best performance
- **cross-platform** -

| Platform      | Support      | Details                        |
|---------------|--------------|--------------------------------|
| Windows       | Yes          |                                |
| macOS         | Partial      | No UTF16 encoding for memory-- |
| Linux         | Partial      | No UTF16 encoding for memory-- |

- **supports every data type** - beside **itself** (so called "streamables"), **raw/smart pointers** (ex:. `std::unique_ptr`, `std::shared_ptr` etc...), **most STL classes** (`std::tuple`, `std::optional`, `std::variant` etc...), **any nested range** (ex.: `std::wstring`, `std::map`, std::vector&lt;std::list&gt; etc...), **PODs** (ex.: POD structs and classes, enums, etc...), **primitive types** (ex.: `bool`, `unsigned int`, `double` etc...)

## Usage

1. Inherit from the `IStreamable` class or any class that implements it.
2. Use the macro **STREAMABLE_DEFINE** and pass your class and the objects you want to parse
3. **OPTIONAL** For (de)serialization of inherited classes use **STREAMABLE_DEFINE_BASE** and pass the classes
4. **OPTIONAL** If "streamables" pointers are (de)serialized you MUST implement **FindDerivedStreamable** (if you forget this, don't worry, a `static_assert` will scream :O )

## Benchmark

This benchmark contains anything from trivial types to maps of strings to STL containers to vectors of derived classes as base class pointers etc...

| Library           | Time (ms) | Memory Usage (KB) |
|-------------------|-----------|-------------------|
| Streamable        | 0.379     | 85                |
| nlohmann::json    | 2.9       | 187               |
| MsgPack           | 0.755     | 87                |

## Downsides and Limitations

While **Streamable** offers many benefits, it's essential to consider its limitations as well:
-  **Custom Format** - it has a custom but simple format for storing the data
- **Programming Language** - supports only C++ >= 20

## Examples

- [Examples](https://github.com/ClaudiuHBann/Streamable_v2/blob/master/Test/Main.cpp) - Mixed
