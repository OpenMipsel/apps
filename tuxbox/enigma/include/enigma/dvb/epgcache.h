#ifndef __epgcache_h_
#define __epgcache_h_

#include <vector>
#include <list>
#include <ext/hash_map>
#include <ext/stl_hash_fun.h>

#include "si.h"
#include "dvb.h"
#include "edvb.h"

#define CLEAN_INTERVAL 60000    //  1 min
#define UPDATE_INTERVAL 3600000  // 60 min
#define ZAP_DELAY 4000          // 4 sek

class eventData;
class eServiceReferenceDVB;

struct uniqueEPGKey
{
	int sid, onid;
	uniqueEPGKey()
		:sid(0), onid(0)
	{
	}
	uniqueEPGKey( int sid, int onid )
		:sid(sid), onid(onid)
	{
	}
	struct equal
	{
		bool operator()(const uniqueEPGKey &a, const uniqueEPGKey &b) const
		{
			return (a.sid == b.sid && a.onid == b.onid);
		}
	};
};

#define eventMap std::map<int, eventData*>

#if defined(__GNUC__) && __GNUC__ >= 3 && __GNUC_MINOR__ >= 1  // check if gcc version >= 3.1
	#define eventCache __gnu_cxx::hash_map<uniqueEPGKey, eventMap, __gnu_cxx::hash<uniqueEPGKey>, uniqueEPGKey::equal>
	#define updateMap __gnu_cxx::hash_map<uniqueEPGKey, time_t, __gnu_cxx::hash<uniqueEPGKey>, uniqueEPGKey::equal >
	namespace __gnu_cxx
#else																													// for older gcc use following
	#define eventCache std::hash_map<uniqueEPGKey, eventMap, std::hash<uniqueEPGKey>, uniqueEPGKey::equalONIDSID >
	#define updateMap std::hash_map<uniqueEPGKey, time_t, std::hash<uniqueEPGKey>, uniqueEPGKey::equalONIDSID >
	namespace std
#endif
{
struct hash<uniqueEPGKey>
{
	inline size_t operator()(const uniqueEPGKey &x) const
	{
		int v=(x.onid^x.sid);
		v^=v>>8;
		return v&0xFF;
	}
};
}

class eventData
{
public:
	enum TYP {SCHEDULE, NOWNEXT};
private:
	__u8* EITdata;
	int ByteSize;
public:
	TYP type;
	static int CacheSize;
	eventData(const eit_event_struct* e, int size, enum TYP t)
	:ByteSize(size), type(t)
	{
		CacheSize+=size;
		EITdata = new __u8[size];
		memcpy(EITdata, (__u8*) e, size);
	}
	~eventData()
	{
		CacheSize-=ByteSize;
		delete [] EITdata;
	}	
	operator const eit_event_struct*() const
	{
		return (const eit_event_struct*) EITdata;
	}
	const eit_event_struct* get() const
	{
		return (const eit_event_struct*) EITdata;
	}
};

class eEPGCache;

class eSchedule: public eSection
{
	friend class eEPGCache;
	inline int sectionRead(__u8 *data);
	eSchedule()  // 0x50, Filter 0xF0
		:eSection(0x12, 80, -1, -1, SECREAD_CRC|SECREAD_NOTIMEOUT, 240)
	{
	}
};

class eNowNext: public eSection
{
	friend class eEPGCache;
	inline int sectionRead(__u8 *data);
	eNowNext()  // 0x4E, 0x4F
		:eSection(0x12, 78 , -1, -1, SECREAD_CRC|SECREAD_NOTIMEOUT, 254)
	{
	}
};

class eEPGCache: public Object
{
	friend class eSchedule;
	friend class eNowNext;
private:
	uniqueEPGKey current_service;
	int current_sid;
	int firstScheduleEventId;
	int firstNowNextEventId;
	int isRunning;
	int sectionRead(__u8 *data, eventData::TYP type);
	static eEPGCache *instance;

	eventCache eventDB;
	updateMap serviceLastUpdated;
	updateMap temp;

	eSchedule scheduleReader;
	eNowNext nownextReader;
	eTimer CleanTimer;
	eTimer zapTimer;
public:
	void startEPG();
	void stopEPG(const eServiceReferenceDVB &);
	void enterService(const eServiceReferenceDVB &, int);
	void cleanLoop();
	void timeUpdated();
public:
	eEPGCache();
	~eEPGCache();
	static eEPGCache *getInstance() { return instance; }
//	EITEvent *lookupEvent(const eServiceReferenceDVB &service, int event_id);
	EITEvent *lookupCurrentEvent(const eServiceReferenceDVB &service);
	inline const eventMap* eEPGCache::getEventMap(const eServiceReferenceDVB &service);

	Signal1<void, bool> EPGAvail;
	Signal1<void, const updateMap*> EPGUpdated;
};

inline const eventMap* eEPGCache::getEventMap(const eServiceReferenceDVB &service)
{
	eventCache::iterator It = eventDB.find( uniqueEPGKey( service.getServiceID().get(), service.getOriginalNetworkID().get() ) );
	return (It != eventDB.end())?(&(It->second)):0;
}

inline int eNowNext::sectionRead(__u8 *data)
{
	return eEPGCache::getInstance()->sectionRead(data, eventData::NOWNEXT);
}

inline int eSchedule::sectionRead(__u8 *data)
{
	return eEPGCache::getInstance()->sectionRead(data, eventData::SCHEDULE);
}

#endif
