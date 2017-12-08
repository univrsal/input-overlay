#ifndef UTIL_H
#define UTIL_H
#include <Windows.h>
#ifndef NULL
#define NULL 0
#endif // !NULL

const char* key_to_text(int key_code);
char* append(char* a, char* b);
#endif