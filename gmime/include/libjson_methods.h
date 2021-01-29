#include "4DPluginAPI.h"
#include "libjson.h"
#include "C_TEXT.h"

#if defined(__APPLE__)
#define DATE_FORMAT_ISO @"yyyy-MM-dd'T'HH:mm:ss"
#define DATE_FORMAT_ISO_GMT @"yyyy-MM-dd'T'HH:mm:ss'Z'"
#endif

void json_push_back_s(JSONNODE *n, const char *value);
void json_set_s(JSONNODE *n, const char *value);
void json_set_s_for_key(JSONNODE *n, json_char *key, const char *value);
void json_set_b_for_key(JSONNODE *n, json_char *key, json_bool_t value);
void json_set_i_for_key(JSONNODE *n, json_char *key, json_int_t value);


/* utf32 to utf16 */
void json_stringify(JSONNODE *json, CUTF16String &t, BOOL pretty);

/* added for gmime */
JSONNODE *json_parse(CUTF16String &t);
