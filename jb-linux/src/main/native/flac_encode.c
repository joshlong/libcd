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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FLAC/metadata.h>
#include <FLAC/stream_encoder.h>
#include "flac_encode.h"

/**
 * All of this was taken from the samples in the flac distribution.
 * TODO rework this to support FILE* instead of char* fileName pointers.
 */

#define READSIZE 1024

void progress_callback(const FLAC__StreamEncoder *encoder,
		FLAC__uint64 bytes_written, FLAC__uint64 samples_written,
		unsigned frames_written, unsigned total_frames_estimate,
		void *client_data) {
	// noop at the moment...

}

static unsigned total_samples = 0; /* can use a 32-bit number due to WAVE size limitations */
static FLAC__byte
		buffer[READSIZE/*samples*/* 2/*bytes_per_sample*/* 2/*channels*/]; /* we read the WAVE data into here */
static FLAC__int32 pcm[READSIZE/*samples*/* 2/*channels*/];

/// convenience method that we also export that should just deelgate to encode_flac_file
int encode_flac_file_name(char * inputFileName, int level /* 0 - 10 */, char * outFile) {

	FILE * fin;
	FILE * fout;

	if ((fin = fopen(inputFileName, "rb")) == NULL) {
		fprintf(stderr, "ERROR: opening %s for output\n", inputFileName);
		return 1;
	}

	if ((fout = fopen(outFile, "w+b")) == NULL) {
		fprintf(stderr, "ERROR: opening %s for output\n", outFile);
		return 1;
	}

	PRBool status = encode_flac_file(fin, level, fout); // todo make sure we build a ptr to an output file somewhere


	PRBool closedIn = fin != NULL && EOF != fclose(fin);
	// NB YOU DO NOT NEED TO CLOSE THE FILE *UNLESS* THERE WAS A BAD RETURN!! PRBool closedOut = fout != NULL && EOF != fclose(fout);
	if (closedIn == PR_TRUE &&  status == PR_TRUE ) {
		return PR_TRUE;
	}
	return PR_FALSE;

}

int encode_flac_file(FILE * fin, int compressionLevel, FILE * oFile) {
	FLAC__bool ok = true;
	FLAC__StreamEncoder *encoder = 0;
	FLAC__StreamEncoderInitStatus init_status;
	FLAC__StreamMetadata *metadata[2];
	FLAC__StreamMetadata_VorbisComment_Entry entry;
 	unsigned sample_rate = 0;
	unsigned channels = 0;
	unsigned bps = 0;

	/* read wav header and validate it */
	if (fread(buffer, 1, 44, fin) != 44 || memcmp(buffer, "RIFF", 4) || memcmp(
			buffer + 8, "WAVEfmt \020\000\000\000\001\000\002\000", 16)
			|| memcmp(buffer + 32, "\004\000\020\000data", 8)) {
		fprintf(
				stderr,
				"ERROR: invalid/unsupported WAVE file, only 16bps stereo WAVE in canonical form allowed\n");
		//fclose(fin);
		return 1;
	}
	sample_rate = ((((((unsigned) buffer[27] << 8) | buffer[26]) << 8)
			| buffer[25]) << 8) | buffer[24];
	channels = 2;
	bps = 16;
	total_samples = (((((((unsigned) buffer[43] << 8) | buffer[42]) << 8)
			| buffer[41]) << 8) | buffer[40]) / 4;

	/* allocate the encoder */
	if ((encoder = FLAC__stream_encoder_new()) == NULL) {
		fprintf(stderr, "ERROR: allocating encoder\n");
		//fclose(fin);
		return 1;
	}

	ok &= FLAC__stream_encoder_set_verify(encoder, true);
	ok &= FLAC__stream_encoder_set_compression_level(encoder, compressionLevel);
	ok &= FLAC__stream_encoder_set_channels(encoder, channels);
	ok &= FLAC__stream_encoder_set_bits_per_sample(encoder, bps);
	ok &= FLAC__stream_encoder_set_sample_rate(encoder, sample_rate);
	ok &= FLAC__stream_encoder_set_total_samples_estimate(encoder,
			total_samples);

	/* now add some metadata; we'll add some tags and a padding block */
	if (ok) {
		if ((metadata[0] = FLAC__metadata_object_new(
				FLAC__METADATA_TYPE_VORBIS_COMMENT)) == NULL || (metadata[1]
				= FLAC__metadata_object_new(FLAC__METADATA_TYPE_PADDING))
				== NULL ||
		/* there are many tag (vorbiscomment) functions but these are convenient for this particular use: */
		!FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry,
				"ARTIST", "Some Artist")
				|| !FLAC__metadata_object_vorbiscomment_append_comment(
						metadata[0], entry, /*copy=*/false) || /* copy=false: let metadata object take control of entry's allocated string */
		!FLAC__metadata_object_vorbiscomment_entry_from_name_value_pair(&entry,
				"YEAR", "1984")
				|| !FLAC__metadata_object_vorbiscomment_append_comment(
						metadata[0], entry, /*copy=*/false)) {
			fprintf(stderr, "ERROR: out of memory or tag error\n");
			ok = false;
		}

		metadata[1]->length = 1234; /* set the padding length */

		ok = FLAC__stream_encoder_set_metadata(encoder, metadata, 2);
	}

	/* initialize encoder */
	if (ok) {

		init_status = FLAC__stream_encoder_init_FILE(encoder, oFile,
				progress_callback, /*client_data=*/NULL);
		if (init_status != FLAC__STREAM_ENCODER_INIT_STATUS_OK) {
			fprintf(stderr, "ERROR: initializing encoder: %s\n",
					FLAC__StreamEncoderInitStatusString[init_status]);
			ok = false;
		}
	}

	/* read blocks of samples from WAVE file and feed to encoder */
	if (ok) {
		size_t left = (size_t) total_samples;
		while (ok && left) {
			size_t need = (left > READSIZE ? (size_t) READSIZE : (size_t) left);
			if (fread(buffer, channels * (bps / 8), need, fin) != need) {
				fprintf(stderr, "ERROR: reading from WAVE file\n");
				ok = false;
			} else {
				/* convert the packed little-endian 16-bit PCM samples from WAVE into an interleaved FLAC__int32 buffer for libFLAC */
				size_t i;
				for (i = 0; i < need * channels; i++) {
					/* inefficient but simple and works on big- or little-endian machines */
					pcm[i]
							= (FLAC__int32) (((FLAC__int16) (FLAC__int8) buffer[2
									* i + 1] << 8)
									| (FLAC__int16) buffer[2 * i]);
				}
				/* feed samples to encoder */
				ok = FLAC__stream_encoder_process_interleaved(encoder, pcm,
						need);
			}
			left -= need;
		}
	}

	ok &= FLAC__stream_encoder_finish(encoder);
#ifdef DEBUG
	fprintf(stderr, "encoding: %s\n", ok ? "succeeded" : "FAILED");
	fprintf( stderr, "   state: %s\n", FLAC__StreamEncoderStateString[FLAC__stream_encoder_get_state( encoder)]);
#endif

	/* now that encoding is finished, the metadata can be freed */
	FLAC__metadata_object_delete(metadata[0]);
	FLAC__metadata_object_delete(metadata[1]);

	FLAC__stream_encoder_delete(encoder);
//	fclose(fin);

	return ok ? PR_TRUE : PR_FALSE;
}
