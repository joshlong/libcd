//http://leapster.org/linux/cdrom/
#include <unistd.h>
#include "libcd.h"
#include <fstab.h>
#include <prlog.h>
#include <prlong.h>
#include <prthread.h>
#include <prtypes.h>//for the tests
#include <cdio/disc.h>

int test_reading_from_drive(char * device_name) {
	PRFloat64 rip_percent_done = 0;
	char * outfile = "/home/jlong/Desktop/myOutfile2.wav";
	PRBool did_it_work = cdripper_rip_track(device_name, 2, outfile, &rip_percent_done);
	printf("did it work: %s, percent done: %f \n", did_it_work == PR_TRUE ? "Y" : "N", rip_percent_done);
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

int main(void) {
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

	test_reading_from_drive(device_name);

	return EXIT_SUCCESS;
}
