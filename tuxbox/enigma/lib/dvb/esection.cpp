#include <lib/dvb/esection.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <sys/ioctl.h>

#include <lib/base/ebase.h>
#include <lib/base/eerror.h>

#include <ost/dmx.h>

ePtrList<eSection> eSection::active;

eSectionReader::eSectionReader()
{
	handle=-1;
}

int eSectionReader::getHandle()
{
	return handle;
}

void eSectionReader::close()
{
	if (handle != -1)
		::close(handle);
	handle=-1;
}

int eSectionReader::open(int pid, __u8 *data, __u8 *mask, int len, int _flags, const char* dmxdev)
{
	flags=flags;
	dmxSctFilterParams secFilterParams;
	close();

	handle=::open(dmxdev, O_RDWR|O_NONBLOCK);
	eDebug("opened handle %d", handle);
	if (handle<0)
	{
		perror(dmxdev);
		return -errno;
	}

	secFilterParams.pid=pid;

	const int maxsize=DMX_FILTER_SIZE;
	memset(secFilterParams.filter.filter, 0, maxsize);
	memset(secFilterParams.filter.mask, 0, maxsize);

	secFilterParams.timeout=0;
	secFilterParams.flags=DMX_IMMEDIATE_START;
	if (flags&SECREAD_CRC)
		secFilterParams.flags|=DMX_CHECK_CRC;
	for (int i = 0; i < DMX_FILTER_SIZE; i++)
	{
		secFilterParams.filter.filter[i]=i<len?data[i]:0;
		secFilterParams.filter.mask[i]=i<len?mask[i]:0;
	}

#ifdef ESECTION_DEBUG
	eDebugNoNewLine("%02x: ", pid);
	for (int i=0; i<DMX_FILTER_SIZE; i++)
		eDebugNoNewLine("%02x ", secFilterParams.filter.filter[i]);
	eDebugNoNewLine(" (napi)\n    ");
	for (int i=0; i<DMX_FILTER_SIZE; i++)
		eDebugNoNewLine("%02x ", secFilterParams.filter.mask[i]);
	eDebug("");
#endif

	if (ioctl(handle, DMX_SET_FILTER, &secFilterParams) < 0)
	{	
		perror("DMX_SET_FILTER\n");
		::close(handle);
		return -1;
	}
	return 0;
}

int eSectionReader::read(__u8 *buf)
{
	if (::read(handle, buf, 3)<0)
	{
		if (errno==EAGAIN)
			return errno;
		perror("read section");
		return errno;
	}
	int seclen=0;
	seclen |= ((buf[1] & 0x0F) << 8);
	seclen |= (buf[2] & 0xFF);
	::read(handle, buf+3, seclen);
	seclen+=3;
	return 0;
}

eSection::eSection(int _pid, int _tableid, int _tableidext, int _version, int _flags, int _tableidmask)
	: pid(_pid), tableid(_tableid), tableidext(_tableidext), tableidmask(_tableidmask), flags(_flags), version(_version)
{
	notifier=0;
	section=0;
	lockcount=0;
	timer=new eTimer(eApp);
	CONNECT(timer->timeout, eSection::timeout);
}

eSection::eSection()
{
	timer=0;
	notifier=0;
	section=0;
	lockcount=0;
}

eSection::~eSection()
{
	if (timer)
		delete timer;
	timer=0;
	closeFilter();
	if (lockcount)
		eDebug("deleted still locked table");
}

int eSection::start( const char* dmxdev )
{
	if (timer && (version==-1) && !(flags&SECREAD_NOTIMEOUT))
		timer->start((pid==0x14)?60000:10000, true);
	return setFilter(pid, tableid, tableidext, version, dmxdev);
}

int eSection::setFilter(int pid, int tableid, int tableidext, int version, const char *dmxdev)
{
	closeFilter();
	__u8 data[4], mask[4];
	data[0]=tableid; mask[0]=tableidmask;
	data[1]=0; mask[1]=0;
	data[2]=0; mask[2]=0;
	data[3]=0; mask[3]=0;
	if (tableidext!=-1)
	{
		data[1]=tableidext>>8; mask[1]=0xFF;
		data[2]=tableidext&0xFF; mask[2]=0xFF;
	} 
	if (version!=-1)
	{
		data[3]=version; mask[3]=0xFF;
	}

	reader.open(pid, data, mask, 4, flags, dmxdev);
	if (reader.getHandle() < 0)
		return -ENOENT;

	if (!(flags&SECREAD_NOABORT))
		active.push_back(this);
	
	if (notifier)
		delete notifier;

	notifier=new eSocketNotifier(eApp, reader.getHandle(), eSocketNotifier::Read);

	CONNECT(notifier->activated, eSection::data);

	return 0;
}

void eSection::closeFilter()
{
	if (reader.getHandle()>0)
	{
		if (!(flags&SECREAD_NOABORT))
			active.remove(this);
		delete notifier;
		notifier=0;
		if (timer)
			timer->stop();
		reader.close();
	}
}

void eSection::data(int socket)
{
	int max = 100;
	while (max--)
	{
		if (lockcount)
			eDebug("eSection::data on locked section!");
		if (timer)
			timer->start(10000, true);
		if (reader.read(buf))
			break;
		maxsec=buf[7];

//		printf("%d/%d, we want %d  | service_id %04x | version %04x\n", buf[6], maxsec, section, (buf[3]<<8)|buf[4], buf[5]);

		if (flags&SECREAD_INORDER)
			version=buf[5];

		if ((!(flags&SECREAD_INORDER)) || (section==buf[6]))
		{
			int err;
			if ((err=sectionRead(buf)))
			{
				if (err>0)
					err=0;
				closeFilter();
				sectionFinish(err);
				return;
			}
			section=buf[6]+1;
		}

		if ((section>maxsec) && (flags&SECREAD_INORDER))		// last section?
		{
			closeFilter();										// stop feeding
			sectionFinish(0);
			break;
		}
	}
}

void eSection::timeout()
{
	eDebug("Section timeout");
	closeFilter();
	sectionFinish(-ETIMEDOUT);
}

int eSection::abort()
{
	if (reader.getHandle()>0)
	{
		closeFilter();
		sectionFinish(-ECANCELED);
		return 0;
	} else
		return -ENOENT;
}

int eSection::abortAll()
{
	while (active.begin() != active.end())
		active.begin()->abort();
	return 0;
}

int eSection::sectionRead(__u8 *data)
{
	return -1;
}

void eSection::sectionFinish(int)
{
}

int eSection::lock()
{
	return ++lockcount;
}

int eSection::unlock()
{
	if (lockcount)
		return lockcount--;
	else
		eDebug("unlocking while not locked");
	return 0;
}

void eTable::sectionFinish(int err)
{
	if (err)
		error=err;
	ready=1;
	/*emit*/ tableReady(error);
}

eTable::eTable(int pid, int tableid, int tableidext, int version): eSection(pid, tableid, tableidext, version, (pid==0x14)?0:(SECREAD_INORDER|SECREAD_CRC))
{
	error=0;
	ready=0;
}

eTable::eTable(): eSection()
{
	error=0;
	ready=0;
}

eTable *eTable::createNext()
{
	return 0;
}

void eAUGTable::slotTableReady(int error)
{
	getNext(error);
}
