//
// Created by Zhihan Guo on 4/12/20.
//

#ifndef SUNDIAL_HELPER_H
#define SUNDIAL_HELPER_H

#include <typeinfo>
#include <mm_malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

//////////////////////////////////////////////////
// type helper
//////////////////////////////////////////////////
//typedef uint32_t UInt32;
//typedef int32_t SInt32;
//typedef uint64_t UInt64;
//typedef int64_t SInt64;

//////////////////////////////////////////////////
// Malloc helper
//////////////////////////////////////////////////
#define MALLOC(size) malloc(size)
#define NEW(name, type, ...) \
    { name = (type *) MALLOC(sizeof(type)); \
      new(name) type(__VA_ARGS__); }

#define FREE(block, size) free(block)
#define DELETE(type, block) { delete block; }
#define ATOM_FETCH_ADD(dest, value) \
    __sync_fetch_and_add(&(dest), value)
#endif //SUNDIAL_HELPER_H
