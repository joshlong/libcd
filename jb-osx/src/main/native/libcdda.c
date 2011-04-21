
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
 
 The very large majority of the code in this project - the Linux and OSX implementations and 
 soon on the Windows implementation - is based on, or derived from reading (and USING!) works like 
 discid, libcdio, and cdparanoia.
 
 These projects are written by C programmers who are a million times more adept than I, and I am forever grateful
 that they shared their code. If you're reading this and find it helpful, consider showing your
 support to one of those projects.
 
 
 */



#include "libcdda.h"

#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

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
#include <cdda_interface.h>
#include <cdda_paranoia.h>
#include <utils.h>


#define MAX_BUFFER   1536
const unsigned MSF_OFFSET = 150;
const unsigned FRAMES_PER_SECOND = 75;
const unsigned int max_buffer = MAX_BUFFER ;



long blocking_write(int outf, char *buffer, long num){
	long words=0,temp;
	
	while(words<num){
		temp=write(outf,buffer+words,num-words);
		if(temp==-1 && errno!=EINTR && errno!=EAGAIN)
			return(-1);
		words+=temp;
	}
	return(0);
}



/// some foreward declarations for our utility functions 
int CddbSum(int n)
{
	int result = 0;
	while (n > 0)
	{
		result += n % 10;
		n /= 10;
	}
	
	return result;
}


// todo, this needs track_count, first CDTOCGetDescriptorCount gets an NPE
UInt32 GetStartFrame(CDTOC* toc,unsigned track)  
{
	return CDConvertMSFToClippedLBA(CDConvertTrackNumberToMSF((UInt8)track+1, toc)) + MSF_OFFSET;
}


static void PutNum(long num,int f,int endianness,int bytes){
	int i;
	unsigned char c;
	
	if(!endianness)
		i=0;
	else
		i=bytes-1;
	while(bytes--){
		c=(num>>(i<<3))&0xff;
		if(write(f,&c,1)==-1){
			perror("Could not write to output.");
			exit(1);
		}
		if(endianness)
			i--;
		else
			i++;
	}
}

void WriteWav(int f,long bytes){
	/* quick and dirty */
	
	write(f,"RIFF",4);               /*  0-3 */
	PutNum(bytes+44-8,f,0,4);        /*  4-7 */
	write(f,"WAVEfmt ",8);           /*  8-15 */
	PutNum(16,f,0,4);                /* 16-19 */
	PutNum(1,f,0,2);                 /* 20-21 */
	PutNum(2,f,0,2);                 /* 22-23 */
	PutNum(44100,f,0,4);             /* 24-27 */
	PutNum(44100*2*2,f,0,4);         /* 28-31 */
	PutNum(4,f,0,2);                 /* 32-33 */
	PutNum(16,f,0,2);                /* 34-35 */
	write(f,"data",4);               /* 36-39 */
	PutNum(bytes,f,0,4);             /* 40-43 */
}

void WriteAiff(int f,long bytes){
	long size=bytes+54;
	long frames=bytes/4;
	
	/* Again, quick and dirty */
	
	write(f,"FORM",4);             /*  4 */
	PutNum(size-8,f,1,4);          /*  8 */
	write(f,"AIFF",4);             /* 12 */
	
	write(f,"COMM",4);             /* 16 */
	PutNum(18,f,1,4);              /* 20 */
	PutNum(2,f,1,2);               /* 22 */
	PutNum(frames,f,1,4);          /* 26 */    
	PutNum(16,f,1,2);              /* 28 */
	write(f,"@\016\254D\0\0\0\0\0\0",10); /* 38 (44.100 as a float) */
	
	write(f,"SSND",4);             /* 42 */
	PutNum(bytes+8,f,1,4);         /* 46 */
	PutNum(0,f,1,4);               /* 50 */
	PutNum(0,f,1,4);               /* 54 */
	
}

void WriteAifc(int f,long bytes){
	long size=bytes+86;
	long frames=bytes/4;
	
	/* Again, quick and dirty */
	
	write(f,"FORM",4);             /*  4 */
	PutNum(size-8,f,1,4);          /*  8 */
	write(f,"AIFC",4);             /* 12 */
	write(f,"FVER",4);             /* 16 */
	PutNum(4,f,1,4);               /* 20 */
	PutNum(2726318400UL,f,1,4);    /* 24 */
	
	write(f,"COMM",4);             /* 28 */
	PutNum(38,f,1,4);              /* 32 */
	PutNum(2,f,1,2);               /* 34 */
	PutNum(frames,f,1,4);          /* 38 */    
	PutNum(16,f,1,2);              /* 40 */
	write(f,"@\016\254D\0\0\0\0\0\0",10); /* 50 (44.100 as a float) */
	
	write(f,"NONE",4);             /* 54 */
	PutNum(14,f,1,1);              /* 55 */
	write(f,"not compressed",14);  /* 69 */
	PutNum(0,f,1,1);               /* 70 */
	
	write(f,"SSND",4);             /* 74 */
	PutNum(bytes+8,f,1,4);         /* 78 */
	PutNum(0,f,1,4);               /* 82 */
	PutNum(0,f,1,4);               /* 86 */
	
}

unsigned long GetFrames(CDTOC* toc,unsigned m_track_count, unsigned track)  
{
	if (track + 1 == m_track_count)
		return GetStartFrame(toc,0xA1) - GetStartFrame(toc,track);
	else
		return GetStartFrame(toc,track + 1) - GetStartFrame(toc,track);
}

 


/**
 
 Some nomenclature: 
 - LBA : absolute disc offset (e.g, the beginning of a hard disk)
 - LSN : Logical sector number: an offset relative to a logical barrier, like a disc partition. 
 Different from a physical barrier, like an LBA
 
 Thus, it's possible to have multiple LSNs inside the range of values for an LBA etc.
 
 
 */
 




void ReadTrackWriteCallback(long inpos, int function)
{ 
	// this is used (for example) to display progress... 
	// but since we don't care, then we won't use it.
} 

void  read_track_to_wav_file( char * deviceName,
						  unsigned track, 
						char * whereToDumpWavData)   
{ 	
	int paranoia_mode=PARANOIA_MODE_FULL^PARANOIA_MODE_NEVERSKIP; 

	long first_sector;
    long last_sector;
    long batch_first;
    long batch_last;
    //int batch_track;
	
	cdrom_drive *d=NULL;
	cdrom_paranoia *p=NULL; 
	
	d=cdda_find_a_cdrom(1,NULL);	
	cdda_verbose_set(d,CDDA_MESSAGE_PRINTIT,CDDA_MESSAGE_PRINTIT);
	
	 	
	int sample_offset=0;
	int offset_skip=sample_offset*4;
	unsigned result =cdda_open(d) ;
	
	
	if(result == 0){
		
		//display_toc(d); // fixme:this should be only done if we have some sort of verbose setting.
		
		// first_sector=cdda_disc_firstsector(d);
		first_sector= cdda_track_firstsector(d ,track) ;
		last_sector =  cdda_track_lastsector(d,cdda_sector_gettrack(d,first_sector));
		
		batch_first = first_sector;
		batch_last = last_sector;
		
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
				 	cursor=batch_last+1;
				//				break;
			}
			
			skipped_flag=0;
			cursor++;
			
			
			
			if(output_endian!=bigendianp()){
				int i;
				for(i=0;i<CD_FRAMESIZE_RAW/2;i++) 
					readbuf[i]=swap16(readbuf[i]);
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


/* Eliminate teeny little writes.  patch submitted by 
 Rob Ross <rbross@parl.ces.clemson.edu> --Monty 19991008 */



#define OUTBUFSZ 32*1024

extern long blocking_write(int outf, char *buffer, long num);


/* GLOBALS FOR BUFFERING CALLS */
static int  bw_fd  = -1;
static long bw_pos = 0;
static char bw_outbuf[OUTBUFSZ];


/* buffering_write() - buffers data to a specified size before writing.
 *
 * Restrictions:
 * - MUST CALL BUFFERING_CLOSE() WHEN FINISHED!!!
 *
 */
long buffering_write(int fd, char *buffer, long num)
{
	if (fd != bw_fd) {
		/* clean up after buffering for some other file */
		if (bw_fd >= 0 && bw_pos > 0) {
			if (blocking_write(bw_fd, bw_outbuf, bw_pos)) {
				perror("write (in buffering_write, flushing)");
			}
		}
		bw_fd  = fd;
		bw_pos = 0;
	}
	
	if (bw_pos + num > OUTBUFSZ) {
		/* fill our buffer first, then write, then modify buffer and num */
		memcpy(&bw_outbuf[bw_pos], buffer, OUTBUFSZ - bw_pos);
		if (blocking_write(fd, bw_outbuf, OUTBUFSZ)) {
			perror("write (in buffering_write, full buffer)");
			return(-1);
		}
		num -= (OUTBUFSZ - bw_pos);
		buffer += (OUTBUFSZ - bw_pos);
		bw_pos = 0;
	}
	/* save data */
	if(buffer && num)
		memcpy(&bw_outbuf[bw_pos], buffer, num);
	bw_pos += num;
	
	return(0);
}

/* buffering_close() - writes out remaining buffered data before closing
 * file.
 *
 */
int buffering_close(int fd)
{
	if (fd == bw_fd && bw_pos > 0) {
		/* write out remaining data and clean up */
		if (blocking_write(fd, bw_outbuf, bw_pos)) {
			perror("write (in buffering_close)");
		}
		bw_fd  = -1;
		bw_pos = 0;
	}
	return(close(fd));
}


/////////////////////


unsigned int _toc_for_device(CDTOC *ptrToTocObj, char* did, char  * deviceName)
{ 
	
	/* CDTOC */
	CDTOC* m_pToc = NULL ;
	unsigned m_track_count = 0;		
 	char *rawName  = get_raw_device_path( deviceName) ;
	int drive = open( rawName, O_RDONLY);
	free(rawName)  ;  
	
	dk_cd_read_toc_t header;

	u_int8_t *buffer = (u_int8_t*) malloc( sizeof( u_int8_t) * max_buffer) ; 
	memset( buffer,0,max_buffer);	
	memset(&header, 0, sizeof(header));
	
	header.format = kCDTOCFormatTOC;
	header.formatAsTime = 1;
	header.address.session = 0;
	header.bufferLength = max_buffer;
	header.buffer = buffer;    
	
	if (ioctl(drive, DKIOCCDREADTOC, &header) >= 0)
	{
		unsigned int count = ((CDTOC *)header.buffer)->length + 2;

		m_pToc =(CDTOC*) malloc(   sizeof(u_int8_t) * count ) ;				
		memcpy(m_pToc, header.buffer, count);						
		m_track_count = CDConvertTrackNumberToMSF(0xA1, m_pToc).minute;
		m_track_count -= CDConvertTrackNumberToMSF(0xA0, m_pToc).minute - 1;
	}
	else
	{
		fprintf(stderr, "Can not read toc\n");
	}
	free(buffer) ;		
	close(drive ) ;
	
	/* Disc ID */
	CDTOC* toc =m_pToc ;
	unsigned tc =m_track_count;
	if (tc > 0)
	{
		unsigned n = 0;
		for (unsigned track = 0; track < tc; ++track)
			n += CddbSum( GetStartFrame(toc,track) / FRAMES_PER_SECOND);
		
		unsigned start_sec = GetStartFrame(toc,0) / FRAMES_PER_SECOND;
		unsigned leadout_sec = ( GetStartFrame(toc,tc - 1) +  GetFrames(toc,tc,tc - 1)) / FRAMES_PER_SECOND;
		unsigned total = leadout_sec - start_sec;			
		unsigned id = ((n % 0xff) << 24 | total << 8 | tc);
		sprintf(did,"%08X", id );
	}
	return m_track_count  ;	

}


  int track_count( char * deviceName)
{
	CDTOC * toc = (CDTOC*) malloc( sizeof(u_int8_t) * max_buffer) ;
	char * did=(char*)malloc(20);
	unsigned tc =  _toc_for_device(  toc,  did, deviceName) ;  
	free(did) ;
	return tc ; 	 
	
	
} 

char * get_raw_device_path (char *  deviceName)
{ 
	char * bsdPath =(char*) malloc (  PATH_MAX );
	strcpy(bsdPath, _PATH_DEV); // '/dev/'
	strcat(bsdPath, "r");      // '/dev/r'
	strcat( bsdPath, deviceName) ;	
	return bsdPath;	
}


int open_cd_drive_door(char *bsdPath ) 
{ 
	CFStringRef  refToBsdPath =  CFStringCreateWithCString( NULL, bsdPath , kCFStringEncodingUTF8 )  ; 	
	DRDeviceRef cdDrive =DRDeviceCopyDeviceForBSDName( refToBsdPath );
	if( cdDrive != NULL)	{	
		return DRDeviceOpenTray( cdDrive ) ;
	}
	return -1 ;
	
}

char * disc_id(char * deviceName) 
{ 
    CDTOC * toc = (CDTOC*) malloc( sizeof(u_int8_t) * max_buffer) ;
	char * did=(char*)malloc(20);
	_toc_for_device(  toc,  did, deviceName) ;  
	return did ; 	 
}

//////////////////////////
/// TODO which of the following reams of code are useful? 
//////////////////////////


 

#ifndef IO_OBJECT_NULL
// This macro is defined in <IOKit/IOTypes.h> starting with Mac OS X 10.4.
#define	IO_OBJECT_NULL ((io_object_t) 0)
#endif

#ifndef ERROR
#define ERROR (-1)
#endif

#ifndef OK
#define OK 0
#endif


static kern_return_t FindEjectableCDMedia(io_iterator_t *mediaIterator);
static kern_return_t GetRawBSDPath(io_iterator_t mediaIterator, char *bsdPath, CFIndex maxPathSize);
static kern_return_t GetBSDPath(io_iterator_t mediaIterator, char *bsdPath, CFIndex maxPathSize);
static int OpenDrive(const char *bsdPath);
static Boolean ReadSector(int fileDescriptor);





// Returns an iterator across all CD media (class IOCDMedia). Caller is responsible for releasing
// the iterator when iteration is complete.
kern_return_t FindEjectableCDMedia(io_iterator_t *mediaIterator)
{
    kern_return_t			kernResult; 
    CFMutableDictionaryRef	classesToMatch;
	
	
    // CD media are instances of class kIOCDMediaClass
    classesToMatch = IOServiceMatching(kIOCDMediaClass); 
    if (classesToMatch == NULL) {
        printf("IOServiceMatching returned a NULL dictionary.\n");
    }
    else {
		
		
		CFDictionarySetValue(classesToMatch, CFSTR(kIOMediaEjectableKey), kCFBooleanTrue); 
        // Each IOMedia object has a property with key kIOMediaEjectableKey which is true if the
        // media is indeed ejectable. So add this property to the CFDictionary we're matching on. 
    }
    
	
    kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault, classesToMatch, mediaIterator);    
    if (KERN_SUCCESS != kernResult) {
        printf("IOServiceGetMatchingServices returned 0x%08x\n", kernResult);
    }
    
    return kernResult;
}


// simply returns something like 'disk1' 
kern_return_t GetBSDPath(io_iterator_t mediaIterator, char *bsdPath, CFIndex maxPathSize)
{
    io_object_t		nextMedia;
    kern_return_t	kernResult = KERN_FAILURE;
    
    *bsdPath = '\0';
    
    nextMedia = IOIteratorNext(mediaIterator);
    if (nextMedia) {
        CFStringRef	bsdPathAsCFString;
		
		
		char *fullPath;
		
        bsdPathAsCFString = (CFStringRef) IORegistryEntryCreateCFProperty(nextMedia,  CFSTR(kIOBSDNameKey),  kCFAllocatorDefault,  0);
		
        if (bsdPathAsCFString) { // we got a disk name (e.g., 'disk1') 
            size_t devPathLength; 
  			
            devPathLength = strlen(bsdPath);            
			
 			
			CFIndex bsdPathLength = CFStringGetLength(bsdPathAsCFString);
			fullPath = (char *)malloc( bsdPathLength + 1 ); // for null terminator
			Boolean conversionResult = CFStringGetCString(
														  bsdPathAsCFString, fullPath, bsdPathLength + 1, kCFStringEncodingUTF8 );
			
			if( conversionResult){
				strcat( bsdPath, fullPath) ;			
			} 			
			
			free( fullPath) ; //malloc'd 			
			
#ifdef DEBUG 
			printf ( "the bsd path is %s \n" , bsdPath) ;
#endif
			
            CFRelease(bsdPathAsCFString);
        }
        IOObjectRelease(nextMedia);
    }
    
    return kernResult;
}


// Given the file descriptor for a device, close that device.
void CloseDrive(int fileDescriptor)
{
    close(fileDescriptor);
}




OSStatus OpenCdDriveDoor( const char *bsdPath   )
{	
	CFStringRef  refToBsdPath =  CFStringCreateWithCString( NULL, bsdPath , kCFStringEncodingUTF8 )  ; 	
	DRDeviceRef cdDrive =DRDeviceCopyDeviceForBSDName( refToBsdPath );
	if( cdDrive != NULL)	{	
		return DRDeviceOpenTray( cdDrive ) ;
	}
	return -1 ;
}

OSStatus EjectCd( const char *bsdPath)
{
	CFStringRef  refToBsdPath =  CFStringCreateWithCString( NULL, bsdPath , kCFStringEncodingUTF8 )  ; 	
	DRDeviceRef cdDrive =DRDeviceCopyDeviceForBSDName( refToBsdPath );
	if(cdDrive != NULL)	{	
		return	DRDeviceEjectMedia(cdDrive)  ;
	} 
	return  -1 ;
}

OSStatus EjectOrOpenCdDrive(const char *bsdPath) 
{
	OSStatus s =OpenCdDriveDoor(bsdPath);
	if( s ==noErr){
		s = EjectCd(bsdPath);
	}
	return s; 
	
}
 

void CloseCdDriveDoor( const char *bsdPath)
{
	CFStringRef  refToBsdPath =  CFStringCreateWithCString( NULL, bsdPath , kCFStringEncodingUTF8 )  ; 	
	DRDeviceRef cdDrive =DRDeviceCopyDeviceForBSDName( refToBsdPath ); 
	DRDeviceCloseTray( cdDrive ) ;
} 




// used by DiscId 

/*
 
 
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

 */

 
  
 /*


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
	
 	long first_sector;
    long last_sector;
	
	long batch_first=first_sector;
	long batch_last=last_sector;
	int sample_offset=0;
	int offset_skip=sample_offset*4;
	unsigned result =cdda_open(d) ;
	if(result == 0){
		
 		first_sector=cdda_track_firstsector(d ,track) ;
		last_sector = cdda_track_lastsector(d, track) ;
		
		
		int track1=cdda_sector_gettrack(d,first_sector);
		int track2=cdda_sector_gettrack(d,last_sector);
		
 
		
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
				 
				puts( "Error!!" );
				break;
			}
			if(skipped_flag  ){
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
			
 			if(sample_offset && cursor>batch_last){
				int i;
 
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
 				skipped_flag=0;
		 
				
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
 

*/

