#include <lib/gdi/glcddc.h>
#include <lib/gdi/lcd.h>

gLCDDC *gLCDDC::instance;

gLCDDC::gLCDDC(eLCD *lcd): lcd(lcd)
{
	instance=this;
	
	update=1;

	pixmap=gImage(lcd->size(), DSPF_A8);
}

gLCDDC::~gLCDDC()
{
	instance=0;
}

void gLCDDC::exec(gOpcode *o)
{
	if (!pixmap)
		return;
	switch (o->opcode)
	{
	case gOpcode::flush:
	case gOpcode::end:
		if (update)
			lcd->update();
	default:
		gPixmapDC::exec(o);
		break;
	}
}

gLCDDC *gLCDDC::getInstance()
{
	return instance;
}

void gLCDDC::setUpdate(int u)
{
	update=u;
}
