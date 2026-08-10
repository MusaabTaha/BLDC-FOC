#include <catch2/catch_test_macros.hpp>

extern "C"
{
#include "foc_math.h"
}

TEST_CASE("Clarke transform")
{
    Clark_t x = clark_trans(1.0f, -0.5f);

    REQUIRE(x.Ialpha == Catch::Approx(1.0f).margin(0.0001f));
    REQUIRE(x.Ibeta  == Catch::Approx(0.0f).margin(0.0001f));
}

TEST_CASE("Park transform at zero electrical angle")
{
    Clark_t input = {1.0f, 0.5f};

    Park_t x = Park_trans(input, 0.0f, 1.0f);

    REQUIRE(x.Id == Catch::Approx(1.0f).margin(0.0001f));
    REQUIRE(x.Iq == Catch::Approx(0.5f).margin(0.0001f));
}

TEST_CASE("Inverse Park at zero electrical angle")
{
    InvPark_t x = invPark(1.0f, 0.5f, 0.0f, 1.0f);

    REQUIRE(x.Valpha == Catch::Approx(1.0f).margin(0.0001f));
    REQUIRE(x.Vbeta  == Catch::Approx(0.5f).margin(0.0001f));
}

TEST_CASE("SVPWM zero vector gives equal half-period on-times")
{
    Svpwm_t x = svpwmcalc(0.0f, 0.0f, 400, 12.0f);

    REQUIRE(x.tona == Catch::Approx(200.0f).margin(0.0001f));
    REQUIRE(x.tonb == Catch::Approx(200.0f).margin(0.0001f));
    REQUIRE(x.tonc == Catch::Approx(200.0f).margin(0.0001f));
}

TEST_CASE("FOC math blocks preserve vector at zero angle")
{
    Clark_t clarke = clark_trans(1.0f, -0.5f);
    Park_t park = Park_trans(clarke, 0.0f, 1.0f);
    InvPark_t inverse = invPark(park.Id, park.Iq, 0.0f, 1.0f);

    REQUIRE(inverse.Valpha == Catch::Approx(clarke.Ialpha).margin(0.0001f));
    REQUIRE(inverse.Vbeta  == Catch::Approx(clarke.Ibeta).margin(0.0001f));
}
