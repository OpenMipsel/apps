#ifndef DISABLE_FILE

#ifndef __lib_codecs_codecmpg_h
#define __lib_codecs_codecmpg_h

#include <map>
#include <lib/base/buffer.h>
#include <lib/codecs/codec.h>

// mpeg-2 ps demuxer.
class eMPEGDemux: public eAudioDecoder
{
	eIOBuffer &input, &video, &audio;
	int minFrameLength;
	int mpegtype;
	unsigned int curAudioStreamID;

	unsigned long last, remaining;
	unsigned long getLong();
	void refill();
	unsigned long getBits(unsigned int num);
	void syncBits();
	std::map<int,int> audiostreams;
	int validpackets;
	int lastAudioPTS, lastVideoPTS;
	int synced;
//	FILE *fAudio, *fVideo;
public:
	eMPEGDemux(eIOBuffer &input, eIOBuffer &video, eIOBuffer &audio);
	~eMPEGDemux()
	{
//		fclose(fAudio);
//		fclose(fVideo);
	}
	virtual int decodeMore(int last, int maxsamples, Signal1<void, unsigned int>*newastreamid=0 ); // returns number of samples(!) written to IOBuffer (out)
	virtual void resync(); // clear (secondary) decoder buffers
	virtual int getMinimumFramelength();
	virtual int getAverageBitrate();
	void setAudioStream( unsigned int id );
};

#endif

#endif //DISABLE_FILE
