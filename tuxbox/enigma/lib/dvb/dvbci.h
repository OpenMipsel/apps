#ifndef __core_dvb_ci_h
#define __core_dvb_ci_h

#include <core/dvb/service.h>
//#include <core/base/buffer.h>

#include <core/base/ebase.h>
#include <core/base/thread.h>
#include <core/base/message.h>
#include <core/system/elock.h>

class eDVBCI: private eThread, public eMainloop, public Object
{
	enum
	{
		stateInit, stateError, statePlaying, statePause
	};
	int state;
	int fd;
	eSocketNotifier *ci;
	
public:
	struct eDVBCIMessage
	{
		enum
		{
			reset, 
			init,
			exit
		};
		int type;
		eDVBCIMessage() { }
		eDVBCIMessage(int type): type(type) { }
	};
	eFixedMessagePump<eDVBCIMessage> messages;
	
	void gotMessage(const eDVBCIMessage &message);

	void dataAvailable(int what);
	
	eDVBCI();
	~eDVBCI();
	
	void thread();
};
#endif
