# allocator

```allocator_t``` is the type for an allocator object, an allocator can be used for a group of pointers that have the same lifetime. (E.g. all memory allocation in a frame of animation)

call ```alloc_new_allocator()``` to initialize a new allocator, call ```alloc_malloc(allocator_t, size_t)```, ```alloc_calloc(allocator_t, size_t, size_t)```, ```alloc_realloc(allocator_t, void*, size_t)``` to allocate or reallocate memory for a point managed by the allocator.

call ```alloc_free(allocator_t, void*)``` to free a pointer managed by the allocator.

call ```alloc_free_allocator(allocator_t)``` to free everything managed by the allocator. 
