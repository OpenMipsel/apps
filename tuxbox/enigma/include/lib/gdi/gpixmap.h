#ifndef __gpixmap_h
#define __gpixmap_h

#include <pthread.h>
#include <directfb.h>

#include <lib/base/estring.h>
#include <lib/base/erect.h>

typedef unsigned int gColor;

#ifndef DFB_COLOR_EQUAL
#define DFB_COLOR_EQUAL(x,y)  ((x).a == (y).a &&  \
                               (x).r == (y).r &&  \
                               (x).g == (y).g &&  \
                               (x).b == (y).b)
#endif

struct gRGB: public DFBColor
{
	gRGB(int _r, int _g, int _b, int _a=255)
	{
		r=_r; g=_g; b=_b; a=_a;
	}
	gRGB(unsigned long val) // : b(val&0xFF), g((val>>8)&0xFF), r((val>>16)&0xFF), a((val>>24)&0xFF)		// ARGB
	{
	}
	gRGB()
	{
	}
	bool operator < (const gRGB &c) const
	{
		if (b < c.b)
			return 1;
		if (b == c.b)
		{
			if (g < c.g)
				return 1;
			if (g == c.g)
			{
				if (r < c.r)
					return 1;
				if (r == c.r)
					return a < c.a;
			}
		}
		return 0;
	}
	bool operator==(const gRGB &c) const
	{
		return DFB_COLOR_EQUAL(*this, c);
	}
};

template <class T>
class gSmartPtr
{
	T *ptr;
protected:
	T &operator*() { return *ptr; }
public:
	gSmartPtr(): ptr(0)
	{
	}
	gSmartPtr(T *c): ptr(c)
	{
		if (c)
			c->AddRef(c);
	}
	gSmartPtr(const gSmartPtr<T> &c)
	{
		ptr=c.ptr;
		if (ptr)
			ptr->AddRef(ptr);
	}
	gSmartPtr &operator=(const gSmartPtr<T> &c)
	{
		ptr=c.ptr;
		if (ptr)
			ptr->AddRef(ptr);
	}
	
	~gSmartPtr()
	{
		if (ptr)
			ptr->Release(ptr);
	}
	T* &ptrref() { ASSERT(!ptr); return ptr; }
	T* operator->() { ASSERT(ptr); return ptr; }
	const T* operator->() const { ASSERT(ptr); return ptr; }
	operator T*() const { return ((gSmartPtr<T>*)this)->ptr; }
};


struct gPalette: gSmartPtr<IDirectFBPalette>
{
	gPalette() { }
	gPalette(IDirectFBPalette *p): gSmartPtr<IDirectFBPalette>(p) { }
	gPalette(const gPalette &p): gSmartPtr<IDirectFBPalette>(p) { }
	gPalette(gRGB *data, int len);
	
	gPalette copy();
	
	~gPalette();
	gColor findColor(const gRGB &rgb) const;
};

struct gLookup
{
	int size;
	gColor *lookup;
	gLookup(int size, const gPalette &pal, const gRGB &start, const gRGB &end);
	gLookup();
	void build(int size, const gPalette &pal, const gRGB &start, const gRGB &end);
};

/**
 * \brief A softreference to a font.
 *
 * The font is specified by a name and a size.
 * \c gFont is part of the \ref gdi.
 */
struct gFont
{
	eString family;
	int pointSize;
	
	/**
	 * \brief Constructs a font with the given name and size.
	 * \param family The name of the font, for example "NimbusSansL-Regular Sans L Regular".
	 * \param pointSize the size of the font in PIXELS.
	 */
	gFont(const eString &family, int pointSize):
			family(family), pointSize(pointSize)
	{
	}
	
	gFont()
		:pointSize(0)
	{
	}
};


struct gPixmap: gSmartPtr<IDirectFBSurface>
{
	eSize getSize() const { unsigned int width, height; ASSERT((gPixmap*)this); (*(gPixmap*)this)->GetSize(*(gPixmap*)this, &width, &height); return eSize(width, height); }
	
	void fill(const eRect &area, const gColor &color);
	enum
	{
		blitAlphaTest=1
	};
	void blit(const gPixmap &src, ePoint pos, const eRect &clip=eRect(), int flags=0);
	void mergePalette(const gPalette &pal);
	void line(ePoint start, ePoint end, gColor color);
		
	gPixmap(IDirectFBSurface*);
	gPixmap();
	virtual ~gPixmap();
};

struct gImage: gPixmap
{
	gImage(eSize size, int dspf);
	~gImage();
};

#endif
