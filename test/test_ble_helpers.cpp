#include "unity.h"
#include "BLEHelpers.h"

void setUp(void)
{
    // set stuff up here
}

void test_(void)
{
    char *baseUuid = "00000000-0000-1000-8000-00805F9B34FB";
    int uuidAlias = 0x1234;
    std::string expectedUuid = "00001234-0000-1000-8000-00805F9B34FB";
    auto uuidBytes = BLEHelpers::mapFromAlias(uuidAlias, baseUuid);
    std::string actualUuid = BLEHelpers::toUUIDString(uuidBytes);

    TEST_ASSERT_EQUAL_STRING(expectedUuid.data(), actualUuid.data());
}

void test_function_should_doAlsoDoBlah(void)
{
    // more test stuff
}

void tearDown(void)
{
    // clean stuff up here
}

int runUnityTests(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_);
    return UNITY_END();
}

int main(void)
{
    return runUnityTests();
}