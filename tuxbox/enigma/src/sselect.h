#ifndef __sselect_h
#define __sselect_h

#include <lib/gui/ewindow.h>
#include <lib/gui/listbox.h>
#include <lib/gui/statusbar.h>
#include <lib/dvb/epgcache.h>
#include <src/channelinfo.h>

#include <lib/dvb/service.h>

class eService;
class eLabel;

class eListBoxEntryService: public eListBoxEntry
{
	friend class eServiceSelector;
	friend class eListBox<eListBoxEntryService>;
	friend struct moveFirstChar;
	friend struct moveServiceNum;
	friend struct _selectService;
	friend struct updateEPGChangedService;
	friend struct renumber;
	eString sort;
	static gFont serviceFont, descrFont, numberFont;
	static int maxNumSize;
	static gPixmap *folder, *marker, *locked;
	eTextPara *numPara, *namePara, *descrPara;
	int nameXOffs, descrXOffs, numYOffs, nameYOffs, descrYOffs;
	int flags;
	int num;
	int curEventId;
public:
	static eListBoxEntryService *selectedToMove;
	static std::set<eServiceReference> hilitedEntrys;
	int getNum() const { return num; }
	void invalidate();
	void invalidateDescr();
	static int getEntryHeight();
	eServiceReference service;
	enum { flagShowNumber=1, flagOwnNumber=2, flagIsReturn=4 };
	eListBoxEntryService(eListBox<eListBoxEntryService> *lb, const eServiceReference &service, int flags, int num=-1);
	~eListBoxEntryService();

	bool operator<(const eListBoxEntry &r) const
	{
		if (flags & flagIsReturn)
			return 1;
		else if (((eListBoxEntryService&)r).flags & flagIsReturn)
			return 0;
		else if (service.getSortKey() == ((eListBoxEntryService&)r).service.getSortKey())
			return sort < ((eListBoxEntryService&)r).sort;
		else // sort andersrum
			return service.getSortKey() > ((eListBoxEntryService&)r).service.getSortKey();
	}
protected:
	const eString &redraw(gPainter *rc, const eRect &rect, gColor, gColor, gColor, gColor, int hilited);
};

class eServiceSelector: public eWindow
{
	eServiceReference selected;
	eServiceReference *result;
	eListBox<eListBoxEntryService> *services, *bouquets;

	eLabel *key[4];
	const eWidget *rfocus;

	eChannelInfo* ci;

	eServicePath path;

	void addService(const eServiceReference &service);
	void addBouquet(const eServiceReference &service);
	int style, lastSelectedStyle;
	int serviceentryflags;

	char BrowseChar;
	eTimer BrowseTimer;
	eTimer ciDelay;

	eListBoxEntryService *goUpEntry;
protected:
	int eventHandler(const eWidgetEvent &event);
private:
	void pathUp();
	void fillServiceList(const eServiceReference &ref);
	void fillBouquetList(const eServiceReference &ref);
	void serviceSelected(eListBoxEntryService *entry);
	void bouquetSelected(eListBoxEntryService *entry);
	void serviceSelChanged(eListBoxEntryService *entry);
	void bouquetSelChanged( eListBoxEntryService *entry);
	void ResetBrowseChar();
	void gotoChar(char c);
	void EPGUpdated( const tmpMap* );
	void updateCi();
	void doSPFlags(const eServiceReference &ref);
public:
	void setKeyDescriptions(bool editMode=false);
	void forEachServiceRef( Signal1<void,const eServiceReference&>, bool );
	int movemode;
	int editMode;
	int plockmode;
	enum { styleInvalid, styleCombiColumn, styleSingleColumn, styleMultiColumn };
	enum { dirNo, dirUp, dirDown, dirFirst, dirLast };

	eServiceSelector();
	~eServiceSelector();

	Signal0<void> rotateRoot;

	enum { listAll, listSatellites, listProvider, listBouquets };
	Signal1<eServicePath,int> getRoot;

	Signal1<void,const eServiceReference &> addServiceToPlaylist; // add service to the Playlist
	Signal2<void,eServiceReference*,int> addServiceToUserBouquet;  // add current service to selected User Bouquet

	Signal1<void,int> setMode;        // set TV, Radio or File

	Signal1<void,eServiceSelector*>	removeServiceFromUserBouquet, // remove service from selected User Bouquet
																	showMenu, // shows the contextmenu
																	toggleStyle, // switch service selector style
																	renameService, renameBouquet,
																	deletePressed, newMarkerPressed;

	Signal3<void,
		const eServiceReference &, 		// path
		const eServiceReference &, 		// service to move
		const eServiceReference &			// service AFTER moved service
		> moveEntry;

	Signal0<void> showMultiEPG;
	
	Signal1<void,eServiceReferenceDVB>showEPGList;

	const eServicePath &getPath() { return path; }
	void setPath(const eServicePath &path, const eServiceReference &select=eServiceReference());

	int getStyle()	{ return style; }
	void setStyle(int newStyle=-1, bool force=false);
	void actualize();
	void toggleButtons();
	bool selectService(const eServiceReference &ref);
	bool selectService(int num);
	bool selectServiceRecursive( eServiceReference &ref );
	bool selServiceRec( eServiceReference &ref );
	int getServiceNum( const eServiceReference &ref);
	void enterDirectory(const eServiceReference &ref);
	const eServiceReference &getSelected() { return selected; }
	const eServiceReference *choose(int irc=-1);
	const eServiceReference *next();
	const eServiceReference *prev();
	void removeCurrent(bool=false);
	void invalidateCurrent(eServiceReference ref=eServiceReference());
	void updateNumbers();

	int toggleMoveMode();  // enable / disable move entry Mode ( only in userBouquets )
	int toggleEditMode();  // enable / disable edit UserBouquet Mode
};

#endif
