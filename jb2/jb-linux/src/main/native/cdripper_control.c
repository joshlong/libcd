//sudo apt-get install libcdio-paranoia-dev
//  libcdio
// libcdda_paranoia
// libcdda_interface

#include <cdio/cdio.h>
#include <cdio/cd_types.h>
#include <cdio/disc.h>
#include <cdio/paranoia.h>
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include "discid/discid.h"
#include <fstab.h>
#include "cdripper_control.h"

char * get_device_details(char * device_name)
{
	return device_name ; // theres nothing secial here, alas :-)
}
char ** list_device_names(void)
{

	char ** devices = cdio_get_devices(DRIVER_DEVICE);
	// char ** devices =cdio_get_devices_() ;
	/*int len=sizeof(devices)/ sizeof(char*);
	int i ;
	for(i = 0; i < len ; i++ )
	{
		char * device = devices[i];
		printf( "the device is %s" , device) ;
	}*/
	return devices ;



}



/**
 * Use the libdiscid to generate the library
 */
char * cdripper_get_discid(char * device) {
	DiscId *disc = discid_new();
	if (discid_read(disc, device) == 0) {
		fprintf(stderr, "Error: %s\n", discid_get_error_msg(disc));
		return NULL;
	}
	char * discid = discid_get_freedb_id(disc) ;
	discid_free(disc);
	return discid;
}

static void PutNum(long num, int f, int endianness, int bytes);
static void WriteWav(int f, long bytes);

static void GainCalc(char *buffer);
static long CDPWrite(int outf, char *buffer);
/// what functionality do we want to support?
// 1. open a cd tray
// 2. close a cd tray
// 3. read the type from a cd track (data? audio?)
// 4. determine the status from a cd drive (playing? idle? empty?)

// when opening a CDROM device. The linux/cdrom.h header file specifies that
// it must be given the O_NONBLOCK flag when opening. My tests showed
// that if this isn't done, this program will not work.

// foreward declaration  <ugh>

static inline short swap16(short x) {
	return ((((unsigned short)x & 0x00ffU) << 8) | (((unsigned short)x
			& 0xff00U) >> 8));
}
static inline int bigendianp(void) {
	int test=1;
	char *hack=(char *)(&test);
	if (hack[0])
		return (0);
	return (1);
}
PRBool cdripper_is_audio_cd(char * cd_device) {
	CdIo_t * ptr= NULL;

	cdrom_drive_t * drive = get_cdrom_drive_by_device_name(cd_device);

	ptr = drive->p_cdio;

	discmode_t discmode = cdio_get_discmode(ptr);

	if (discmode == CDIO_DISC_MODE_CD_MIXED || discmode == CDIO_DISC_MODE_CD_DA)
		return PR_TRUE;

	return PR_FALSE;
}

PRBool cdripper_is_disc_loaded(char * cd_device) {
	CdIo_t * ptr= NULL;

	cdrom_drive_t * drive = get_cdrom_drive_by_device_name(cd_device);

	ptr = drive->p_cdio;

	discmode_t discmode = cdio_get_discmode(ptr);

	if (discmode == CDIO_DISC_MODE_NO_INFO || discmode == CDIO_DISC_MODE_ERROR)
		return PR_FALSE;

	return PR_TRUE;
}

cdrom_drive_t * get_cdrom_drive_by_device_name(char * cd_device) {

	char ** msg_digest;
	cdrom_drive_t * drive = cdio_cddap_identify(cd_device, 1, msg_digest) ;

	//	printf("you were searching for %s, and u found %s\n", cd_device,
	//		drive->cdda_device_name) ;

	return drive;

}

PRInt32 cdripper_get_num_of_tracks(char * cd_device) {

	CdIo_t * ptr= NULL;

	cdrom_drive_t * drive = get_cdrom_drive_by_device_name(cd_device);

	ptr = drive->p_cdio;

	track_t trax = cdio_get_num_tracks(ptr);
	PRInt32 traxCnt = trax;
	return traxCnt;

}

PRStatus cdripper_close_cd_tray(char * device) {
	int cdrom;
	if ((cdrom = open(device, O_RDONLY | O_NONBLOCK)) < 0) {
		perror("open");
		return PR_FAILURE;
	}
	if (ioctl(cdrom, CDROMCLOSETRAY, 0)<0) {
		perror("ioctl");
	}
	close(cdrom);
	return PR_SUCCESS;
}

PRStatus cdripper_eject_cd_tray(char * cd_device) {
	int cdrom;
	if ((cdrom = open(cd_device, O_RDONLY | O_NONBLOCK)) < 0) {
		perror("open");
		return PR_FAILURE;
	}
	if (ioctl(cdrom, CDROMEJECT, 0)<0) {
		perror("ioctl");
	}
	close(cdrom);
	return PR_SUCCESS;
}

/// ------------------------------------------------------------------------
/// Rip cd track data!
/// ------------------------------------------------------------------------

// support for actually ripping the data from a track to the hard disk
// global variables are because we cant message to callback from user data
//int *global_rip_smile_level;
//FILE *global_output_fp;
/** this is the callback function for cdripper_rip_track
 ***/
static void _cdripper_rip_track_progress_update(long inpos, int function);

static void PutNum(long num, int f, int endianness, int bytes) {
	int i;
	unsigned char c;

	if (!endianness)
		i=0;
	else
		i=bytes-1;
	while (bytes--) {
		c=(num>>(i<<3))&0xff;
		if (write(f, &c, 1)==-1) {
			perror("Could not write to output.");
			exit(1);
		}
		if (endianness)
			i--;
		else
			i++;
	}
}

/* Do the replay gain calculation on a sector */
static void GainCalc(char *buffer) {
	static PRFloat64 l_samples[588];
	static PRFloat64 r_samples[588];
	long count;
	short *data;

	data=(short *)buffer;

	for (count=0; count<588; count++) {
		l_samples[count]=(PRFloat64)data[count*2];
		r_samples[count]=(PRFloat64)data[(count*2)+1];
	}

	//AnalyzeSamples(l_samples,r_samples,588,2);
}

static long CDPWrite(int outf, char *buffer) {
	long words=0, temp;
	long num=CD_FRAMESIZE_RAW;

	while (words<num) {
		temp=write(outf, buffer+words, num-words);
		if (temp==-1) {
			if (errno!=EINTR && errno!=EAGAIN)
				return (-1);
			temp=0;
		}
		words+=temp;
	}

	return (0);
}

static void WriteWav(int f, long bytes) {
	/* quick and dirty */

	write(f, "RIFF", 4); /*  0-3 */
	PutNum(bytes+44-8, f, 0, 4); /*  4-7 */
	write(f, "WAVEfmt ", 8); /*  8-15 */
	PutNum(16, f, 0, 4); /* 16-19 */
	PutNum(1, f, 0, 2); /* 20-21 */
	PutNum(2, f, 0, 2); /* 22-23 */
	PutNum(44100, f, 0, 4); /* 24-27 */
	PutNum(44100*2*2, f, 0, 4); /* 28-31 */
	PutNum(4, f, 0, 2); /* 32-33 */
	PutNum(16, f, 0, 2); /* 34-35 */
	write(f, "data", 4); /* 36-39 */
	PutNum(bytes, f, 0, 4); /* 40-43 */
}

PRBool _cdripper_rip_track(char *device, char *generic_scsi_device, int track,
		long first_sector, long last_sector, char *outfile,
		PRInt32 paranoia_mode, PRFloat64 *rip_percent_done,
		PRBool *stop_thread_rip_now, PRBool do_gain_calc) {

	int force_cdrom_endian=-1;
	int force_cdrom_sectors=-1;
	int force_cdrom_overlap=-1;
	int output_endian=0; /* -1=host, 0=little, 1=big */
	printf("setup variables") ;
	/* full paranoia, but allow skipping */
	int out;
	int verbose=CDDA_MESSAGE_FORGETIT;
	int i;
	long cursor, offset;
	cdrom_drive_t *d=NULL;
	cdrom_paranoia_t *p=NULL;
	printf("set up ptrs to structs") ;
	//	global_rip_smile_level=rip_smile_level;
	//global_output_fp=output_fp;

	/* Query the cdrom/disc; */

	d=cdda_identify(device,verbose,NULL);
	printf("managed to identify *d") ;
	if (!d) {
		if (!verbose) {
			printf("cant write to cdrom drive!") ;
		}//	fprintf(output_fp, "\nUnable to open cdrom drive.\n");

		return PR_FALSE;
	}

	if (verbose)
		cdda_verbose_set(d,CDDA_MESSAGE_PRINTIT,CDDA_MESSAGE_PRINTIT);
	else
		cdda_verbose_set(d,CDDA_MESSAGE_PRINTIT,CDDA_MESSAGE_FORGETIT);

	/* possibly force hand on endianness of drive, sector request size */
	if (force_cdrom_endian!=-1) {
		d->bigendianp=force_cdrom_endian;
		switch (force_cdrom_endian) {
		case 0:
			printf("Forcing CDROM sense to little-endian; ignoring preset and autosense");
			break;
		case 1:
			printf("Forcing CDROM sense to big-endian; ignoring preset and autosense");
			break;
		}
	}

	if (force_cdrom_sectors!=-1) {
		if (force_cdrom_sectors<0 || force_cdrom_sectors>100) {
			printf("Default sector read size must be 1<= n <= 100\n");
			cdda_close(d);

			return PR_FALSE;
		}

		printf("Forcing default to read %d sectors; "
			"ignoring preset and autosense", force_cdrom_sectors);

		d->nsectors=force_cdrom_sectors;
		//d->bigbuff=force_cdrom_sectors*CD_FRAMESIZE_RAW;
	}

	if (force_cdrom_overlap!=-1) {
		if (force_cdrom_overlap<0 || force_cdrom_overlap>75) {
			printf("Search overlap sectors must be 0<= n <=75\n");
			cdda_close(d);

			return PR_FALSE;
		}

		printf("Forcing search overlap to %d sectors; "
			"ignoring autosense", force_cdrom_overlap);
	}

	switch (cdda_open(d)) {
	case -2:
	case -3:
	case -4:
	case -5:
		printf("\nUnable to open disc.  Is there an audio CD in the drive?");
		cdda_close(d);
		return PR_FALSE;
	case -6:
		printf("\nCdparanoia could not find a way to read audio from this drive.");
		cdda_close(d);
		return PR_FALSE;
	case 0:
		break;
	default:
		printf("\nUnable to open disc.");
		cdda_close(d);
		return PR_FALSE;
	}

	/*	if (d->interface==GENERIC_SCSI && d->bigbuff<=CD_FRAMESIZE_RAW) {
	 fprintf(output_fp,
	 "WARNING: You kernel does not have generic SCSI 'SG_BIG_BUFF'\n"
	 "         set, or it is set to a very small value.  Paranoia\n"
	 "         will only be able to perform single sector reads\n"
	 "         making it very unlikely Paranoia can work.\n\n"
	 "         To correct this problem, the SG_BIG_BUFF define\n"
	 "         must be set in /usr/src/linux/include/scsi/sg.h\n"
	 "         by placing, for example, the following line just\n"
	 "         before the last #endif:\n\n"
	 "         #define SG_BIG_BUFF 65536\n\n"
	 "         and then recompiling the kernel.\n\n"
	 "         Attempting to continue...\n\n");
	 }
	 */
	if (d->nsectors==1) {
		printf("WARNING: The autosensed/selected sectors per read value is\n"
			"         one sector, making it very unlikely Paranoia can \n"
			"         work.\n\n"
			"         Attempting to continue...\n\n");
	}

	if (!cdda_track_audiop(d,track)) {
		printf("Selected track is not an audio track. Aborting.\n\n");
		cdda_close(d);
		return PR_FALSE;
	}

	offset=cdda_track_firstsector(d,track);
	first_sector+=offset;
	last_sector+=offset;

	p=paranoia_init(d);
	paranoia_modeset(p,paranoia_mode);

	if (force_cdrom_overlap!=-1)
		paranoia_overlapset(p,force_cdrom_overlap);

	if (verbose)
		cdda_verbose_set(d,CDDA_MESSAGE_LOGIT,CDDA_MESSAGE_LOGIT);
	else
		cdda_verbose_set(d,CDDA_MESSAGE_FORGETIT,CDDA_MESSAGE_FORGETIT);

	paranoia_seek(p,cursor=first_sector,SEEK_SET);

	/* this is probably a good idea in general */
	/*  seteuid(getuid());
	 setegid(getgid());*/

	out=open(outfile, O_RDWR|O_CREAT|O_TRUNC, 0666);
	if (out==-1) {

		cdda_close(d);
		paranoia_free(p);

		return PR_FALSE;
	}

	WriteWav(out, (last_sector-first_sector+1)*CD_FRAMESIZE_RAW);

	/* Off we go! */

	while (cursor<=last_sector) {
		/* read a sector */
		PRInt16 *readbuf=paranoia_read(p,_cdripper_rip_track_progress_update);
		char *err=cdda_errors(d);
		char *mes=cdda_messages(d);

		*rip_percent_done=(PRFloat64)cursor/(PRFloat64)last_sector;

		if (mes || err)
			printf("\r                               "
				"                                           \r%s%s\n",
					mes ? mes : "", err ? err : "");

		if (err)
			free(err);
		if (mes)
			free(mes);

		if (*stop_thread_rip_now) {
			*stop_thread_rip_now=PR_FALSE;

			cdda_close(d);
			paranoia_free(p);

			return PR_FALSE;
		}

		if (readbuf==NULL) {
			printf("\nparanoia_read: Unrecoverable error, bailing.\n");
			cursor=last_sector+1;
			paranoia_seek(p,cursor,SEEK_SET);
			break;
		}

		cursor++;

		if (output_endian!=bigendianp()) {
			for (i=0; i<CD_FRAMESIZE_RAW/2; i++)
				readbuf[i]=swap16(readbuf[i]);
		}

		_cdripper_rip_track_progress_update(cursor*(CD_FRAMEWORDS)-1, -2);

		if (do_gain_calc)
			GainCalc((char *)readbuf);

		if (CDPWrite(out, (char *)readbuf)) {
			printf("Error writing output: ");

			cdda_close(d);
			paranoia_free(p);

			return PR_FALSE;
		}

		if (output_endian!=bigendianp()) {
			for (i=0; i<CD_FRAMESIZE_RAW/2; i++)
				readbuf[i]=swap16(readbuf[i]);
		}
	}

	_cdripper_rip_track_progress_update(cursor*(CD_FRAMESIZE_RAW/2)-1, -1);
	close(out);

	paranoia_free(p);

	cdda_close(d);

	return PR_TRUE;
}

static void _cdripper_rip_track_progress_update(long inpos, int function) {
	static long c_sector=0, v_sector=0;
	static int last=0;
	static long lasttime=0;
	long sector, osector=0;
	struct timeval thistime;
	static char heartbeat=' ';
	static int overlap=0;
	static int slevel=0;
	static int slast=0;
	static int stimeout=0;
	long test;

	osector=inpos;
	sector=inpos/CD_FRAMEWORDS;

	if (function==-2) {
		v_sector=sector;
		return;
	}

	if (function==-1) {
		last=8;
		heartbeat='*';
		slevel=0;
		v_sector=sector;
	} else
		switch (function) {
		case PARANOIA_CB_VERIFY:
			if (stimeout>=30) {
				if (overlap>CD_FRAMEWORDS)
					slevel=2;
				else
					slevel=1;
			}
			break;
		case PARANOIA_CB_READ:
			if (sector>c_sector)
				c_sector=sector;
			break;

		case PARANOIA_CB_FIXUP_EDGE:
			if (stimeout>=5) {
				if (overlap>CD_FRAMEWORDS)
					slevel=2;
				else
					slevel=1;
			}
			break;
		case PARANOIA_CB_FIXUP_ATOM:
			if (slevel<3 || stimeout>5)
				slevel=3;
			break;
		case PARANOIA_CB_READERR:
			slevel=6;
			break;
		case PARANOIA_CB_SKIP:
			slevel=8;
			break;
		case PARANOIA_CB_OVERLAP:
			overlap=osector;
			break;
		case PARANOIA_CB_SCRATCH:
			slevel=7;
			break;
		case PARANOIA_CB_DRIFT:
			if (slevel<4 || stimeout>5)
				slevel=4;
			break;
		case PARANOIA_CB_FIXUP_DROPPED:
		case PARANOIA_CB_FIXUP_DUPED:
			slevel=5;
			break;
		}

	gettimeofday(&thistime, NULL);
	test=thistime.tv_sec*10+thistime.tv_usec/100000;

	if (lasttime!=test || function==-1 || slast!=slevel) {
		if (lasttime!=test || function==-1) {
			last++;
			lasttime=test;
			if (last>7)
				last=0;
			stimeout++;
			switch (last) {
			case 0:
				heartbeat=' ';
				break;
			case 1:
			case 7:
				heartbeat='.';
				break;
			case 2:
			case 6:
				heartbeat='o';
				break;
			case 3:
			case 5:
				heartbeat='0';
				break;
			case 4:
				heartbeat='O';
				break;
			}

			if (function==-1)
				heartbeat='*';

		}
		if (slast!=slevel) {
			stimeout=0;
		}
		slast=slevel;
	}

}

PRBool cdripper_rip_track(char * device_name, int track, char * outfile,
		PRFloat64 * rip_percent_done) {
	PRBool stop_thread_rip_now= PR_FALSE;
	CdIo_t * ptr= NULL;
	cdrom_drive_t * drive = get_cdrom_drive_by_device_name(device_name);
	ptr = drive->p_cdio;
	///printf("ptr is null? %s", ptr == ( CdIo_t*) NULL ? "y":"n") ;
	PRInt32 start_lsn = cdio_get_track_lsn(ptr, track) ;
	PRInt32 stop_lsn = cdio_get_track_last_lsn(ptr, track);
	printf("start lsn: %d, stop lsn: %d \n", start_lsn, stop_lsn) ;
	PRBool did_it_work = _cdripper_rip_track(device_name,
	NULL, 1, start_lsn, stop_lsn, outfile, PARANOIA_MODE_DISABLE,
			rip_percent_done, &stop_thread_rip_now, PR_FALSE) ;
	printf("did it work : %s \n", did_it_work == PR_TRUE ? "y" : "n") ;

	return did_it_work;
}





