#pragma once

#include "../pch.h" // Streamable pch

#include "Streamable/IStreamable.h"
#include "Streamable/SizeFinder.h"
#include "Streamable/Stream.h"
#include "Streamable/StreamReader.h"
#include "Streamable/StreamWriter.h"

// Shared
#define CATCH_AMALGAMATED_CUSTOM_MAIN

#include "Shared/Catch2-3.4.0/catch_amalgamated.hpp"

using namespace Catch;

// nlohmann JSON
#include <Shared/json-3.11.2/json.hpp>

using namespace nlohmann;

// msgpack
#define MSGPACK_NO_BOOST

#include <Shared/msgpack-6.1.0/msgpack.hpp>

using namespace msgpack;

// Test
#include <fstream>

using namespace std::string_literals;
using namespace std::string_view_literals;
