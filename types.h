#ifndef __MINX86TYPES_H
#define __MINX86TYPES_H

/* what this header is supposed to provide:
 *
 *    {int,uint}{8,16,32,64}_t   stdint.h style definitions
 *    minx86_optimal_t           optimal access size typedef
 *    minx86_optimal_size        #define of the size
 *    minx86_read_ptr_t          typedef for reading from RAM. Most cases this is uint8_t*, for DOS/Win16 it is uint8_t far*
 *
 *    ENABLE_64BIT               #define set if compiler supports 64-bit variables (if not set we have to fake it or disable functionality)
 *
 *    WARNING: This code supports using FAR pointers to read off data, BUT properly handling far pointer normalization is entirely up to you.
 *             If you feed this code a pointer who's offset rolls over and incorrectly increments the segment by 1, that's YOUR problem.
 */














/* typedefs to native data types and fixed data types.
 * this allows the library to optimize itself for fast decoding. */
#if defined(__GNUC__)
/* GCC is our compiler. usually that means we can use GLIBC-style headers */
#  include <stdint.h> /* like this one (standard int types like uint32_t) */
#  include <endian.h> /* and this one (what endian/byte order we are) */

#  define ENABLE_64BIT

/* the uint32_t types, etc. we use in this program anyhow, no need to define them */

/* endian.h provides LITTLE_ENDIAN and BIG_ENDIAN */

/* GCC does not in any way support FAR pointers */
typedef uint8_t *minx86_read_ptr_t;

/* other data types we use: mainly for optimized access */
#  if defined(__amd64__)
/* x86-64 host: 64-bit is optimal */
     typedef uint64_t	minx86_optimal_t;
#    define minx86_optimal_size 64
#  elif defined(__i386__)
/* typical x86 32-bit optimal */
     typedef uint32_t	minx86_optimal_t;
#    define minx86_optimal_size 32
#  else
/* uh, well, we don't know. assume 8-bit */
     typedef uint8_t	minx86_optimal_t;
#    define minx86_optimal_size 8
#  endif
















#elif defined(_MSC_VER)
/* Microsoft C++
 * Microsoft does not have a stdint.h or endian.h
 * Fortunately due to the heterogenous DOS/Win environment we can assume little endian.
 * The question remains: DOS? Windows? 16-bit? 32-bit?
 * TODO: Does MSVC++ have 32-bit DOS? */

typedef unsigned char uint8_t;
typedef signed char int8_t;

#  if defined(_WIN64)
   /* 64-bit Windows */
#    define _MSC_SIZE 64
#    define ENABLE_64BIT
#    include <windows.h>
     typedef uint8_t *minx86_read_ptr_t; /* no FAR pointers */
#  elif defined(_WIN32)
   /* 32-bit Windows */
#    define _MSC_SIZE 32
#    define ENABLE_64BIT
#    include <windows.h>
     typedef uint8_t *minx86_read_ptr_t; /* no FAR pointers */
#  elif defined(_WINDOWS)
   /* then it must be 16-bit windows */
#    define _MSC_SIZE 16
     typedef uint8_t far *minx86_read_ptr_t; /* 16:16 FAR pointers */
#  elif defined(_DOS)
   /* ooh goody, 16-bit DOS! */
#    define _MSC_SIZE 16
     typedef uint8_t far *minx86_read_ptr_t; /* 16:16 FAR pointers */
#  else
#    error Microsoft C++ detected but not target platform
#  endif

/* always Little Endian */
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __BYTE_ORDER __LITTLE_ENDIAN

#  define minx86_optimal_size _MSC_SIZE

#  if _MSC_SIZE == 64
     typedef uint64_t		minx86_optimal_t;
     typedef unsigned short	uint16_t;
     typedef signed short	int16_t;
     typedef unsigned int	uint32_t; /* <-- MSVC 64-bit follows C++ rules where sizeof(int) is still 32-bit right? */
     typedef signed int		int32_t;
     typedef unsigned long	uint64_t;
     typedef signed long	int64_t;
#  elif _MSC_SIZE == 32
     typedef uint32_t		minx86_optimal_t;
     typedef unsigned short	uint16_t;
     typedef signed short	int16_t;
     typedef unsigned int	uint32_t;
     typedef signed int		int32_t;
     typedef unsigned __int64	uint64_t;
     typedef signed __int64	int64_t;
#  elif _MSC_SIZE == 16
     typedef uint16_t		minx86_optimal_t;
     typedef unsigned int	uint16_t;
     typedef signed int		int16_t;
     typedef unsigned long	uint32_t;
     typedef signed long	int32_t;
     /* no 64-bit */
#  endif














#elif defined(__WATCOMC__)
/* Watcom C/OpenWatcom
 * Hey, Watcom DOES have stdint.h! Nice!
 * No endian.h though. No problem, Watcom's targets (DOS and Windows) are little endian */
#  include <stdint.h>

/* FIXME: OpenWatcom DOES have a "long long" after all, even in 16-bit real mode? */
#  define ENABLE_64BIT

#  if defined(__386__)
#    define _WC_SIZE 32
     typedef uint8_t far *minx86_read_ptr_t; /* 16:32 FAR pointers */
#  else
#    define _WC_SIZE 16
     typedef uint8_t far *minx86_read_ptr_t; /* 16:16 FAR pointers */
#  endif

/* always Little Endian */
#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __BYTE_ORDER __LITTLE_ENDIAN

#  define minx86_optimal_size _WC_SIZE

#  if _WC_SIZE == 32
     typedef uint32_t		minx86_optimal_t;
#  elif _WC_SIZE == 16
     typedef uint16_t		minx86_optimal_t;
#  endif













#else
#  error I dont know this environment and compiler
#endif
















/* if the types were not defined for any reason, this will make the compiler choke */
static inline minx86_read_ptr_t minx86_read_ptr_t_test(minx86_read_ptr_t x) { return x; }
static inline uint8_t minx86_uint8_t_test(uint8_t x) { return x; }
static inline uint16_t minx86_uint16_t_test(uint16_t x) { return x; }
static inline uint32_t minx86_uint32_t_test(uint32_t x) { return x; }
static inline uint64_t minx86_uint64_t_test(uint64_t x) { return x; }
static inline minx86_optimal_t minx86_optimal_t_test(minx86_optimal_t x) { return x; }


#endif /* __MINX86TYPES_H */

