// file: bit_vector.cc
// author: Marc Bumble
// June1, 2000
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

#include <bit_vector.h>

namespace bit_vec_space {

  //////////////////////////////////////////////////////////////////////////////////
  //////                        Bit Vector routines
  //////////////////////////////////////////////////////////////////////////////////
  /////
  /////    These routine search, mark, and free bits within the bit vectors found 
  /////    on the allocated memory pages
  /////
  //////////////////////////////////////////////////////////////////////////////////
  

  // bit_vector constructor
  bit_vector::bit_vector(int num_of_elements,unsigned char* start_addr)
    : bit_vec_sz((num_of_elements/8) + 1),
    nelem(num_of_elements) {

    bit_vec=start_addr;
    head=0;			// set head to point to the first free block
    for (int i=0; i<bit_vec_sz; i++)
      bit_vec[i]=0x00;
  };

  // copy constructor
  bit_vector::bit_vector(const bit_vector& t)
    : bit_vec_sz(t.bit_vec_sz),
    nelem(t.nelem) {
    head=t.head;
    for (int i=0; i<bit_vec_sz; i++)
      bit_vec[i]=t.bit_vec[i];
  };  // copy constructor
  
  // assignment
  bit_vector& bit_vector::operator=(const bit_vector& t) {
    if (this != &t) {
      head=t.head;
      for (int i=0; i<bit_vec_sz; i++) {
	bit_vec[i]=t.bit_vec[i];
      }
    }  //  if (this != &t)
    return *this;
  };  // assignment



  void bit_vector::mark(int global_start_block,int blocks_needed) {
    // Mark the reqested block as assigned.  At the end, if the
    // head block is being assigned, move the head block to point
    // to the next (ie first) free block.  See at bottom

    
    // The const 8 here is 8 bits per byte
    int start_block         = global_start_block%nelem;
    int start_bit           = start_block%8;
    int start_byte          = start_block/8;
    int block_count       = blocks_needed;
    // get the byte - const 8 ==  8 bits/(unsigned char)
    std::bitset<8> val = bit_vec[start_byte];
    if ((start_bit)||(block_count<8)) {
      // if bits to be marked begin with less than one full char
      for (int i=start_bit; ((i<8)&&(block_count>0)); i++,block_count--) {
	// set the bit
	val.set(i,1);
      } // 
      // write the byte back
      bit_vec[start_byte] = val.to_ulong();
      start_byte++;		// increment the start byte
    }  // if ((start_bit)||(((unsigned)block_count)<sizeof(unsigned char)))
    // Now proceed byte by byte
    if (block_count >= 8) {
      // if still need more blocks, go char by char
      while (block_count>=8) {
	bit_vec[start_byte] = 0xff;
	start_byte++;
	block_count-=8;
      } // while (block_count>8)
    } // if (block_count > 8)
    if (block_count>0) {
      // Still need final blocks < full bytes worth
      // Allocate bit by bit
      // get the byte
      val = bit_vec[start_byte];
      for (int i=0; ((i<8) && (block_count));
	   i++,block_count--) {
	// set the bit
	val.set(i,1);
      }
      bit_vec[start_byte] = val.to_ulong();
    }
    // Now adjust the head block to point to the next free block if necessary.
    if (head == global_start_block) {
      head = find_next_free_block(global_start_block,1);
    }
    
  }; // mark()
  
  
  void bit_vector::clear(int global_start_block, int blocks_needed) {
    // Release num_of_blocks in the bit vector starting at start_block by
    // clearing the corresponding bits.  Once completed, if the head block
    // pointer is greater than the newly released blocks, move the head
    // backwards to allow memory to be recycled.
    
    // Chunk* this_chunk     = find_chunk(global_start_block);
//      const int nelem       = bit_vec_sz/esize;
    int start_block       = global_start_block%nelem;
    int start_byte        = start_block/8;
    int start_bit         = start_block%8;
    int block_count       = blocks_needed;
//      Chunk* this_chunk = find_chunk(global_start_block,&chunk_num);
    std::bitset<8> val = bit_vec[start_byte];
    if ((start_bit)||(block_count<8)) {
      // clear initial bits
      for(int i=start_bit; ((i<8)&&(block_count)); i++,block_count--) {
	val.reset(i);
      }
      bit_vec[start_byte] = val.to_ulong();
      // next see if byte by byte clearing makes sense
      start_byte+=1;
    }
    if (block_count>=8) {
      // if still need more blocks, go char by char
      while (block_count>=8) {
	bit_vec[start_byte] = 0x00;
	start_byte++;
	block_count-=8;
      } // while (block_count>8)
    }
    // Now finish off any non-integral or individual trailing bits
    if (block_count>0) {
      val = bit_vec[start_byte];
      for (int i=0; ((i<8) && (block_count));
	   i++,block_count--) {
	// set the bit
	val.reset(i);
      }
      bit_vec[start_byte] = val.to_ulong();
    }
    // if the head point is greater than the newly freed address, move the
    // head backwards to attempt memory recyling
    if (head > global_start_block)
      head = global_start_block;
  }; // clear()
  
  
  //////////////////////////////////////////////////////////////////////////////////
  //////                          find
  //////////////////////////////////////////////////////////////////////////////////
  /////
  /////    Finds a free block of space containing blocks_needed.  Returns the 
  /////    starting block address, or -1 if no suitable block is available in
  /////    this chunk.
  /////    
  /////    
  /////    Works in 3 stages:
  /////          1. Searchs individual bits until full free integral chars
  /////          2. Searches char by char in the bit_vector
  /////          3. Searches through any necessary tailing individual bits/////          
  /////          
  /////          
  //////////////////////////////////////////////////////////////////////////////////
  
  int bit_vector::find(size_type blocks_needed) {
    
    // First, get head block
    int start_block;
    if (head==-1) {
      return head;
    } else {
      start_block = head;
    }
    // int start_block = head;

    // const 8 is the number of bits in an unsigned char
    // int start_bit         = start_block%8;
    // int start_byte        = start_block/8;
    // Break search into three sections, any of which can fail and cause the search
    // to restart.  On failure, each of the three routines must put the failed block
    // in the start_block location.  The failed block in start_block is used to
    // determine the new starting point.
    bool successful        = false;
    int start_block_attempt;
    while (!successful) {
      // renew the block count with each new attempt.
      int block_count       = blocks_needed;
      start_block_attempt   = start_block;
      // find initial leading indvidual bits
      successful = find_single_bits(&start_block_attempt,&block_count);
      if ((successful)&&(block_count>=8)) {
	// search byte by byte for free blocks
	successful = find_bytes(&start_block_attempt,&block_count);
      }
      if ((successful)&&(block_count>0)) {
	// find tailing individual bits
	successful = find_single_bits(&start_block_attempt,&block_count);
      }
      if (!successful) {
	// if any attempt failed, that attempt should have updated start_block with
	// the site of the last failed block attempt.  This block is then used to
	// find the next search starting block.
	start_block = find_next_free_block(start_block_attempt,blocks_needed);
	// if there is no next free block, find_next_free_block() will return a -1.
	// at this point, this chunk has been successfully searched, so exit up to
	// the higher level to get the next chunk.
	if (start_block==-1) {
	  successful=true;
	}
      }  // if (!successful)
    } // while (!successful)
    return start_block;
  }
  
  //////////////////////////////////////////////////////////////////////////////////
  //////                          find_single_bits
  //////////////////////////////////////////////////////////////////////////////////
  /////
  /////    Assists in finding free blocks of memory for allocation        
  /////    This routine finds the leading discrete blocks of memory in the bit       
  /////    vector in a bit by bit search.  On success, returns true, on failure, 
  /////    returns false and puts the last failed block in start_block.  Block
  /////    count is used to keep track of the number of remaining blocks still 
  /////    required in the search.  
  /////    On failure, block_count is expected to be reset by the calling routine.
  /////    
  //////////////////////////////////////////////////////////////////////////////////
  
  bool bit_vector::find_single_bits(int *global_start_block,int *block_count) {
    // Find which bit and which byte this head_block corresponds to in the
    // memory array.  On failure, block_count is expected to be reset by
    // the calling routine.
//      const int nelem = bit_vec_sz/esize;
    int start_block = (*global_start_block)%nelem;
    int start_byte  = start_block/8;
    int start_bit   = start_block%8;
    bool success    = false;
    
    // Only bother with this routine if either 
    // 1. need less than 8 blocks (one bit vector byte's worth of blocks) or
    // 2. start bit is not on an even byte boundry
    if ((start_bit>0)||(*block_count<8)) {
      // first get correct page chunk
//        int chunk_num=0;
//        Chunk* this_chunk = find_chunk(*global_start_block,&chunk_num);
      success           = true;
      // const 8 bits == 1 unsigned char
      std::bitset<8> val = bit_vec[start_byte];
      // int upper_bound = ((start_bit+(*block_count)-1) > 8)?8:(start_bit+(*block_count));
      for (int i = start_bit; ((i<8) && ((*block_count)>0));
	   i++,(*global_start_block)++,(*block_count)--) {
	// while still under a full byte boundry, and still more blocks are needed
	// test each new consecutive bit to see if avail.
	if (val.test(i)) {
	  // if a 1 is detected in the bit vector, the block is allocated and we
	  // fail here.
	  success=false;
	  break;	// exit the for loop
	  // block count in block_count will be reset by calling routine.
	}  //  if (val.test(i))
      }  //  
    } else {
      // need to search byte by byte
      // haven't failed at this point
      success=true;
    }
    
    return success;
  };
  
  //////////////////////////////////////////////////////////////////////////////////
  //////                          find_bytes
  //////////////////////////////////////////////////////////////////////////////////
  /////
  /////    Assists in finding free blocks of memory for allocation        
  /////    This routine finds the middle blocks of memory in the bit vector in a       
  /////    byte by byte search.  On success, returns true, on failure, 
  /////    returns false and puts the last failed block in start_block.  Block
  /////    count is used to keep track of the number of remaining blocks still 
  /////    required in the search.  
  /////    On failure, block_count is expected to be reset by the calling routine.
  /////    
  /////    
  //////////////////////////////////////////////////////////////////////////////////
  
  bool bit_vector::find_bytes(int *global_start_block,int *block_count) {
    // Search byte by byte to ensure that the request bytes are not allocated
    // The global_start_block should be the first block on a byte or wrong
    // routine was called.
//      const int nelem = bit_vec_sz/esize;
    int start_block = *global_start_block%nelem;
    int start_bit   = start_block%8;
    int start_byte  = start_block/8;
    bool success = true;
    
    if (start_bit!=0) {
      std::cerr << __FILE__ << ':' << __LINE__ << ':' << " find_bytes(): ";
      std::cerr << "routine started on non-byte boundry.\n";
      exit(1);
    }
    int chunk_num=0;
//      Chunk* this_chunk = find_chunk(*global_start_block,&chunk_num);
    std::bitset<8> val;
    
    int upper_bound = start_byte+((*block_count)/8);
    if (upper_bound>bit_vec_sz) {
      return false;
    }
    
    for (int i=start_byte; ((i<upper_bound)&&((*block_count)>=0));
	 i++,(*block_count)-=8,(*global_start_block)+=8) {
      val = bit_vec[i];
      if (val.any()) {
	// found an allocated block - failure
	// working backwards through the byte, find the block which caused the failure
	int j;
	for (j=7; j>=0; j--)
	  if (val.test(j))
	    break;
	// i == bytes worth of blocks
	// j == each bit is one block
	// put the failed block in the global_start_block
	*global_start_block = chunk_num*nelem + i*8 + j;
	success=false;
	break;			// exit the for loop
      }
    }
    
    return success;
  };
  
  
  //////////////////////////////////////////////////////////////////////////////////
  //////                        find_prev_free_block()
  //////////////////////////////////////////////////////////////////////////////////
  /////
  /////    Find the previous free block to the block cited in the parameter start_block.
  /////    This routine is called during allocation of memory so that the pointer stored
  /////    in the previous free memory block will be redirected to point to the free 
  /////    spacce after the newly allocated memory block.
  /////    
  /////    
  //////////////////////////////////////////////////////////////////////////////////
  
  int bit_vector::find_prev_free_block(int start_block) {
    // Find the previous free block to the block cited in the parameter start_block.
    // Jump to the start_block and from there work backwards to find the first free
    // block in the bit_vec.  If a -1 is returned, the head pointer is the previous
    // free block
    
    if (start_block==0)
      return -1;
    else
      start_block--;
    // first find the correct chunk address for this block
//      int chunk_num=0;
//      Chunk* this_chunk = find_chunk(start_block,&chunk_num);
    
    // correct chunk is hopefully now in this chunk
    // search for previous block starting from start_block-1
    // Search in 2 parts
    //     1. individual bits before start_block bit
    //     2. byte by byte before start_block
    //     3. individual bits within found byte (if necessary)
    
    // Start with previous bits within this byte
    // Find which bit and which byte this head_block corresponds to in the memory array
    // within the chunk
//      const int nelem = bit_vec_sz/esize;
    int local_start_block = start_block%nelem;
    int start_bit   = local_start_block%8;
    int start_byte  = local_start_block/8;
    // int start_chunk = local_start_block/(bit_vec_sz);
    int result_block= -1;
    bool finished = false;
    std::bitset<8> val;
    
    if (start_bit!=8-1) {
      // if we are not directly on a byte border (not on the last bit of a byte)
      val = bit_vec[start_byte];
      for (int i=start_bit; ((i>-1)&&(!finished)); i--) {
	if (!val.test(i)) {
	  // found the empty, unused block
	  result_block = start_byte*8 + i;
	  finished=true;
	  break;
	}
      }
      start_byte--;
    } // if (start_bit!=sizeof(unsigned char)*8-1)
    
    // Search backwards through the remaining bytes to the beginning of the chunk
    if (!finished) {
      // search byte by byte backwards
      if (start_byte<0) {
	// No place left to go backwards in this block, check for a previous block
	result_block = -1; //set the head to point forwards
	finished=true;
      }  else {
	// start_byte is not the 0th byte of the chunk, so there is space to search
	// backwards through.
	for (int i=start_byte; ((i>=0)&&(!finished)); i--,start_byte-=8) {
	  val = bit_vec[i];
	  val.flip(); // invert bits
	  if (val.any()) {
	    // found free block in this byte, need to identify which one
	    for (int j=7; ((j>0)&&(!finished)); j--) {
	      if (val.test(j)) {
		// found the specific bit set
		result_block = i*8 + j;// which block
		finished = true;
	      } // if (val.test(j))
	    }  // for (int j=7; ((j>0)&&(!finished)); j--) 
	  }  // if (val.any())
	}  // for (int i=start_byte; ((i>=0)&&(!finished)); i--)
      }  // if (start_byte==0)
    }
    if (!finished) {
      result_block = -1; //set the head to point forwards
      finished=true;
    }  // if (!finished)
    return result_block;
  }; // find_prev_free_block(int start_block)
  
  
  //////////////////////////////////////////////////////////////////////////////////
  //////                        find_next_free_block()
  //////////////////////////////////////////////////////////////////////////////////
  /////
  /////    Find the next free block after the start_block.  This routine is used 
  /////    during a find failure.  It lets the find continue with a new starting 
  /////    point.  The blocks_needed parameter allows the routine to look ahead to 
  /////    see if the routine is now too close to the end of a given page to  
  /////    possibly find an allocatable block.
  /////    
  //////////////////////////////////////////////////////////////////////////////////
  
  int bit_vector::find_next_free_block(int global_start_block,int blocks_needed) {
    // Search the this_chunk page bitvector for a new starting block to find
    // blocks_needed.  Search and find the next free block starting at 
    // start_block.  If no free block is found, grow a new chunk and return a
    // pointer to that new chunk.
    
    int free_block;
    bool finished=false;
    int start_block = global_start_block;
    // first see if there is any possible space in this chunk
    // if not, add new chunk.
    int chunk_num=0;
    // bit_vec_sz is in bytes, 8*bit_vec_sz is the number of elements
    if ((((blocks_needed+start_block)/8.0)+1) > bit_vec_sz) {
      // if not check for next chunk
      free_block = -1;
      finished=true;
    }  // if ((blocks_needed+start_block+1) > bit_vec_sz)
    if (!finished) {
      // now find the first avail block and stop there
      
      int start_bit = start_block%8;
      int start_byte= start_block/8;
      // first find the correct chunk address for this block
      std::bitset<8> val = bit_vec[start_byte];
      free_block=-1;
      // 1. search bits into next integral char
      // 2. search for char with open bit
      // 3. identify and return which open bit within the char
      
      // Search bit by bit to the first integral char
      finished=false;
      if ((start_bit != 0)||(blocks_needed<8)) {
	// if not starting at the beginning of a full char
	// See if you need to search to the end of this char, or just within this char
	// int upper_bound = ((start_bit+blocks_needed) > 8)?8:(start_bit+blocks_needed);
	for (int i = start_bit; ((i<8)&&(blocks_needed>0)); i++) {
	  if (!val.test(i)) {
	    // found the first free location at block i.
	    // Now need to see if blocks from i to
	    // ((i + blocks_needed)>8)?8:(i + blocks_needed)
	    // or end of this char are available and free
	    // Create a bit mask to test block availability
	    std::bitset<8> mask;
	    const int upper_bound = ((i+blocks_needed) > 8)?8:(i+blocks_needed);
	    for (int j = i; j < upper_bound; j++)
	      mask.set(j);
	    // test needed bit set
	    mask &= val;
	    // if zero, then sucess an the search continues if neccessary
	    if (mask.none()) {
	      blocks_needed-=(upper_bound-i);
	      if (blocks_needed == 0) {
		finished=true;
		// set free_block to the chunk wide value
		free_block=i + start_byte*8;
		break;
	      }
	    }
	    // otherwise; failed on this attempt, i gets incremented
	    // loop back and try again.
	  }  // if (!val.test(i))
	}
	start_byte+=1;
      }  // if (start_bit != 0)
      
      // Search byte by byte to the first free block
      while (!finished) {
	// Search until a free block is found.  If no free block is encountered
	// add a new chunk and allocate from it.
	
	// if not finished, continue the search char by char
	for (int i=start_byte; i<bit_vec_sz; i++) {
	  val = bit_vec[i];
	  // 1. flip all bit values.
	  // 2. then search the result for any ones (previously where 0's)
	  val.flip();		// all 1's to 0's and visa versa
	  if (val.any()) {
	    finished=true;
	    // find the explicit bit within this found byte
	    int j;
	    for (j = 0; j<8; j++) {
	      // bits have been flipped so find the 1 here
	      if (val.test(j)) {
		break;
	      }
	    }
	    // Set value of finished free_block
	    // i == finished byte #
	    // j == finished bit #
	    free_block = i*8 + j + chunk_num*bit_vec_sz;
	    finished=true;
	    break;
	  }  //  if (val.any())
	}  // for (int i=start_byte; i<size; i++)
	if (!finished) {
	  // reached the end of the current chunk and still no empty byte
	  // move to next chunk, or if no next chunk add a new one.
	  // if not check for next chunk
	  free_block = -1;
	  finished=true;
	}  // if (!finished)
      }  // if (!finished)
    }  // if (!finished)
    return free_block;
  };  // find_next_free_block()


}  // namespace bit_vec_space 
