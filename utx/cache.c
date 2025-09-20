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

#include <sewer/cassert.h>
#include <osbs/log.h>
#include <core/heap.h>
#include <core/setst.h>
#include <core/clock.h>
#include <core/stream.h>
#include <core/hfile.h>

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
 * @brief Create new glyph image node
 * -------------------------------------------------------------------------- */
static UtxGlyphImage* nodeCreate(UtxCache* cache, uint16_t glyphId) {
    cassert_no_null(cache);
    cassert(glyphId != EOL);

    RenderClosure *closure = cache->closure;
    Image *img = closure->render(glyphId, closure->context);

    if (img == NULL) {
        return NULL;
    }

    UtxGlyphImage *newGlyphImage = setst_insert(
        cache->glyphImages, &glyphId,
        UtxGlyphImage, uint16_t);

    if (newGlyphImage != NULL) {
        newGlyphImage->glyphId = glyphId;
        newGlyphImage->prevGlyphId = EOL;
        newGlyphImage->nextGlyphId = EOL;
        newGlyphImage->image = img;
    } else {
        image_destroy(&img);
        log_printf("Failed to create node for gid#%d", glyphId);
    }

    return newGlyphImage;
}

/** ----------------------------------------------------------------------------
 * @brief Detach given glyph image node from the list
 * -------------------------------------------------------------------------- */
static UtxGlyphImage* nodeDetach(UtxCache* cache, UtxGlyphImage *gi) {
    cassert_no_null(cache);
    cassert_no_null(gi);

    if (cache->headId == gi->glyphId) {
        cache->headId = gi->prevGlyphId;
    }

    if (cache->tailId == gi->glyphId) {
        cache->tailId = gi->nextGlyphId;
    }

    if (gi->prevGlyphId != EOL) {
        UtxGlyphImage *prev = setst_get(
            cache->glyphImages, &gi->prevGlyphId,
            UtxGlyphImage, uint16_t);
        cassert_no_null(prev);
        prev->nextGlyphId = gi->nextGlyphId;
    }

    if (gi->nextGlyphId != EOL) {
        UtxGlyphImage *next = setst_get(
            cache->glyphImages, &gi->nextGlyphId,
            UtxGlyphImage, uint16_t);
        cassert_no_null(next);
        next->prevGlyphId = gi->prevGlyphId;
    }

    gi->prevGlyphId = EOL;
    gi->nextGlyphId = EOL;

    return gi;
}

/** ----------------------------------------------------------------------------
 * @brief Add given glyph image node to the top of the list
 * -------------------------------------------------------------------------- */
static void nodeAddToTop(UtxCache* cache, UtxGlyphImage *gi) {
    cassert_no_null(cache);
    cassert_no_null(gi);

    gi->prevGlyphId = cache->headId;
    gi->nextGlyphId = EOL;

    if (cache->headId != EOL) {
        UtxGlyphImage *mru = setst_get(
            cache->glyphImages, &cache->headId,
            UtxGlyphImage, uint16_t);
        cassert_no_null(mru);
        cassert(mru->nextGlyphId == EOL);
        mru->nextGlyphId = gi->glyphId;
    }
    cache->headId = gi->glyphId;

    if (cache->tailId == EOL) {
        cache->tailId = gi->glyphId;
    }
}

/** ----------------------------------------------------------------------------
 * @brief Delete given glyph image node from the list
 * -------------------------------------------------------------------------- */
static bool_t nodeDelete(UtxCache* cache, UtxGlyphImage *gi) {
    cassert_no_null(cache);
    cassert_no_null(gi);
    bool_t deleted = setst_delete(
        cache->glyphImages, &gi->glyphId,
        fGlyphDestroy,
        UtxGlyphImage, uint16_t);
    return deleted;
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

    UtxGlyphImage *gi = setst_get(
        cache->glyphImages, &glyphId,
        UtxGlyphImage, uint16_t);

    if (gi != NULL) { /* Cache Hit ===========================================*/
        nodeDetach(cache, gi);
        nodeAddToTop(cache, gi);

        /* Stats -------------------------------------------------------------*/
        cache->hits += 1;
        real64_t t = clock_elapsed(clock);
        uint32_t N = cache->hits > FACTOR ? cache->hits : FACTOR;
        cache->avgHitTime += (t - cache->avgHitTime) / N;

    } else { /* Cache Miss ===================================================*/
        gi = nodeCreate(cache, glyphId);
        if (gi != NULL) {
            nodeAddToTop(cache, gi);

            /* If capcity exceeded, evict the MRU list tail */
            while (setst_size(cache->glyphImages, UtxGlyphImage) > cache->capacity) {
                {
                    UtxGlyphImage *lru = setst_get(
                        cache->glyphImages, &cache->tailId,
                        UtxGlyphImage, uint16_t);
                    nodeDetach(cache, lru);
                    bool_t deleted = nodeDelete(cache, lru);
                    cassert(deleted);
                }
                cache->evictions += 1;
            }
        }

        /* Stats -------------------------------------------------------------*/
        real64_t t = clock_elapsed(clock);
        uint32_t misses = cache->requests - cache->hits;
        uint32_t N = misses > FACTOR ? misses : FACTOR;
        cache->avgMissPenalty += (t - cache->avgMissPenalty) / N;
    }

    clock_destroy(&clock);
    if (gi != NULL){
        return gi->image;
     } else {
        return NULL;
     }
}

/** ----------------------------------------------------------------------------
 * @brief Get image of the given glyph id
 * -------------------------------------------------------------------------- */
void cacheInvalidate(UtxCache* cache) {
    if (cache == NULL || cache->glyphImages == NULL) {
        return NULL;
    }

    /* Clear all images */
    setst_destroy(&cache->glyphImages, fGlyphDestroy, UtxGlyphImage);

    /* Reset the image cache as new */
    cache->glyphImages = setst_create(fGlyphCompare, UtxGlyphImage, uint16_t);
    cache->headId = EOL;
    cache->tailId = EOL;
}

/** ----------------------------------------------------------------------------
 * @brief Calculate cache load factor
 * -------------------------------------------------------------------------- */
real64_t cacheLoadFactor(UtxCache* cache) {
    if (cache == NULL) {
        return 0.0;
    }

    if (cache->capacity > 0) {
        return (real64_t)setst_size(cache->glyphImages, UtxGlyphImage) /
               (real64_t)cache->capacity;
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
        return (real64_t)cache->hits /
               (real64_t)cache->requests;
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
        return (real64_t)(cache->requests - cache->hits) /
               (real64_t)cache->requests;
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
        return (real64_t)(cache->evictions) /
               (real64_t)cache->requests;
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

/** ----------------------------------------------------------------------------
 * @brief Dump glyph cache data structure to given Stream.
 * -------------------------------------------------------------------------- */
void cacheToStream(UtxCache *cache, Stream *strm) {
    if (cache == NULL || strm == NULL) {
        return;
    }

    stm_printf(strm, "Cache(%3d:%3d) = [ ",
        cache->capacity,
        setst_size(cache->glyphImages, UtxGlyphImage)
    );
    setst_foreach(glyphImage, cache->glyphImages, UtxGlyphImage);
        if (glyphImage_i > 0) { stm_printf(strm, ", "); }
        stm_printf(strm, "%3d", glyphImage->glyphId);
    setst_fornext(glyphImage, cache->glyphImages, UtxGlyphImage);
    stm_printf(strm, " ]\n");

    UtxGlyphImage *gi;
    uint16_t i;

    stm_printf(strm, "MRU            = [ ");
    gi = setst_get(cache->glyphImages, &cache->headId,
                   UtxGlyphImage, uint16_t);
    i = 0;
    while(gi != NULL) {
        if (i > 0) { stm_printf(strm, ", "); }
        stm_printf(strm, "%3d", gi->glyphId);
        if (gi->prevGlyphId == EOL) { break; }
        gi = setst_get(cache->glyphImages, &gi->prevGlyphId,
            UtxGlyphImage, uint16_t);
        i++;
    }
    stm_printf(strm, " ] (%3d)\n", i+1);

    stm_printf(strm, "LRU            = [ ");
    gi = setst_get(cache->glyphImages, &cache->tailId,
                   UtxGlyphImage, uint16_t);
    i = 0;
    while(gi != NULL) {
        if (i > 0) { stm_printf(strm, ", "); }
        stm_printf(strm, "%3d", gi->glyphId);
        if (gi->nextGlyphId == EOL) { break; }
        gi = setst_get(cache->glyphImages, &gi->nextGlyphId,
                       UtxGlyphImage, uint16_t);
        i++;
    }
    stm_printf(strm, " ] (%3d)\n", i+1);
}

/** ----------------------------------------------------------------------------
 * @brief Dump glyph cache data structure to given file.
 * -------------------------------------------------------------------------- */
void cacheDump(UtxCache *cache, const char_t *filename, const char_t *label) {
    if (cache == NULL || cache->glyphImages == NULL || filename == NULL) {
        return;
    }

    ferror_t error;
    Stream *strm;
    if (hfile_exists(filename, NULL)) {
        strm = stm_append_file(filename, &error);
    } else {
        strm = stm_to_file(filename, &error);
    }
    if (error != ekFOK) {
        log_printf("Error opening '%s' file to dump the cache.", filename);
        return;
    }

    stm_set_write_utf(strm, ekUTF8);
    stm_printf(strm, "%s\n", label);
    cacheToStream(cache, strm);
    stm_close(&strm);
}

/*----------------------------------------------------------------------------*/
