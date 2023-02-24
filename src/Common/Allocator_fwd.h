#pragma once
/**
  * This file provides forward declarations for Allocator.
  */

template <bool clear_memory_, bool mmap_populate = false>
class Allocator;

template <typename Base, unsigned long N = 64, unsigned long Alignment = 1>
class AllocatorWithStackMemory;
