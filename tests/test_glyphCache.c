/*******************************************************************************
 * Copyright (c) 2025. All rights reserved.
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license,
 * visit # http://creativecommons.org/licenses/by/4.0/.
 *
 * Author: roximn <roximn148@gmail.com>
 ******************************************************************************/
#include <stdio.h>

#include "cache.h"

#include <sewer/bstd.h>
#include <core/core.h>
#include <core/heap.h>
#include <core/clock.h>
#include <core/setst.h>

#include "unity.h"

/*----------------------------------------------------------------------------*/
static uint32_t CACHE_CAPACITY = 10;
/*----------------------------------------------------------------------------*/
void setUp(void) {
    heap_stats(TRUE);
}

/*----------------------------------------------------------------------------*/
void tearDown(void) {
    TEST_ASSERT_FALSE(heap_leaks());
}


/*----------------------------------------------------------------------------*/
Image* renderer(uint16_t glyphId) {
    return NULL;
    unref(glyphId);
}

/*----------------------------------------------------------------------------*/
void test_createZeroCapacityCache(void) {
    UtxCache* cache = cacheCreate(0, renderer);
    TEST_ASSERT_NULL(cache);
}

/*----------------------------------------------------------------------------*/
void test_createNullRenderer(void) {
    UtxCache* cache = cacheCreate(10, NULL);
    TEST_ASSERT_NULL(cache);
}

/*----------------------------------------------------------------------------*/
void test_createCache(void) {
    UtxCache* cache = cacheCreate(CACHE_CAPACITY, renderer);

    TEST_ASSERT_NOT_NULL(cache);
    TEST_ASSERT_EQUAL_UINT32(CACHE_CAPACITY, cache->capacity);

    TEST_ASSERT_NOT_NULL(cache->glyphImages);
    TEST_ASSERT_EQUAL_UINT32(0, setst_size(cache->glyphImages, UtxGlyphImage));

    TEST_ASSERT_EQUAL_UINT32(0, cache->requests);
    TEST_ASSERT_EQUAL_UINT32(0, cache->hits);
    TEST_ASSERT_EQUAL_UINT32(0, cache->evictions);

    TEST_ASSERT_EQUAL_DOUBLE(0.0, cache->avgHitTime);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, cache->avgMissPenalty);

    cacheDestroy(&cache);
    TEST_ASSERT_NULL(cache);
}

/*----------------------------------------------------------------------------*/
void test_cacheAddItems(void) {
    UtxCache* cache = cacheCreate(CACHE_CAPACITY, renderer);

    TEST_ASSERT_NOT_NULL(cache);
    TEST_ASSERT_EQUAL_UINT32(CACHE_CAPACITY, cache->capacity);

    TEST_ASSERT_NOT_NULL(cache->glyphImages);
    TEST_ASSERT_EQUAL_UINT32(0, setst_size(cache->glyphImages, UtxGlyphImage));

    for(uint16_t i = 0; i < CACHE_CAPACITY;) {
        UtxGlyphImage *gi = setst_insert(cache->glyphImages, &i, UtxGlyphImage, uint16_t);
        TEST_ASSERT_NOT_NULL(gi);

        gi->glyphId = i;
        gi->image = renderer(i);

        i++;
        real64_t expected = (real64_t)i/(real64_t)CACHE_CAPACITY;
        TEST_ASSERT_EQUAL_DOUBLE(expected, cacheLoadFactor(cache));
    }

    TEST_ASSERT_EQUAL_UINT32(CACHE_CAPACITY,
        setst_size(cache->glyphImages, UtxGlyphImage));

    setst_foreach(glyphImage, cache->glyphImages, UtxGlyphImage)
        TEST_ASSERT_EQUAL_UINT16(glyphImage_i, glyphImage->glyphId);
        TEST_ASSERT_NULL(glyphImage->image);
    setst_fornext(glyphImage, cache->glyphImages, UtxGlyphImage);

    cacheDestroy(&cache);
    TEST_ASSERT_NULL(cache);
}

/*----------------------------------------------------------------------------*/
void test_cacheGetItems(void) {
    UtxCache* cache = cacheCreate(CACHE_CAPACITY, renderer);

    TEST_ASSERT_NOT_NULL(cache);
    TEST_ASSERT_EQUAL_UINT32(CACHE_CAPACITY, cache->capacity);

    TEST_ASSERT_NOT_NULL(cache->glyphImages);
    TEST_ASSERT_EQUAL_UINT32(0, setst_size(cache->glyphImages, UtxGlyphImage));

    for(uint16_t i = 0; i < CACHE_CAPACITY; i++) {
        UtxGlyphImage *gi = setst_insert(cache->glyphImages, &i, UtxGlyphImage, uint16_t);
        TEST_ASSERT_NOT_NULL(gi);
        gi->glyphId = i;
        gi->image = renderer(i);
    }

    TEST_ASSERT_EQUAL_UINT32(CACHE_CAPACITY, setst_size(cache->glyphImages, UtxGlyphImage));
    TEST_ASSERT_EQUAL_DOUBLE(1.0, cacheLoadFactor(cache));

    for(uint16_t gid = 0; gid < CACHE_CAPACITY; gid++) {
        UtxGlyphImage *gi = setst_get(cache->glyphImages, &gid, UtxGlyphImage, uint16_t);
        TEST_ASSERT_NOT_NULL_MESSAGE(gi, "Glyph Image should be present");
        TEST_ASSERT_EQUAL_UINT16(gid, gi->glyphId);
        TEST_ASSERT_NULL(gi->image);
    }

    cacheDestroy(&cache);
    TEST_ASSERT_NULL(cache);
}

/*----------------------------------------------------------------------------*/
int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_createZeroCapacityCache);
    RUN_TEST(test_createNullRenderer);
    RUN_TEST(test_createCache);

    RUN_TEST(test_cacheAddItems);
    RUN_TEST(test_cacheGetItems);

    return UNITY_END();
}

/*----------------------------------------------------------------------------*/
