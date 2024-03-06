#include "pch.h"

typedef struct _guid
{
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(_guid, Data1, Data2, Data3, Data4);
    MSGPACK_DEFINE(Data1, Data2, Data3, Data4);

    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} guid;

inline constexpr guid GUID_RND = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

class Shape : public virtual hbann::IStreamable
{
    STREAMABLE_DEFINE(Shape, STREAMABLE_DEFINE_BASE(), mType, mID);

  public:
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Shape, mType, mID);
    MSGPACK_DEFINE(mType, mID);

    enum class Type : uint8_t
    {
        NONE,
        CIRCLE,
        RECTANGLE
    };

    Shape() = default;
    Shape(const Type aType, const guid &aID) : mType(aType), mID(aID)
    {
    }

    virtual ~Shape() = default;

    bool operator==(const Shape &aShape) const
    {
        return mType == aShape.mType && !memcmp(&mID, &aShape.mID, sizeof(mID));
    }

    Type GetType()
    {
        return mType;
    }

  private:
    Type mType = Type::NONE;
    guid mID{};

    static hbann::IStreamable *FindDerivedStreamable(hbann::StreamReader &aStreamReader);
};

MSGPACK_ADD_ENUM(Shape::Type);

class Circle : public Shape
{
    STREAMABLE_DEFINE(Circle, STREAMABLE_DEFINE_BASE(Shape), mSVG, mURL, mVariant);

  public:
    friend void to_json(nlohmann::json &nlohmann_json_j, const Circle &nlohmann_json_t)
    {
        nlohmann::to_json(nlohmann_json_j, static_cast<const Shape &>(nlohmann_json_t));

        nlohmann_json_j["mSVG"] = *nlohmann_json_t.mSVG;
        nlohmann_json_j["mURL"] = nlohmann_json_t.mURL;

        std::visit([&nlohmann_json_j](auto &&arg) { nlohmann_json_j["mVariant"] = arg; }, nlohmann_json_t.mVariant);
    }

    friend void from_json(const nlohmann::json &nlohmann_json_j, Circle &nlohmann_json_t)
    {
        nlohmann::from_json(nlohmann_json_j, static_cast<Shape &>(nlohmann_json_t));

        std::string svg;
        nlohmann_json_j.at("mSVG").get_to(svg);
        nlohmann_json_t.mSVG = svg;

        nlohmann_json_j.at("mURL").get_to(nlohmann_json_t.mURL);

        std::vector<double> doubles;
        nlohmann_json_j.at("mVariant").get_to(doubles);
        nlohmann_json_t.mVariant = doubles;
    }

    template <typename Packer> void msgpack_pack(Packer &msgpack_pk) const
    {
        type::make_define_array(MSGPACK_BASE(Shape), mSVG, mURL.native(), std::get<std::vector<double>>(mVariant))
            .msgpack_pack(msgpack_pk);
    }

    void msgpack_unpack(object const &msgpack_o)
    {
        std::filesystem::path::string_type url;
        std::vector<double> doubles;
        type::make_define_array(MSGPACK_BASE(Shape), mSVG, url, doubles).msgpack_unpack(msgpack_o);
        mURL.assign(url);
        mVariant = doubles;
    }

    template <typename MSGPACK_OBJECT> void msgpack_object(MSGPACK_OBJECT *msgpack_o, zone &msgpack_z) const
    {
        type::make_define_array(MSGPACK_BASE(Shape), mSVG, mURL, std::get<std::vector<double>>(mVariant))
            .msgpack_object(msgpack_o, msgpack_z);
    }

    Circle() = default;
    Circle(const guid &aID, const std::optional<std::string> &aSVG, const std::filesystem::path &aURL,
           std::variant<std::vector<double>, bool> &&aVariant)
        : Shape(Type::CIRCLE, aID), mSVG(aSVG), mURL(aURL), mVariant(aVariant)
    {
    }

    bool operator==(const Circle &aCircle) const
    {
        return *(Shape *)this == *(Shape *)&aCircle && mSVG == aCircle.mSVG && mURL == aCircle.mURL &&
               mVariant == aCircle.mVariant;
    }

  private:
    std::optional<std::string> mSVG{};
    std::filesystem::path mURL{};
    std::variant<std::vector<double>, bool> mVariant{};
};

class Sphere : public Circle
{
    STREAMABLE_DEFINE(Sphere, STREAMABLE_DEFINE_BASE(Circle), mReflexion, mTuple, mPair);

  public:
    friend void to_json(nlohmann::json &nlohmann_json_j, const Sphere &nlohmann_json_t)
    {
        nlohmann::to_json(nlohmann_json_j, static_cast<const Circle &>(nlohmann_json_t));

        nlohmann_json_j["mReflexion"] = *nlohmann_json_t.mReflexion;
        nlohmann_json_j["mTuple"] = nlohmann_json_t.mTuple;
        nlohmann_json_j["mPair"] = nlohmann_json_t.mPair;
    }

    friend void from_json(const nlohmann::json &nlohmann_json_j, Sphere &nlohmann_json_t)
    {
        nlohmann::from_json(nlohmann_json_j, static_cast<Circle &>(nlohmann_json_t));

        nlohmann_json_j.at("mReflexion").get_to(*nlohmann_json_t.mReflexion);
        nlohmann_json_j.at("mTuple").get_to(nlohmann_json_t.mTuple);
        nlohmann_json_j.at("mPair").get_to(nlohmann_json_t.mPair);
    }

    template <typename Packer> void msgpack_pack(Packer &msgpack_pk) const
    {
        type::make_define_array(MSGPACK_BASE(Circle), mReflexion, mTuple, mPair).msgpack_pack(msgpack_pk);
    }

    void msgpack_unpack(object const &msgpack_o)
    {
        type::make_define_array(MSGPACK_BASE(Circle), mReflexion, mTuple, mPair).msgpack_unpack(msgpack_o);
    }

    template <typename MSGPACK_OBJECT> void msgpack_object(MSGPACK_OBJECT *msgpack_o, zone &msgpack_z) const
    {
        type::make_define_array(MSGPACK_BASE(Circle), mReflexion, mTuple, mPair).msgpack_object(msgpack_o, msgpack_z);
    }

    Sphere() : mReflexion(std::make_unique<bool>())
    {
    }

    Sphere(const Circle &aCircle, unique_ptr<bool> &&aReflexion, std::tuple<std::string, std::list<int>> &&aTuple,
           std::pair<Circle, double> &&aPair)
        : Circle(aCircle), mReflexion(std::move(aReflexion)), mTuple(std::move(aTuple)), mPair(std::move(aPair))
    {
    }

    bool operator==(const Sphere &aSphere) const
    {
        return *(Circle *)this == *(Circle *)&aSphere && *mReflexion == *aSphere.mReflexion &&
               mTuple == aSphere.mTuple && mPair == aSphere.mPair;
    }

  private:
    std::unique_ptr<bool> mReflexion{};
    std::tuple<std::string, std::list<int>> mTuple{};
    std::pair<Circle, double> mPair{};
};

class RectangleEx : public Shape
{
    STREAMABLE_DEFINE(ReclangleEx, STREAMABLE_DEFINE_BASE(Shape), mCenter, mMap, mCells);

  public:
    NLOHMANN_DEFINE_DERIVED_TYPE_INTRUSIVE(RectangleEx, Shape, mCenter, mMap, mCells);

    template <typename Packer> void msgpack_pack(Packer &msgpack_pk) const
    {
        type::make_define_array(MSGPACK_BASE(Shape), mCenter, mMap, mCells).msgpack_pack(msgpack_pk);
    }

    void msgpack_unpack(object const &msgpack_o)
    {
        type::make_define_array(MSGPACK_BASE(Shape), mCenter, mMap, mCells).msgpack_unpack(msgpack_o);
    }

    template <typename MSGPACK_OBJECT> void msgpack_object(MSGPACK_OBJECT *msgpack_o, zone &msgpack_z) const
    {
        type::make_define_array(MSGPACK_BASE(Shape), mCenter, mMap, mCells).msgpack_object(msgpack_o, msgpack_z);
    }

    RectangleEx() = default;
    RectangleEx(const guid &aID, Sphere &&aCenter, const std::vector<std::vector<std::wstring>> &aCells)
        : Shape(Type::RECTANGLE, aID), mCenter(std::move(aCenter)), mCells(aCells)
    {
    }

    bool operator==(const RectangleEx &aRectangle) const
    {
        return *(Shape *)this == *(Shape *)&aRectangle && mCenter == aRectangle.mCenter && mMap == aRectangle.mMap &&
               mCells == aRectangle.mCells;
    }

  private:
    Sphere mCenter{};
    std::map<int, double> mMap{};
    std::vector<std::vector<std::wstring>> mCells{};
};

class Diamond : public Circle, public RectangleEx
{
    STREAMABLE_DEFINE(Diamond, STREAMABLE_DEFINE_BASE(Circle, RectangleEx));

  public:
    Diamond() = default;
    Diamond(const guid &aIDC, const std::optional<std::string> &aSVG, const std::filesystem::path &aURL,
            std::variant<std::vector<double>, bool> &&aVariant, const guid &aIDR, Sphere &&aCenter,
            const std::vector<std::vector<std::wstring>> &aCells)
        : Circle(aIDC, aSVG, aURL, move(aVariant)), RectangleEx(aIDR, move(aCenter), aCells)
    {
    }

    bool operator==(const Diamond &aDiamond) const
    {
        return *(Circle *)this == *(Circle *)&aDiamond && *(RectangleEx *)this == *(RectangleEx *)&aDiamond;
    }
};

hbann::IStreamable *Shape::FindDerivedStreamable(hbann::StreamReader &aStreamReader)
{
    Type type{};
    aStreamReader.ReadAll(type);

    switch (type)
    {
    case Shape::Type::CIRCLE:
        return new Circle;
    case Shape::Type::RECTANGLE:
        return new RectangleEx;

    default:
        return nullptr;
    }
}

class Context : public hbann::IStreamable
{
    STREAMABLE_DEFINE(Context, STREAMABLE_DEFINE_BASE(), mShapes);

  public:
    friend void to_json(nlohmann::json &nlohmann_json_j, const Context &nlohmann_json_t)
    {
        json jsonArray = json::array();
        for (auto &shape : *nlohmann_json_t.mShapes)
        {
            json json;
            switch (shape->GetType())
            {
            case Shape::Type::CIRCLE:
                to_json(json, *(Circle *)shape);
                break;

            case Shape::Type::RECTANGLE:
                to_json(json, *(RectangleEx *)shape);
                break;
            }

            jsonArray.push_back(json);
        }

        nlohmann_json_j["mShapes"] = jsonArray;
    }

    friend void from_json(const nlohmann::json &nlohmann_json_j, Context &nlohmann_json_t)
    {
        for (auto &shapeJSON : nlohmann_json_j.at("mShapes"))
        {
            Shape *shapeDerived{};
            switch (shapeJSON.at("mType").get<Shape::Type>())
            {
            case Shape::Type::CIRCLE: {
                shapeDerived = new Circle;
                from_json(shapeJSON, *(Circle *)shapeDerived);

                break;
            }

            case Shape::Type::RECTANGLE: {
                shapeDerived = new RectangleEx;
                from_json(shapeJSON, *(RectangleEx *)shapeDerived);

                break;
            }
            }

            nlohmann_json_t.mShapes->push_back(shapeDerived);
        }
    }

    template <typename Packer> void msgpack_pack(Packer &msgpack_pk) const
    {
        // HARDCODED bcz MSG_PACK is GARBAGE
        type::make_define_array(*(Circle *)mShapes->front(), *(RectangleEx *)mShapes->back()).msgpack_pack(msgpack_pk);
    }

    void msgpack_unpack(object const &msgpack_o)
    {
        // HARDCODED bcz MSG_PACK is GARBAGE
        auto shapeFront = new Circle;
        auto shapeBack = new RectangleEx;
        type::make_define_array(*shapeFront, *shapeBack).msgpack_unpack(msgpack_o);

        mShapes->push_back(shapeFront);
        mShapes->push_back(shapeBack);
    }

    template <typename MSGPACK_OBJECT> void msgpack_object(MSGPACK_OBJECT *msgpack_o, zone &msgpack_z) const
    {
        type::make_define_array(mShapes).msgpack_object(msgpack_o, msgpack_z);
    }

    Context() : mShapes(std::make_shared<std::vector<Shape *>>())
    {
    }

    Context(std::shared_ptr<std::vector<Shape *>> &&aShapes) : mShapes(std::move(aShapes))
    {
    }

    ~Context()
    {
        for (auto &shape : *mShapes)
        {
            delete shape;
        }
    }

    bool operator==(const Context &aContext) const
    {
        if (mShapes->size() != aContext.mShapes->size())
        {
            return false;
        }

        for (size_t i = 0; i < mShapes->size(); i++)
        {
            if ((*mShapes)[i]->GetType() != (*aContext.mShapes)[i]->GetType())
            {
                return false;
            }

            switch ((*mShapes)[i]->GetType())
            {
            case Shape::Type::CIRCLE: {
                if (*(Circle *)(*mShapes)[i] != *(Circle *)(*aContext.mShapes)[i])
                {
                    return false;
                }

                break;
            }

            case Shape::Type::RECTANGLE: {
                if (*(RectangleEx *)(*mShapes)[i] != *(RectangleEx *)(*aContext.mShapes)[i])
                {
                    return false;
                }

                break;
            }
            }
        }

        return true;
    }

  private:
    std::shared_ptr<std::vector<Shape *>> mShapes{};
};

TEST_CASE("Streamable", "[Streamable]")
{
    SECTION("SizeFinder")
    {
        int i{42};
        REQUIRE(hbann::SizeFinder::FindRangeRank<decltype(i)>() == 0);

        std::list<std::pair<int, float>> l{{22, 14.f}, {93, 32.f}};
        REQUIRE(hbann::SizeFinder::FindRangeRank<decltype(l)>() == 1);

        std::vector<double> v{512., 52., 77., 42321.};
        REQUIRE(hbann::SizeFinder::FindRangeRank<decltype(v)>() == 1);

        enum class enumClassTest : uint8_t
        {
            NONE,
            NOTHING,
            NADA
        };

        std::list<std::vector<enumClassTest>> lv{{enumClassTest::NONE, enumClassTest::NOTHING},
                                                 {enumClassTest::NOTHING, enumClassTest::NADA}};
        REQUIRE(hbann::SizeFinder::FindRangeRank<decltype(lv)>() == 2);

        size_t lvSize = 1;
        for (const auto &lvItem : lv)
        {
            lvSize += 1 + lvItem.size() * sizeof(decltype(lv)::value_type::value_type);
        }

        std::vector<std::vector<std::string>> vvs{{"gsbbbawf", "hbann", "1fwah10"}, {"palelica", "t43hachhew"}};
        REQUIRE(hbann::SizeFinder::FindRangeRank<decltype(vvs)>() == 3); // the string is a range itself

        size_t vvsSize = 1;
        for (const auto &vsItem : vvs)
        {
            vvsSize += 1;
            for (const auto &sItem : vsItem)
            {
                vvsSize += 1 + sItem.size() * sizeof(std::remove_cvref_t<decltype(sItem)>::value_type);
            }
        }
    }

    SECTION("Stream")
    {
        hbann::Stream stream;

        std::string biceps("biceps");
        stream.Write({reinterpret_cast<uint8_t *>(biceps.data()), biceps.size()});
        const auto bicepsView = stream.Read(biceps.size());
        REQUIRE(!std::memcmp(biceps.c_str(), bicepsView.data(), bicepsView.size()));

        REQUIRE(!stream.Read(1).size());

        std::string triceps("triceps");
        stream.Write({reinterpret_cast<uint8_t *>(triceps.data()), triceps.size()});
        const auto tricepsView = stream.Read(triceps.size());
        REQUIRE(!std::memcmp(triceps.c_str(), tricepsView.data(), tricepsView.size()));

        REQUIRE(!stream.Read(1).size());

        std::string cariceps("cariceps");
        stream.Write({reinterpret_cast<uint8_t *>(cariceps.data()), cariceps.size()});
        const auto caricepsView = stream.Read(cariceps.size());
        REQUIRE(!std::memcmp(cariceps.c_str(), caricepsView.data(), caricepsView.size()));
    }

    SECTION("StreamWriter")
    {
        hbann::Stream stream;
        hbann::StreamWriter streamWriter(stream);

        double d = 12.34;
        std::string s("cariceps");
        streamWriter.WriteAll(d, s);

        const auto dView = stream.Read(sizeof(d)).data();
        REQUIRE(d == *reinterpret_cast<const decltype(d) *>(dView));

        const auto requiredBytes = hbann::Size::FindRequiredBytes(stream.Current());
        const auto sSize = hbann::Size::MakeSize(stream.Read(requiredBytes));
        REQUIRE(s.size() == sSize);

        const auto sView = stream.Read(sSize);
        REQUIRE(!std::memcmp(s.c_str(), sView.data(), sView.size()));
    }

    SECTION("StreamReader")
    {
        hbann::Stream stream;
        hbann::StreamWriter streamWriter(stream);
        hbann::StreamReader streamReader(stream);

        double d = 12.34;
        std::string s("cariceps");
        streamWriter.WriteAll(d, s);

        double dd{};
        std::string ss{};
        streamReader.ReadAll(dd, ss);

        REQUIRE(d == dd);
        REQUIRE(s == ss);
    }
}

TEST_CASE("IStreamable", "[IStreamable]")
{
    SECTION("Simple")
    {
        Shape shapeStart(Shape::Type::RECTANGLE, GUID_RND);
        Shape shapeEnd{};
        shapeEnd.Deserialize(shapeStart.Serialize());

        REQUIRE(shapeStart == shapeEnd);
    }

    SECTION("Derived")
    {
        Shape *circleStart = new Circle(GUID_RND, std::nullopt, L"URL\\SHIT", true);
        Shape *circleEnd = new Circle();
        circleEnd->Deserialize(circleStart->Serialize());

        REQUIRE(*(Circle *)circleStart == *(Circle *)circleEnd);

        delete circleStart;
        delete circleEnd;
    }

    SECTION("DerivedxN")
    {
        Circle circle(GUID_RND, {}, L"URL\\SHIT", false);
        std::vector<std::vector<std::wstring>> cells{{L"smth", L"else"}, {L"HBann", L"Sefu la bani"}};

        Sphere sphere(circle, std::make_unique<bool>(true), {"Commit: added tuple support", {22, 100}}, {circle, 22.});

        Diamond diamondStart(GUID_RND, {}, L"URL\\SHIT", false, GUID_RND, std::move(sphere), cells);

        Diamond diamondEnd;
        diamondEnd.Deserialize(diamondStart.Serialize());

        REQUIRE(diamondStart == diamondEnd);
    }

    SECTION("Derived++")
    {
        Circle circle(GUID_RND, "SVG", L"URL\\SHIT", std::vector{69., 420.});
        Sphere sphereStart(circle, std::make_unique<bool>(true), {"Commit: added tuple support", {22, 100}},
                           {circle, 22.});
        Sphere sphereEnd;
        sphereEnd.Deserialize(sphereStart.Serialize());

        REQUIRE(sphereStart == sphereEnd);
    }

    SECTION("BaseClass*")
    {
        Circle circle(GUID_RND, {}, L"URL\\SHIT", false);
        std::vector<std::vector<std::wstring>> cells{{L"smth", L"else"}, {L"HBann", L"Sefu la bani"}};

        Sphere sphere(circle, std::make_unique<bool>(true), {"Commit: added tuple support", {22, 100}}, {circle, 22.});

        auto shapes = std::make_shared<std::vector<Shape *>>();
        shapes->push_back(new Circle(GUID_RND, "Circle1_SVG", "Circle1_URL", true));
        shapes->push_back(new RectangleEx(GUID_RND, std::move(sphere), cells));
        shapes->push_back(new Circle(GUID_RND, "Circle2_SVG", "Circle2_URL", std::vector{420., 69.}));

        ::Context contextStart(std::move(shapes));

        ::Context contextEnd;
        contextEnd.Deserialize(contextStart.Serialize());

        REQUIRE(contextStart == contextEnd);
    }
}

TEST_CASE("Benchmarks", "[Benchmarks]")
{
    std::string circleSVGStr{};
    for (size_t i = 0; i < 999; i++)
    {
        circleSVGStr += "SVGSVGSVG";
    }
    std::optional<std::string> circleSVG{circleSVGStr};

    std::filesystem::path circleURL{};
    for (size_t i = 0; i < 666; i++)
    {
        circleURL += L"URL\\SHIT\\";
    }

    std::vector<std::vector<std::wstring>> cells{};
    for (size_t i = 0; i < 333; i++)
    {
        cells.push_back({L"Streamable e sefu la bani!", L"penguinz0 - Goofy evening"});
        cells.push_back({L"Parazitii - Standarde (nr.42)", L"Parazitii - Asa cum vreau (nr.92)"});
    }

    std::variant<std::vector<double>, bool> circleVariant1{std::vector{69., 420.}};
    std::variant<std::vector<double>, bool> circleVariant2{std::vector{69., 420.}};
    std::variant<std::vector<double>, bool> circleVariant3{std::vector{69., 420.}};

    Sphere center({GUID_RND, circleSVG, circleURL, std::move(circleVariant2)}, std::make_unique<bool>(true),
                  {"Commit: added tuple support", {22, 100}},
                  {{GUID_RND, circleSVG, circleURL, std::move(circleVariant3)}, 69.420});
    auto shapes = std::make_shared<std::vector<Shape *>>();
    shapes->push_back(new Circle(GUID_RND, circleSVG, circleURL, std::move(circleVariant1)));
    shapes->push_back(new RectangleEx(GUID_RND, std::move(center), cells));
    ::Context contextStart(std::move(shapes));

    BENCHMARK("Streamable")
    {
        ::Context contextEnd;
        contextEnd.Deserialize(contextStart.Serialize());

        REQUIRE(contextStart == contextEnd);
    };

    const auto view = contextStart.Serialize().View();
    std::cout << std::endl << std::format("Streamable stream had {} bytes.", view.size()) << std::endl;

    BENCHMARK("JSON")
    {
        json json;
        to_json(json, contextStart);

        ::Context contextEnd;
        from_json(json, contextEnd);

        REQUIRE(contextStart == contextEnd);
    };

    json json;
    to_json(json, contextStart);
    std::cout << std::endl << std::format("JSON stream had {} bytes.", to_string(json).size()) << std::endl;

    BENCHMARK("MsgPack")
    {
        sbuffer stream;
        pack(stream, contextStart);

        auto contextObject(unpack(stream.data(), stream.size()));
        ::Context contextEnd(contextObject->as<::Context>());

        REQUIRE(contextStart == contextEnd);
    };

    sbuffer stream;
    pack(stream, contextStart);
    std::cout << std::endl << std::format("MsgPack stream had {} bytes.", stream.size()) << std::endl;
}

int main(int argc, char **argv)
{
    int returnCode{};
    {
        Session session;

#if _DEBUG
        ConfigData configData{.showSuccessfulTests = false,
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
