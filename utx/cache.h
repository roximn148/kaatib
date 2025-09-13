/*******************************************************************************
 * Copyright (c) 2025. All rights reserved.
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license,
 * visit # http://creativecommons.org/licenses/by/4.0/.
 *
 * Author: roximn <roximn148@gmail.com>
 ******************************************************************************/
#ifndef __UTX_CACHE_H__
#define __UTX_CACHE_H__

/*----------------------------------------------------------------------------*/
#include "utx.hxx"
#include <draw2d/image.h>

/*----------------------------------------------------------------------------*/
typedef struct _utx_glyph_image UtxGlyphImage;
struct _utx_glyph_image {
    uint16_t glyphId;   /* Glyph Index, limited to TTF 16-bit range */
    Image* image;       /* Rendered image of the glyph */
};

DeclSt(UtxGlyphImage);

/* -------------------------------------------------------------------------- */
typedef Image* (*FPtrGlyphRender)(uint16_t gid);

/*----------------------------------------------------------------------------*/
typedef struct _utx_cache UtxCache;
struct _utx_cache {
    uint32_t capacity;

    FPtrGlyphRender render;

    SetSt(UtxGlyphImage)* glyphImages;

    /* Stats -----------------------------------------------------------------*/
    uint32_t requests;          /* Total requests */
    uint32_t hits;              /* Total hits */
                                /* Total misses = requests - hits */
    uint32_t evictions;         /* Total evictions */

    real64_t avgHitTime;        /* Average time spent on a cache hit */
    real64_t avgMissPenalty;    /* Average time spent on a cache miss */

    /* Average Access Time (AAT) = (Hit Time + Miss Penalty) x Cache Hit Rate */
    /* Cache Load Factor = Occupied Cache Lines / Total Cache Capacity */

    /* Hit Rate/Ratio = Number of Hits / Total Number of Requests */
    /* Miss Rate/Ratio = Number of Misses / Total Number of Requests */
    /* Eviction Rate = Number of Evictions / Total Number of Requests */
};

/*----------------------------------------------------------------------------*/
__EXTERN_C

/*----------------------------------------------------------------------------*/
_utx_api UtxCache* cacheCreate(uint32_t capacity, FPtrGlyphRender source);
_utx_api void cacheDestroy(UtxCache **pCache);
_utx_api Image* cacheGet(UtxCache* cache, uint16_t glyphId);

_utx_api real64_t cacheLoadFactor(UtxCache* cache);
_utx_api real64_t cacheHitRate(UtxCache* cache);
_utx_api real64_t cacheMissRate(UtxCache* cache);
_utx_api real64_t cacheEvictionRate(UtxCache* cache);
_utx_api real64_t cacheAverageAccessTime(UtxCache* cache);

/*----------------------------------------------------------------------------*/
__END_C

/*----------------------------------------------------------------------------*/
#endif /* __UTX_CACHE_H__ */
/*----------------------------------------------------------------------------*/
