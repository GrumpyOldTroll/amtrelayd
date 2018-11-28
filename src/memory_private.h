/*
 * COPYRIGHT AND LICENSE
 *
 * Copyright (c) 2004-2005, Juniper Networks, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *       1.      Redistributions of source code must retain the above
 * copyright notice, this list of conditions and the following
 * disclaimer.
 *       2.      Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *       3.      The name of the copyright owner may not be used to
 * endorse or promote products derived from this software without specific
 * prior written permission.
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * $Id: memory_private.h,v 1.1.1.8 2007/05/09 20:41:35 sachin Exp $
 *
 * Memory allocation API for AMT
 * Memory allocation is wrapped up so we can track the data structures
 * allocated and free'ed and find memory leaks by data structure.
 */

#ifndef AMT_LIBMEM_MEMORY_PRIVATE_H
#define AMT_LIBMEM_MEMORY_PRIVATE_H

#include <sys/types.h>
#include <sys/queue.h>

#define TRUE 1
#define FALSE 0
#define MAX_MEM_NAME 128

typedef struct mem_bits
{
    TAILQ_ENTRY(mem_bits) m_next; /* list */
    u_int32_t m_size;             /* size of memory type */
    u_int32_t m_alloced;          /* number of allocs */
    u_int32_t m_freed;            /* number of freed */
    char m_name[MAX_MEM_NAME];    /* memory type description */
} mem_bits_t;

TAILQ_HEAD(mem_list, mem_bits);

#endif  // AMT_LIBMEM_MEMORY_PRIVATE_H
