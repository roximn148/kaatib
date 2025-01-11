/*******************************************************************************
 * Copyright (c) 2024. All rights reserved.
 *
 * This work is licensed under the Creative Commons Attribution 4.0
 * International License. To view a copy of this license,
 * visit # http://creativecommons.org/licenses/by/4.0/.
 *
 * Author: roximn <roximn148@gmail.com>
 ******************************************************************************/
#include <sewer/unicode.h>
#include <sewer/cassert.h>
#include <core/heap.h>

#include "string16.h"


/** ----------------------------------------------------------------------------
 * @brief Dynamically allocated UTF16 encoded Pascal-style Unicode string.
 *
 * `wcString` is a 16-bit unsigned char Pascal-style string representing a sequence of
 * Unicode Basic Multilingual Plane (BMP) only characters. It is dynamically
 * allocated and stores the length at the beginning of the data array. It uses
 * `char16_t` for storing characters and is designed to handle strings up to
 * `2^32 - 1` characters long (excluding the length field). It can represent all
 * Unicode characters within the BMP range (`U+0000` to `U+FFFF`) including the
 * `NULL` character but excluding the surrogate pair ranges of
 * `U+D800` to `U+DBFF` and `U+DC00` to `U+DFFF`. This provides 1-1 correspondence with
 * array index and character codepoints.
 *
 * -------------------------------------------------------------------------- */
struct _wcString_t {
    uint32_t length;  /**< Length of the string/Number of characters */
    char16_t data[1]; /**< `char16_t` array of UTF-16 encoded BMP unicode characters */
};

#define wcBUFFER_SIZE(l) (((l) * sizeof32(char16_t)) + sizeof32(char16_t))
#define wcTOTAL_SIZE(l) (sizeof32(uint32_t) + wcBUFFER_SIZE(l))

#define wcSTRING(wcs) (cast((wcs), wcString))
#define wcLENGTH(wcs) (wcSTRING(wcs)->length)
#define wcDATA(wcs) (wcSTRING(wcs)->data)
#define wcDATA_C(wcs) (cast(wcSTRING(wcs)->data, char_t))
#define wcDATA_B(wcs) (cast(wcSTRING(wcs)->data, byte_t))

/** ----------------------------------------------------------------------------
 * @brief Creates a new `wcString` from provided UTF-8 encoded string.
 *
 * The function allocates memory for the wcString structure including the data array,
 * which holds the UTF-16 format characters. The length of the string is stored
 * at the beginning of the allocated memory block, followed by an array of
 * `char16_t` elements that represent the Unicode characters in the BMP range.
 *
 * This function takes a UTF-8 encoded string as input, checks if all characters
 * are within the Basic Multilingual Plane (BMP), and then converts it to a
 * wchar string (UTF-16). If any character outside BMP is found or memory
 * allocation fails, the function returns `NULL`.
 *
 * The length of the string is immutable and cannot be changed after creation.
 * Any modifications require creating a new string. The contents however can be
 * accessed and modified through the provided functions.
 *
 * @param[in] str The UTF-8 encoded input string. Returns `NULL`, if input is `NULL`.
 *
 * @return `wcString`* A pointer to the newly created wchar string on success,
 *         or `NULL` if an error occurs (e.g., out-of-memory or non-BMP characters).
 *
 * **Example**:
 * ```C
 * const char_t *utf8Str = "Hello, world!";
 * wcString *wcs = wcStringNew(utf8Str);
 * if (wcs != NULL) {
 *     // Use the wide character string
 *     // The caller is responsible for freeing the instance when done.
 *     wcStringDestroy(wcs);
 * } else {
 *     // Handle error
 * }
 * ```
 *
 * @note
 * The ownership of the object is transferred to the caller. The caller must
 * free the allocated instance using `wcStringDestroy`.
 *
 * -------------------------------------------------------------------------- */
wcString* wcStringNew(const char_t *str) {
    if (str == NULL) {
        return NULL;
    }

    /* Validate utf8 encoding and check all code point within BMP range */
    if (!unicode_valid_str(str, ekUTF8)) {
        return NULL;
    }
    const char_t *s = str;
    while (*s) {
        if (unicode_to_u32(s, ekUTF8) > 0xFFFF) {
            return NULL;
        }
        s = unicode_next(s, ekUTF8);
    }

    uint32_t isize = (uint32_t)(s - str); /* Convert pointer difference to number of bytes */
    uint32_t nchars = unicode_nchars(str, ekUTF8); /* Excluding NULL char */

    /* Create object instance */
    uint32_t objSize = wcTOTAL_SIZE(nchars);
    wcString *wcs = cast(
        heap_aligned_calloc(objSize, sizeof32(char16_t), "wcString"),
        wcString
    );
    if (wcs == NULL) {
        return NULL;
    }

    /* Convert UTF8 to UTF16, knowing its limited to BMP */
    uint32_t osize = wcBUFFER_SIZE(nchars);
    uint32_t obytes = unicode_convers_n(
        str, wcDATA_C(wcs),
        ekUTF8, ekUTF16,
        isize, osize
    );
    cassert_msg(obytes == osize, "UTF8-UT16 conversion failed.");

    wcs->data[nchars] = (char16_t)'\0'; /* NULL-terminate the string */
    wcs->length = nchars;

    return wcs;
}

/** ----------------------------------------------------------------------------
 * @brief Returns the length of a `wcString`.
 *
 * This function takes a pointer to a `wcString` instance and returns its length.
 * If the input string is `NULL`, the function returns `0`.
 *
 * @param[in] wcs Pointer to the `wcString` instance.
 * @return Character count of the `wcString`  string, or `0` if the input is `NULL`.
 *
 * **Example**:
 * ```C
 * wcString *myString = wcStringNew("..."); // Initialize your wcString
 * uint32_t length = wcl(myString); // Get the length of the string
 * printf("The length of the string is: %u\n", length);
 * ```
 *
 * -------------------------------------------------------------------------- */
uint32_t wcl(const wcString *wcs) {
    if (wcs == NULL) {
        return 0;
    }
    return wcLENGTH(wcs);
}

/** ----------------------------------------------------------------------------
 * @brief Returns a read/write pointer to the `wcString`'s `char16_t` array.
 *
 * This function returns a pointer to the data array of the `wcString`.
 * If the input string is `NULL`, it returns `NULL`.
 *
 * @param[in] wcs Pointer to the `wcString`.
 * @return A pointer to the `char16_t` array of the string, or `NULL` if the input is `NULL`.
 *
 * **Example**:
 * ```C
 * wcString *myString = wcStringNew("..."); // Initialize your wcString
 * char16_t *data = wc(myString); // Get the pointer to data array.
 * data[0] = 0x0627; // Modify the data
 * printf("The wide character data of the string is: %ls\n", data);
 * ```
 *
 * -------------------------------------------------------------------------- */
char16_t* wc(const wcString *wcs) {
    if (wcs == NULL) {
        return NULL;
    }
    return wcDATA(wcs);
}

/** ----------------------------------------------------------------------------
 * @brief Returns a readonly pointer to the `char16_t` array of the `wcString`.
 *
 * This function returns a pointer to the `const char16_t` array of the `wcString`.
 * If the input string is `NULL`, it returns `NULL`.
 *
 * @param[in] wcs Pointer to the `wcString` object.
 * @return A pointer to the const `char16_t` array of the string, or NULL if the input is NULL.
 *
 * **Example**:
 * ```C
 * const wcString *myString = wcStringNew("..."); // Initialize your wcString
 * const char16_t *data = wcc(myString); // Get the pointer to const data array.
 * printf("The wide character data of the string is: %ls\n", data);
 * ```
 *
 * -------------------------------------------------------------------------- */
const char16_t* wcc(const wcString *wcs) {
    if (wcs == NULL) {
        return NULL;
    }
    return cast(wcDATA(wcs), const char16_t);
}

/** ----------------------------------------------------------------------------
 * @brief Frees the dynamic `wcString` instance and frees its allocated memory.
 *
 * This function destroys a `wcString` instance and frees all the memory
 * associated with it. If the input string is `NULL` or already destroyed, it
 * does nothing.
 *
 * @param[in, out] str Pointer to the pointer of the `wcString`. Nullified on success.
 *
 * **Example**:
 * ```C
 * wcString *myString = wcStringNew("..."); // Initialize your wcString
 * // Use the string...
 * wcStringDestroy(&myString); // Destroy the string and free memory.
 * ```
 *
 * -------------------------------------------------------------------------- */
void wcStringDestroy(wcString **str) {
    cassert_no_null(str);
    cassert_no_null(*str);
    heap_free(dcast(str, byte_t), wcTOTAL_SIZE((*str)->length), "wcString");
    return;
}

/*----------------------------------------------------------------------------*/
