package com.joshlong.cdworld.utils.cdda.tests;

import com.joshlong.cdworld.utils.cdda.AudioCompactDisc;
import com.joshlong.cdworld.utils.cdda.IAudioCompactDisc;
import junit.framework.TestCase;
import org.apache.commons.lang.StringUtils;
import org.apache.commons.lang.SystemUtils;

import java.io.File;

/**
 * @author Josh Long
 *         <p/>
 *         This class only serves to demonstrate the native library integration working
 */
public class TestUsingNativeAudioCompactDisc extends TestCase {

    static public void log(String msg, Object... pars) {
        System.out.println(String.format(msg, pars));
    }

    static boolean CHECK_CDDB_FUNCTIONALITY = true;
    static boolean CHECK_DRIVE_CONTROL = false;

    static boolean CHECK_CDDA_FUNCTIONALITY = false;

    
    static void exerciseNativeAPIs() throws Throwable {

        IAudioCompactDisc iAudioCompactDisc = new AudioCompactDisc();
        String cdDevice = null; //"2" 
        if (SystemUtils.IS_OS_LINUX) cdDevice =  iAudioCompactDisc.listDiscDevices()[0];  //"/dev/scd0"; //
        if (SystemUtils.IS_OS_WINDOWS)
            cdDevice = StringUtils.isEmpty(cdDevice) ?
                    iAudioCompactDisc.listDiscDevices()[0]
                    : "2";

        // lets get the introspection tests done now :
        for(String deviceName: iAudioCompactDisc.listDiscDevices()){
            log ( "device: %s, details:%s", deviceName, iAudioCompactDisc.getDiscDeviceDetailsForDisc(deviceName)) ;
        }

        int trax = iAudioCompactDisc.countTracksOnAudioCompactDisc(cdDevice);
        log("there are " + trax + " on the drive");
        if (iAudioCompactDisc.ejectCompactDiscFromCompactDiscDrive(cdDevice)) {
            log("the drive is open? %s", iAudioCompactDisc.isCompactDiscDriveDoorOpen(cdDevice));
            Thread.sleep(1000);
            iAudioCompactDisc.injectCompactDiscIntoCompactDiscDrive(cdDevice);
        }
        Thread.sleep(1000);
        log("the drive is open? %s", iAudioCompactDisc.isCompactDiscDriveDoorOpen(cdDevice));
        File file = new File(SystemUtils.getUserHome(), "myOuttest.wav");
        if (iAudioCompactDisc.ripTrackFromCompactDisc(cdDevice, 2, file)) {
            log(file.getAbsolutePath() + " was just rendered");
        }
        String discId = iAudioCompactDisc.getFreeDbCompactDiscDiscId(cdDevice);
        log("the disc ID is %s", discId);
        for (String deviceName : iAudioCompactDisc.listDiscDevices()) {
            String detailName = iAudioCompactDisc.getDiscDeviceDetailsForDisc(deviceName);
            log("deviceName: %s, detail name: %s", deviceName, detailName);
        }
    }

    public static void main(String[] args) throws Throwable {
//        linux();

        exerciseNativeAPIs();
    }
}
