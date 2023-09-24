#include "pch.h"

TEST_CASE("Streamable", "[Streamable]")
{
    SECTION("SizeFinder")
    {
        int i{42};
        REQUIRE(SizeFinder::FindRangeRank<decltype(i)>() == 0);
        REQUIRE(SizeFinder::FindParseSize(i) == (size_range)sizeof(i));

        std::list<pair<int, float>> l{{22, 14.f}, {93, 32.f}};
        REQUIRE(SizeFinder::FindRangeRank<decltype(l)>() == 1);
        REQUIRE(SizeFinder::FindParseSize(l) ==
                size_range(sizeof(size_range) + l.size() * sizeof(decltype(l)::value_type)));

        vector<double> v{512., 52., 77., 42321.};
        REQUIRE(SizeFinder::FindRangeRank<decltype(v)>() == 1);
        REQUIRE(SizeFinder::FindParseSize(v) ==
                size_range(sizeof(size_range) + v.size() * sizeof(decltype(v)::value_type)));

        enum class enumClassTest : uint8_t
        {
            NONE,
            NOTHING,
            NADA
        };

        std::list<vector<enumClassTest>> lv{{enumClassTest::NONE, enumClassTest::NOTHING},
                                            {enumClassTest::NOTHING, enumClassTest::NADA}};
        REQUIRE(SizeFinder::FindRangeRank<decltype(lv)>() == 2);

        size_t lvSize = sizeof(size_range);
        for (const auto &lvItem : lv)
        {
            lvSize += sizeof(size_range) + lvItem.size() * sizeof(decltype(lv)::value_type::value_type);
        }
        REQUIRE(SizeFinder::FindParseSize(lv) == (size_range)lvSize);

        vector<vector<vector<string>>> vvv{{{"000", "001"}, {"010", "011"}}, {{"100", "101"}, {"110", "111"}}};
        REQUIRE(SizeFinder::FindRangeRank<decltype(vvv)>() == 4); // the string is a range itself

        size_t vvvSize = sizeof(size_range);
        for (const auto &vvvItem : vvv)
        {
            if (vvvItem.size())
            {
                vvvSize += sizeof(size_range);
            }

            for (const auto &vvItem : vvvItem)
            {
                if (vvItem.size())
                {
                    vvvSize += sizeof(size_range);
                }

                for (const auto &vItem : vvItem)
                {
                    vvvSize += sizeof(size_range) + vItem.size() * sizeof(get_raw_t<decltype(vItem)>::value_type);
                }
            }
        }
        REQUIRE(SizeFinder::FindParseSize(vvv) == (size_range)vvvSize);

        // TODO: add IStreamable SizeFinder test
    }

    SECTION("Stream")
    {
        Stream stream;
        stream.Reserve(21);

        string biceps("biceps");
        stream.Write(biceps.c_str(), (size_range)biceps.size()).Flush();
        const auto bicepsView = stream.Read((size_range)biceps.size());
        REQUIRE(biceps.compare(bicepsView) == 0);

        REQUIRE(!stream.Read(1).size());

        string triceps("triceps");
        stream.Write(triceps.c_str(), (size_range)triceps.size()).Flush();
        const auto tricepsView = stream.Read((size_range)triceps.size());
        REQUIRE(triceps.compare(tricepsView) == 0);

        REQUIRE(!stream.Read(1).size());

        string cariceps("cariceps");
        stream.Write(cariceps.c_str(), (size_range)cariceps.size()).Flush();
        const auto caricepsView = stream.Read((size_range)cariceps.size());
        REQUIRE(cariceps.compare(caricepsView) == 0);
    }

    SECTION("StreamWriter")
    {
        Stream stream;
        StreamWriter streamWriter(stream);

        double d = 12.34;
        string s("cariceps");
        streamWriter.WriteAll(d, s);

        const auto dView = stream.Read(sizeof(d)).data();
        REQUIRE(d == *reinterpret_cast<const decltype(d) *>(dView));

        const auto sSizeView = stream.Read(sizeof(size_range)).data();
        const auto sSize = *reinterpret_cast<const size_range *>(sSizeView);
        REQUIRE(s.size() == sSize);
        const auto sView = stream.Read(sSize);
        REQUIRE(s.compare(sView) == 0);

        // TODO: add IStreamable StreamWriter test
    }

    SECTION("StreamReader")
    {
        Stream stream;
        StreamWriter streamWriter(stream);
        StreamReader streamReader(stream);

        double d = 12.34;
        string s("cariceps");
        streamWriter.WriteAll(d, s);

        double dd{};
        string ss{};
        streamReader.ReadAll(dd, ss);

        REQUIRE(d == dd);
        REQUIRE(s == ss);

        // TODO: add IStreamable StreamReader test
    }
}

TEST_CASE("IStreamable", "[IStreamable]")
{
}

int main(int argc, char **argv)
{
    int returnCode{};
    {
        Session session;

#if _DEBUG
        ConfigData configData{.showSuccessfulTests = true,
                              .shouldDebugBreak = true,
                              .noThrow = true,
                              .showInvisibles = true,
                              .filenamesAsTags = true,

                              .verbosity = Verbosity::High,
                              .warnings =
                                  WarnAbout::What(WarnAbout::What::NoAssertions | WarnAbout::What::UnmatchedTestSpec),
                              .showDurations = ShowDurations::Always,
                              .defaultColourMode = ColourMode::PlatformDefault};

        session.useConfigData(configData);
#endif // _DEBUG

        returnCode = session.applyCommandLine(argc, argv);
        if (!returnCode)
        {
            returnCode = session.run();
        }
    }

    return returnCode;
}
