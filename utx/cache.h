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

#define EOL UINT16_MAX
/*----------------------------------------------------------------------------*/
typedef struct _utx_glyph_image UtxGlyphImage;
struct _utx_glyph_image {
    uint16_t glyphId;   /* Glyph Index, limited to TTF 16-bit range */

    /* for recent usage ordering (UINT16_MAX = EOL) */
    uint16_t nextGlyphId;
    uint16_t prevGlyphId;

    Image* image;       /* Rendered image of the glyph */
};

DeclSt(UtxGlyphImage);

/* -------------------------------------------------------------------------- */
typedef Image* (*FPtrGlyphRender)(uint16_t gid, void *context);

typedef struct _utx_render_closure RenderClosure;
struct _utx_render_closure {
    void *context;
    FPtrGlyphRender render;
};

/*----------------------------------------------------------------------------*/
typedef struct _utx_cache UtxCache;
struct _utx_cache {
    uint32_t capacity;

    RenderClosure *closure;

    SetSt(UtxGlyphImage)* glyphImages;
    uint16_t headId;
    uint16_t tailId;

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
_utx_api UtxCache* cacheCreate(uint32_t capacity, RenderClosure *source);
_utx_api void cacheDestroy(UtxCache **pCache);
_utx_api Image* cacheGet(UtxCache* cache, uint16_t glyphId);

_utx_api real64_t cacheLoadFactor(UtxCache* cache);
_utx_api real64_t cacheHitRate(UtxCache* cache);
_utx_api real64_t cacheMissRate(UtxCache* cache);
_utx_api real64_t cacheEvictionRate(UtxCache* cache);
_utx_api real64_t cacheAverageAccessTime(UtxCache* cache);

_utx_api void cacheDump(UtxCache *cache, const char_t *filename, const char_t *label);
_utx_api void cacheToStream(UtxCache *cache, Stream *strm);

/*----------------------------------------------------------------------------*/
__END_C

/*----------------------------------------------------------------------------*/
#endif /* __UTX_CACHE_H__ */
/*----------------------------------------------------------------------------*/
