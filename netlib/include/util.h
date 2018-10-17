/**
 * This file is part of netlib
 * which is licensed under the zlib license
 *
 * github.com/univrsal/netlib
 * netlib is a small network library
 * heavily based on SDL_net without
 * the dependency on SDL
 *
 * Documentation is directly taken from SDL_net
 * I take no credit for this code, I only
 * reformatted it for my needs and made slight changes
 */

#ifndef NET_LIB_UTIL_H
#define NET_LIB_UTIL_H

#ifdef _WIN32
#define DECLSPEC __declspec(dllexport)
#define NETLIB_CALL __cdecl
#define FORCE_INLINE __forceinline
#elif defined UNIX
#define DECLSPEC
#define NETLIB_CALL
#define FORCE_INLINE __attribute__((always_inline)) static __inline__
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#endif
#endif

#endif