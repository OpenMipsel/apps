#include <png.h>
#include <stdio.h>
#include <lib/gdi/epng.h>
#include <unistd.h>

extern IDirectFB *DirectFB;

gPixmap loadPNG(const char *filename, const gPixmap &comp)
{
	IDirectFBImageProvider *provider;
	DFBSurfaceDescription dsc;
	if (DirectFB->CreateImageProvider(DirectFB, filename, &provider))
		return 0;
	gPixmap ret;

	if (!provider->GetSurfaceDescription(provider, &dsc))
	{
		int copy_pal=0;
		if (comp) // we need to create a compatible pixmap
		{
				// use same pixel format
			if (!comp->GetPixelFormat(comp, &dsc.pixelformat))
			{
				(int&)dsc.flags |= (int)DSDESC_PIXELFORMAT;
				copy_pal = DFB_PIXELFORMAT_IS_INDEXED(dsc.pixelformat);
			}
		}
		if (!DirectFB->CreateSurface (DirectFB, &dsc, &ret.ptrref()))
		{
			if (copy_pal)
			{
					// we need to copy the palette first
				gPalette pal;
				if (! comp->GetPalette(comp, &pal.ptrref()))
					ret->SetPalette(ret, pal);
			}
			provider->RenderTo(provider, ret, 0);
			eDebug("loading %s success!", filename);
		}
		ret->SetSrcColorKeyIndex(ret, 0);
	}
	provider->Release(provider);
	return ret;
}

int savePNG(const char *filename, gPixmap *pixmap)
{
#if 0
	FILE *fp=fopen(filename, "wb");
	if (!fp)
		return -1;
	png_structp png_ptr=png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
	if (!png_ptr)
	{
		eDebug("write png, couldnt allocate write struct");
		fclose(fp);
		unlink(filename);
		return -2;
	}
	png_infop info_ptr=png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		eDebug("info");
		png_destroy_write_struct(&png_ptr, 0);
		fclose(fp);
		unlink(filename);
		return -3;
	}
	if (setjmp(png_ptr->jmpbuf))
	{
		eDebug("error :/");
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		unlink(filename);
		return -4;
	}
	png_init_io(png_ptr, fp);
	png_set_filter(png_ptr, 0, PNG_FILTER_NONE|PNG_FILTER_SUB|PNG_FILTER_PAETH);
	png_set_compression_level(png_ptr, Z_BEST_COMPRESSION);

	png_set_IHDR(png_ptr, info_ptr, pixmap->x, pixmap->y, pixmap->bpp, 
		pixmap->clut.data ? PNG_COLOR_TYPE_PALETTE : PNG_COLOR_TYPE_GRAY, 
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	if (pixmap->clut.data)
	{
		png_color palette[pixmap->clut.colors];
		png_byte trans[pixmap->clut.colors];
		for (int i=0; i<pixmap->clut.colors; ++i)
		{
			palette[i].red=pixmap->clut.data[i].r;
			palette[i].green=pixmap->clut.data[i].g;
			palette[i].blue=pixmap->clut.data[i].b;
			trans[i]=255-pixmap->clut.data[i].a;
		}
		png_set_PLTE(png_ptr, info_ptr, palette, pixmap->clut.colors);
		png_set_tRNS(png_ptr, info_ptr, trans, pixmap->clut.colors, 0);
	}
	png_write_info(png_ptr, info_ptr);
	png_set_packing(png_ptr);
	png_byte *row_pointers[pixmap->y];
	for (int i=0; i<pixmap->y; ++i)
		row_pointers[i]=((png_byte*)pixmap->data)+i*pixmap->stride;
	png_write_image(png_ptr, row_pointers);
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	fclose(fp);
	eDebug("wrote png ! fine !");

#endif
	return 0;
}
