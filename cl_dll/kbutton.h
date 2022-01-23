//========= Copyright (c) 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#ifndef __KBUTTON_H__
#define __KBUTTON_H__

typedef struct kbutton_s
{
	int down[2]; // key nums holding it down
	int state;   // low bit is down state
} kbutton_t;

#endif // __KBUTTON_H__
