#ifndef __wizard_scan_h
#define __wizard_scan_h

#include <lib/gui/listbox.h>
#include <lib/gui/ewindow.h>

class eDiseqcChoice;

class eWizardSelectDiseqc: public eWindow
{
	eListBox<eDiseqcChoice> *diseqclist;
public:
	eWizardSelectDiseqc();
	static int run();
};

#endif
