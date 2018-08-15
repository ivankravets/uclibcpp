#pragma once

#ifndef __cplusplus
#  define __cplusplus 199711L
#endif

#if __cplusplus >= 199711L
#  ifndef __cpp_std_98
#    define __cpp_std_98
#  endif
#else
#  ifdef __cpp_std_98
#    undef __cpp_std_98
#  endif
#endif

#if __cplusplus >= 201103L
#  ifndef __cpp_std_11
#    define __cpp_std_11
#  endif
#else
#  ifdef __cpp_std_11
#    undef __cpp_std_11
#  endif
#endif

#if __cplusplus >= 201402L
#  ifndef __cpp_std_14
#    define __cpp_std_14
#  endif
#else
#  ifdef __cpp_std_14
#    undef __cpp_std_14
#  endif
#endif

#if __cplusplus >= 201703L
#  ifndef __cpp_std_17
#    define __cpp_std_17
#  endif
#else
#  ifdef __cpp_std_17
#    undef __cpp_std_17
#  endif
#endif
