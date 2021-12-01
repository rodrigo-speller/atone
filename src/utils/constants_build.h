// Copyright (c) Rodrigo Speller. All rights reserved.
// Licensed under the MIT License. See LICENSE.txt in the project root for license information.

#ifndef ATONE_BUILD_VERSION
#define ATONE_BUILD_VERSION "0.0"
#endif

#ifndef ATONE_BUILD_NUMBER
#define ATONE_BUILD_NUMBER "development"
#endif

#if defined(__x86_64__) || defined(_M_X64)
#define ATONE_BUILD_ARCH "x86_64"
#elif defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86)
#define ATONE_BUILD_ARCH "x86_32"
#elif defined(__ARM_ARCH_2__)
#define ATONE_BUILD_ARCH "ARM2"
#elif defined(__ARM_ARCH_3__) || defined(__ARM_ARCH_3M__)
#define ATONE_BUILD_ARCH "ARM3"
#elif defined(__ARM_ARCH_4T__) || defined(__TARGET_ARM_4T)
#define ATONE_BUILD_ARCH "ARM4T"
#elif defined(__ARM_ARCH_5_) || defined(__ARM_ARCH_5E_)
#define ATONE_BUILD_ARCH "ARM5"
#elif defined(__ARM_ARCH_6T2_) || defined(__ARM_ARCH_6T2_)
#define ATONE_BUILD_ARCH "ARM6T2"
#elif defined(__ARM_ARCH_6__) || defined(__ARM_ARCH_6J__) || defined(__ARM_ARCH_6K__) || defined(__ARM_ARCH_6Z__) || defined(__ARM_ARCH_6ZK__)
#define ATONE_BUILD_ARCH "ARM6"
#elif defined(__ARM_ARCH_7__) || defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
#define ATONE_BUILD_ARCH "ARM7"
#elif defined(__ARM_ARCH_7A__) || defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
#define ATONE_BUILD_ARCH "ARM7A"
#elif defined(__ARM_ARCH_7R__) || defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7S__)
#define ATONE_BUILD_ARCH "ARM7R"
#elif defined(__ARM_ARCH_7M__)
#define ATONE_BUILD_ARCH "ARM7M"
#elif defined(__ARM_ARCH_7S__)
#define ATONE_BUILD_ARCH "ARM7S"
#elif defined(__aarch64__) || defined(_M_ARM64)
#define ATONE_BUILD_ARCH "ARM64"
#elif defined(mips) || defined(__mips__) || defined(__mips)
#define ATONE_BUILD_ARCH "MIPS"
#elif defined(__sh__)
#define ATONE_BUILD_ARCH "SUPERH"
#elif defined(__powerpc) || defined(__powerpc__) || defined(__powerpc64__) || defined(__POWERPC__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
#define ATONE_BUILD_ARCH "POWERPC"
#elif defined(__PPC64__) || defined(__ppc64__) || defined(_ARCH_PPC64)
#define ATONE_BUILD_ARCH "POWERPC64"
#elif defined(__sparc__) || defined(__sparc)
#define ATONE_BUILD_ARCH "SPARC"
#elif defined(__m68k__)
#define ATONE_BUILD_ARCH "M68K"
#else
#define ATONE_BUILD_ARCH "UNKNOWN"
#endif

#if defined(__linux__)
#define ATONE_BUILD_OS "linux"
#elif defined(__unix__)
#define ATONE_BUILD_OS "unix"
#elif defined(_POSIX_VERSION)
#define ATONE_BUILD_OS "posix"
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define ATONE_BUILD_OS "windows"
#else
#define ATONE_BUILD_OS "UNKNOWN"
#endif

#ifndef ATONE_BUILD_VERSION_STAMP
#define ATONE_BUILD_VERSION_STAMP (ATONE_BUILD_VERSION " build " ATONE_BUILD_NUMBER " " ATONE_BUILD_ARCH "-" ATONE_BUILD_OS)
#endif
