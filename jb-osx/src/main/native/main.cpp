

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




int main(void)
{	
    io_iterator_t	mediaIterator;
    char bsdPath[ MAXPATHLEN ];

 
    FindEjectableCDMedia(&mediaIterator);
	GetBSDPath(mediaIterator, bsdPath, sizeof(bsdPath));
	
	
	
	COSDisc * cdInDrive = new COSDisc( bsdPath ); // this is just 'disk1' 
	
	char * buffer =  	cdInDrive->DiscId( );
	printf ( "the DISCID is: %s. \n", buffer ) ;	
	free(buffer) ;
	
	// no need cdInDrive->ForceOpenOrEject();
	

    if (mediaIterator) {
        IOObjectRelease(mediaIterator);
		mediaIterator = IO_OBJECT_NULL; 
    }
	delete cdInDrive;
    return 0;
}

