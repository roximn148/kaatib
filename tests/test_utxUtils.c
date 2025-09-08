/*******************************************************************************
* Copyright (c) 2025. All rights reserved.
*
* This work is licensed under the Creative Commons Attribution 4.0
* International License. To view a copy of this license,
* visit # http://creativecommons.org/licenses/by/4.0/.
*
* Author: roximn <roximn148@gmail.com>
*******************************************************************************/
#include <stdio.h>

#include <core/strings.h>
#include <core/heap.h>

#include <unity.h>
#include "utils.h"

/*----------------------------------------------------------------------------*/
void setUp(void) {
    heap_verbose(TRUE);
    heap_stats(TRUE);
}

/*----------------------------------------------------------------------------*/
void tearDown(void) {
    TEST_ASSERT_FALSE(heap_leaks());
}

/*----------------------------------------------------------------------------*/
void test_utils_convert0(void) {
    String *s = number2IndicNumeral(0);
    char_t ref[] = "\xdb\xb0";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert1(void) {
    String *s = number2IndicNumeral(1);
    char_t ref[] = "\xdb\xb1";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert2(void) {
    String *s = number2IndicNumeral(2);
    char_t ref[] = "\xdb\xb2";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert3(void) {
    String *s = number2IndicNumeral(3);
    char_t ref[] = "\xdb\xb3";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert4(void) {
    String *s = number2IndicNumeral(4);
    char_t ref[] = "\xdb\xb4";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert5(void) {
    String *s = number2IndicNumeral(5);
    char_t ref[] = "\xdb\xb5";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert6(void) {
    String *s = number2IndicNumeral(6);
    char_t ref[] = "\xdb\xb6";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert7(void) {
    String *s = number2IndicNumeral(7);
    char_t ref[] = "\xdb\xb7";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert8(void) {
    String *s = number2IndicNumeral(8);
    char_t ref[] = "\xdb\xb8";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert9(void) {
    String *s = number2IndicNumeral(9);
    char_t ref[] = "\xdb\xb9";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert10(void) {
    String *s = number2IndicNumeral(10);
    char_t ref[] = "\xdb\xb1\xdb\xb0";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert99(void) {
    String *s = number2IndicNumeral(99);
    char_t ref[] = "\xdb\xb9\xdb\xb9";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert100(void) {
    String *s = number2IndicNumeral(100);
    char_t ref[] = "\xdb\xb1\xdb\xb0\xdb\xb0";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert999(void) {
    String *s = number2IndicNumeral(999);
    char_t ref[] = "\xdb\xb9\xdb\xb9\xdb\xb9";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert1000(void) {
    String *s = number2IndicNumeral(1000);
    char_t ref[] = "\xdb\xb1\xdb\xb0\xdb\xb0\xdb\xb0";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert9999(void) {
    String *s = number2IndicNumeral(9999);
    char_t ref[] = "\xdb\xb9\xdb\xb9\xdb\xb9\xdb\xb9";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert10000(void) {
    String *s = number2IndicNumeral(10000);
    char_t ref[] = "\xdb\xb1\xdb\xb0\xdb\xb0\xdb\xb0\xdb\xb0";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert99999(void) {
    String *s = number2IndicNumeral(99999);
    char_t ref[] = "\xdb\xb9\xdb\xb9\xdb\xb9\xdb\xb9\xdb\xb9";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert100000(void) {
    String *s = number2IndicNumeral(100000);
    char_t ref[] = "\xdb\xb1\xdb\xb0\xdb\xb0\xdb\xb0\xdb\xb0\xdb\xb0";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
void test_utils_convert999999(void) {
    String *s = number2IndicNumeral(999999);
    char_t ref[] = "\xdb\xb9\xdb\xb9\xdb\xb9\xdb\xb9\xdb\xb9\xdb\xb9";
    TEST_ASSERT_EQUAL_UINT32(0, str_cmp(s, ref));
    str_destroy(&s);
}

/*----------------------------------------------------------------------------*/
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_utils_convert0);
    RUN_TEST(test_utils_convert1);
    RUN_TEST(test_utils_convert2);
    RUN_TEST(test_utils_convert3);
    RUN_TEST(test_utils_convert4);
    RUN_TEST(test_utils_convert5);
    RUN_TEST(test_utils_convert6);
    RUN_TEST(test_utils_convert7);
    RUN_TEST(test_utils_convert8);
    RUN_TEST(test_utils_convert9);

    RUN_TEST(test_utils_convert10);
    RUN_TEST(test_utils_convert99);

    RUN_TEST(test_utils_convert100);
    RUN_TEST(test_utils_convert999);

    RUN_TEST(test_utils_convert1000);
    RUN_TEST(test_utils_convert9999);

    RUN_TEST(test_utils_convert10000);
    RUN_TEST(test_utils_convert99999);

    RUN_TEST(test_utils_convert100000);
    RUN_TEST(test_utils_convert999999);

    return UNITY_END();
}

/*----------------------------------------------------------------------------*/
