#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <IOKit/storage/IOCDMediaBSDClient.h>

#include "MacIoCtlDisc.h"

const unsigned MSF_OFFSET = 150;

CMacIoCtlDisc::CMacIoCtlDisc(const char *device)
  : m_pToc(NULL),
    m_track_count(0)
{
  int drive = open(device, O_RDONLY);
  if (drive >= 0)
  {
    dk_cd_read_toc_t header;
    u_int8_t *buffer = new u_int8_t[max_buffer];

    memset(&header, 0, sizeof(header));
    memset(buffer, 0, max_buffer);
    header.format = kCDTOCFormatTOC;
    header.formatAsTime = 1;
    header.address.session = 0;
    header.bufferLength = max_buffer;
    header.buffer = buffer;    

    if (ioctl(drive, DKIOCCDREADTOC, &header) >= 0)
    {
      unsigned int count = ((CDTOC *)header.buffer)->length + 2;
      m_pToc = (CDTOC *)new u_int8_t[count];
      memcpy(m_pToc, header.buffer, count);
      m_track_count = CDConvertTrackNumberToMSF(0xA1, m_pToc).minute;
      m_track_count -= CDConvertTrackNumberToMSF(0xA0, m_pToc).minute - 1;
    }
    else
    {
      fprintf(stderr, "Can not read toc\n");
    }

    delete [] buffer;
    close(drive);
  }
  else
    fprintf(stderr, "Can not open %s\n", device);
}

CMacIoCtlDisc::~CMacIoCtlDisc()
{
  delete [] (u_int8_t *)m_pToc;
  m_pToc = NULL;
}

unsigned CMacIoCtlDisc::GetTrackCount() const
{
  return m_track_count;
}

unsigned long CMacIoCtlDisc::GetStartFrame(unsigned track) const
{
  return CDConvertMSFToClippedLBA(CDConvertTrackNumberToMSF((UInt8)track+1, m_pToc)) + MSF_OFFSET;
}

unsigned long CMacIoCtlDisc::GetFrames(unsigned track) const
{
  if (track + 1 == m_track_count)
    return GetStartFrame(0xA1) - GetStartFrame(track);
  else
    return GetStartFrame(track + 1) - GetStartFrame(track);
}
