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
#include <stdlib.h>
#include <time.h>

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
    int max = rand() * 1000;
    for (int i = 0; i < max; i++) {}
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

    TEST_ASSERT_NULL(cache->mru);
    TEST_ASSERT_NULL(cache->lru);

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
        gi->image = NULL;
        gi->nextGlyph = NULL;
        gi->prevGlyph = NULL;

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
void test_cacheGetDifferentItems(void) {
    UtxCache* cache = cacheCreate(CACHE_CAPACITY, renderer);

    TEST_ASSERT_NOT_NULL(cache);
    TEST_ASSERT_EQUAL_UINT32(CACHE_CAPACITY, cache->capacity);

    TEST_ASSERT_NOT_NULL(cache->glyphImages);
    TEST_ASSERT_EQUAL_UINT32(0, setst_size(cache->glyphImages, UtxGlyphImage));

    TEST_ASSERT_NULL(cache->mru);
    TEST_ASSERT_NULL(cache->lru);

    uint32_t n = 0;
    for(uint16_t gid = 0; gid < CACHE_CAPACITY; gid++) {
        Image* img = cacheGet(cache, gid);
        TEST_ASSERT_NULL(img);

        TEST_ASSERT_NOT_NULL(cache->mru);
        TEST_ASSERT_NOT_NULL(cache->lru);

        n++;
        TEST_ASSERT_EQUAL_UINT32(n, setst_size(cache->glyphImages, UtxGlyphImage));
        TEST_ASSERT_EQUAL_UINT32(n, cache->requests);
        TEST_ASSERT_EQUAL_UINT32(0, cache->hits);
        TEST_ASSERT_EQUAL_DOUBLE(0.0, cacheHitRate(cache));
        TEST_ASSERT_EQUAL_DOUBLE(1.0, cacheMissRate(cache));

        UtxGlyphImage *gi = setst_get(cache->glyphImages, &gid, UtxGlyphImage, uint16_t);
        TEST_ASSERT_NOT_NULL(gi);
        TEST_ASSERT_EQUAL_UINT16(gid, gi->glyphId);
        TEST_ASSERT_NULL(gi->image);

        TEST_ASSERT_NULL(gi->nextGlyph);
        TEST_ASSERT_EQUAL_PTR(cache->mru, gi);

        if (cache->lru != gi) {
            TEST_ASSERT_NOT_NULL(gi->prevGlyph);
        }
    }

    UtxGlyphImage *gi;
    gi = cache->lru;
    for(uint16_t gid = 0; gid < CACHE_CAPACITY - 1; gid++) {
        TEST_ASSERT_EQUAL_UINT16(gid, gi->glyphId);
        TEST_ASSERT_NOT_NULL(gi->nextGlyph);
        gi = gi->nextGlyph;
    }
    TEST_ASSERT_EQUAL_PTR(cache->mru, gi);
    TEST_ASSERT_NULL(cache->mru->nextGlyph);

    gi = cache->mru;
    for(uint16_t gid = (uint16_t)CACHE_CAPACITY - 1; gid > 0; gid--) {
        TEST_ASSERT_EQUAL_UINT16(gid, gi->glyphId);
        TEST_ASSERT_NOT_NULL(gi->prevGlyph);
        gi = gi->prevGlyph;
    }
    TEST_ASSERT_EQUAL_PTR(cache->lru, gi);
    TEST_ASSERT_NULL(cache->lru->prevGlyph);

    bstd_printf("*** 100%% miss rate\n");
    bstd_printf("Test cache hit average time %.3f ms\n", cache->avgHitTime*1000.);
    bstd_printf("Test cache miss penalty time %.3f ms\n", cache->avgMissPenalty*1000.);
    bstd_printf("Test cache average access time %.3f ms\n\n", cacheAverageAccessTime(cache)*1000.);

    cacheDestroy(&cache);
    TEST_ASSERT_NULL(cache);
}

/*----------------------------------------------------------------------------*/
void test_cacheGetSameItems(void) {
    UtxCache* cache = cacheCreate(CACHE_CAPACITY, renderer);

    TEST_ASSERT_NOT_NULL(cache);
    TEST_ASSERT_EQUAL_UINT32(CACHE_CAPACITY, cache->capacity);

    TEST_ASSERT_NOT_NULL(cache->glyphImages);
    TEST_ASSERT_EQUAL_UINT32(0, setst_size(cache->glyphImages, UtxGlyphImage));

    uint16_t gid = 1024;
    UtxGlyphImage *gi = setst_insert(cache->glyphImages, &gid, UtxGlyphImage, uint16_t);
    TEST_ASSERT_NOT_NULL(gi);
    gi->glyphId = gid;
    gi->image = NULL;
    gi->nextGlyph = NULL;
    gi->prevGlyph = NULL;

    cache->mru = gi;
    cache->lru = gi;

    uint32_t n = 0;
    for(uint16_t i = 0; i < 10; i++) {
        Image* img = cacheGet(cache, gid);
        TEST_ASSERT_NULL(img);

        n++;
        TEST_ASSERT_EQUAL_UINT32(1, setst_size(cache->glyphImages, UtxGlyphImage));
        TEST_ASSERT_EQUAL_UINT32(n, cache->requests);
        TEST_ASSERT_EQUAL_UINT32(n, cache->hits);
        TEST_ASSERT_EQUAL_DOUBLE(1.0, cacheHitRate(cache));
        TEST_ASSERT_EQUAL_DOUBLE(0.0, cacheMissRate(cache));

        UtxGlyphImage *g = setst_get(cache->glyphImages, &gid, UtxGlyphImage, uint16_t);
        TEST_ASSERT_NOT_NULL(g);
        TEST_ASSERT_EQUAL_UINT16(gid, g->glyphId);
        TEST_ASSERT_NULL(g->image);

        TEST_ASSERT_NULL_MESSAGE(g->prevGlyph, "Previous Glyph should be NULL");
        TEST_ASSERT_NULL_MESSAGE(g->nextGlyph, "Next Glyph should be NULL");
        TEST_ASSERT_EQUAL_PTR(cache->mru, g);
        TEST_ASSERT_EQUAL_PTR(cache->lru, g);
    }

    bstd_printf("*** 100%% hit rate\n");
    bstd_printf("Test cache hit average time %.3f ms\n", cache->avgHitTime*1000.);
    bstd_printf("Test cache miss penalty time %.3f ms\n", cache->avgMissPenalty*1000.);
    bstd_printf("Test cache average access time %.3f ms\n\n", cacheAverageAccessTime(cache)*1000.);

    cacheDestroy(&cache);
    TEST_ASSERT_NULL(cache);
}
/*----------------------------------------------------------------------------*/
void test_cacheEvictions(void) {
    UtxCache* cache = cacheCreate(CACHE_CAPACITY, renderer);

    TEST_ASSERT_NOT_NULL(cache);
    TEST_ASSERT_EQUAL_UINT32(CACHE_CAPACITY, cache->capacity);

    TEST_ASSERT_NOT_NULL(cache->glyphImages);
    TEST_ASSERT_EQUAL_UINT32(0, setst_size(cache->glyphImages, UtxGlyphImage));

    TEST_ASSERT_NULL(cache->mru);
    TEST_ASSERT_NULL(cache->lru);

    uint16_t MIN = ((uint16_t)CACHE_CAPACITY);
    uint16_t MAX = ((uint16_t)CACHE_CAPACITY * 2);
    uint32_t n = 0;

    /* Fill the cache to capacity */
    for(uint16_t gid = 0; gid < MIN; gid++) {
        Image* img = cacheGet(cache, gid);
        TEST_ASSERT_NULL(img);
        n++;
        TEST_ASSERT_EQUAL_UINT32(n, setst_size(cache->glyphImages, UtxGlyphImage));
    }

    /* Refill with new items */
    for(uint16_t gid = MIN; gid < MAX; gid++) {
        Image* img = cacheGet(cache, gid);
        TEST_ASSERT_NULL(img);

        TEST_ASSERT_NOT_NULL(cache->mru);
        TEST_ASSERT_NOT_NULL(cache->lru);

        n++;
        TEST_ASSERT_EQUAL_UINT32(CACHE_CAPACITY, setst_size(cache->glyphImages, UtxGlyphImage));
        TEST_ASSERT_EQUAL_UINT32(n, cache->requests);
        TEST_ASSERT_EQUAL_UINT32(0, cache->hits);
        TEST_ASSERT_EQUAL_DOUBLE(0.0, cacheHitRate(cache));
        TEST_ASSERT_EQUAL_DOUBLE(1.0, cacheMissRate(cache));

        UtxGlyphImage *gi = setst_get(cache->glyphImages, &gid, UtxGlyphImage, uint16_t);
        TEST_ASSERT_NOT_NULL(gi);
        TEST_ASSERT_EQUAL_UINT16(gid, gi->glyphId);
        TEST_ASSERT_NULL(gi->image);

        TEST_ASSERT_NULL(gi->nextGlyph);
        TEST_ASSERT_EQUAL_PTR(cache->mru, gi);
    }

    /* Confirm previous items do not exist */
    for(uint16_t gid = 0; gid < MIN; gid++) {
        UtxGlyphImage *gi = setst_get(cache->glyphImages, &gid, UtxGlyphImage, uint16_t);
        TEST_ASSERT_NULL(gi);
    }

    /* Confirm integrity of the LRU chain */
    UtxGlyphImage *gi;
    gi = cache->lru;
    for(uint16_t gid = MIN; gid < MAX - 1; gid++) {
        TEST_ASSERT_EQUAL_UINT16(gid, gi->glyphId);
        TEST_ASSERT_NOT_NULL(gi->nextGlyph);
        gi = gi->nextGlyph;
    }
    TEST_ASSERT_EQUAL_PTR(cache->mru, gi);
    TEST_ASSERT_NULL(cache->mru->nextGlyph);

    /* Confirm integrity of the MRU chain */
    gi = cache->mru;
    for(uint16_t gid = MAX - 1; gid > MIN; gid--) {
        TEST_ASSERT_EQUAL_UINT16(gid, gi->glyphId);
        TEST_ASSERT_NOT_NULL(gi->prevGlyph);
        gi = gi->prevGlyph;
    }
    TEST_ASSERT_EQUAL_PTR(cache->lru, gi);
    TEST_ASSERT_NULL(cache->lru->prevGlyph);

    bstd_printf("*** 100%% miss rate\n");
    bstd_printf("Test cache hit average time %.3f ms\n", cache->avgHitTime*1000.);
    bstd_printf("Test cache miss penalty time %.3f ms\n", cache->avgMissPenalty*1000.);
    bstd_printf("Test cache average access time %.3f ms\n\n", cacheAverageAccessTime(cache)*1000.);

    cacheDestroy(&cache);
    TEST_ASSERT_NULL(cache);
}

/*----------------------------------------------------------------------------*/
int main(void) {
    UNITY_BEGIN();

    srand((int)time(NULL));
    RUN_TEST(test_createZeroCapacityCache);
    RUN_TEST(test_createNullRenderer);
    RUN_TEST(test_createCache);

    RUN_TEST(test_cacheAddItems);
    RUN_TEST(test_cacheGetDifferentItems);
    RUN_TEST(test_cacheGetSameItems);

    RUN_TEST(test_cacheEvictions);

    return UNITY_END();
}

/*----------------------------------------------------------------------------*/
