
/**  
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 @author Josh Long (sort of)
 
 The very large majority of this code is based on, or derived from works like DISCID and libcdio
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <IOKit/storage/IOCDMediaBSDClient.h>
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


/**
 
 Some nomenclature: 
  - LBA : absolute disc offset (e.g, the beginning of a hard disk)
  - LSN : Logical sector number: an offset relative to a logical barrier, like a disc partition. 
		  Different from a physical barrier, like an LBA
 
 Thus, it's possible to have multiple LSNs inside the range of values for an LBA etc.


 */

const unsigned MSF_OFFSET = 150;
const unsigned FRAMES_PER_SECOND = 75;

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
	if( s ==noErr){
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
	
	deviceName =(char*) malloc(PATH_MAX);
	strcpy(deviceName, device) ;
	char *rawName  = RawPath () ;
   // make sure that we have a copy of the dvice name locally
	//strcpy(deviceName, device);
  int drive = open( rawName, O_RDONLY);
  free(rawName)  ;  
	
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
	
    char * bsdPath =(char*) malloc (  PATH_MAX );
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

void CMacIoCtlDisc::ForceOpenOrEject () 
{
   EjectOrOpenCdDrive(deviceName); 
}

Boolean CMacIoCtlDisc::TestForDisc() 
{ 
   return hasCd( deviceName ); 
}

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


	
	/// READ TRACK
	//   lifted straight from the libcdio examples 

#include <fcntl.h>
#include <stdio.h>
#define writestr(fd, s) write(fd, s, sizeof(s)-1)  /* Subtract 1 for trailing '\0'. */

static void put_num(long int num, int f, int bytes)
{
	unsigned int i;
	unsigned char c;
	for (i=0; bytes--; i++) {
		c = (num >> (i<<3)) & 0xff;
		if (write(f, &c, 1)==-1) {
			perror("Could not write to output.");
			exit(1);
		}
	}
}



	/* Write a the header for a WAV file. */
	static void write_WAV_header(int fd, int32_t i_bytecount){
		/* quick and dirty */
		writestr(fd, "RIFF");              /*  0-3 */
		put_num(i_bytecount+44-8, fd, 4);  /*  4-7 */
		writestr(fd, "WAVEfmt ");          /*  8-15 */
		put_num(16, fd, 4);                /* 16-19 */
		put_num(1, fd, 2);                 /* 20-21 */
		put_num(2, fd, 2);                 /* 22-23 */
		put_num(44100, fd, 4);             /* 24-27 */
		put_num(44100*2*2, fd, 4);         /* 28-31 */
		put_num(4, fd, 2);                 /* 32-33 */
		put_num(16, fd, 2);                /* 34-35 */
		writestr(fd, "data");              /* 36-39 */
		put_num(i_bytecount, fd, 4);       /* 40-43 */
	}


 
u_int32_t BlockSizeForDevice(int fd)
{
	 u_int32_t	blockSize;
	// This ioctl call retrieves the preferred block size for the media. It is functionally
    // equivalent to getting the value of the whole media object's "Preferred Block Size"
    // property from the IORegistry.
    if (ioctl( fd , DKIOCGETBLOCKSIZE, &blockSize)) {
        perror("Error getting preferred block size");
        
        // Set a reasonable default if we can't get the actual preferred block size. A real
        // app would probably want to bail at this point.
        blockSize = kCDSectorSizeCDDA;
    }
    

	
	return blockSize ;
}

// Given the file descriptor for a whole-media CD device, read a sector from the drive.
// Return true if successful, otherwise false.
Boolean ReadSector(int fileDescriptor)
{
	
	///dk_cd_read_t cd_read;
	
    void		*buffer;
    ssize_t		numBytes;
    u_int32_t	blockSize = BlockSizeForDevice(fileDescriptor);
    
	printf("Media has block size of %d bytes.\n", blockSize);
    
    // Allocate a buffer of the preferred block size. In a real application, performance
    // can be improved by reading as many blocks at once as you can.
    buffer =  malloc(blockSize);
    
    // Do the read. Note that we use read() here, not fread(), since this is a raw device
    // node.
    numBytes = read(fileDescriptor, buffer, blockSize);
	
    // Free our buffer. Of course, a real app would do something useful with the data first.
    free(buffer);
  //  CDConvertMSFToLBA
    return numBytes == blockSize ? true : false;
}
 


void  CMacIoCtlDisc::ReadTrackToWavFile( unsigned track, char * whereToDumpWavData)   
{
	// FIXME repl w/ self->deviceName
	
	char *rawDeviceName = RawPath() ; 
	int in= open( rawDeviceName  , O_RDONLY); 
	int out=  creat ( whereToDumpWavData, 0644); 
	
	
	if( in != -1)
	{
		
		
		//lsn_t l ;
		
		write_WAV_header(out,  1024);
		
		CDTOC *toc = m_pToc; // weve already got it defined, lets use it to iterate over CDDA
		
		
		
		
		
		
		if(out!=-1) 
			close(out);		
		
		if(in!=-1)
			close(in);
		
	}
	
}

 