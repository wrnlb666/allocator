//MIT License
//
//Copyright (c) 2022 Zhang Yixiang
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.


#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


typedef struct allocator_t
{
    intptr_t*               address;                //address arraylist
    size_t                  size;                   //current size of the arraylist
    size_t                  capacity;               //current capacity of the arraylist
    struct allocator_t*     previous;               //previous element in the linked list
    struct allocator_t*     next;                   //next element in the linked list
}allocator_t;

typedef struct allocator_list_t
{
    allocator_t *head;
    allocator_t *tail;
}allocator_list_t;



//functions
void            alloc_cleanup( void );                                              //this function will be called automatically at exit.
allocator_t*    alloc_new_allocator( void );                                        //init a new allocator. The memory for the allocator is allocated on the heap. 
void            alloc_begin_allocator( allocator_t **allocator );
void*           alloc_malloc( allocator_t* allocator, size_t size );                //just like malloc.
void*           alloc_calloc( allocator_t* allocator, size_t nmemb, size_t size);   //just like calloc.
void*           alloc_realloc( allocator_t* allocator, void* ptr, size_t size );    //just like realloc. But if size is 0 and ptr is not allocated by the allocator, alloc_realloc() will not free that ptr. Instead, the program will exit failure. 
int             alloc_free( allocator_t* allocator, void* ptr );                    //just like free, return 0 on success. But if the ptr is not allocated by the allocator, alloc_free() will not free that ptr. Instead, the function return -1, and exit failure. 
int             alloc_free_allocator( allocator_t* allocator );                     //free everything inside the target allocator, return 0 on success. But if the allocator does not exist or it is manually changed, the function return -1, and exit failure. 


#define concat( name )      name ## __LINE__
#define scope( start, end )                                     \
    for ( int concat(scope) = ( start, 0 ); !concat(scope); concat(scope)++, end )
#define alloc_scope( /* allocator_t* */ allocator )             \
    scope( alloc_begin_allocator( &allocator ), alloc_free_allocator( allocator ) )


#endif      // __ALLOCATOR_H__
