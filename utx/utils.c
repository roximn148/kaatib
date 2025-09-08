/*******************************************************************************
 * Copyright (c) 2025. All rights reserved.
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
 * Convert an integer to its Arabic numeral representation.
 *
 * This function takes unsigned integer (uint32_t) and returns a string
 * containing the Arabic Indic numeral representation of the number. The caller
 * is responsible for freeing up the returned string.
 * @param n Unsigned integer to convert.
 * @return A pointer to a string containing the Arabic numeral representation
 *         of the input number. Returns NULL if an error occurs.
 * -------------------------------------------------------------------------- */
String* number2ArabicNumeral(uint32_t n) {
    String *romanDigits = str_printf("%d", n);
    uint32_t digitCount = str_len(romanDigits);

    /* Create output string filled with space char */
    String *arabicDigits = str_fill(digitCount*2, '\x20');

    const char_t *si = tc(romanDigits);
    char_t *so = tcc(arabicDigits);
    for(uint32_t i = 0; i < digitCount; i++) {
        if (si[i] >= '0' && si[i] <= '9') {
            so[0] = '\xd9';
            so[1] = '\xa0' + (si[i] - '0');
        }
        so += 2;
    }
    str_destroy(&romanDigits);
    return arabicDigits;
}

/** ----------------------------------------------------------------------------
 * Convert an integer to its Indic numeral representation.
 *
 * This function takes unsigned integer (uint32_t) and returns a string
 * containing the Eastern Arabic Indic numeral representation of the number.
 * The caller is responsible for freeing up the returned string.
 * @param n Unsigned integer to convert.
 * @return A pointer to a string containing the Eastern Arabic Indic numeral
 *         representation of the input number. Returns NULL if an error occurs.
 * -------------------------------------------------------------------------- */
String* number2IndicNumeral(uint32_t n) {
    String *romanDigits = str_printf("%d", n);
    uint32_t digitCount = str_len(romanDigits);

    /* Create output string filled with space char */
    String *indicDigits = str_fill(digitCount*2, '\x20');

    const char_t *si = tc(romanDigits);
    char_t *so = tcc(indicDigits);
    for(uint32_t i = 0; i < digitCount; i++) {
        if (si[i] >= '0' && si[i] <= '9') {
            so[0] = '\xdb';
            so[1] = '\xb0' + (si[i] - '0');
        }
        so += 2;
    }
    str_destroy(&romanDigits);
    return indicDigits;
}

/*----------------------------------------------------------------------------*/
