#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-can");
    BOOST_CHECK(GetBoolArg("-can"));
    BOOST_CHECK(GetBoolArg("-can", false));
    BOOST_CHECK(GetBoolArg("-can", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-cano"));
    BOOST_CHECK(!GetBoolArg("-cano", false));
    BOOST_CHECK(GetBoolArg("-cano", true));

    ResetArgs("-can=0");
    BOOST_CHECK(!GetBoolArg("-can"));
    BOOST_CHECK(!GetBoolArg("-can", false));
    BOOST_CHECK(!GetBoolArg("-can", true));

    ResetArgs("-can=1");
    BOOST_CHECK(GetBoolArg("-can"));
    BOOST_CHECK(GetBoolArg("-can", false));
    BOOST_CHECK(GetBoolArg("-can", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nocan");
    BOOST_CHECK(!GetBoolArg("-can"));
    BOOST_CHECK(!GetBoolArg("-can", false));
    BOOST_CHECK(!GetBoolArg("-can", true));

    ResetArgs("-nocan=1");
    BOOST_CHECK(!GetBoolArg("-can"));
    BOOST_CHECK(!GetBoolArg("-can", false));
    BOOST_CHECK(!GetBoolArg("-can", true));

    ResetArgs("-can -nocan");  // -can should win
    BOOST_CHECK(GetBoolArg("-can"));
    BOOST_CHECK(GetBoolArg("-can", false));
    BOOST_CHECK(GetBoolArg("-can", true));

    ResetArgs("-can=1 -nocan=1");  // -can should win
    BOOST_CHECK(GetBoolArg("-can"));
    BOOST_CHECK(GetBoolArg("-can", false));
    BOOST_CHECK(GetBoolArg("-can", true));

    ResetArgs("-can=0 -nocan=0");  // -can should win
    BOOST_CHECK(!GetBoolArg("-can"));
    BOOST_CHECK(!GetBoolArg("-can", false));
    BOOST_CHECK(!GetBoolArg("-can", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--can=1");
    BOOST_CHECK(GetBoolArg("-can"));
    BOOST_CHECK(GetBoolArg("-can", false));
    BOOST_CHECK(GetBoolArg("-can", true));

    ResetArgs("--nocan=1");
    BOOST_CHECK(!GetBoolArg("-can"));
    BOOST_CHECK(!GetBoolArg("-can", false));
    BOOST_CHECK(!GetBoolArg("-can", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-can", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-can", "eleven"), "eleven");

    ResetArgs("-can -bar");
    BOOST_CHECK_EQUAL(GetArg("-can", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-can", "eleven"), "");

    ResetArgs("-can=");
    BOOST_CHECK_EQUAL(GetArg("-can", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-can", "eleven"), "");

    ResetArgs("-can=11");
    BOOST_CHECK_EQUAL(GetArg("-can", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-can", "eleven"), "11");

    ResetArgs("-can=eleven");
    BOOST_CHECK_EQUAL(GetArg("-can", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-can", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-can", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-can", 0), 0);

    ResetArgs("-can -bar");
    BOOST_CHECK_EQUAL(GetArg("-can", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-can=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-can", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-can=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-can", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--can");
    BOOST_CHECK_EQUAL(GetBoolArg("-can"), true);

    ResetArgs("--can=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-can", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nocan");
    BOOST_CHECK(!GetBoolArg("-can"));
    BOOST_CHECK(!GetBoolArg("-can", true));
    BOOST_CHECK(!GetBoolArg("-can", false));

    ResetArgs("-nocan=1");
    BOOST_CHECK(!GetBoolArg("-can"));
    BOOST_CHECK(!GetBoolArg("-can", true));
    BOOST_CHECK(!GetBoolArg("-can", false));

    ResetArgs("-nocan=0");
    BOOST_CHECK(GetBoolArg("-can"));
    BOOST_CHECK(GetBoolArg("-can", true));
    BOOST_CHECK(GetBoolArg("-can", false));

    ResetArgs("-can --nocan");
    BOOST_CHECK(GetBoolArg("-can"));

    ResetArgs("-nocan -can"); // can always wins:
    BOOST_CHECK(GetBoolArg("-can"));
}

BOOST_AUTO_TEST_SUITE_END()
