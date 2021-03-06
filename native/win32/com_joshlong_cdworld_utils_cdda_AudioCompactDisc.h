/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_joshlong_cdworld_utils_cdda_AudioCompactDisc */

#ifndef _Included_com_joshlong_cdworld_utils_cdda_AudioCompactDisc
#define _Included_com_joshlong_cdworld_utils_cdda_AudioCompactDisc
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _countTracksOnAudioCompactDisc
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1countTracksOnAudioCompactDisc
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _ejectCompactDiscFromCompactDiscDrive
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1ejectCompactDiscFromCompactDiscDrive
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _injectCompactDiscIntoCompactDiscDrive
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1injectCompactDiscIntoCompactDiscDrive
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _isCompactDiscDriveDoorOpen
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1isCompactDiscDriveDoorOpen
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _isAudioCompactDiscLoaded
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1isAudioCompactDiscLoaded
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _ripTrackFromCompactDisc
 * Signature: (Ljava/lang/String;ILjava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1ripTrackFromCompactDisc
  (JNIEnv *, jobject, jstring, jint, jstring);

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _getDiscDeviceDetailsForDisc
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1getDiscDeviceDetailsForDisc
  (JNIEnv *, jobject, jstring);

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _listDiscDevices
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1listDiscDevices
  (JNIEnv *, jobject);

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _getFreeDbCompactDiscDiscId
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1getFreeDbCompactDiscDiscId
  (JNIEnv *, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
