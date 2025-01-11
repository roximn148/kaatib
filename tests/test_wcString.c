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

#include <unity.h>
#include <sewer/blib.h>
#include <sewer/unicode.h>
#include <core/heap.h>

#include <string16.h>

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
void test_wcSringNew_Null(void) {
    wcString *wcs = wcStringNew(NULL);
    TEST_ASSERT_NULL(wcs);
}

/*----------------------------------------------------------------------------*/
void test_wcSringNew_Empty(void) {
    wcString *wcs = wcStringNew("");
    TEST_ASSERT_NOT_NULL(wcs);
    TEST_ASSERT_EQUAL_UINT32(0, wcl(wcs));

    const char16_t* s = wcc(wcs);
    TEST_ASSERT_EQUAL_UINT16(0, s[0]);
    wcStringDestroy(&wcs);
}


/*----------------------------------------------------------------------------*/
void test_wcSringDestroy_Empty(void) {
    wcString *wcs = wcStringNew("");
    TEST_ASSERT_NOT_NULL(wcs);

    wcStringDestroy(&wcs);
    TEST_ASSERT_NULL(wcs);
}

/*----------------------------------------------------------------------------*/
void test_wcSringNew_HelloWorld(void) {
    char_t *helloWorld = "Hello, World!";
    uint32_t charCount = blib_strlen(helloWorld);
    TEST_ASSERT_EQUAL_UINT32(13, charCount);

    wcString *wcs = wcStringNew(helloWorld);

    TEST_ASSERT_NOT_NULL(wcs);
    TEST_ASSERT_EQUAL_UINT32(charCount, wcl(wcs));

    const char16_t* s = wcc(wcs);
    TEST_ASSERT_NOT_NULL(s);

    for(uint32_t i = 0; i < charCount; ++i) {
        TEST_ASSERT_EQUAL_UINT16(helloWorld[i], s[i]);
    }

    wcStringDestroy(&wcs);
}

/*----------------------------------------------------------------------------*/
void test_wcSringNew_Abjad(void) {
    char_t *abjad = "\xd8\xa7\xd8\xa8\xd9\xbe";
    uint32_t charCount = blib_strlen(abjad);
    uint32_t uinCharCount = unicode_nchars(abjad, ekUTF8);
    TEST_ASSERT_EQUAL_UINT32(6, charCount);
    TEST_ASSERT_EQUAL_UINT32(3, uinCharCount);

    wcString *wcs = wcStringNew(abjad);

    TEST_ASSERT_NOT_NULL(wcs);
    TEST_ASSERT_EQUAL_UINT32(uinCharCount, wcl(wcs));

    const char16_t* s = wcc(wcs);
    TEST_ASSERT_NOT_NULL(s);

    TEST_ASSERT_EQUAL_UINT16(0x0627, s[0]);
    TEST_ASSERT_EQUAL_UINT16(0x0628, s[1]);
    TEST_ASSERT_EQUAL_UINT16(0x067E, s[2]);

    wcStringDestroy(&wcs);
}

/*----------------------------------------------------------------------------*/
void test_wcSringNew_ULimit(void) {
    char_t *ulimit = "\xef\xbf\xbf";
    uint32_t charCount = blib_strlen(ulimit);
    uint32_t uniCharCount = unicode_nchars(ulimit, ekUTF8);
    TEST_ASSERT_EQUAL_UINT32(3, charCount);
    TEST_ASSERT_EQUAL_UINT32(1, uniCharCount);

    wcString *wcs = wcStringNew(ulimit);

    TEST_ASSERT_NOT_NULL(wcs);
    TEST_ASSERT_EQUAL_UINT32(uniCharCount, wcl(wcs));

    const char16_t* s = wcc(wcs);
    TEST_ASSERT_NOT_NULL(s);

    TEST_ASSERT_EQUAL_UINT16(0xFFFF, s[0]);

    wcStringDestroy(&wcs);
}

/*----------------------------------------------------------------------------*/
void test_wcSringNew_OutsideLimit(void) {
    /* Single character out of bounds */
    char_t *olimit1 = "\xf0\x90\x80\x80";
    /* Out of bounds char embedded in acceptable string */
    char_t *olimit2 = "\xd8\xa7\xd8\xa8\xd9\xbe\xf0\x90\x8d\x88\xd8\xa7\xd8\xa8\xd9\xbe";

    uint32_t charCount, uniCharCount;
    wcString *wcs;

    charCount = blib_strlen(olimit1);
    uniCharCount = unicode_nchars(olimit1, ekUTF8);
    TEST_ASSERT_EQUAL_UINT32(4, charCount);
    TEST_ASSERT_EQUAL_UINT32(1, uniCharCount);

    wcs = wcStringNew(olimit1);
    TEST_ASSERT_NULL(wcs);

    charCount = blib_strlen(olimit2);
    uniCharCount = unicode_nchars(olimit2, ekUTF8);
    TEST_ASSERT_EQUAL_UINT32(16, charCount);
    TEST_ASSERT_EQUAL_UINT32(7, uniCharCount);

    wcs = wcStringNew(olimit2);
    TEST_ASSERT_NULL(wcs);
}

/*----------------------------------------------------------------------------*/
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_wcSringNew_Null);
    RUN_TEST(test_wcSringNew_Empty);
    RUN_TEST(test_wcSringDestroy_Empty);

    RUN_TEST(test_wcSringNew_HelloWorld);
    RUN_TEST(test_wcSringNew_Abjad);
    RUN_TEST(test_wcSringNew_ULimit);
    RUN_TEST(test_wcSringNew_OutsideLimit);
    return UNITY_END();
}

/*----------------------------------------------------------------------------*/
