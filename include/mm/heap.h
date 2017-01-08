#ifndef heap_h
#define heap_h

#include <stdint.h>
#include <stddef.h>

#include <mm/heapBlock.h>

void * heap_alloc (size_t bytes);
size_t heap_free (void * ptr);
void heap_init ();

#endif
