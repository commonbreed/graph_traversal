// Test the variable-length UTF-8 handling

#include <string.h>
#include "litmus_unicode.h"

static inline int test_unicode_multibyte() {
	return strlen(u8"\u2500") > 1;
}

static inline int test_unicode_single() {
	return strlen(u8"\u001A") > 1;
}

int test_unicode() {
	return test_unicode_multibyte() && test_unicode_single();
}
