/*******************************************************************************
* Copyright (c) 2024. All rights reserved.
*
* This work is licensed under the Creative Commons Attribution 4.0 
* International License. To view a copy of this license,
* visit # http://creativecommons.org/licenses/by/4.0/.
*
* Author: roximn <roximn148@gmail.com>
*******************************************************************************/
#include "unity.h"
#include "utx.h"

/*----------------------------------------------------------------------------*/
void setUp(void) {
    // set stuff up here
}

/*----------------------------------------------------------------------------*/
void tearDown(void) {
    // clean stuff up here
}

/*----------------------------------------------------------------------------*/
void test_utxCreate_shouldRet1(void) {
    TEST_ASSERT_EQUAL(utxCreate(NULL), 1);
}

/*----------------------------------------------------------------------------*/
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_utxCreate_shouldRet1);
    return UNITY_END();
}

/*----------------------------------------------------------------------------*/
