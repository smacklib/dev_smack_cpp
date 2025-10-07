/* Smack C++ @ https://github.com/smacklib/dev_smack_cpp
 *
 * Tests.
 *
 * Copyright © 2021 Michael Binz
 */

#include <gtest/gtest.h>

#include <string>

#include <smack_util.hpp>
#include "test_common.hpp"

using std::string;
using std::string_view;
using std::vector;

TEST(SmackStrings, StringSplit) {
    string splitMe = "abc:def";

    auto v = smack::util::strings::split(splitMe, ":");

    EXPECT_EQ(2, v.size());
    EXPECT_EQ("abc", v[0]);
    EXPECT_EQ("def", v[1]);
}

TEST(SmackStrings, StringSplit2) {
    string splitMe = "milchmolch";

    auto v = smack::util::strings::split(splitMe, "lc");

    EXPECT_EQ(3, v.size());
    EXPECT_EQ("mi", v[0]);
    EXPECT_EQ("hmo", v[1]);
    EXPECT_EQ("h", v[2]);
}

TEST(SmackStrings, StringSplitEmpty) {
    string splitMe = ":";

    auto v = smack::util::strings::split(splitMe, ":");

    EXPECT_EQ(2, v.size());
    EXPECT_TRUE(v[0].empty());
    EXPECT_TRUE(v[1].empty());
}

TEST(SmackStrings, StringSplitEmptySlots) {
    string splitMe = ":a::b:";

    auto v = smack::util::strings::split(splitMe, ":");

    EXPECT_EQ(5, v.size());
    EXPECT_TRUE(v[0].empty());
    EXPECT_EQ("a", v[1]);
    EXPECT_TRUE(v[2].empty());
    EXPECT_EQ("b", v[3]);
    EXPECT_TRUE(v[4].empty());
}

TEST(SmackStrings, StringSplitNone) {
    string splitMe;

    auto v = smack::util::strings::split(splitMe, ":");

    EXPECT_EQ(0, v.size());
}

TEST(SmackStrings, StringSplitNoDelimiter) {
    string splitMe{ "313" };

    auto v = smack::util::strings::split(splitMe, "");

    EXPECT_EQ(1, v.size());
    EXPECT_EQ("313", v[0]);
}

TEST(SmackStrings, StringTrimNone) {
    string trimMe{ "q" };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("q", r);
}

TEST(SmackStrings, StringViewTrimNone) {
    string_view trimMe{ "q" };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("q", r);
}

TEST(SmackStrings, StringTrimAll) {
    string trimMe{ "\n\t " };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("", r);
}

TEST(SmackStrings, StringViewTrimAll) {
    string_view trimMe{ "\n\t " };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("", r);
}

TEST(SmackStrings, StringTrimFront) {
    string trimMe{ "\t\t313" };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("313", r);
}

TEST(SmackStrings, StringViewTrimFront) {
    string_view trimMe{ "\t\t313" };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("313", r);
}

TEST(SmackStrings, StringTrimBack) {
    string trimMe{ "313\t\t" };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("313", r);
}

TEST(SmackStrings, StringViewTrimBack) {
    string_view trimMe{ "313\t\t" };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("313", r);
}

TEST(SmackStrings, StringTrimBoth) {
    string trimMe{ "\t\n313\t\r" };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("313", r);
}

TEST(SmackStrings, StringViewTrimBoth) {
    string_view trimMe{ "\t\n313\t\r" };

    auto r = smack::util::strings::trim(trimMe);
    EXPECT_EQ("313", r);
}

TEST(SmackStrings, StringTrimCharsSingle) {
    string trimMe{ "313" };

    auto r = smack::util::strings::trim(trimMe, "3");
    EXPECT_EQ("1", r);
}

TEST(SmackStrings, StringViewTrimCharsSingle) {
    string_view trimMe{ "313" };

    auto r = smack::util::strings::trim(trimMe, "3");
    EXPECT_EQ("1", r);
}

TEST(SmackStrings, StringTrimCharsMultiple) {
    string trimMe{ "121micbinz313" };

    auto r = smack::util::strings::trim(trimMe, "123456789");
    EXPECT_EQ("micbinz", r);
}

TEST(SmackStrings, StringViewTrimCharsMultiple) {
    string_view trimMe{ "121micbinz313" };

    auto r = smack::util::strings::trim(trimMe, "123456789");
    EXPECT_EQ("micbinz", r);
}

TEST(SmackStrings, StringTrimCharsAll) {
    string trimMe{ "121658765576313" };

    auto r = smack::util::strings::trim(trimMe, "123456789");
    EXPECT_EQ("", r);
}

TEST(SmackStrings, StringViewTrimCharsAll) {
    string_view trimMe{ "121658765576313" };

    auto r = smack::util::strings::trim(trimMe, "123456789");
    EXPECT_EQ("", r);
}

TEST(SmackStrings, StringConcat1) {
    vector<string> sv = { "a" };

    string r = smack::util::strings::concat(sv, "-");

    EXPECT_EQ("a", r);
}

TEST(SmackStrings, StringConcat2) {
    vector<string> sv = { "a", "b" };

    string r = smack::util::strings::concat(sv, "-");

    EXPECT_EQ("a-b", r);
}

TEST(SmackStrings, StringConcat3) {
    vector<string> sv = { "a", "b", "c" };

    string r = smack::util::strings::concat(sv, "++");

    EXPECT_EQ("a++b++c", r);
}

TEST(SmackStrings, StringConcatEmptyDelimiter) {
    vector<string> sv = { "a", "b", "c" };

    string r = smack::util::strings::concat(sv, "");

    EXPECT_EQ("abc", r);
}

TEST(SmackStrings, StringSplitConcat3) {
    vector<string> sv = { "a", "b", "c" };

    string r = smack::util::strings::concat(sv, "++");

    EXPECT_EQ("a++b++c", r);

    auto v = smack::util::strings::split(r, "++");
    EXPECT_EQ(3, v.size());
    EXPECT_EQ("a", v[0]);
    EXPECT_EQ("b", v[1]);
    EXPECT_EQ("c", v[2]);
}

TEST(SmackStrings, StringStartsWith) {
    using smack::util::strings::starts_with;
    string s = "prefixAndMore";

    EXPECT_TRUE( starts_with( s, "prefix" ) );
    EXPECT_FALSE( starts_with(s, "And") );
    EXPECT_TRUE( starts_with( s, "" ) );
    EXPECT_TRUE( starts_with( "", "" ) );
    EXPECT_FALSE( starts_with( "", "x" ) );
    EXPECT_FALSE( starts_with( s, "x" ) );
}

TEST(SmackStrings, StringEndsWith) {
    using smack::util::strings::ends_with;

    string s = "prefixAndMore";

    EXPECT_TRUE( ends_with( s, "More" ) );
    EXPECT_FALSE(ends_with(s, "And"));
    EXPECT_TRUE( ends_with( s, "" ) );
    EXPECT_TRUE( ends_with( "", "" ) );
    EXPECT_FALSE( ends_with( "", "x" ) );
    EXPECT_FALSE( ends_with( s, "x" ) );
}

TEST( SmackUtil, sort_int )
{
    std::vector<int> ints = {313, -1 , 121};

    smack::sort( ints );

    ASSERT_EQ( 3, ints.size() );
    ASSERT_EQ( -1, ints[0] );
    ASSERT_EQ( 121, ints[1] );
    ASSERT_EQ( 313, ints[2] );
}

TEST(SmackUtil, OutputRedirection) {

    smack::test::common::redir out{ std::cout };

    string test{ "redirected" };

    std::cout << test;

    EXPECT_EQ(test, out.str());
}

TEST(SmackUtil, OutputRedirectionMultiline) {

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

TEST(SmackUtil, Disposer) {
    {
        Disposer handle{ argOpen(), argClose };
        EXPECT_EQ(7, handle);
        EXPECT_EQ(0, closedCount);
        EXPECT_EQ(313, argDo(handle));
    }
    EXPECT_EQ(1, closedCount);
}

namespace {

std::string tracerName{ "KONG" };

void handleNonConst1(smack::test::common::Tracer t)
{
    EXPECT_EQ(1, t.copyCount());
    EXPECT_EQ(tracerName, t.name());
}
void handleNonConst2(smack::test::common::Tracer& t)
{
    EXPECT_EQ(0, t.copyCount());
    EXPECT_EQ(tracerName, t.name());
}

void handleConst(const smack::test::common::Tracer& t)
{
    EXPECT_EQ(0, t.copyCount());
    EXPECT_EQ(tracerName, t.name());
}

}

TEST(SmackUtil, CopyTracer) {

    smack::test::common::redir out{ std::cout };

    smack::test::common::Tracer tracer(tracerName);
    handleNonConst1(tracer);
    handleNonConst2(tracer);
    handleConst(tracer);

    auto lines = out.strs();

    EXPECT_EQ(2, lines.size());
    EXPECT_EQ("KONG(1) copy", lines[0]);
    EXPECT_TRUE( lines[1].empty() );
}
