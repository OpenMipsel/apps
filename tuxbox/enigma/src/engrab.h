#ifndef __engrab_h
#define __engrab_h

#include <lib/gui/listbox.h>
#include <lib/socket/socket.h>

 class ENgrab: public eWindow
 {
		 eListBox<eListBoxEntryMenu> lb;
         void onBackSelected();
         eString startxml();
         eString stoppxml();
        // void nsetup();
		 void sending(eString sxml);
		 void userxmls();
		 void NgrabMenue();
 public:
 		 void sendstart();
 		 void sendstopp();
		 ENgrab();
         ~ENgrab();
		 eSocket *sd;

 };

#endif
