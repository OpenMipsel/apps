#include <plugin.h>
#include <stdio.h>
#include <lib/gui/emessage.h>

extern "C" int plugin_exec( PluginParam *par );

int plugin_exec( PluginParam *par )
{
	eMessageBox hello("hello world.", "BLAAA :)");
	
	hello.show();
	hello.exec();
	hello.hide();
	
	return 0;
}
