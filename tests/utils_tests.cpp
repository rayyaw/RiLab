#include "catch.hpp"

#include <string>

#include "../src/data/utils.h"

using std::string;

TEST_CASE("isFloat valid", "[isFloat]") {
    REQUIRE(isFloat("10.0"));
}