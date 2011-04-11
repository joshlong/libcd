package com.joshlong.jukebox.system.integration;

import java.io.File;

/**
 * this class encapsulates access to a local CD drive and the music contained therein.
 *
 * @author Josh Long
 */
public interface AudioCompactDisc {
/*
    *//**
     * this leverages libdiscid (a library from MusicBrainz) to generate our discid for us.
     *
     * @param deviceName the device name
		 *
     *//*
    String getCompactDiscDiscId (String deviceName);

    boolean readTrack(String deviceName, int trackNo, File destinationOfFile);

    String getDiscDeviceDetailsForDisc(String deviceName);

    *//**
     * Identify how many tracks are on the device
     *
     * @param deviceName the device string, which is obtainable from #list
     * @return
     *//*
    int countTracksOnAudioCompactDisc(String deviceName);

    String[] listDiscDevices();

    *//**
     * Instructs the compact disc drive bay door to open
     *
     * @param deviceName the device name (ie, 'H' on Windows, /dev/scd0 on Linux, etc.)
     * @return whether the command worked or not, as best as we can determine.
     *//*
    boolean ejectCompactDiscFromCompactDiscDrive(String deviceName);

    *//**
     * Instructs the compact disc drive bay door to open
     *
     * @param deviceName the device name (ie, 'H' on Windows, /dev/scd0 on Linux, etc.)
     * @return whether the command worked or not, as best as we can determine.
     *//*
    boolean injectCompactDiscIntoCompactDiscDrive(String deviceName);

    *//**
     * Queries the drive to see if the drive bay door is open or the CD is ejected.
     *
     * @param deviceName the device name (ie, 'H' on Windows, /dev/scd0 on Linux, etc.)
     * @return status as to whether an audio compact disc is in the drive
     *//*
    boolean isCompactDiscDriveDoorOpen(String deviceName);

    *//**
     * This will query if there is a valid CDDA compact disc in the drive.
     * Anything that is CDDA or CDDA + mixed should be supported.
     * A data CD or an empty drive or any kind of error will yeild false
     *
     * @param deviceName the device name (ie, 'H' on Windows, /dev/scd0 on Linux, etc.)
     * @return status as to whether an audio compact disc is in the drive
     *//*
    boolean isAudioCompactDiscLoaded(String deviceName);*/
}
