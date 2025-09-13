/*******************************************************************************
 * Copyright (c) 2025. All rights reserved.
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license,
 * visit # http://creativecommons.org/licenses/by/4.0/.
 *
 * Author: roximn <roximn148@gmail.com>
 ******************************************************************************/
#include "cache.h"

#include <sewer/bstd.h>
#include <core/heap.h>
#include <core/setst.h>
#include <core/clock.h>

/** ----------------------------------------------------------------------------
 * @brief Create a new glyph cache of given capacity
 * -------------------------------------------------------------------------- */
static void fGlyphDestroy(UtxGlyphImage *gi) {
    if (gi) {
        /* code */
        bstd_printf("Releasing %d glyph, ", gi->glyphId);
        if (gi->image != NULL) {
            image_destroy(&gi->image);
        } else {
            bstd_printf("ERROR: gid#%d removing NULL glyph render\n", gi->glyphId);
        }
    } else {
        bstd_printf("ERROR: removing NULL glyph\n");
    }
}

/** ----------------------------------------------------------------------------
 * @brief Compare two glyph IDs and return the result
 * -------------------------------------------------------------------------- */
static int fGlyphCompare(const UtxGlyphImage *gi, const uint16_t *glyphId) {
    int this = gi->glyphId;
    int other = *cast(glyphId, uint16_t);

    return this - other;
}

/** ----------------------------------------------------------------------------
 * @brief Create a new glyph cache of given capacity
 * -------------------------------------------------------------------------- */
UtxCache* cacheCreate(uint32_t capacity, FPtrGlyphRender fpRenderer) {
    if (capacity < 1 || fpRenderer == NULL) {
        return NULL;
    }

    UtxCache *cache = heap_new0(UtxCache);
    cache->capacity = capacity;
    cache->render = fpRenderer;

    cache->glyphImages = setst_create(fGlyphCompare, UtxGlyphImage, uint16_t);

    return cache;
}

/** ----------------------------------------------------------------------------
 * Minimum number of values to accumulate before calculating the average
 * -------------------------------------------------------------------------- */
#define FACTOR 3

/** ----------------------------------------------------------------------------
 * @brief Get image of the given glyph id
 * -------------------------------------------------------------------------- */
Image* cacheGet(UtxCache* cache, uint16_t glyphId) {
    if (cache == NULL || cache->glyphImages == NULL) {
        return NULL;
    }

    Clock *clock = clock_create(0.0);

    clock_reset(clock);
    cache->requests += 1;
    UtxGlyphImage *gi = setst_get(cache->glyphImages, &glyphId, UtxGlyphImage, uint16_t);
    if (gi != NULL) {
        cache->hits += 1;
        real64_t t = clock_elapsed(clock);
        uint32_t N = cache->hits > FACTOR ? cache->hits : FACTOR;
        cache->avgHitTime += (t - cache->avgHitTime) / N;
    } else {
        gi = setst_insert(cache->glyphImages, &glyphId, UtxGlyphImage, uint16_t);
        if (gi != NULL) {
            gi->glyphId = glyphId;
            gi->image = cache->render(glyphId);
            real64_t t = clock_elapsed(clock);
            uint32_t misses = cache->requests-cache->hits;
            uint32_t N = misses > FACTOR ? misses : FACTOR;
            cache->avgMissPenalty += (t - cache->avgMissPenalty) / N;
        }
    }

    clock_destroy(&clock);
    return gi->image;
}

/** ----------------------------------------------------------------------------
 * @brief Calculate cache load factor
 * -------------------------------------------------------------------------- */
real64_t cacheLoadFactor(UtxCache* cache) {
    if (cache == NULL) {
        return 0.0;
    }

    if (cache->capacity > 0) {
        return (real64_t)setst_size(cache->glyphImages, UtxGlyphImage) / (real64_t)cache->capacity;
    } else {
        return 0.0;
    }
}

/** ----------------------------------------------------------------------------
 * @brief Calculate cache hit rate
 * -------------------------------------------------------------------------- */
real64_t cacheHitRate(UtxCache* cache) {
    if (cache == NULL) {
        return 0.0;
    }

    if (cache->requests > 0) {
        return (real64_t)cache->hits / (real64_t)cache->requests;
    } else {
        return 0.0;
    }
}

/** ----------------------------------------------------------------------------
 * @brief Calculate cache miss rate
 * -------------------------------------------------------------------------- */
real64_t cacheMissRate(UtxCache* cache) {
    if (cache == NULL) {
        return 0.0;
    }

    if (cache->requests > 0) {
        return (real64_t)(cache->requests - cache->hits) / (real64_t)cache->requests;
    } else {
        return 0.0;
    }
}

/** ----------------------------------------------------------------------------
 * @brief Calculate cache eviction rate
 * -------------------------------------------------------------------------- */
real64_t cacheEvictionRate(UtxCache* cache) {
    if (cache == NULL) {
        return 0.0;
    }

    if (cache->requests > 0) {
        return (real64_t)(cache->evictions) / (real64_t)cache->requests;
    } else {
        return 0.0;
    }
}

/** ----------------------------------------------------------------------------
 * @brief Calculate average access time
 * -------------------------------------------------------------------------- */
real64_t cacheAverageAccessTime(UtxCache* cache) {
    if (cache == NULL) {
        return 0.0;
    }

    return (cache->avgHitTime + cache->avgMissPenalty) * cacheHitRate(cache);
}

/** ----------------------------------------------------------------------------
 * @brief Delete glyph cache
 * -------------------------------------------------------------------------- */
void cacheDestroy(UtxCache **pCache) {
    if (pCache == NULL || *pCache == NULL) {
        return;
    }

    UtxCache *cache = *pCache;

    if (cache->glyphImages) {
        setst_destroy(&cache->glyphImages, fGlyphDestroy, UtxGlyphImage);
    }

    heap_delete(pCache, UtxCache);
}

/*----------------------------------------------------------------------------*/
