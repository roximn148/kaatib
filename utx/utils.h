/*******************************************************************************
 * Copyright (c) 2024. All rights reserved.
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license,
 * visit # http://creativecommons.org/licenses/by/4.0/.
 *
 * Author: roximn <roximn148@gmail.com>
 ******************************************************************************/
#ifndef __UTX_UTILS_H__
#define __UTX_UTILS_H__
/*----------------------------------------------------------------------------*/
#include "utx.def"
#include <core/core.hxx>

/*----------------------------------------------------------------------------*/
__EXTERN_C
/*----------------------------------------------------------------------------*/
_utx_api String* number2IndicNumeral(uint32_t n);
_utx_api String* number2ArabicNumeral(uint32_t n);

/*----------------------------------------------------------------------------*/
__END_C

/*----------------------------------------------------------------------------*/
#endif /* __UTX_UTILS_H__ */
/*----------------------------------------------------------------------------*/
