#include "catch.hpp"

#include <pthread.h>
#include <string>

#include "../src/utils.h"

using std::string;

TEST_CASE("isFloat valid", "[isFloat]") {
    REQUIRE(isFloat("10.0"));
}

// Producer Consumer Queue Tests