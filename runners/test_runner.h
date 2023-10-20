/*
 * Runner for littlefs tests
 *
 * Copyright (c) 2022, The littlefs authors.
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H


// override LFS_TRACE
void test_trace(const char *fmt, ...);

#define LFS_TRACE_(fmt, ...) \
    test_trace("%s:%d:trace: " fmt "%s\n", \
        __FILE__, \
        __LINE__, \
        __VA_ARGS__)
#define LFS_TRACE(...) LFS_TRACE_(__VA_ARGS__, "")
#define LFS_EMUBD_TRACE(...) LFS_TRACE_(__VA_ARGS__, "")


// note these are indirectly included in any generated files
#include "bd/lfs_emubd.h"
#include <stdio.h>

// give source a chance to define feature macros
#undef _FEATURES_H
#undef _STDIO_H


// generated test configurations
struct lfs_config;

enum test_flags {
    TEST_INTERNAL  = 0x1,
    TEST_REENTRANT = 0x2,
};
typedef uint8_t test_flags_t;

typedef struct test_define {
    intmax_t (*cb)(void *data, size_t i);
    void *data;
    size_t permutations;
} test_define_t;

struct test_case {
    const char *name;
    const char *path;
    test_flags_t flags;

    const test_define_t *defines;
    size_t permutations;

    bool (*if_)(void);
    void (*run)(struct lfs_config *cfg);
};

struct test_suite {
    const char *name;
    const char *path;
    test_flags_t flags;

    const char *const *define_names;
    size_t define_count;

    const struct test_case *cases;
    size_t case_count;
};

extern const struct test_suite *const test_suites[];
extern const size_t test_suite_count;


// this variable tracks the number of powerlosses triggered during the
// current test permutation, this is useful for both tests and debugging
extern size_t test_pls;

#define TEST_PLS test_pls

// deterministic prng for pseudo-randomness in testes
uint32_t test_prng(uint32_t *state);

#define TEST_PRNG(state) test_prng(state)

// generation of specific permutations of an array for exhaustive testing
size_t test_factorial(size_t x);
void test_permutation(size_t i, uint32_t *buffer, size_t size);

#define TEST_FACTORIAL(x) test_factorial(x)
#define TEST_PERMUTATION(i, buffer, size) test_permutation(i, buffer, size)


// access generated test defines
intmax_t test_define(size_t define);

#define TEST_DEFINE(i) test_define(i)

// a few preconfigured defines that control how tests run

#define TEST_IMPLICIT_DEFINE_COUNT 15
#define TEST_GEOMETRY_DEFINE_COUNT 3

#define READ_SIZE_i          0
#define PROG_SIZE_i          1
#define BLOCK_SIZE_i         2
#define BLOCK_COUNT_i        3
#define DISK_SIZE_i          4
#define CACHE_SIZE_i         5
#define INLINE_SIZE_i        6
#define FRAGMENT_SIZE_i      7
#define CRYSTALLIZE_SIZE_i   8
#define LOOKAHEAD_SIZE_i     9
#define BLOCK_CYCLES_i       10
#define ERASE_VALUE_i        11
#define ERASE_CYCLES_i       12
#define BADBLOCK_BEHAVIOR_i  13
#define POWERLOSS_BEHAVIOR_i 14

#define READ_SIZE           TEST_DEFINE(READ_SIZE_i)
#define PROG_SIZE           TEST_DEFINE(PROG_SIZE_i)
#define BLOCK_SIZE          TEST_DEFINE(BLOCK_SIZE_i)
#define BLOCK_COUNT         TEST_DEFINE(BLOCK_COUNT_i)
#define DISK_SIZE           TEST_DEFINE(DISK_SIZE_i)
#define CACHE_SIZE          TEST_DEFINE(CACHE_SIZE_i)
#define INLINE_SIZE         TEST_DEFINE(INLINE_SIZE_i)
#define FRAGMENT_SIZE       TEST_DEFINE(FRAGMENT_SIZE_i)
#define CRYSTALLIZE_SIZE    TEST_DEFINE(CRYSTALLIZE_SIZE_i)
#define LOOKAHEAD_SIZE      TEST_DEFINE(LOOKAHEAD_SIZE_i)
#define BLOCK_CYCLES        TEST_DEFINE(BLOCK_CYCLES_i)
#define ERASE_VALUE         TEST_DEFINE(ERASE_VALUE_i)
#define ERASE_CYCLES        TEST_DEFINE(ERASE_CYCLES_i)
#define BADBLOCK_BEHAVIOR   TEST_DEFINE(BADBLOCK_BEHAVIOR_i)
#define POWERLOSS_BEHAVIOR  TEST_DEFINE(POWERLOSS_BEHAVIOR_i)

#define TEST_IMPLICIT_DEFINES \
    /*       name                value (overridable)                       */ \
    TEST_DEF(READ_SIZE,          PROG_SIZE                                  ) \
    TEST_DEF(PROG_SIZE,          BLOCK_SIZE                                 ) \
    TEST_DEF(BLOCK_SIZE,         0                                          ) \
    TEST_DEF(BLOCK_COUNT,        DISK_SIZE/BLOCK_SIZE                       ) \
    TEST_DEF(DISK_SIZE,          1024*1024                                  ) \
    TEST_DEF(CACHE_SIZE,         lfs_max(16, lfs_max(READ_SIZE, PROG_SIZE)) ) \
    TEST_DEF(INLINE_SIZE,        BLOCK_SIZE/8                               ) \
    TEST_DEF(FRAGMENT_SIZE,      CACHE_SIZE                                 ) \
    TEST_DEF(CRYSTALLIZE_SIZE,   BLOCK_SIZE/4                               ) \
    TEST_DEF(LOOKAHEAD_SIZE,     16                                         ) \
    TEST_DEF(BLOCK_CYCLES,       -1                                         ) \
    TEST_DEF(ERASE_VALUE,        0xff                                       ) \
    TEST_DEF(ERASE_CYCLES,       0                                          ) \
    TEST_DEF(BADBLOCK_BEHAVIOR,  LFS_EMUBD_BADBLOCK_PROGERROR               ) \
    TEST_DEF(POWERLOSS_BEHAVIOR, LFS_EMUBD_POWERLOSS_NOOP                   )

#define TEST_GEOMETRIES \
    /*        name      read_size  prog_size  block_size   */ \
    TEST_GEO("default", 16,        16,        512           ) \
    TEST_GEO("eeprom",  1,         1,         512           ) \
    TEST_GEO("emmc",    512,       512,       512           ) \
    TEST_GEO("nor",     1,         1,         4096          ) \
    TEST_GEO("nand",    4096,      4096,      32768         )

#define TEST_CFG \
    .read_size          = READ_SIZE,        \
    .prog_size          = PROG_SIZE,        \
    .block_size         = BLOCK_SIZE,       \
    .block_count        = BLOCK_COUNT,      \
    .block_cycles       = BLOCK_CYCLES,     \
    .cache_size         = CACHE_SIZE,       \
    .inline_size        = INLINE_SIZE,      \
    .fragment_size      = FRAGMENT_SIZE,    \
    .crystallize_size   = CRYSTALLIZE_SIZE, \
    .lookahead_size     = LOOKAHEAD_SIZE,

#define TEST_BDCFG \
    .erase_value        = ERASE_VALUE,          \
    .erase_cycles       = ERASE_CYCLES,         \
    .badblock_behavior  = BADBLOCK_BEHAVIOR,


#endif
