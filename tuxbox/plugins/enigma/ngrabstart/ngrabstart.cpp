#include <plugin.h>
#include <stdio.h>
#include <src/enigma_main.h>

	// our plugin entry point, declared to use C calling convention
extern "C" int plugin_exec( PluginParam *par );

	// our entry point.
int plugin_exec( PluginParam *par )
{
	eZapMain::getInstance()->startNGrabRecord();	
	return 0;
}
