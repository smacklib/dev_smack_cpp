#include <gtest/gtest.h> // googletest header file

#include <string>

#include <smack_util.hpp>
#include "test_common.hpp"

using std::string;
using std::vector;

TEST(SmackUtilTest, StringSplit) {
    string splitMe = "abc:def";

    auto v = smack::util::strings::split(splitMe, ":");

    EXPECT_EQ(2, v.size());
    EXPECT_EQ("abc", v[0]);
    EXPECT_EQ("def", v[1]);
}

TEST(SmackUtilTest, StringSplit2) {
    string splitMe = "milchmolch";

    auto v = smack::util::strings::split(splitMe, "lc");

    EXPECT_EQ(3, v.size());
    EXPECT_EQ("mi", v[0]);
    EXPECT_EQ("hmo", v[1]);
    EXPECT_EQ("h", v[2]);
}

TEST(SmackUtilTest, StringSplitEmpty) {
    string splitMe = ":";

    auto v = smack::util::strings::split(splitMe, ":");

    EXPECT_EQ(2, v.size());
    EXPECT_TRUE(v[0].empty());
    EXPECT_TRUE(v[1].empty());
}

TEST(SmackUtilTest, StringSplitEmptySlots) {
    string splitMe = ":a::b:";

    auto v = smack::util::strings::split(splitMe, ":");

    EXPECT_EQ(5, v.size());
    EXPECT_TRUE(v[0].empty());
    EXPECT_EQ("a", v[1]);
    EXPECT_TRUE(v[2].empty());
    EXPECT_EQ("b", v[3]);
    EXPECT_TRUE(v[4].empty());
}

TEST(SmackUtilTest, StringSplitNone) {
    string splitMe;

    auto v = smack::util::strings::split(splitMe, ":");

    EXPECT_EQ(0, v.size());
}

TEST(SmackUtilTest, StringSplitNoDelimiter) {
    string splitMe{ "313" };

    auto v = smack::util::strings::split(splitMe, "");

    EXPECT_EQ(1, v.size());
    EXPECT_EQ("313", v[0]);
}

TEST(SmackUtilTest, StringTrimNone) {
    string trimMe{ "q" };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("q", r);
}

TEST(SmackUtilTest, StringTrimAll) {
    string trimMe{ "\n\t " };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("", r);
}

TEST(SmackUtilTest, StringTrimFront) {
    string trimMe{ "\t\t313" };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("313", r);
}

TEST(SmackUtilTest, StringTrimBack) {
    string trimMe{ "313\t\t" };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("313", r);
}

TEST(SmackUtilTest, StringTrimBoth) {
    string trimMe{ "\t\n313\t\r" };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("313", r);
}

TEST(SmackUtilTest, StringTrimCharsSingle) {
    string trimMe{ "313" };

    auto r = smack::util::strings::trim(trimMe, "3");
    EXPECT_EQ("1", r);
}

TEST(SmackUtilTest, StringTrimCharsMultiple) {
    string trimMe{ "121micbinz313" };

    auto r = smack::util::strings::trim(trimMe, "123456789");
    EXPECT_EQ("micbinz", r);
}

TEST(SmackUtilTest, StringTrimCharsAll) {
    string trimMe{ "121658765576313" };

    auto r = smack::util::strings::trim(trimMe, "123456789");
    EXPECT_EQ("", r);
}

TEST(SmackUtilTest, StringConcat1) {
    vector<string> sv = { "a" };

    string r = smack::util::strings::concat(sv, "-");

    EXPECT_EQ("a", r);
}

TEST(SmackUtilTest, StringConcat2) {
    vector<string> sv = { "a", "b" };

    string r = smack::util::strings::concat(sv, "-");

    EXPECT_EQ("a-b", r);
}

TEST(SmackUtilTest, StringConcat3) {
    vector<string> sv = { "a", "b", "c" };

    string r = smack::util::strings::concat(sv, "++");

    EXPECT_EQ("a++b++c", r);
}

TEST(SmackUtilTest, StringConcatEmptyDelimiter) {
    vector<string> sv = { "a", "b", "c" };

    string r = smack::util::strings::concat(sv, "");

    EXPECT_EQ("abc", r);
}

TEST(SmackUtilTest, StringSplitConcat3) {
    vector<string> sv = { "a", "b", "c" };

    string r = smack::util::strings::concat(sv, "++");

    EXPECT_EQ("a++b++c", r);

    auto v = smack::util::strings::split(r, "++");
    EXPECT_EQ(3, v.size());
    EXPECT_EQ("a", v[0]);
    EXPECT_EQ("b", v[1]);
    EXPECT_EQ("c", v[2]);
}

TEST(SmackUtilTest, StringStartsWith) {
    using smack::util::strings::starts_with;
    string s = "prefixAndMore";

    EXPECT_TRUE( starts_with( s, "prefix" ) );
    EXPECT_FALSE( starts_with(s, "And") );
    EXPECT_TRUE( starts_with( s, "" ) );
    EXPECT_TRUE( starts_with( "", "" ) );
    EXPECT_FALSE( starts_with( "", "x" ) );
    EXPECT_FALSE( starts_with( s, "x" ) );
}

TEST(SmackUtilTest, StringEndsWith) {
    using smack::util::strings::ends_with;

    string s = "prefixAndMore";

    EXPECT_TRUE( ends_with( s, "More" ) );
    EXPECT_FALSE(ends_with(s, "And"));
    EXPECT_TRUE( ends_with( s, "" ) );
    EXPECT_TRUE( ends_with( "", "" ) );
    EXPECT_FALSE( ends_with( "", "x" ) );
    EXPECT_FALSE( ends_with( s, "x" ) );
}

TEST(SmackUtilTest, OutputRedirection) {

    smack::test::common::redir out{ std::cout };

    string test{ "redirected" };

    std::cout << test;

    EXPECT_EQ(test, out.str());
}

TEST(SmackUtilTest, OutputRedirectionMultiline) {

    smack::test::common::redir out{ std::cout };

    string eins{ "une" };
    string zwei{ "deux" };
    string drei{ "trois" };
    string vier{ "quatre" };

    std::cout << eins << std::endl;
    std::cout << zwei << std::endl;
    std::cout << drei << std::endl;
    std::cout << vier << std::endl;

    auto lines = out.strs();

    EXPECT_EQ(5, lines.size());
    EXPECT_EQ(eins, lines[0]);
    EXPECT_EQ(zwei, lines[1]);
    EXPECT_EQ(drei, lines[2]);
    EXPECT_EQ(vier, lines[3]);
    EXPECT_EQ("", lines[4]);
}

namespace {
    using std::cout;
    using std::endl;
    using smack::util::Disposer;

    int closedCount = 0;

    int argOpen() {
        auto seven = 7;
        return seven;
    }

    int argDo(int what) {
        return 313;
    }

    void argClose(int what) {
        closedCount++;
    }
}

TEST(SmackUtilTest, Disposer) {
    {
        Disposer handle{ argOpen(), argClose };
        EXPECT_EQ(7, handle);
        EXPECT_EQ(0, closedCount);
        EXPECT_EQ(313, argDo(handle));
    }
    EXPECT_EQ(1, closedCount);
}

namespace {

void handleNonConst1(smack::test::common::Tracer t)
{
    EXPECT_EQ(1, t.copyCount());
}
void handleNonConst2(smack::test::common::Tracer& t)
{
    EXPECT_EQ(0, t.copyCount());
}

void handleConst(const smack::test::common::Tracer& t)
{
    EXPECT_EQ(0, t.copyCount() );
}

}

TEST(SmackUtilTest, traced) {
    smack::test::common::Tracer tracer("traced");
    handleNonConst1(tracer);
    handleNonConst2(tracer);
    handleConst(tracer);
}
