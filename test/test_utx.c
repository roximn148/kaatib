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
#include <osbs/bfile.h>
#include <core/hfile.h>
#include <sewer/bmath.h>

#include "unity.h"
#include "utx.h"

/*----------------------------------------------------------------------------*/
String* generateUuid(void) {
    /*
    00000000-0000-0000-0000-000000000000 - UUID format
    012345678901234567890123456789012345 - char indices
    */
   
    static const char DIGITS[] = "0123456789abcdef";
    String *buf = str_c("00000000-0000-0000-0000-000000000000");

    char_t *s = tcc(buf);
    for(int i = 0; i < 36; ++i) {
        s[i] = DIGITS[bmath_randi(0, 15)];
    }
    s[8] = s[13] = s[18] = s[23] = '-';

    fprintf(stderr, "Generated UUID: %s\n", s);
    return buf;
}

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
void test_utxCreate_New(void) {
    UtxFile* utx = utxCreateNew();

    TEST_ASSERT_NOT_NULL(utx);
    TEST_ASSERT_NOT_NULL(utx->fileName);
    TEST_ASSERT_FALSE(str_empty(utx->fileName));
    TEST_ASSERT_NOT_NULL(utx->contents);
    TEST_ASSERT_TRUE(str_empty(utx->contents));
    TEST_ASSERT_NULL(utx->fileFolder);
    TEST_ASSERT_FALSE(utx->isModified);

    utxDestroy(&utx);
}

/*----------------------------------------------------------------------------*/
void test_utxCreate_UniqueTitle(void) {
    UtxFile* utx1 = utxCreateNew();
    TEST_ASSERT_NOT_NULL(utx1);
    TEST_ASSERT_NOT_NULL(utx1->fileName);
    TEST_ASSERT_FALSE(str_empty(utx1->fileName));

    UtxFile* utx2 = utxCreateNew();
    TEST_ASSERT_NOT_NULL(utx2);
    TEST_ASSERT_NOT_NULL(utx2->fileName);
    TEST_ASSERT_FALSE(str_empty(utx2->fileName));
    
    TEST_ASSERT_NOT_EQUAL(0, str_scmp(utx1->fileName, utx2->fileName));

    utxDestroy(&utx1);
    utxDestroy(&utx2);
}

/*----------------------------------------------------------------------------*/
void test_utxCreateFrom_Null(void) {
    TEST_ASSERT_NULL(utxCreateFromString(NULL));
}

/*----------------------------------------------------------------------------*/
void test_utxCreateFrom_String(void) {
    char_t cs[] = "A test string";
    String* testString = str_c(cs);
    UtxFile* utx = utxCreateFromString(testString);

    TEST_ASSERT_NOT_NULL(utx);
    TEST_ASSERT_NOT_NULL(utx->contents);
    TEST_ASSERT_FALSE(str_empty(utx->contents));
    TEST_ASSERT_EQUAL(0, str_scmp(utx->contents, testString));
    TEST_ASSERT_NOT_NULL(utx->fileName);
    TEST_ASSERT_FALSE(str_empty(utx->fileName));
    TEST_ASSERT_NULL(utx->fileFolder);
    TEST_ASSERT_FALSE(utx->isModified);

    str_destroy(&testString);
    utxDestroy(&utx);
}

/*----------------------------------------------------------------------------*/
void test_utxSetContents_UtxNull(void) {
    Result result = utxSetContents(NULL, NULL);
    TEST_ASSERT_EQUAL(RInvalidUtxPointer, result);
}

/*----------------------------------------------------------------------------*/
void test_utxSetContents_ContentsNull(void) {
    UtxFile* utx = utxCreateNew();
    TEST_ASSERT_NOT_NULL(utx);
    Result result = utxSetContents(utx, NULL);
    TEST_ASSERT_EQUAL(RInvalidContents, result);
    utxDestroy(&utx);
}

/*----------------------------------------------------------------------------*/
void test_utxSetContents_NewString(void) {
    char_t cs5[] = "A test string 00001";
    char_t cs10[] = "A test string 00002";
    String* testString5 = str_c(cs5);
    String* testString10 = str_c(cs10);

    UtxFile* utx = utxCreateNew();
    TEST_ASSERT_TRUE(str_empty(utx->contents));

    utxSetContents(utx, testString5);
    TEST_ASSERT_FALSE(str_empty(utx->contents));
    TEST_ASSERT_EQUAL(0, str_scmp(utx->contents, testString5));
    TEST_ASSERT_NOT_EQUAL(0, str_scmp(utx->contents, testString10));

    utxSetContents(utx, testString10);
    TEST_ASSERT_FALSE(str_empty(utx->contents));
    TEST_ASSERT_EQUAL(0, str_scmp(utx->contents, testString10));
    TEST_ASSERT_NOT_EQUAL(0, str_scmp(utx->contents, testString5));

    str_destroy(&testString5);
    str_destroy(&testString10);
    utxDestroy(&utx);
}

/*----------------------------------------------------------------------------*/
void test_utxContentLength(void) {
    char_t cs5[] = "12345";
    char_t cs10[] = "1234567890";
    String* testString5 = str_c(cs5);
    String* testString10 = str_c(cs10);

    UtxFile* utx0 = utxCreateNew();
    TEST_ASSERT_EQUAL(0, utxLength(utx0));
    
    UtxFile* utx1 = utxCreateFromString(testString5);
    TEST_ASSERT_EQUAL(5, utxLength(utx1));

    utxSetContents(utx1, testString10);
    TEST_ASSERT_EQUAL(10, utxLength(utx1));

    str_destroy(&testString5);
    str_destroy(&testString10);
    utxDestroy(&utx0);
    utxDestroy(&utx1);
}

/*----------------------------------------------------------------------------*/
void test_utxDestruction(void) {
    UtxFile* utx = utxCreateNew();
    TEST_ASSERT_NOT_NULL(utx);
    utxDestroy(&utx);
    TEST_ASSERT_NULL(utx);
}

/*----------------------------------------------------------------------------*/
void test_utxRead_UtxNull(void) {
    TEST_ASSERT_EQUAL(RInvalidUtxPointer, utxRead(NULL, NULL));
}

/*----------------------------------------------------------------------------*/
String* createTempFilename(void) {
    String *fileNameStem = generateUuid();
    String *fileName = str_printf("%s.txt", tc(fileNameStem));
    
    String* filePath = hfile_tmp_path(tc(fileName));

    str_destroy(&fileNameStem);
    str_destroy(&fileName);
    return filePath;
}

/*----------------------------------------------------------------------------*/
String* createUtf8File(String* text) {
    String *filePath = createTempFilename();

    ferror_t error;
    hfile_from_string(tc(filePath), text, &error);
    TEST_ASSERT_EQUAL(ekFOK, error);
    fprintf(stderr, "Created tmp file: %s\n", tc(filePath));

    return filePath;
}

/*----------------------------------------------------------------------------*/
void test_utxRead_AllFilePathNull(void) {
    UtxFile* utx = utxCreateNew();
    TEST_ASSERT_NOT_NULL(utx);
    TEST_ASSERT_NULL(utx->fileFolder);

    TEST_ASSERT_EQUAL(RFileError, utxRead(utx, NULL));

    TEST_ASSERT_NULL(utx->fileFolder);
    utxDestroy(&utx);
}

/*----------------------------------------------------------------------------*/
void test_utxReadFileContents(void) {
    char_t cs10[] = "1234567890";
    String *testString10 = str_c(cs10);
    String *filePath = createUtf8File(testString10);

    UtxFile* utx = utxCreateNew();
    TEST_ASSERT_NULL(utx->fileFolder);
    TEST_ASSERT_TRUE(str_empty(utx->contents));
    TEST_ASSERT_EQUAL(0, utxLength(utx));

    Result result = utxReadContentsFromFile(utx, tc(filePath));
    TEST_ASSERT_EQUAL(ROkay, result);

    TEST_ASSERT_EQUAL(0, str_cmp(utx->contents, cs10));
    
    ferror_t error;
    bfile_delete(tc(filePath), &error);
    TEST_ASSERT_EQUAL(ekFOK, error);

    str_destroy(&filePath);
    str_destroy(&testString10);
    utxDestroy(&utx);
}

/*----------------------------------------------------------------------------*/
void test_utxCreateFromFile(void) {
    char_t cs10[] = "1234567890";
    String *testString10 = str_c(cs10);
    String *filePath = createUtf8File(testString10);
    String *folder, *fileName;
    str_split_pathname(tc(filePath), &folder, &fileName);
    fprintf(stderr, "filePath: '%s'\n", tc(filePath));
    fprintf(stderr, "fileName: '%s'\n", tc(fileName));
    fprintf(stderr, "fileFolder: '%s'\n", tc(folder));

    UtxFile* utx = utxCreateFromFile(tc(filePath));
    TEST_ASSERT_NOT_NULL(utx);
    TEST_ASSERT_FALSE(str_empty(utx->contents));
    TEST_ASSERT_EQUAL(0, str_scmp(utx->contents, testString10));
    TEST_ASSERT_FALSE(utx->isModified);

    TEST_ASSERT_EQUAL(0, str_scmp(utx->fileName, fileName));
    str_cat(&folder, "/");
    TEST_ASSERT_EQUAL(0, str_scmp(utx->fileFolder, folder));
    
    ferror_t error;
    bfile_delete(tc(filePath), &error);
    TEST_ASSERT_EQUAL(ekFOK, error);

    str_destroy(&filePath);
    str_destroy(&testString10);
    utxDestroy(&utx);
}

/*----------------------------------------------------------------------------*/
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_utxCreate_New);
    RUN_TEST(test_utxCreate_UniqueTitle);
    RUN_TEST(test_utxCreateFrom_Null);
    RUN_TEST(test_utxCreateFrom_String);
    RUN_TEST(test_utxDestruction);

    RUN_TEST(test_utxSetContents_UtxNull);
    RUN_TEST(test_utxSetContents_ContentsNull);
    RUN_TEST(test_utxSetContents_NewString);
    
    RUN_TEST(test_utxContentLength);

    RUN_TEST(test_utxRead_UtxNull);
    RUN_TEST(test_utxRead_AllFilePathNull);
    RUN_TEST(test_utxReadFileContents);
    RUN_TEST(test_utxCreateFromFile);
    return UNITY_END();
}

/*----------------------------------------------------------------------------*/
