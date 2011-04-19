 
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <paths.h>
#include <sys/param.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOBSD.h>
#include <IOKit/storage/IOMediaBSDClient.h>
#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IOCDTypes.h>
#include <CoreFoundation/CoreFoundation.h> 
#include <DiscRecording/DiscRecording.h> 
#include <IOKit/storage/IOCDTypes.h>

#include "MacIoCtlDisc.h"

const unsigned MSF_OFFSET = 150;
const unsigned FRAMES_PER_SECOND = 75;

 


/*
 * Opens the CD drive door on OS X. 
 *
 */
OSStatus OpenCdDriveDoor( const char *bsdPath   )
{	
	CFStringRef  refToBsdPath =  CFStringCreateWithCString( NULL, bsdPath , kCFStringEncodingUTF8 )  ; 	
	DRDeviceRef cdDrive =DRDeviceCopyDeviceForBSDName( refToBsdPath );
	if( cdDrive != NULL)	{	
		return DRDeviceOpenTray( cdDrive ) ;
	}
	return NULL ;
}

OSStatus EjectCd( const char *bsdPath)
{
	CFStringRef  refToBsdPath =  CFStringCreateWithCString( NULL, bsdPath , kCFStringEncodingUTF8 )  ; 	
	DRDeviceRef cdDrive =DRDeviceCopyDeviceForBSDName( refToBsdPath );
	if(cdDrive != NULL)	{	
		return	DRDeviceEjectMedia(cdDrive)  ;
	} 
	return NULL ;
}

OSStatus EjectOrOpenCdDrive(const char *bsdPath) 
{
	OSStatus s =OpenCdDriveDoor(bsdPath);
	if( s!=noErr){
		s = EjectCd(bsdPath);
	}
	return s; 
	
}

Boolean hasCd(const char *bsdPath) { 
	CMacIoCtlDisc * cdInDrive = new CMacIoCtlDisc( bsdPath);
	char * buffer = cdInDrive->DiscId();
	Boolean cd = (buffer !=NULL); // ie, no discid ==no cd 	
	free(buffer) ;
	delete cdInDrive;
	return cd; 
	
}

void CloseCdDriveDoor( const char *bsdPath)
{
	CFStringRef  refToBsdPath =  CFStringCreateWithCString( NULL, bsdPath , kCFStringEncodingUTF8 )  ; 	
	DRDeviceRef cdDrive =DRDeviceCopyDeviceForBSDName( refToBsdPath ); 
	 DRDeviceCloseTray( cdDrive ) ;
 } 




// used by DiscId 
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

CMacIoCtlDisc::CMacIoCtlDisc(const char *device)
  : m_pToc(NULL),
    m_track_count(0)
{
   // make sure that we have a copy of the dvice name locally
	strcpy(deviceName, device);
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



char *  CMacIoCtlDisc::Path(){ 
	return deviceName ; 
}

char  * CMacIoCtlDisc::RawPath()
{ 
	
    char bsdPath[ PATH_MAX ];
	strcpy(bsdPath, _PATH_DEV); // '/dev/'
	strcat(bsdPath, "r");      // '/dev/r'
	strcat( bsdPath, deviceName) ;	
	return bsdPath;
}


/**
 this returns the CDDB DISCID, which sucks, but its fairly commodotized... 
 */
char * CMacIoCtlDisc::DiscId() 
{
	unsigned track_count =  GetTrackCount();
	if (track_count > 0)
	{
		unsigned n = 0;
		for (unsigned track = 0; track < track_count; ++track)
			n += cddb_sum( GetStartFrame(track) / FRAMES_PER_SECOND);
		
		unsigned start_sec = GetStartFrame(0) / FRAMES_PER_SECOND;
		unsigned leadout_sec = ( GetStartFrame(track_count - 1) +  GetFrames(track_count - 1)) / FRAMES_PER_SECOND;
		unsigned total = leadout_sec - start_sec;			
		unsigned id = ((n % 0xff) << 24 | total << 8 | track_count);
		char * result=(char*) malloc( 20) ; 
		sprintf(result,"%08X", id );
			
		return result;
	}
	
	return NULL; 
	
}

void CMacIoCtlDisc::ForceOpenOrEject () { EjectOrOpenCdDrive(deviceName); }

Boolean CMacIoCtlDisc::TestForDisc() { return hasCd( deviceName ); }

CMacIoCtlDisc::~CMacIoCtlDisc()
{
  delete [] (u_int8_t *)m_pToc;
  delete deviceName;
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
