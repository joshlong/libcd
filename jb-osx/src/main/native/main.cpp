

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

#include "MacIoCtlDisc.h"


typedef CMacIoCtlDisc COSDisc; 

const unsigned FRAMES_PER_SECOND = 75;

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

// Given an iterator across a set of CD media, return the BSD path to the
// next one. If no CD media was found the path name is set to an empty string.
kern_return_t GetRawBSDPath(io_iterator_t mediaIterator, char *bsdPath, CFIndex maxPathSize)
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
            strcpy(bsdPath, _PATH_DEV); // '/dev/'
            
            // Add "r" before the BSD node name from the I/O Registry to specify the raw disk
            // node. The raw disk nodes receive I/O requests directly and do not go through
            // the buffer cache.
            

            strcat(bsdPath, "r");      // '/dev/r'
			
            devPathLength = strlen(bsdPath);            
			
			// printf("the dev path length: %s\n", bsdPath);
			
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
          //  strcpy(bsdPath, _PATH_DEV); // '/dev/'
            
            // Add "r" before the BSD node name from the I/O Registry to specify the raw disk
            // node. The raw disk nodes receive I/O requests directly and do not go through
            // the buffer cache.
            
			
            //strcat(bsdPath, "r");      // '/dev/r'
			
            devPathLength = strlen(bsdPath);            
			
			// printf("the dev path length: %s\n", bsdPath);
			
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

/*
// Given the path to a CD drive, open the drive.
// Return the file descriptor associated with the device.
int OpenDrive(const char *bsdPath)
{
    int	fileDescriptor;  
	 // dont look for non-removable media as root owns those. See <http://developer.apple.com/documentation/Security/Conceptual/authorization_concepts/index.html
    fileDescriptor = open(bsdPath, O_RDONLY);    
    if (fileDescriptor == -1) {
        printf("Error opening device %s: ", bsdPath);
        perror(NULL);
    }    
    return fileDescriptor;
} */
/*
// Given the file descriptor for a whole-media CD device, read a sector from the drive.
// Return true if successful, otherwise false.
Boolean ReadSector(int fileDescriptor)
{
    char		*buffer;
    ssize_t		numBytes;
    u_int32_t	blockSize;
    
    // This ioctl call retrieves the preferred block size for the media. It is functionally
    // equivalent to getting the value of the whole media object's "Preferred Block Size"
    // property from the IORegistry.
    if (ioctl(fileDescriptor, DKIOCGETBLOCKSIZE, &blockSize)) {
        perror("Error getting preferred block size");
        
        // Set a reasonable default if we can't get the actual preferred block size. A real
        // app would probably want to bail at this point.
        blockSize = kCDSectorSizeCDDA;
    }
    
    printf("Media has block size of %d bytes.\n", blockSize);
    
    // Allocate a buffer of the preferred block size. In a real application, performance
    // can be improved by reading as many blocks at once as you can.
    buffer = (char*) malloc(blockSize);
    
    // Do the read. Note that we use read() here, not fread(), since this is a raw device
    // node.
    numBytes = read(fileDescriptor, buffer, blockSize);
	
    // Free our buffer. Of course, a real app would do something useful with the data first.
    free(buffer);
    
    return numBytes == blockSize ? true : false;
}
*/
// Given the file descriptor for a device, close that device.
void CloseDrive(int fileDescriptor)
{
    close(fileDescriptor);
}


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
	COSDisc * cdInDrive = new COSDisc( bsdPath);

	char * buffer =  	cdInDrive->DiscId( );
	//printf ( "the DISCID is: %s. \n", buffer ) ;
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


int main(void)
{	
    io_iterator_t	mediaIterator;
    char bsdPath[ MAXPATHLEN ];
	char rawPath[MAXPATHLEN];
 
    FindEjectableCDMedia(&mediaIterator);
	GetBSDPath(mediaIterator, bsdPath, sizeof(bsdPath));
	
	// EjectCd(  bsdPath );
	if(EjectOrOpenCdDrive( bsdPath) == noErr){
		puts("just opened the drive door. " ) ;  
	
		CloseCdDriveDoor( bsdPath) ;  // has no effect on OSX and a sideloading notebook, for example
		//EjectCd( bsdPath);
		puts( "just closed the drive door. ") ;
	}
	
	// take 2, this time going after the rawPath
	FindEjectableCDMedia(&mediaIterator)  ;
	GetRawBSDPath(mediaIterator, rawPath, sizeof(rawPath));

	
	if(hasCd(rawPath)){
	puts( "there's a CDDA in the device already." ) ;
	}
	
	COSDisc * cdInDrive = new COSDisc( rawPath);
	char * buffer =  	cdInDrive->DiscId( );
	printf ( "the DISCID is: %s. \n", buffer ) ;
	free(buffer) ;

    if (mediaIterator) {
        IOObjectRelease(mediaIterator);
		mediaIterator = IO_OBJECT_NULL; 
    }
	delete cdInDrive;
    return 0;
}

