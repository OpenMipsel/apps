#include <src/wizard_scan.h>
#include <lib/gdi/font.h>
#include <lib/gui/listbox.h>
#include <lib/system/init_num.h>

class eDiseqcChoice: public eListBoxEntry
{
	friend class eListBox<eDiseqcChoice>;
	gPixmap *pixmap;
	eTextPara *para;
	static gFont font;
	int choice;
	eString text;
	int yOffs;
public:
	enum { none, simple, complex };
	eDiseqcChoice(eListBox<eDiseqcChoice>* lb, int choice)
		: eListBoxEntry( (eListBox<eListBoxEntry>*)lb), choice(choice)
	{
		pixmap=eSkin::getActive()->queryImage(eString().sprintf("diseqc_%d", choice));
		if (!font.pointSize)
			font = eSkin::getActive()->queryFont("eListBox.EntryText.normal");
		para=0;
	}
	
	~eDiseqcChoice()
	{	
		delete para;
	}
	
	int getDiseqcChoice() const
	{
		return choice;
	}
	
protected:
	static int getEntryHeight()
	{
		return 150;
	}
	
	const eString& redraw(gPainter *rc, const eRect& rect, gColor coActiveB, gColor coActiveF, gColor coNormalB, gColor coNormalF, int state )
	{
		drawEntryRect( rc, rect, coActiveB, coActiveF, coNormalB, coNormalF, state );

		if (!para)
		{
			para = new eTextPara( eRect( rect.left(), 0, rect.width(), rect.height() ) );
			para->setFont(font);
			para->renderString(text);
			para->realign(eTextPara::dirCenter);
			yOffs = ((rect.height() - para->getBoundBox().height()) / 2) - para->getBoundBox().top();
		}
		rc->renderPara(*para, ePoint(0, rect.top() + yOffs ) );
		if (pixmap)
			rc->blit(*pixmap, ePoint(rect.left()+20, rect.top()+5));

		return text;
	}
};

gFont eDiseqcChoice::font;

eWizardSelectDiseqc::eWizardSelectDiseqc()
{
	diseqclist=new eListBox<eDiseqcChoice>(this);
	diseqclist->setName("choices");
	diseqclist->setColumns(3);

	if (eSkin::getActive()->build(this, "eWizardDiseqc"))
		eFatal("skin load of \"eWizardDiseqc\" failed");
		
	new eDiseqcChoice(diseqclist, eDiseqcChoice::none);
	new eDiseqcChoice(diseqclist, eDiseqcChoice::simple);
	new eDiseqcChoice(diseqclist, eDiseqcChoice::complex);
}

int eWizardSelectDiseqc::run()
{
	eWizardSelectDiseqc *wizard=new eWizardSelectDiseqc();
	wizard->show();
	int res=wizard->exec();
	wizard->hide();
	return res;
}

class eWizardScanInit
{
public:
	eWizardScanInit()
	{
	//	eWizardSelectDiseqc::run();
	}
};

eAutoInitP0<eWizardScanInit> init_eWizardScanInit(eAutoInitNumbers::wizard+3, "wizard: scan");
