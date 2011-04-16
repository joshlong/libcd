package com.joshlong.cdworld.utils.cdda;

import org.apache.commons.lang.SystemUtils;
import org.apache.commons.lang.exception.ExceptionUtils;

import java.io.File;

/**
 * @author Josh Long
 *         <p> This specifies the interface of the object that deals with the native code. <p/>
 *         <p> It provides the methods every implementatio will have to support, even if they are no-ops on specific platforms.</p>
 *         <p> Interesting read on JNI: http://java.sun.com/docs/books/jni/html/start.html</p>
 *         <p/>
 *         <p/>
 *         <P>
 *         Another interesting problem is that the
 *         </p>
 *         <p/>
 *         <p/>
 *         RUN WITH:
 *         <p/>
 *         -Djava.library.path="C:\Documents and Settings\Owner.jlong\Desktop\cdripper\native\cdex_working_native_solution\Debug_UNICODE"
 *         <p/>
 *         OR
 *         <p/>
 *         -Djava.library.path=/home/jlong/Documents/code/cdripper/native2/libcdrip/lib/linux
 */
public class AudioCompactDisc implements IAudioCompactDisc {

    public static String AUDIO_COMPACT_DISC_LIB_DIRECTORY_PROPERTY = "audio.compactdisc.lib.directory";

    /**
     * Remember: there are all sorts of rules for loading the library
     * - on linux the library needs to be lib$FOO.so, in Java you just load $FOO
     * - win32?
     * -unix ?
     *
     * // http://dev.eclipse.org/newslists/news.eclipse.technology.equinox/msg01581.html
     *
     */
    static {
        /**

         * specified by java.library.path for those dependencies. Thus, you need to manually load those DLLs for it
         */

        String[] libraries = new String[0];
        if (SystemUtils.IS_OS_LINUX) libraries = new String[]{
                /*System.mapLibraryName*/("cdrippercddainterface")};
        if (SystemUtils.IS_OS_WINDOWS) libraries = new String[]{"CDRip", "taglib", "id3lib", "libsndfile", "CDex"};
        for (String library : libraries) {
            System.loadLibrary(library);
        }
        // now

/*
String libraryName = SystemUtils.IS_OS_LINUX ? "cdrippercddainterface" :
        (SystemUtils.IS_OS_WINDOWS ? "CDex" : null);

if (StringUtils.isEmpty(libraryName)) {
    log("the library name is null| empty!? why?");
}
if (SystemUtils.IS_OS_LINUX || SystemUtils.IS_OS_WINDOWS) {
    cdrpperCompactDiscLibDirectoryProperty = new File(
            System.getProperty(AUDIO_COMPACT_DISC_LIB_DIRECTORY_PROPERTY),
            System.mapLibraryName(libraryName)).getAbsolutePath();
    System.out.println("Loading libraries for " + SystemUtils.OS_NAME + " from " + cdrpperCompactDiscLibDirectoryProperty);
    System.load(cdrpperCompactDiscLibDirectoryProperty);  // load the static library

} else {
    log("ths will not work on your platform!");
}*/
    }

    public native int _countTracksOnAudioCompactDisc(String deviceName);

    /**
     * Returns how many tracks there are on the Audio Compact Disc
     *
     * @param deviceName the device name (ie, 'H' on Windows, /dev/scd0 on Linux, etc.)
     * @return the count of how many tracks are on the audio compact disc.
     */
    public int countTracksOnAudioCompactDisc(String deviceName) {
        return _countTracksOnAudioCompactDisc(deviceName);
    }

    public native boolean _ejectCompactDiscFromCompactDiscDrive(String deviceName);

    /**
     * Instructs the compact disc drive bay door to open
     *
     * @param deviceName the device name (ie, 'H' on Windows, /dev/scd0 on Linux, etc.)
     * @return whether the command worked or not, as best as we can determine.
     */
    public boolean ejectCompactDiscFromCompactDiscDrive(String deviceName) {
        return _ejectCompactDiscFromCompactDiscDrive(deviceName);
    }

    public native boolean _injectCompactDiscIntoCompactDiscDrive(String dn);

    /**
     * Instructs the compact disc drive bay door to open
     *
     * @param deviceName the device name (ie, 'H' on Windows, /dev/scd0 on Linux, etc.)
     * @return whether the command worked or not, as best as we can determine.
     */
    public boolean injectCompactDiscIntoCompactDiscDrive(String deviceName) {
        return _injectCompactDiscIntoCompactDiscDrive(deviceName);
    }

    public native boolean _isCompactDiscDriveDoorOpen(String deviceName);

    /**
     * Queries the drive to see if the drive bay door is open or the CD is ejected.
     *
     * @param deviceName the device name (ie, 'H' on Windows, /dev/scd0 on Linux, etc.)
     * @return status as to whether an audio compact disc is in the drive
     */
    public boolean isCompactDiscDriveDoorOpen(String deviceName) {
        return _isCompactDiscDriveDoorOpen(deviceName);
    }

    public native boolean _isAudioCompactDiscLoaded(String deviceName);

    /**
     * This will query if there is a valid CDDA compact disc in the drive.
     * Anything that is CDDA or CDDA + mixed should be supported.
     * A data CD or an empty drive or any kind of error will yeild false
     *
     * @param deviceName the device name (ie, 'H' on Windows, /dev/scd0 on Linux, etc.)
     * @return status as to whether an audio compact disc is in the drive
     */
    public boolean isAudioCompactDiscLoaded(String deviceName) {
        return _isAudioCompactDiscLoaded(deviceName);
    }

    public native boolean _ripTrackFromCompactDisc(String deviceName, int trackNo, String destinationOfFile);

    public boolean ripTrackFromCompactDisc(String deviceName, int trackNo, File destinationOfFile) {

        if (destinationOfFile.exists()) {
            try {
                destinationOfFile.delete();
            } catch (Throwable t) {
                log("Exception thrown:" + ExceptionUtils.getFullStackTrace(t));
                return false;
            }
        }

        if (!destinationOfFile.getParentFile().exists())
            destinationOfFile.getParentFile().mkdirs();

        String path = destinationOfFile.getAbsolutePath();
        log("the path is " + path);
        boolean status = _ripTrackFromCompactDisc(deviceName, trackNo, path);
        log("the status is " + status);
        return (status);
    }

    native public String _getDiscDeviceDetailsForDisc(String deviceName);

    public String getDiscDeviceDetailsForDisc(String deviceName) {
        return _getDiscDeviceDetailsForDisc(deviceName);
    }

    /**
     * the idea is that this should provide us with the valid values for each CD drive.
     * <p/>
     * <P>You should also be able to pass the string into another    method on this interface and get a 'detail' string in exchange</P>
     *
     * @return an array of device names that you may use to get more info from this API.
     */
    public native String[] _listDiscDevices();

    public String[] listDiscDevices() {
        return _listDiscDevices();
    }

    // native public String _getMusicBrainzCompactDiscId(String deviceName);

    native public String _getFreeDbCompactDiscDiscId(String deviceName);

    public String getFreeDbCompactDiscDiscId(String deviceName) {
        return _getFreeDbCompactDiscDiscId(deviceName);
    }

    private static void log(String msg, Object... params) {
        String output = String.format(msg, params);
        System.out.println(output);
    }
}