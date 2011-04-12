/*
 * Copyright 2011 Josh Long
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


// http://www.parashift.com/c++-faq-lite/mixing-c-and-cpp.html -- very cool bit about mixing c / c++ together
// http://leapster.org/linux/cdrom/
// http://svn.icculus.org/quake2/trunk/src/linux/cd_linux.c?view=markup&pathrev=53
// http://www.ibm.com/developerworks/library/l-devctrl-migration/index.html

#ifdef __cplusplus
extern "C" {
#endif


#include <cdio/cdio.h>
#include <cdio/cd_types.h>
#include <cdio/disc.h>
#include <cdio/paranoia.h>
#include <cdio/device.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <limits.h>
#include <linux/cdrom.h>
#include <math.h>
#include <prlog.h>
#include <prlong.h>
#include <prthread.h>
#include <prtypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include "discid/discid.h"
#include <fstab.h>

/**
 * returns the details about a given device.
 *
 */
char * get_device_details(char * device_name) ;

/** return the devices that we can detect.
 **/
char ** list_device_names();

/***
 *  This isnt cross platform but nonetheless its very important
 */
cdrom_drive_t * get_cdrom_drive_by_device_name(char * cd_device);

/**
 * 
 *
 * Used to eject a cdrom device
 *
 * Possible values are: '/dev/sr0', '/dev/cdrom', etc.  *
 **/
PRStatus cdripper_eject_cd_tray(char * cd_device);

/**
 *  Used to close a cdrom device's tray
 *
 *
 * Possible values are: '/dev/sr0', '/dev/cdrom', etc.  *
 */
PRStatus cdripper_close_cd_tray(char * cd_device);

/**
 * Looking at the reqs
 */
PRInt32 cdripper_get_num_of_tracks(char * cd_device);

 /** tells us whether the cd itself is an audio
  * 	CD that we should bother with.
  */
PRBool cdripper_is_audio_cd( char * cd_device );



 /**
  * Tells us whether theres a CD loaded at all.
  */
PRBool cdripper_is_disc_loaded(char * cd_device) ;

 /**
  * Actually rip the track to some pointer on the file system.
  *
  * You can specify *device, or *generic_scsi_device, but no need to specify both.
  *
  */

PRBool cdripper_rip_track( char * device, int track, char * outfile, PRFloat64 * rip_percent_done ) ;

char * cdripper_get_discid(char * device) ;




#ifdef __cplusplus
}
#endif


