#include <fstab.h>
#include <stdio.h>
#include <time.h>
#include <jni.h>
#include "com_joshlong_cdworld_utils_cdda_AudioCompactDisc.h"
#include "cdripper_control.h"

/**
 * http://books.google.com/books?id=NFnBRcu8DHEC&pg=PA24&lpg=PA24&dq=how+do+I+convert+jstring+to+a+char+*&source=web&ots=K_9t8bV1WN&sig=Syj5OxvJ0I9a_ekc515EqeCYbeg&hl=en&sa=X&oi=book_result&resnum=6&ct=result#PPA25,M1
 **/


/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _countTracksOnAudioCompactDisc
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1countTracksOnAudioCompactDisc
	(JNIEnv * env, jobject o, jstring deviceName) {

	const jbyte * str = (*env)->GetStringUTFChars(env, deviceName, NULL) ;
	if(str == NULL) return NULL ;

	int ctr = cdripper_get_num_of_tracks((char*) str) ;

	(*env)->ReleaseStringUTFChars(env, deviceName,str) ;

	return ctr ;
}

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _ejectCompactDiscFromCompactDiscDrive
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1ejectCompactDiscFromCompactDiscDrive
  (JNIEnv * env, jobject cePtr, jstring deviceName)
{


	const jbyte * str = (*env)->GetStringUTFChars(env, deviceName, NULL) ;
	if(str == NULL) return NULL ;

	PRStatus was_ejected = cdripper_eject_cd_tray((char*) str) ;
	//printf( was_ejected ? "was ejected": "was not ejected") ;
	(*env)->ReleaseStringUTFChars(env, deviceName,str) ;

	return was_ejected == PR_SUCCESS ? JNI_TRUE :JNI_FALSE;
}

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _injectCompactDiscIntoCompactDiscDrive
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1injectCompactDiscIntoCompactDiscDrive
  (JNIEnv * env, jobject cePtr, jstring deviceName)
{

	const jbyte * str = (*env)->GetStringUTFChars(env, deviceName, NULL) ;
	if(str == NULL) return NULL ;

	PRStatus was_closed = cdripper_close_cd_tray((char*) str) ;
	//printf( was_closed ? "  was_closed": "was not closed") ;
	(*env)->ReleaseStringUTFChars(env, deviceName,str) ;

	return was_closed == PR_SUCCESS ? JNI_TRUE :JNI_FALSE;
}


/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _isCompactDiscDriveDoorOpen
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1isCompactDiscDriveDoorOpen
  (JNIEnv * env, jobject o, jstring deviceName)
{

	const jbyte * str = (*env)->GetStringUTFChars(env, deviceName, NULL) ;
		if(str == NULL) return NULL ;

	PRBool ret  =  cdripper_is_disc_loaded((char*) str) ;
	(*env)->ReleaseStringUTFChars(env, deviceName,str) ;
	return ret == PR_TRUE ? JNI_TRUE : JNI_FALSE ;
}

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _isAudioCompactDiscLoaded
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1isAudioCompactDiscLoaded
  (JNIEnv * env, jobject cePtr, jstring deviceName)
{

	const jbyte * str = (*env)->GetStringUTFChars(env, deviceName, NULL) ;

	if(str == NULL) return NULL ;

	PRBool ret  =  cdripper_is_audio_cd((char*) str) ;
	(*env)->ReleaseStringUTFChars(env, deviceName,str) ;
	return ret == PR_TRUE ? JNI_TRUE : JNI_FALSE ;
}


/*
* Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
* Method:    _ripTrackFromCompactDisc
* Signature: (Ljava/lang/String;ILjava/lang/String;)Z
*/
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1ripTrackFromCompactDisc
(JNIEnv * env, jobject ce, jstring devName, jint trackNo, jstring fPath)
{
	const jbyte * deviceName = (*env)->GetStringUTFChars(env, devName, NULL) ;
	const jbyte * fileOutputPath = (*env)->GetStringUTFChars(env, fPath, NULL) ;

	if(deviceName == NULL||fileOutputPath==NULL)
		return NULL ;

	// now we do the actual ripping
	PRFloat64  myStatusFloat = 0 ;
	PRBool result = cdripper_rip_track(deviceName,(int)trackNo,fileOutputPath,&myStatusFloat) ;

	//printf("just returned from ripping track!\n") ;
	(*env)->ReleaseStringUTFChars(env, devName, deviceName) ;
	(*env)->ReleaseStringUTFChars(env, fPath, fileOutputPath) ;

	printf("result: %s, status float: %f \n" , result == PR_TRUE ? "Y":"N", myStatusFloat) ;

	if(result == PR_TRUE && (PRFloat64)myStatusFloat == (PRFloat64) 1.00)
		return JNI_TRUE ;

	return JNI_FALSE ;
}


/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _getFreeDbCompactDiscDiscId
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1getFreeDbCompactDiscDiscId
  (JNIEnv * env, jobject cePtr, jstring deviceName)
{


	const jbyte * str = (*env)->GetStringUTFChars(env, deviceName, NULL) ;

	if(str == NULL) return NULL ;

	char * discId =  cdripper_get_discid((char*) str) ;
	(*env)->ReleaseStringUTFChars(env, deviceName,str) ;
	if(discId != NULL)
	{
		 /// then we can go ahead and build a java string to return
		jstring answer = (jstring) (*env)->NewStringUTF( env, discId);
		return answer ;
	}
	return NULL ;

}




/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _getDiscDeviceDetailsForDisc
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1getDiscDeviceDetailsForDisc
  (JNIEnv * env, jobject cePtr, jstring deviceName ){

	const jbyte * str = (*env)->GetStringUTFChars(env, deviceName, NULL) ;

	if(str == NULL) return NULL ;

	char * driveDetails =  get_device_details((char*)str) ;

	(*env)->ReleaseStringUTFChars(env, deviceName,str) ;
	jstring nStr = (*env)->NewStringUTF(env , driveDetails);
	return nStr ;


}


/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _listDiscDevices
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1listDiscDevices
  (JNIEnv * env, jobject cePtr ) {

	char ** devices = list_device_names() ;
	unsigned long sizeOfDevices = sizeof(devices) ;
	unsigned long sizeOfChar = sizeof(char*);
	//printf("sizeOfDevs:%d", sizeOfDevices);
	//printf("sizeOfChar:%d", sizeOfChar);
	unsigned long size = sizeOfDevices / sizeOfChar ; //(unsigned long)( sizeof((**devices))  / sizeof(char*) );
	//printf( "size: %d",size) ;

	jclass clazzOfString =(*env)->FindClass( env , "java/lang/String");
	//printf("clazzOfString was retrievd") ;
	jstring nStringUtf = (*env)->NewStringUTF(env,"") ;
	//printf("new string was retreived" );

	jobjectArray sArr =(jobjectArray)(*env)->NewObjectArray ( env, size,  clazzOfString , nStringUtf) ;
	//printf(sArr == NULL ? "sarr is null": "sarr is not null") ;

	int indx = 0   ;
	for( indx = 0; indx < size ;indx++)
	{
		char * strForId = devices[indx];
	//	printf( "iterating through loop: %s",strForId) ;
		jstring s = (*env)->NewStringUTF( env ,strForId );
		(*env)->SetObjectArrayElement( env , sArr,indx,s);

	}

	return sArr ;
}
