// Emacs style mode select   -*- C++ -*- 
//-----------------------------------------------------------------------------
//
// $Id: r_segs.h,v 1.1 2002/01/16 13:30:31 fx2 Exp $
//
// Copyright (C) 1993-1996 by id Software, Inc.
//
// This source is available for distribution and/or modification
// only under the terms of the DOOM Source Code License as
// published by id Software. All rights reserved.
//
// The source is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// FITNESS FOR A PARTICULAR PURPOSE. See the DOOM Source Code License
// for more details.
//
// DESCRIPTION:
//	Refresh module, drawing LineSegs from BSP.
//
//-----------------------------------------------------------------------------


#ifndef __R_SEGS__
#define __R_SEGS__


#ifdef __GNUG__
#pragma interface
#endif


void
R_RenderMaskedSegRange
( drawseg_t*	ds,
  int		x1,
  int		x2 );


#endif
//-----------------------------------------------------------------------------
//
// $Log: r_segs.h,v $
// Revision 1.1  2002/01/16 13:30:31  fx2
// added doom , dont install bouquet.so, X11-bugfix rcinput.c, yahtzee.c ?
//
//
//-----------------------------------------------------------------------------
