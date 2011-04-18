#ifndef IOCTLDISC_H
#define IOCTLDISC_H

#include "Disc.h"
#include <IOKit/storage/IOCDTypes.h>


class CMacIoCtlDisc: public CDisc {
public:
	CMacIoCtlDisc(const char *device  );
	virtual ~CMacIoCtlDisc();

	virtual unsigned GetTrackCount() const;
	virtual unsigned long GetStartFrame(unsigned track) const;
	virtual unsigned long GetFrames(unsigned track) const;

private:
	CDTOC *m_pToc;
	unsigned int m_track_count;
	static const unsigned int max_buffer = 1536;
};

#endif // IOCTLDISC_H
