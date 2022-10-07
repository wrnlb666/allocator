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


#include "allocator.h"

static bool             is_init         = false;
static allocator_list_t *allocator_list = NULL;


void alloc_cleanup( void )
{
    allocator_t *temp = { 0 };
    while ( allocator_list->head )
    {
        for ( size_t i = 0; i < allocator_list->head->size; i++ )
        {
            free( ( void* ) allocator_list->head->address[i] );
        }
        free( allocator_list->head->address );
        temp = allocator_list->head;
        allocator_list->head = allocator_list->head->next;
        free( temp );
    }
}


allocator_t* alloc_new_allocator( void )
{
    allocator_t *temp = (allocator_t*) malloc( sizeof (allocator_t) );
    if ( !temp )
    {
        perror( "ERROR: Allocating memory for allocator" );
        abort();
    }
    *temp = (allocator_t) { 0 }; 
    if ( !is_init )
    {
        allocator_list = ( allocator_list_t* ) malloc( sizeof (allocator_list_t) );
        if ( !allocator_list )
        {
            perror( "ERROR: Allocating memory for allocator_list" );
            abort();
        }
        allocator_list->head = allocator_list->tail = temp;
        atexit( alloc_cleanup );
        is_init = true;
    }
    else
    {
        if ( allocator_list->head )
        {
            allocator_list->head->previous  = temp;
            temp->next                      = allocator_list->head;
            allocator_list->head            = temp;
        }
        else
        {
            allocator_list->head = allocator_list->tail = temp;
        }
    }
    return temp;
}


void alloc_begin_allocator( allocator_t **allocator )
{
    *allocator = alloc_new_allocator();
}


void* alloc_malloc( allocator_t* allocator, size_t size )
{
    if ( allocator->capacity && allocator->size >= allocator->capacity )
    {
        allocator->capacity *= 2;
        allocator->address = ( intptr_t* ) realloc( allocator->address, sizeof (intptr_t) * allocator->capacity );
        if ( !allocator->address )
        {
            perror( "ERROR: Reallocating memory for allocator->address" );
            abort();
        }
    }
    else if ( !allocator->capacity )
    {
        allocator->capacity ++;
        allocator->address = ( intptr_t* ) malloc( sizeof (intptr_t) * allocator->capacity );
        if ( !allocator->address )
        {
            perror( "ERROR: Allocating memory for allocator->address" );
            abort();
        }
    }

    void* temp = malloc( size );
    if ( !temp )
    {
        perror( "ERROR: malloc" );
        abort();
    }
    allocator->address[allocator->size++] = (intptr_t) temp;

    return temp;
}



void* alloc_calloc( allocator_t* allocator, size_t nmemb, size_t size )
{
    if ( allocator->capacity && allocator->size >= allocator->capacity )
    {
        allocator->capacity *= 2;
        allocator->address = (intptr_t*) realloc( allocator->address, sizeof (intptr_t) * allocator->capacity );
        if ( !allocator->address )
        {
            perror( "ERROR: Reallocating memory for allocator->address" );
            abort();
        }
    }
    else if ( !allocator->capacity )
    {
        allocator->capacity ++;
        allocator->address = (intptr_t*) malloc( sizeof (intptr_t) * allocator->capacity );
        if ( !allocator->address )
        {
            perror( "ERROR: Allocating memory for allocator->address" );
            abort();
        }
    }

    void* temp = calloc( nmemb, size );
    if ( !temp )
    {
        perror( "ERROR: calloc" );
        abort();
    }
    allocator->address[allocator->size++] = (intptr_t) temp;

    return temp;
}


void* alloc_realloc( allocator_t * allocator, void* ptr, size_t size )
{
    void* temp = NULL;

    for ( size_t i = 0; i < allocator->size; i++ )          //if ptr is already in the allocator
    {
        if ( allocator->address[i] == (intptr_t) ptr )
        {
            temp = realloc( ptr, size );
            if ( size != 0 )
            {
                allocator->address[i] = (intptr_t) temp;
            }
            else
            {
                ptr = NULL;
                allocator->address[i] = allocator->address[--allocator->size];
                if ( allocator->size < allocator->capacity / 4 )
                {
                    allocator->capacity /= 2;
                    allocator->address = (intptr_t*) realloc( allocator->address, sizeof (intptr_t) * allocator->capacity );
                    if ( !allocator->address )
                    {
                        perror( "ERROR: Reallocating memory for allocator->address" );
                        abort();
                    }
                }
                if ( allocator->size == 0 )
                {
                    free( allocator->address );
                    allocator->address = NULL;
                    allocator->capacity = 0;
                }
            }
            return temp;
        }
    }

    if ( size != 0)
    {
        if ( allocator->capacity && allocator->size >= allocator->capacity )    //if ptr is not in the allocator
        {
            allocator->capacity *= 2;
            allocator->address = (intptr_t*) realloc( allocator->address, sizeof (intptr_t) * allocator->capacity);
            if ( !allocator->address )
            {
                perror( "ERROR: Reallocating memory for allocator->address" );
                abort();
            }
        }
        else if( !allocator->capacity )
        {
            allocator->capacity ++;
            allocator->address = (intptr_t*) malloc( sizeof (intptr_t) * allocator->capacity );
            if ( !allocator->address )
            {
                perror( "ERROR: Allocating memory for allocator->address" );
                abort();
            }
        }

        temp = realloc( ptr, size );
        if ( !temp )
        {
            perror( "ERROR: realloc" );
            abort();
        }
        allocator->address[allocator->size++] = (intptr_t) temp;
    }
    else
    {
        fprintf( stderr, "WARNING: Trying to alloc_realloc() a ptr to size 0, \nbut the ptr is not in the allocator\n" );
        EXIT_FAILURE;
    }

    return temp;
}


int alloc_free( allocator_t* allocator, void* ptr )
{
    for ( size_t i = 0; i < allocator->size; i++ )
    {
        if ( allocator->address[i] == (intptr_t) ptr )
        {
            free( ptr );
            allocator->address[i] = allocator->address[--allocator->size];
            if ( allocator->size < allocator->capacity / 4 )
            {
                allocator->capacity /= 2;
                allocator->address = (intptr_t*) realloc( allocator->address, sizeof (intptr_t) * allocator->capacity );
            }
            if ( allocator->size == 0 )
            {
                free( allocator->address );
                allocator->address = NULL;
                allocator->capacity = 0;
            }
            return 0;
        }
    }
    fprintf( stderr, "WARNING: Trying to alloc_free() a ptr, \nbut the ptr is not in the allocator\n" );
    EXIT_FAILURE;
    return -1;
}


int alloc_free_allocator( allocator_t* allocator )
{
    allocator_t *temp = allocator_list->head;
    while ( temp )
    {
        if ( allocator == temp )
        {
            if ( temp->previous && temp->next )
            {
                temp->previous->next = temp->next;
                temp->next->previous = temp->previous;
            }
            else if ( temp->previous )
            {
                temp->previous->next = NULL;
                allocator_list->tail = temp->previous;
            }
            else if ( temp->next )
            {
                temp->next->previous = NULL;
                allocator_list->head = temp->next;
            }
            else
            {
                allocator_list->head = allocator_list->tail = NULL;
            }

            for ( size_t i = 0; i < temp->size; i++ )
            {
                free( (void*) temp->address[i] );
            }
            free( temp->address );
            free( temp );
            temp = NULL;
            return 0;
        }
        else
        {
            temp = temp->next;
        }
    }
    fprintf( stderr, "WARNING: Trying to alloc_free_allocator() an allocator, \nbut the allocator is not in the allocator list\n" );
    EXIT_FAILURE;
    return -1;
}
