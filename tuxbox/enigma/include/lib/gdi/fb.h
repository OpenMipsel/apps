#ifndef __FB_H
#define __FB_H

#include <linux/fb.h>
#include <lib/base/eerror.h>
#include <directfb.h>

class fbClass
{
	int available;
	
	IDirectFBSurface *primary;
	static fbClass *instance;
	
	int locked;
public:
	IDirectFBSurface *grabPrimary() { IDirectFBSurface *c=primary; primary=0; return c; }
//	int showConsole(int state);
	int SetMode(unsigned int xRes, unsigned int yRes, unsigned int bpp);
	int Available() { return available; }

	fbClass();
	~fbClass();
	
	static fbClass *getInstance();

	int lock();
	void unlock();
};

#endif
