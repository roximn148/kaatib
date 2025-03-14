/*******************************************************************************
 * Copyright (c) 2024. All rights reserved.
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license,
 * visit # http://creativecommons.org/licenses/by/4.0/.
 *
 * Author: roximn <roximn148@gmail.com>
 ******************************************************************************/
#include <core/strings.h>

#include "utils.h"

/** ----------------------------------------------------------------------------
 *
 * -------------------------------------------------------------------------- */
String* number2ArabicNumeral(uint32_t n) {
    String *romanDigits = str_printf("%d", n);
    uint32_t digitCount = str_len(romanDigits);

    String *arabicDigits = str_fill(digitCount*2, '\x20');

    const char_t *si = tc(romanDigits);
    char_t *so = tcc(arabicDigits);
    for(uint32_t i = 0; i < digitCount; i++) {
        if (si[i] >= '0' && si[i] <= '9') {
            /* Convert ASCII digit to Unicode Arabic numeral */
            so[0] = '\xd9';
            so[1] = '\xa0' + (si[i] - '0');
        }
        so += 2;
    }
    str_destroy(&romanDigits);
    return arabicDigits;
}

/** ----------------------------------------------------------------------------
 *
 * -------------------------------------------------------------------------- */
String* number2IndicNumeral(uint32_t n) {
    String *romanDigits = str_printf("%d", n);
    uint32_t digitCount = str_len(romanDigits);
    String *indicDigits = str_fill(digitCount*2, '\x20');

    const char_t *si = tc(romanDigits);
    char_t *so = tcc(indicDigits);
    /* Convert each digit to its Eastern Arabic Indic numeral equivalent. */
    for(uint32_t i = 0; i < digitCount; i++) {
        if (si[i] >= '0' && si[i] <= '9') {
            /* Convert ASCII digit to Unicode Arabic numeral */
            so[0] = '\xdb';
            so[1] = '\xb0' + (si[i] - '0');
        }
        so += 2;
    }
    str_destroy(&romanDigits);
    return indicDigits;
}

/*----------------------------------------------------------------------------*/
