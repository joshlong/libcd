
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

 
 http://www.oramind.com/index.php?option=com_content&view=article&id=91:installing-portable-libraries-on-os-x&catid=18:programming-articles&Itemid=39
 http://www.gnu.org/software/libcdio/libcdio.html 
 */

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
#include <sys/types.h>
#include "libcdda.h"
#include <stdio.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif

 
 

int main(int argc, const char *argv[])
{ 
	
	
	const char * device_name = "disk1" ;  
	char *cddb_id = disc_id(  device_name );
	printf( "the disc id is %s \n", cddb_id);
	
	
	const char * raw_device_path = get_raw_device_path( device_name) ; // /dev/rdisk1				
	printf( "the device_name is %s, and the raw_device_path is %s \n", device_name, raw_device_path) ;	

	int tracks = track_count( device_name) ; 
	printf ("there are %d tracks.\n", tracks) ; 
	
	

	
 /*
  
  io_iterator_t	mediaIterator;
  char bsdPath[ MAXPATHLEN ];
  
  
  FindEjectableCDMedia(&mediaIterator);
  GetBSDPath(mediaIterator, bsdPath, sizeof(bsdPath));
  
  
  
  COSDisc * cdInDrive = new COSDisc( bsdPath ); // this is just 'disk1' 
  
  char * buffer =  	cdInDrive->DiscId( );
  printf ( "the DISCID is: %s. \n", buffer ) ;	
  free(buffer) ;
  
  
  cdInDrive->ReadTrackToWavFile( 2, "/Users/jolong/Desktop/2.wav" ) ;
  
  // no need cdInDrive->ForceOpenOrEject();
  
  
  if (mediaIterator) {
  IOObjectRelease(mediaIterator);
  mediaIterator = IO_OBJECT_NULL; 
  }
  delete cdInDrive;
  return 0;
  */
	
}  