#ifndef IOCTLDISC_H
#define IOCTLDISC_H

#include <IOKit/storage/IOCDTypes.h>

class CDisc
{
public:
	virtual unsigned GetTrackCount() const = 0;
	virtual unsigned long GetStartFrame(unsigned track) const = 0;
	virtual unsigned long GetFrames(unsigned track) const = 0;
	
	virtual ~CDisc() { };
};

class CMacIoCtlDisc : public CDisc
{
public:
  CMacIoCtlDisc(const char *device = "/dev/rdisk1");
  virtual ~CMacIoCtlDisc();
  virtual char* DiscId() ;
  virtual unsigned GetTrackCount() const;
  virtual unsigned long GetStartFrame(unsigned track) const;
  virtual unsigned long GetFrames(unsigned track) const;

private:
  CDTOC *m_pToc;
  unsigned int m_track_count;
  static const unsigned int max_buffer = 1536;
};

#endif // IOCTLDISC_H
