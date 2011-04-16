
 
#include "stdafx.h"
#include <time.h>
#include <jni.h>

//extern "C++" {
//#include "CAudioCD.h"  
//}
#include "cdripper_interop.h"
#include "com_joshlong_cdworld_utils_cdda_AudioCompactDisc.h"
#include <windows.h>
/** 
 * http://books.google.com/books?id=NFnBRcu8DHEC&pg=PA24&lpg=PA24&dq=how+do+I+convert+jstring+to+a+char+*&source=web&ots=K_9t8bV1WN&sig=Syj5OxvJ0I9a_ekc515EqeCYbeg&hl=en&sa=X&oi=book_result&resnum=6&ct=result#PPA25,M1
 **/

 
/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _countTracksOnAudioCompactDisc
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1countTracksOnAudioCompactDisc
	(JNIEnv * env, jobject o, jstring deviceName) 
{

	char * str  = (char *) (*env)->GetStringUTFChars(env, deviceName, NULL) ; 

	char fc = (str)[0];	

	jint cnt = (jint)countTracks(fc) ;
	
	(*env)->ReleaseStringUTFChars(env, deviceName,str) ;

	return cnt; 
}

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _ejectCompactDiscFromCompactDiscDrive
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1ejectCompactDiscFromCompactDiscDrive
  (JNIEnv * env, jobject cePtr, jstring deviceName)
{


	/* const jbyte * str = (*env)->GetStringUTFChars(env, deviceName, NULL) ; 
	if(str == NULL) return NULL ;
	
	PRStatus was_ejected = cdripper_eject_cd_tray((char*) str) ;
	//printf( was_ejected ? "was ejected": "was not ejected") ;
	(*env)->ReleaseStringUTFChars(env, deviceName,str) ;
	
	return was_ejected == PR_SUCCESS ? JNI_TRUE :JNI_FALSE; */

	return JNI_FALSE ;
}

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _injectCompactDiscIntoCompactDiscDrive
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1injectCompactDiscIntoCompactDiscDrive
  (JNIEnv * env, jobject cePtr, jstring deviceName)
{
 /* 
 	
	const jbyte * str = (*env)->GetStringUTFChars(env, deviceName, NULL) ; 
	if(str == NULL) return NULL ;
	
	PRStatus was_closed = cdripper_close_cd_tray((char*) str) ;
	//printf( was_closed ? "  was_closed": "was not closed") ;
	(*env)->ReleaseStringUTFChars(env, deviceName,str) ;
	
	return was_closed == PR_SUCCESS ? JNI_TRUE :JNI_FALSE;  */ 
	return JNI_FALSE ;
}


/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _isCompactDiscDriveDoorOpen
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1isCompactDiscDriveDoorOpen
  (JNIEnv * env, jobject o, jstring deviceName)
{
/* 

	const jbyte * str = (*env)->GetStringUTFChars(env, deviceName, NULL) ; 
		if(str == NULL) return NULL ; 
		 
	PRBool ret  =  cdripper_is_disc_loaded((char*) str) ;
	(*env)->ReleaseStringUTFChars(env, deviceName,str) ;
	return ret == PR_TRUE ? JNI_TRUE : JNI_FALSE ; */

	return JNI_FALSE ;
}

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _isAudioCompactDiscLoaded
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1isAudioCompactDiscLoaded
  (JNIEnv * env, jobject cePtr, jstring deviceName)
{
	
	 /* 
	 const jbyte * str = (*env)->GetStringUTFChars(env, deviceName, NULL) ; 
	
	if(str == NULL) return NULL ; 
		 
	PRBool ret  =  cdripper_is_audio_cd((char*) str) ;
	(*env)->ReleaseStringUTFChars(env, deviceName,str) ;
	return ret == PR_TRUE ? JNI_TRUE : JNI_FALSE ; 
	*/
	return JNI_FALSE ;
}
//----------------------------------------------------------------------------
/// HACKS FROM http://www.gamedev.net/community/forums/topic.asp?topic_id=185015
// ----------------------------------------------------------------------------

//

//

BOOL UnicodeToAnsi(
	LPWSTR pszwUniString, 
	LPSTR  pszAnsiBuff,
	DWORD  dwAnsiBuffSize
	)
{
	int iRet = 0;
    iRet = WideCharToMultiByte(
		CP_ACP,
		0,
		pszwUniString,
		-1,
		pszAnsiBuff,
		dwAnsiBuffSize,
		NULL,
		NULL
		);
	return ( 0 != iRet );
}


// ----------------------------------------------------------------------------

//

//

BOOL AnsiToUnicode(
    LPSTR  pszAnsiString, 
    LPWSTR pszwUniBuff, 
    DWORD dwUniBuffSize
    )
{

	int iRet = 0;
    iRet = MultiByteToWideChar(
		CP_ACP,
		0,
		pszAnsiString,
		-1,
		pszwUniBuff,
		dwUniBuffSize
		);

	return ( 0 != iRet );
}

/*
* Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
* Method:    _ripTrackFromCompactDisc
* Signature: (Ljava/lang/String;ILjava/lang/String;)Z
*/
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1ripTrackFromCompactDisc
(JNIEnv * env, jobject ce, jstring devName, jint jtrackNo, jstring fPath)
{
 	char  * fileOutputPath = (char *) (*env)->GetStringUTFChars(env, fPath, NULL) ;
	char *  deviceNameStr =  (char *) (*env)->GetStringUTFChars(env, devName, NULL)  ;
	char deviceName =  deviceNameStr[0];
	long trackNo = (long) jtrackNo; 
	printf( "the file output path is %s, the device name is %c, and the trackNo is %d" , fileOutputPath, deviceName, trackNo );
	ripTrackToFile(deviceName,(ULONG) trackNo, TEXT("C:\\mout.wav") ) ;
	 
	(*env)->ReleaseStringUTFChars(env, fPath, fileOutputPath) ;
	(*env)->ReleaseStringUTFChars(env, devName, deviceNameStr) ;
	return JNI_TRUE; 
}


/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _getMusicBrainzCompactDiscId
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1getMusicBrainzCompactDiscId
  (JNIEnv * env, jobject cePtr, jstring devName)
{
	
	
	return NULL ; 
}

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _getFreeDbCompactDiscDiscId
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1getFreeDbCompactDiscDiscId
  (JNIEnv * env, jobject cePtr, jstring deviceName)
{	
	
	/* 
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
	return NULL ; */ 
	
	return NULL ; 

	
}



