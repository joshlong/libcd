#include <stdio.h>
#include <string.h>
#include "discid.h"

// this is the only one we're including!
#include "MacIoCtlDisc.h"

typedef CMacIoCtlDisc COSDisc; 

#ifndef ERROR
#define ERROR (-1)
#endif

#ifndef OK
#define OK 0
#endif

const char *version = "discid 0.1.2";

static int cddb_sum(int n)
{
  int result = 0;
  while (n > 0)
  {
    result += n % 10;
    n /= 10;
  }

  return result;
}

static int discid(const char *p_device)
{
  int status = 0;
  CDisc *p_disc = NULL;

  if (p_device)
    p_disc = new COSDisc(p_device);


  if (p_disc)
  {
	
    unsigned track_count = p_disc->GetTrackCount();
    if (track_count > 0)
    {

      unsigned n = 0;

      for (unsigned track = 0; track < track_count; ++track)
        n += cddb_sum(p_disc->GetStartFrame(track) / FRAMES_PER_SECOND);

      unsigned start_sec = p_disc->GetStartFrame(0) / FRAMES_PER_SECOND;
      unsigned leadout_sec = (p_disc->GetStartFrame(track_count - 1) + p_disc->GetFrames(track_count - 1)) / FRAMES_PER_SECOND;
      unsigned total = leadout_sec - start_sec;
      unsigned id = ((n % 0xff) << 24 | total << 8 | track_count);

      printf("%08X %d", id, track_count);

      for (unsigned index = 0; index < track_count; ++index) 
		printf(" %ld", p_disc->GetStartFrame(index));

      unsigned long leadout_frame =
        p_disc->GetStartFrame(track_count - 1) + p_disc->GetFrames(track_count - 1);
      printf(" %ld\n", leadout_frame / FRAMES_PER_SECOND);
    }
    else
      status = ERROR;
  }
  else
    status = ERROR;

  delete p_disc;

  return status;
}

int main(int argc, char *argv[])
{
  return discid("/dev/rdisk1");
}
