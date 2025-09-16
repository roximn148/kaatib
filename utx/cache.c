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
#include <osbs/log.h>
#include <core/heap.h>
#include <core/setst.h>
#include <core/clock.h>

/** ----------------------------------------------------------------------------
 * @brief Release resources of the GlyphImage item when removing from cache
 * -------------------------------------------------------------------------- */
static void fGlyphDestroy(UtxGlyphImage *gi) {
    if (gi != NULL && gi->image != NULL) {
        image_destroy(&gi->image);
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
UtxCache* cacheCreate(uint32_t capacity, RenderClosure *closure) {
    if (capacity < 1 || closure == NULL) {
        return NULL;
    }

    UtxCache *cache = heap_new0(UtxCache);
    cache->capacity = capacity;
    cache->closure = closure;

    cache->glyphImages = setst_create(fGlyphCompare, UtxGlyphImage, uint16_t);
    cache->headId = EOL;
    cache->tailId = EOL;

    return cache;
}

/** ----------------------------------------------------------------------------
 * Minimum number of values to accumulate before calculating the average
 * -------------------------------------------------------------------------- */
#define FACTOR 1

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
    UtxGlyphImage *pCurrentGlyphImage = setst_get(cache->glyphImages, &glyphId, UtxGlyphImage, uint16_t);
    if (pCurrentGlyphImage != NULL) {
        /* Cache Hit =========================================================*/
        /* Detach */
        if(cache->headId != pCurrentGlyphImage->glyphId) {
            if (pCurrentGlyphImage->prevGlyphId != EOL) {
                UtxGlyphImage *prev = setst_get(cache->glyphImages, &pCurrentGlyphImage->prevGlyphId, UtxGlyphImage, uint16_t);
                prev->nextGlyphId = pCurrentGlyphImage->nextGlyphId;
            }
            if (pCurrentGlyphImage->nextGlyphId != EOL) {
                UtxGlyphImage *next = setst_get(cache->glyphImages, &pCurrentGlyphImage->nextGlyphId, UtxGlyphImage, uint16_t);
                next->prevGlyphId = pCurrentGlyphImage->prevGlyphId;
            }

            /* Push to list top */
            if (cache->headId != EOL) {
                UtxGlyphImage *mru = setst_get(cache->glyphImages, &cache->headId, UtxGlyphImage, uint16_t);
                mru->nextGlyphId = pCurrentGlyphImage->glyphId;
                pCurrentGlyphImage->prevGlyphId = mru->glyphId;
            }
            cache->headId = pCurrentGlyphImage->glyphId;
            pCurrentGlyphImage->nextGlyphId = EOL;
        }

        /* Stats =============================================================*/
        cache->hits += 1;
        real64_t t = clock_elapsed(clock);
        uint32_t N = cache->hits > FACTOR ? cache->hits : FACTOR;
        cache->avgHitTime += (t - cache->avgHitTime) / N;
    } else {
        /* Cache Miss ========================================================*/
        pCurrentGlyphImage = setst_insert(cache->glyphImages, &glyphId, UtxGlyphImage, uint16_t);
        if (pCurrentGlyphImage != NULL) {
            pCurrentGlyphImage->glyphId = glyphId;
            RenderClosure *closure = cache->closure;
            pCurrentGlyphImage->image = closure->render(glyphId, closure->context);
            pCurrentGlyphImage->prevGlyphId = EOL;
            pCurrentGlyphImage->nextGlyphId = EOL;

            /* Push to list top */
            if (cache->headId != EOL) {
                UtxGlyphImage *mru = setst_get(cache->glyphImages, &cache->headId, UtxGlyphImage, uint16_t);
                mru->nextGlyphId = pCurrentGlyphImage->glyphId;
                pCurrentGlyphImage->prevGlyphId = mru->glyphId;
            } else {
                cache->tailId = pCurrentGlyphImage->glyphId;
            }
            cache->headId = pCurrentGlyphImage->glyphId;

            /* If capcity exceeded, evict the MRU list tail */
            while (setst_size(cache->glyphImages, UtxGlyphImage) > cache->capacity) {
                UtxGlyphImage *lru = setst_get(cache->glyphImages, &cache->tailId, UtxGlyphImage, uint16_t);
                if (lru != NULL) {
                    UtxGlyphImage *next = setst_get(cache->glyphImages, &lru->nextGlyphId, UtxGlyphImage, uint16_t);
                    if (next != NULL) {
                        next->prevGlyphId = EOL;
                        cache->tailId = next->glyphId;
                    }
                    setst_delete(cache->glyphImages, &lru->glyphId, fGlyphDestroy, UtxGlyphImage, uint16_t);
                    cache->evictions += 1;
                }
            }

            /* Stats =========================================================*/
            real64_t t = clock_elapsed(clock);
            uint32_t misses = cache->requests - cache->hits;
            uint32_t N = misses > FACTOR ? misses : FACTOR;
            cache->avgMissPenalty += (t - cache->avgMissPenalty) / N;
        }
    }

    clock_destroy(&clock);
    return pCurrentGlyphImage->image;
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

    return cache->avgHitTime + (cache->avgMissPenalty * cacheMissRate(cache));
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
