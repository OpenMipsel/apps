#include <lib/gui/decoration.h>
#include <lib/gdi/gpixmap.h>
#include <lib/gdi/grc.h>
#include <lib/gui/eskin.h>

/*

	+-------+-----------------+--------+
	|TopLeft|        Top      |TopRight|
	+------++-----------------+--+-----+
	|  Left|     client          |Right|
	+------+---+-----------+-----+-----+
	|BottomLeft|   Bottom  |BottomRight|
	+----------+-----------+-----------+

*/

eDecoration::eDecoration()
{
	iTopLeft=iTop=iTopRight=iLeft=iRight=iBottomLeft=iBottom=iBottomRight=0;
	borderLeft=borderTop=borderRight=borderBottom=0;
}

bool eDecoration::load(const eString& base)
{
	if (basename != base)
	{
		basename=base;	// all your
		iTopLeft=eSkin::getActive()->queryImage(basename + ".topLeft");
		iTop=eSkin::getActive()->queryImage(basename + ".top");
		iTopRight=eSkin::getActive()->queryImage(basename + ".topRight");
		iLeft=eSkin::getActive()->queryImage(basename + ".left");
		iRight=eSkin::getActive()->queryImage(basename + ".right");
		iBottomLeft=eSkin::getActive()->queryImage(basename + ".bottomLeft");
		iBottom=eSkin::getActive()->queryImage(basename + ".bottom");
		iBottomRight=eSkin::getActive()->queryImage(basename + ".bottomRight");

		borderLeft=borderTop=borderRight=borderBottom=0;
	
		if (iTop)
			borderTop = iTop.getSize().height();
		if (iLeft)
			borderLeft = iLeft.getSize().width();
		if (iRight)
			borderRight = iRight.getSize().width();
		if (iBottom)
			borderBottom = iBottom.getSize().height();
	}
	return operator bool();
}

void eDecoration::drawDecoration(gPainter *target, ePoint size)
{
  int x=0, xm=size.x(), y, ym;

	if (iTopLeft)
	{
		target->blit(iTopLeft, ePoint(0, 0) );
		x+=iTopLeft.getSize().width();
	}

	if (iTopRight)
	{
		xm-=iTopRight.getSize().width();
		target->blit(iTopRight, ePoint(xm, 0), eRect(x, 0, size.x()-x, size.y()));
	}

	if (iTop)
	{
		while (x<xm)
		{
			target->blit(iTop, ePoint(x, 0), eRect(x, 0, xm-x, size.y()));
			x+=iTop.getSize().width();
		}
  }

	x=0;
	xm=size.x();

	if (iBottomLeft)
	{
    target->blit(iBottomLeft, ePoint(0, size.y()-iBottomLeft.getSize().height()));
		x+=iBottomLeft.getSize().width();
	}

	if (iBottomRight)
	{
		xm-=iBottomRight.getSize().width();
		target->blit(iBottomRight, ePoint(xm, size.y()-iBottomRight.getSize().height()), eRect(x, size.y()-iBottomRight.getSize().height(), size.x()-x, iBottomRight.getSize().height()));
	}
	
	if (iBottom)
	{
    while (x<xm)
		{
			target->blit(iBottom, ePoint(x, size.y()-iBottom.getSize().height()), eRect(x, size.y()-iBottom.getSize().height(), xm-x, iBottom.getSize().height()));
			x+=iBottom.getSize().width();
		}
	}
	
	y=0; ym=size.y();
	
	if (iTopLeft)
		y=iTopLeft.getSize().height();
	if (iBottomLeft)
		ym=size.y()-iBottomLeft.getSize().height();
	if (iLeft)
  {
    while (y<ym)
		{
      target->blit(iLeft, ePoint(0, y), eRect(0, y, iLeft.getSize().width(), ym-y));
			y+=iLeft.getSize().height();
		}
  }

	if (iTopRight)
		y=iTopRight.getSize().height();
	if (iBottomRight)
		ym=size.y()-iBottomRight.getSize().height();
	if (iRight)
  {
    while (y<ym)
		{
			target->blit(iRight, ePoint(size.x()-iRight.getSize().width(), y), eRect(size.x()-iRight.getSize().width(), y, iRight.getSize().width(), ym-y));
			y+=iRight.getSize().height();
		}
	}
}
