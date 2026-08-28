#include "notify_change/notify_change.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

namespace {

class MyClass {
public:
    void value( double v ) { mValue = v; }
    double value() const { return mValue; }
    size_t changed() const { return mChanged; }

private:
    void onChanged()
    {
        ++mChanged;
    }
    size_t mChanged{};
    phreak::notify_change<double> mValue{*this,&MyClass::onChanged};
};


TEST_CASE("Test callable", "[notify_change]")
{
    phreak::callable<int> c;

    REQUIRE_THROWS(c(1));

    c = []{};
    REQUIRE_NOTHROW(c(1));

    phreak::callable<int> c2{c};
    REQUIRE_NOTHROW(c(1));
    REQUIRE_NOTHROW(c2(1));
    c2=c;
    REQUIRE_NOTHROW(c(1));
    REQUIRE_NOTHROW(c2(1));
    c2=std::move(c);
    //REQUIRE(!c);
    //REQUIRE_THROWS(c(1));
    REQUIRE_NOTHROW(c2(1));
}

TEST_CASE("Test notify_change", "[notify_change]")
{
    size_t c{};

    phreak::notify_change v{[&c](){ ++c; }, 1.2 };
    REQUIRE(c == 0);
    v = 3.1;
    REQUIRE(v == 3.1);
    REQUIRE(c == 1);
    v = 3.1;
    REQUIRE(v == 3.1);
    REQUIRE(c == 1);
    v = 3.11;
    REQUIRE(v == 3.11);
    REQUIRE(c == 2);
}

TEST_CASE("Test notify_always", "[notify_change]")
{
    size_t c{};

    phreak::notify_always<double> v{[&c](){ ++c; }, 1.2 };
    REQUIRE(c == 0);
    v = 3.1;
    REQUIRE(v == 3.1);
    REQUIRE(c == 1);
    v = 3.1;
    REQUIRE(v == 3.1);
    REQUIRE(c == 2);
    v = 3.11;
    REQUIRE(v == 3.11);
    REQUIRE(c == 3);
}

TEST_CASE("Test MyClass", "[notify_change]")
{
    MyClass c{};

    REQUIRE(c.changed() == 0);
    c.value(3.1);
    REQUIRE(c.value() == 3.1);
    REQUIRE(c.changed() == 1);
    c.value(3.11);
    REQUIRE(c.value() == 3.11);
    REQUIRE(c.changed() == 2);
}

TEST_CASE("Test string", "[notify_change]")
{
    size_t c{};

    phreak::notify_change<std::string> v{[&c](){ ++c; }, "hello" };
    REQUIRE(c == 0);
    REQUIRE(v == "hello");

    v = "world";
    REQUIRE(v == "world");
    REQUIRE(c == 1);
    v = "3.11";
    REQUIRE(v == "3.11");
    REQUIRE(c == 2);
}


} // ::
