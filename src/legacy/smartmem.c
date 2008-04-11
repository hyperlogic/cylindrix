/*
    Copyright (C) 2001 Hotwarez LLC, Goldtree Enterprises
  
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; 
    version 2 of the License.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
  
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "smartmem.h"
#include "types.h"


typedef long Align;  /* for alignment to long boundary */

union smart_heap_header {                /* block header: */
    struct {
        union smart_heap_header *next;   /* next block if on free list */
        size_t size;                     /* size of this block in bytes */
    } s;
    Align x;                             /* force alignment on 32-bit word boundries */
};

typedef union smart_heap_header SmartHeapHeader;



/* global pointer to the smart_heap */

SmartHeapHeader *smart_heap = NULL;

/* total size of the smart_heap */

size_t smart_heap_size = 0;

/* global pointer to the first free block */

SmartHeapHeader *free_list = NULL;



int allocate_smart_heap( size_t size )
{    
    /* if size isn't a multiple of sizeof(SmartHeapHeader) */
    
    if( size % sizeof(SmartHeapHeader) != 0 ) {
    
        /* make the heap_size a little bigger so that is is a multiple of
           sizeof(SmartHeapHeader) */
           
        smart_heap_size = size + (size % sizeof(SmartHeapHeader));
    }
    else {
        smart_heap_size = size;
    }
    
    /* allocate the smart_heap */
    
    if( (smart_heap = (SmartHeapHeader *)malloc( smart_heap_size )) == NULL ) {
        return FALSE;
    }
    
    /* initialize the smart_heap header */
    
    smart_heap->s.next = NULL;
    smart_heap->s.size = smart_heap_size - sizeof(SmartHeapHeader);
    
    /* initialize the free_list */
    
    free_list = smart_heap;
    
    return TRUE;
}



void erase_smart_heap()
{
    /* erase the entire smart_heap */
    
    memset( (char *)smart_heap, smart_heap_size, 0 );
    
    /* initialize the smart_heap header */
    
    smart_heap->s.next = NULL;
    smart_heap->s.size = smart_heap_size - sizeof(SmartHeapHeader);
    
    /* initialize the free_list */
    
    free_list = smart_heap;
}

void free_smart_heap()
{
    free( smart_heap );
    smart_heap = NULL;
    free_list = NULL;
    smart_heap_size = 0;
}



void *smart_malloc( size_t size )
{
    SmartHeapHeader* best_fit = NULL;
    SmartHeapHeader* current = NULL;
    SmartHeapHeader* previous = NULL;
    SmartHeapHeader* old_next = NULL;
    SmartHeapHeader* new_free_node = NULL;
    size_t best_fit_size = smart_heap_size;    
    size_t old_size = 0;
    
    /* return NULL if there aren't any free blocks */
    
    if( free_list == NULL ) {
        return NULL;
    }
    
    /* search the free_list for the best fit */        

    current = free_list;
    
    if( current->s.size >= size ) {
        best_fit = current;
        best_fit_size = current->s.size;
    }
    previous = current;
    current = current->s.next;
    
    while( current != NULL ) {
        if( (current->s.size >= size) && (current->s.size < best_fit_size) ) {
            best_fit = current;
            best_fit_size = current->s.size;
        }
        previous = current;
        current = current->s.next;
    }
    
    /*
    printf("best_fit = %ld\n", (long)best_fit );
    printf("best_fit->s.size = %ld\n", (long)best_fit->s.size );
    printf("best_fit->s.next = %ld\n", (long)best_fit->s.next );
    */
    
    /* if there isn't a best_fit then barf */
    
    if( best_fit == NULL ) {
        return NULL;
    }    
    
    /* insert the new block into the heap and update the free_list */    
    
    /* if there's still room at the end of the best_fit block */
    
    if( size <= (best_fit->s.size + sizeof(SmartHeapHeader)) ) {
    
        old_size = best_fit->s.size;
        old_next = best_fit->s.next;
        
        /*
        printf("old_size = %ld\n", (long)old_size );
        printf("old_next = %ld\n", (long)old_next );
        */
        
        best_fit->s.size = size + (size % sizeof(SmartHeapHeader) );
        best_fit->s.next = best_fit;
        
        new_free_node = best_fit + 1 + ((size + (size % sizeof(SmartHeapHeader))) / sizeof(SmartHeapHeader) );
        new_free_node->s.size = old_size - size - (size % sizeof(SmartHeapHeader)) - sizeof(SmartHeapHeader);
        new_free_node->s.next = old_next;
        
        /*
        printf("new_free_node = %ld\n", (long)new_free_node );
        printf("new_free_node->s.size = %ld\n", (long)new_free_node->s.size );
        printf("new_free_node->s.next = %ld\n", (long)new_free_node->s.next );        
        */
        
        if( best_fit != free_list ) {
            previous->s.next = new_free_node;
        }
        else {
            free_list = new_free_node;
        }
        
    }
    else {
    
        old_size = best_fit->s.size;
        old_next = best_fit->s.next;
        
        best_fit->s.size = size + (size % sizeof(SmartHeapHeader));
        best_fit->s.next = best_fit;
        
        if( best_fit != free_list ) {
            previous->s.next = old_next;
        }
        else {
            free_list = new_free_node;
        }

    }
    
    /* return the user the pointer */
    
    return (void *)(best_fit + 1);
}




void smart_free( void *ptr )
{
    SmartHeapHeader* temp;
    SmartHeapHeader* previous;
    SmartHeapHeader* current;
    SmartHeapHeader* new_ptr;
    
    new_ptr = ((SmartHeapHeader *)ptr) - 1;
    
    if( new_ptr < free_list ) {
    
        temp = free_list;
        free_list = new_ptr;
        free_list->s.next = temp;
        
    }
    else {
    
        previous = free_list;
        current = free_list->s.next;
        
        while( current < new_ptr ) {
            previous = current;
            current = current->s.next;
        }
        previous->s.next = new_ptr;
        new_ptr->s.next = current;
        
    }        
}

void print_smart_heap()
{
    SmartHeapHeader* current;
    
    current = free_list;
    
    while( current != NULL ) {
        printf("-------------------------\n");
        printf("address = %ld\n", (long)current );
        printf("size = %ld\n", (long)current->s.size );
        printf("next = %ld\n", (long)current->s.next );
        current = current->s.next;
    }
    
}

