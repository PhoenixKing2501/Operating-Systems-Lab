// file: bit_vector.h
// author: Marc Bumble
// May 12, 2000
// Class and functions used to maintain a bit vector for memory allocation
// Copyright (C) 2000 by Marc D. Bumble

//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.

//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.

//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.



#ifndef BIT_VECTOR_H
#define BIT_VECTOR_H

#include <math.h>
#include <bitset>

#include <global_defs.h>

namespace bit_vec_space {

  class bit_vector {
    int head;	                // points to first free block
    unsigned char* bit_vec;	// points to the front of the bit vector
    const int bit_vec_sz;	// length of the bit vector in bytes
    const int nelem;		// number of elements (ie how many bits)
  public:
    typedef size_t size_type;

    // constructor
    bit_vector(int length,unsigned char* start_addr);
    // destructor
    ~bit_vector() {};
    // copy constructor
    bit_vector(const bit_vector& t);
    // assignment
    bit_vector& operator=(const bit_vector& t);
    int get_size() {return bit_vec_sz;}

    void mark(int start_block,int blocks_needed);
    void clear(int start_block, int num_of_blocks);
    int find(size_type blocks_needed);
    bool find_single_bits(int *start_block,int *block_count);
    bool find_bytes(int *start_block,int *block_count);
    int find_prev_free_block(int start_block); // find prev block to start_block
    int find_next_free_block(int start_block,int blocks_needed);
  }; // class bit_vector_type

  
  
} // namespace bit_vec_space

#endif // BIT_VECTOR_H
