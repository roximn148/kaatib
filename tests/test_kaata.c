/*******************************************************************************
* Copyright (c) 2024. All rights reserved.
*
* This work is licensed under the Creative Commons Attribution 4.0 
* International License. To view a copy of this license,
* visit # http://creativecommons.org/licenses/by/4.0/.
*
* Author: roximn <roximn148@gmail.com>
*******************************************************************************/
#include <stdio.h>

#include <core/core.h>
#include <core/strings.h>
#include <core/heap.h>
#include <osbs/osbs.h>
#include <osbs/bfile.h>
#include <core/hfile.h>
#include <sewer/bmath.h>

#include <raqm.h>

#include "unity.h"
#include "utx.h"

/*----------------------------------------------------------------------------*/
void setUp(void) {
    heap_stats(TRUE);
}

/*----------------------------------------------------------------------------*/
void tearDown(void) {
    TEST_ASSERT_FALSE(heap_leaks());
}

/*----------------------------------------------------------------------------*/
void test_RaqmLink(void) {
    unsigned int major, minor, patch;
    raqm_version(&major, &minor, &patch);
    fprintf(stderr, "Raqm version: %d.%d.%d\n", major, minor, patch);
}

/*----------------------------------------------------------------------------*/
void test_InvertBool(void) {
    bool_t b = TRUE;

    TEST_ASSERT_TRUE(b);
    TEST_ASSERT_FALSE(!b);

    b = FALSE;
    TEST_ASSERT_FALSE(b);
    TEST_ASSERT_TRUE(!b);
}

/*----------------------------------------------------------------------------*/
void test_StringCapacity(void) {
    String *str = str_reserve(10);
    char_t *s = tcc(str);
    s[0] = '\0';

    TEST_ASSERT_EQUAL_UINT32(10, str_len(str));
    TEST_ASSERT_EQUAL_UINT32(0, str_len_c(tc(str)));
}

/*----------------------------------------------------------------------------*/
void test_WorkingDirectory_Suffix(void) {
    platform_t platform = osbs_platform();

    const uint32_t PATH_SIZE = 512;
    String *path = str_reserve(PATH_SIZE);

    uint32_t sz = bfile_dir_work(tcc(path), PATH_SIZE);
    TEST_ASSERT_LESS_THAN_UINT32(PATH_SIZE, sz);

    if (platform == ekWINDOWS) {
        TEST_ASSERT_FALSE(str_is_sufix(tc(path), "\\"));
    } else {
        TEST_ASSERT_FALSE(str_is_sufix(tc(path), "/"));
    }

    str_destroy(&path);
}

/*----------------------------------------------------------------------------*/
void test_StringTrim(void) {
    const char_t suffix[] = "\\";

    const uint32_t PATH_SIZE = 512;
    String *path = str_reserve(PATH_SIZE);

    uint32_t sz = bfile_dir_tmp(tcc(path), PATH_SIZE);
    TEST_ASSERT_LESS_THAN_UINT32(PATH_SIZE, sz);

    TEST_ASSERT_TRUE(str_is_sufix(tc(path), suffix));

    const uint32_t szO = str_len_c(tc(path));

    String *trimmedPath = str_trim_n(tc(path), szO - 1);
    const uint32_t szT = str_len(trimmedPath);

    TEST_ASSERT_EQUAL_INT32(szO - 1, szT);

    fprintf(stderr, "Original [%d]: %s\n", szO, tc(path));
    fprintf(stderr, "Trimmed [%d]: %s\n", szT, tc(trimmedPath));

    str_destroy(&path);
    str_destroy(&trimmedPath);
}



/*----------------------------------------------------------------------------*/
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_InvertBool);
    RUN_TEST(test_StringTrim);
    RUN_TEST(test_StringCapacity);
    RUN_TEST(test_WorkingDirectory_Suffix);

    RUN_TEST(test_RaqmLink);
    return UNITY_END();
}

/*----------------------------------------------------------------------------*/
