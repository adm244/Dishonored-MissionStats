/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _TYPES_H
#define _TYPES_H

#define internal static

#ifndef arraysize
#define arraysize(a) (sizeof(a)/sizeof(a[0]))
#endif

#ifndef offsetof
#define offsetof(s, m) ((size_t)&(((s *)0)->m))
#endif

#ifndef CDECL
#define CDECL __cdecl
#endif
#define STDCALL __stdcall
#define THISCALL __thiscall
#define NAKED __declspec(naked)
#define NOINLINE __declspec(noinline)
#define INLINE __inline

#define assert_size(obj, size) static_assert(sizeof(obj) == size, "Size of " #obj " should be " #size)

typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef unsigned int uint;

typedef float r32;
typedef double r64;

typedef i8 b8;
typedef i16 b16;
typedef i32 b32;

#endif
