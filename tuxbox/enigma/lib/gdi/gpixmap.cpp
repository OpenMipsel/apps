#include <lib/gdi/gpixmap.h>

extern IDirectFB *DirectFB;

gLookup::gLookup()
{
	size=0;
	lookup=0;
}

gLookup::gLookup(int size, const gPalette &pal, const gRGB &start, const gRGB &end)
{
	size=0;
	lookup=0;
	build(size, pal, start, end);
}

void gLookup::build(int _size, const gPalette &pal, const gRGB &start, const gRGB &end)
{
	if (lookup)
	{
		delete lookup;
		lookup=0;
		size=0;
	}
	size=_size;
	if (!size)
		return;
	lookup=new gColor[size];
	
	for (int i=0; i<size; i++)
	{
		gRGB col;
		if (i)
		{
			int rdiff=-start.r+end.r;
			int gdiff=-start.g+end.g;
			int bdiff=-start.b+end.b;
			int adiff=-start.a+end.a;
			rdiff*=i; rdiff/=(size-1);
			gdiff*=i; gdiff/=(size-1);
			bdiff*=i; bdiff/=(size-1);
			adiff*=i; adiff/=(size-1);
			col.r=start.r+rdiff;
			col.g=start.g+gdiff;
			col.b=start.b+bdiff;
			col.a=start.a+adiff;
		} else
			col=start;
		lookup[i]=pal.findColor(col);
	}
}

void gPixmap::fill(const eRect &area, const gColor &color)
{
	if ((area.height()<=0) || (area.width()<=0))
		return;
		
//	(*this)->SetClip(*this, 0);
	(*this)->SetColorIndex(*this, color);
	(*this)->FillRectangle(*this, area.x(), area.y(), area.width(), area.height());
}

void gPixmap::blit(const gPixmap &src, ePoint pos, const eRect &clip, int flag)
{
	eRect area=eRect(pos, src.getSize());
	if (!clip.isNull())
		area&=clip;
	area&=eRect(ePoint(0, 0), getSize());
	if ((area.width()<0) || (area.height()<0))
		return;
	
	eRect srcarea=area;
	srcarea.moveBy(-pos.x(), -pos.y());
	
	DFBRectangle srca=srcarea;
	DFBRegion a=area;
	
//	(*this)->SetClip(*this, &a);
	if (!(flag & blitAlphaTest))
		(*this)->SetBlittingFlags (*this, DSBLIT_NOFX);
	else
		(*this)->SetBlittingFlags (*this, DSBLIT_SRC_COLORKEY);
	(*this)->Blit(*this, (gPixmap &)src, &srca, area.x(), area.y());
}

void gPixmap::mergePalette(const gPalette &target)
{
	gPalette pal;
	unsigned int entries;
	
	eDebug("merge palette... won't work");
	
		// get size
	if (target->GetSize(target, &entries))
		return;
		
	DFBColor colors[entries];
	
		// get entries
	if (target->GetEntries(target, colors, entries, 0))
		return;
		
		// get source palette
	if ((*this)->GetPalette(*this, &pal.ptrref()))
		return;
		
		// build lookup old -> new
	unsigned int lookup[entries];

	for (int i=0; i<entries; ++i)
		if (pal->FindBestMatch(pal, colors[i].r, colors[i].g, colors[i].b, colors[i].a, &lookup[i]))
			return;
	
		// set new palette
	(*this)->SetPalette(*this, target);
	
	__u8 *dstptr;
	
	int pitch;
	
		// translate data...
	if ((*this)->Lock(*this, DFBSurfaceLockFlags(DSLF_READ|DSLF_WRITE), (void**)&dstptr, &pitch))
		return;
		
	eSize size=getSize();

	for (int ay=0; ay<size.height(); ay++)
	{
		for (int ax=0; ax<size.width(); ax++)
			dstptr[ax]=lookup[dstptr[ax]];
		dstptr+=pitch;
	}
	
	(*this)->Unlock(*this);
}

void gPixmap::line(ePoint start, ePoint dst, gColor color)
{
	(*this)->SetColorIndex(*this, color);
	(*this)->DrawLine(*this, start.x(), start.y(), dst.x(), dst.y());
}

gPixmap::~gPixmap()
{
}

gPalette::gPalette(gRGB *rgb, int size)
{
	DFBPaletteDescription desc;
	desc.flags=DFBPaletteDescriptionFlags(DPDESC_CAPS|DPDESC_SIZE|DPDESC_ENTRIES);
	desc.caps=DFBPaletteCapabilities(DPCAPS_NONE);
	desc.size=size;
	desc.entries=rgb;
	DirectFB->CreatePalette(DirectFB, &desc, &ptrref());
}

gColor gPalette::findColor(const gRGB &rgb) const
{
	gColor col=0;
	(*this)->FindBestMatch(*this, rgb.r, rgb.g, rgb.b, rgb.a, &col);
	return col;
}

gPixmap::gPixmap(IDirectFBSurface *surface): gSmartPtr<IDirectFBSurface>(surface)
{
}

gPixmap::gPixmap()
{
}

gPalette gPalette::copy()
{
	gPalette n;
	(*this)->CreateCopy(*this, &n.ptrref());
	return n;
}

gPalette::~gPalette()
{
}

gImage::gImage(eSize size, int colorformat)
{
	DFBSurfaceDescription desc;
	desc.flags=DFBSurfaceDescriptionFlags(DSDESC_CAPS|DSDESC_WIDTH|DSDESC_HEIGHT|DSDESC_PIXELFORMAT);
	desc.caps=DSCAPS_NONE;
	desc.pixelformat=DFBSurfacePixelFormat(colorformat);
	desc.width=size.width();
	desc.height=size.height();
	if (DirectFB->CreateSurface(DirectFB, &desc, &ptrref()) != DFB_OK)
		eFatal("CreateSurface failed!");
}

gImage::~gImage()
{
}
