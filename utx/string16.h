/*******************************************************************************
 * Copyright (c) 2024. All rights reserved.
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license,
 * visit # http://creativecommons.org/licenses/by/4.0/.
 *
 * Author: roximn <roximn148@gmail.com>
 ******************************************************************************/
#ifndef __STRIN16_H__
#define __STRIN16_H__
/*----------------------------------------------------------------------------*/
#include "utx.def"
#include <core/core.hxx>

/*----------------------------------------------------------------------------*/
__EXTERN_C
/*----------------------------------------------------------------------------*/
typedef struct _wcString_t wcString;
typedef uint16_t char16_t;

/*----------------------------------------------------------------------------*/
_utx_api wcString* wcStringNew(const char_t *str);
_utx_api void wcStringDestroy(wcString **str);

_utx_api uint32_t wcl(const wcString *wcs);
_utx_api char16_t* wc(const wcString *wcs);
_utx_api const char16_t* wcc(const wcString *wcs);

/*----------------------------------------------------------------------------*/
__END_C

/*----------------------------------------------------------------------------*/
#endif /* __STRIN16_H__ */
/*----------------------------------------------------------------------------*/
