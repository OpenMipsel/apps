#include "enumber.h"
#include "fb.h"
#include "rc.h"
#include "eskin.h"

QRect eNumber::getNumberRect(int n)
{
	return QRect(n*space, 0, space, size.height());
}

void eNumber::redrawNumber(gPainter *p, int n, const QRect &area)
{
	QRect pos=QRect(n*space, 0, space, size.height());

	if (!area.contains(pos))
		return;
	
	p->setForegroundColor((have_focus && n==active)?cursor:normal);
	p->fill(pos);
	p->setFont(font);
	p->renderText(pos, QString().sprintf("%s%d", n?".":"", number[n]));
	p->flush();
}

void eNumber::redrawWidget(gPainter *p, const QRect &area)
{
	for (int i=0; i<len; i++)
		redrawNumber(p, i, area);
}

int eNumber::eventFilter(const eWidgetEvent &event)
{
	switch (event.type)
	{
	case eWidgetEvent::changedSize:
		space=(size.width()-2)/len;
		break;
	}
	return 0;
}

eNumber::eNumber(eWidget *parent, int len, int min, int max, int maxdigits, int *init, int isactive, eLabel* descr)
:eWidget(parent, 1), len(len), min(min), max(max), maxdigits(maxdigits), isactive(isactive), descr(descr?descr->getText():"")
{
	active=0;
	digit=isactive;
	for (int i=0; i<len; i++)
		number[i]=init[i];
	have_focus=0;
	cursor=eSkin::getActive()->queryScheme("focusedColor");
	normal=eSkin::getActive()->queryScheme("fgColor");
}

eNumber::~eNumber()
{
}

void eNumber::keyUp(int key)
{
}

void eNumber::keyDown(int key)
{
	if (LCDTmp)
		((eNumber*) LCDTmp)->keyDown(key);

	switch (key)
	{
	case eRCInput::RC_OK:
	case eRCInput::RC_RIGHT:
	{
		int oldac=active;
		active++;
		redraw(getNumberRect(oldac));
		if (active>=len)
		{
			if (key==eRCInput::RC_OK)
				emit selected(number);
			active=0;
		}

		if (active!=oldac)
			redraw(getNumberRect(active));
		digit=0;
		break;
	}
	case eRCInput::RC_LEFT:
	{
		int oldac=active;
		active--;
		redraw(getNumberRect(oldac));
		if (active<0)
			active=len-1;
		if (active!=oldac)
			redraw(getNumberRect(active));
		digit=0;
		break;
	}
	case eRCInput::RC_0 ... eRCInput::RC_9:
	{
		int nn=(digit!=0)?number[active]*10:0;
		nn+=key-eRCInput::RC_0;
		if (nn>=min && nn<=max)
		{
			number[active]=nn;
			redraw(getNumberRect(active));
			digit++;
			if ((digit>=maxdigits) || (nn==0))
			{
				active++;
				redraw(getNumberRect(active-1));
				digit=0;
			
				if (active>=len)
				{
					emit selected(number);
					active=0;
				}
				else
					redraw(getNumberRect(active));
			}
		}
		break;
	}
	}
}

void eNumber::gotFocus()
{
	have_focus++;
	digit=isactive;
	redraw(getNumberRect(active));
	if (parent && parent->LCDElement)
	{
		if (descr != "")
		{
			LCDTmp = new eNumber(parent->LCDElement, len, min, max, maxdigits, &(number[0]), isactive);
			QSize s = parent->LCDElement->getSize();
			LCDTmp->move(QPoint(0,s.height()/2));
			LCDTmp->resize(QSize(s.width(), s.height()/2));
	  	parent->LCDElement->setText(descr);
		}
		else
		{
			LCDTmp = new eNumber(parent->LCDElement, len, min, max, maxdigits, &(number[0]), isactive);
			QSize s = parent->LCDElement->getSize();
			LCDTmp->resize(s);
			LCDTmp->move(QPoint(0,0));
		}
		((eNumber*)LCDTmp)->digit=digit;
		((eNumber*)LCDTmp)->active=active;
	}
}

void eNumber::lostFocus()
{
	if (LCDTmp)
	{
		delete LCDTmp;
		LCDTmp=0;
	}

	have_focus--;
	redraw(getNumberRect(active));
}
