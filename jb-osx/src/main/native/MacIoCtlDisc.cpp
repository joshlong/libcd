
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


#include "MacIoCtlDisc.h"
#include <CoreFoundation/CoreFoundation.h> 
#include <DiscRecording/DiscRecording.h> 
#include <IOKit/IOBSD.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/storage/IOCDMedia.h>
#include <IOKit/storage/IOCDMediaBSDClient.h>
#include <IOKit/storage/IOCDTypes.h>
#include <IOKit/storage/IOMedia.h>
#include <IOKit/storage/IOMediaBSDClient.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <math.h>
#include <paths.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <IOKit/storage/IOCDTypes.h>
#include <unistd.h>

#include <interface/cdda_interface.h>
#include <paranoia/cdda_paranoia.h>
#include <paranoia/utils.h>;


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
 

// from cdparanoi/header.h
extern void WriteWav(int f,long bytes); 

// 
void ReadTrackWriteCallback(long inpos, int function){ } 


void  CMacIoCtlDisc::ReadTrackToWavFile( unsigned track, char * whereToDumpWavData)   
{ 	
	int paranoia_mode=PARANOIA_MODE_FULL^PARANOIA_MODE_NEVERSKIP; 
	
	cdrom_drive *d=NULL;
	cdrom_paranoia *p=NULL; 
	

	d=cdda_identify("/dev/cdrom",CDDA_MESSAGE_PRINTIT,NULL); 
	  cdda_verbose_set(d,CDDA_MESSAGE_PRINTIT,CDDA_MESSAGE_PRINTIT);
	
	// the trick is to figure out the first and last sector for tracks 
	long first_sector;
    long last_sector;
	
	long batch_first=first_sector;
	long batch_last=last_sector;
	int sample_offset=0;
	int offset_skip=sample_offset*4;
	unsigned result =cdda_open(d) ;
	if(result == 0){
		
		//display_toc(d); // fixme:this should be only done if we have some sort of verbose setting.
		
		// first_sector=cdda_disc_firstsector(d);
		first_sector=cdda_track_firstsector(d ,track) ;
		last_sector = cdda_track_lastsector(d, track) ;
		
		
		int track1=cdda_sector_gettrack(d,first_sector);
		int track2=cdda_sector_gettrack(d,last_sector);
		
		//long off1=first_sector-cdda_track_firstsector(d,track1);
		//long off2=last_sector-cdda_track_firstsector(d,track2);
		
		int i;	
		
		for(i=track1;i<=track2;i++){
			if(!cdda_track_audiop(d,i)) 
			{
				puts ("Selected span contains non audio tracks.  Aborting.\n\n");
				exit(1);
			}
		}
		
		p=paranoia_init(d);
		
		paranoia_modeset(p,paranoia_mode);
		long cursor;
		int16_t offset_buffer[1176];
		int offset_buffer_used=0;
//		int offset_skip=sample_offset*4;
		
		paranoia_seek(p,cursor=first_sector,SEEK_SET);      
		
		
		seteuid(getuid());
		setegid(getgid());
		
		int out=open( whereToDumpWavData ,O_RDWR|O_CREAT|O_TRUNC,0666);
		
		WriteWav(out,(batch_last-batch_first+1)*CD_FRAMESIZE_RAW);

		int output_endian=0;		
		int max_retries = 5;
		
		/// now we actually do the writes
		int skipped_flag=0;
		while(cursor<=batch_last){
			/* read a sector */
			int16_t *readbuf=paranoia_read_limited(p,ReadTrackWriteCallback,max_retries);
			char *err=cdda_errors(d);
			char *mes=cdda_messages(d);
			
			if(mes || err)
				fprintf(stderr,"\r                               "
						"                                           \r%s%s\n",
						mes?mes:"",err?err:"");
			
			if(err)free(err);
			if(mes)free(mes);
			if(readbuf==NULL){
				/* if(errno==EBADF || errno==ENOMEDIUM){
				 report("\nparanoia_read: CDROM drive unavailable, bailing.\n");
				 exit(1);
				 }
				 skipped_flag=1;
				 report("\nparanoia_read: Unrecoverable error, bailing.\n");
*/
				puts( "Error!!" );
				break;
			}
			if(skipped_flag  ){
//				cursor=batch_last+1;
//				break;
			}
			
			skipped_flag=0;
			cursor++;
			

			
			if(output_endian!=bigendianp()){
				int i;
				for(i=0;i<CD_FRAMESIZE_RAW/2;i++)readbuf[i]=swap16(readbuf[i]);
			}
			
			ReadTrackWriteCallback(cursor*(CD_FRAMEWORDS)-1,-2);
			
			if(buffering_write(out,((char *)readbuf)+offset_skip,
							   CD_FRAMESIZE_RAW-offset_skip)){
				printf  ("Error writing output: %s",strerror(errno));
				exit(1);
			}
			offset_skip=0;
			
			if(output_endian!=bigendianp()){
				int i;
				for(i=0;i<CD_FRAMESIZE_RAW/2;i++)readbuf[i]=swap16(readbuf[i]);
			}
			
			/* One last bit of silliness to deal with sample offsets */
			if(sample_offset && cursor>batch_last){
				int i;
				/* read a sector and output the partial offset.  Save the
				 rest for the next batch iteration */
				readbuf=paranoia_read_limited(p,ReadTrackWriteCallback,max_retries);
				err=cdda_errors(d);mes=cdda_messages(d);
				
				if(mes || err)
					fprintf(stderr,"\r                               "
							"                                           \r%s%s\n",
							mes?mes:"",err?err:"");
				
				if(err)free(err);if(mes)free(mes);
				if(readbuf==NULL){
					skipped_flag=1;
					puts ("\nparanoia_read: Unrecoverable error reading through "
						   "sample_offset shift\n\tat end of track, bailing.\n");
					break;
				}
				//if(skipped_flag && abort_on_skip)break;
				skipped_flag=0;
				/* do not move the cursor */
				
				if(output_endian!=bigendianp())
					for(i=0;i<CD_FRAMESIZE_RAW/2;i++)
						offset_buffer[i]=swap16(readbuf[i]);
				else
					memcpy(offset_buffer,readbuf,CD_FRAMESIZE_RAW);
				offset_buffer_used=sample_offset*4;
				
				ReadTrackWriteCallback(cursor*(CD_FRAMEWORDS),-2);
				
				if(buffering_write(out,(char *)offset_buffer,
								   offset_buffer_used)){
					printf ("Error writing output: %s",strerror(errno));
					exit(1);
				}
			}
		}
	}
}
		
		
 