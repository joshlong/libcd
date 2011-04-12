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
//http://leapster.org/linux/cdrom/
#include <unistd.h>
#include "libjukebox.h"
#include <fstab.h>
#include <prlog.h>
#include <FLAC/all.h>
#include <prlong.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <prthread.h>
#include <prtypes.h>//for the tests
#include <cdio/disc.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flac_encode.h"

////////////////////////////////////////////////////////////////////////////
/** CD specific operations */

int test_reading_from_drive(char * fnToWriteTo, char * device_name) {
	PRFloat64 rip_percent_done = 0;
	PRBool did_it_work = cdripper_rip_track(device_name, 2, fnToWriteTo,
			&rip_percent_done);
	printf("did it work: %s, percent done: %f \n",
			did_it_work == PR_TRUE ? "Y" : "N", rip_percent_done);
	return EXIT_SUCCESS;
}

int test_discid_from_drive(char * device_name) {
	char * id = cdripper_get_discid(device_name);
	printf("discid: %s \n", id);
	return EXIT_SUCCESS;
}

int test_manipulating_drive(char * device_name) {
	printf("tray was closed: %s\n",
			cdripper_close_cd_tray(device_name) == PR_SUCCESS ? "y" : "n");

	sleep(5);

	printf("number of tracks: %d\n", cdripper_get_num_of_tracks(device_name));

	PRBool is_audio_cd = cdripper_is_audio_cd(device_name);

	printf("is audio cd: %s\n", is_audio_cd == PR_TRUE ? "y" : "n");

	PRBool is_disc_loaded = cdripper_is_disc_loaded(device_name);

	printf("disc loaded: %s\n", is_disc_loaded == PR_TRUE ? "y" : "n");

	if (is_disc_loaded == PR_TRUE && is_audio_cd == PR_TRUE) {
		test_discid_from_drive(device_name);
	}

	printf("tray was opened: %s\n",
			cdripper_eject_cd_tray(device_name) == PR_SUCCESS ? "y" : "n");

	sleep(5);

	printf("disc loaded: %s\n",
			cdripper_is_disc_loaded(device_name) == PR_TRUE ? "y" : "n");

	printf("tray was closed: %s\n",
			cdripper_close_cd_tray(device_name) == PR_SUCCESS ? "y" : "n");

	sleep(5);

	return PR_SUCCESS;
}

static PRBool file_exists(char * fn) {
	struct stat stat_p;
	stat(fn, &stat_p);
	return S_ISREG(stat_p.st_mode);
}

int main(void) {
	PRBool test_flac_functionality = PR_TRUE;

	PRBool test_cd_functionality = PR_FALSE;

	char * wav = "/home/jlong/Desktop/myOutfile2.wav";

	if (test_cd_functionality == PR_TRUE) {
		printf("running main! \n");
		char * device_name = "/dev/scd0";
		test_manipulating_drive(device_name);
		char ** drives = list_device_names();
		int ctr;
		int len = sizeof(drives) / sizeof(char*);
		for (ctr = 0; ctr < len; ctr++) {
			char * device_name = drives[ctr];
			printf("deviceName: %s \n", device_name);
			char * details_on_drive = get_device_details(device_name);
			printf("details: %s \n", details_on_drive);
		}
		test_reading_from_drive(wav, device_name);
	}
	if (test_flac_functionality == PR_TRUE) {
		char * out = "/home/jlong/Desktop/myOutfile2.flac";
		if (file_exists(out) == PR_TRUE) {
			unlink(out);
		}
		encode_flac_file_name(wav, 10, out);
	}

	return EXIT_SUCCESS;
}
