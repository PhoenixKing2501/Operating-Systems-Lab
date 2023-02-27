// file: global_defs.h
// author: Marc Bumble
// January 15, 1999
// This file is used to establish the types of templates
// make type changes here
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

#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

// inclusion of iostream is needed to introduce the 
#include <iostream>

#ifdef USE_NAMESPACES
//  # ifdef USE_STLPORT
//    // stlport:: is defined, whether or not it equals to std::
//    //  namespace stl = stlport;
//    namespace stl = STLPORT;
//  # else
//    //compiler may not provide std:: contents 
//    namespace stl = std;
//  # endif /* USES_STLPORT */
namespace stl = std;
# else
  // namespaces are not available, make it global
# define stl
# endif /* NAMESPACES */

#endif // GLOBAL_DEFS_H
