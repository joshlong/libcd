#include <AtlBase.h>
 
#include "stdafx.h"
//#include <time.h>
#include <jni.h>
#include "atlbase.h"
#include "atlstr.h"
//#include "comutil.h>
//extern "C++" {
//#include "CAudioCD.h"  
//}
#include "cdripper_interop.h"
#include "com_joshlong_cdworld_utils_cdda_AudioCompactDisc.h"
#include <windows.h>
#define NUM_OF(x) (sizeof (x) / sizeof *(x))
/** 
 * http://books.google.com/books?id=NFnBRcu8DHEC&pg=PA24&lpg=PA24&dq=how+do+I+convert+jstring+to+a+char+*&source=web&ots=K_9t8bV1WN&sig=Syj5OxvJ0I9a_ekc515EqeCYbeg&hl=en&sa=X&oi=book_result&resnum=6&ct=result#PPA25,M1
 **/

 
/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _countTracksOnAudioCompactDisc
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1countTracksOnAudioCompactDisc
	(JNIEnv * env, jobject o, jstring deviceName)  { 
	const char * drive =  (*env).GetStringUTFChars( deviceName, NULL) ; 
	int driveNo =atoi( drive);
	jint cnt = (jint)countTracks(driveNo) ;
	(*env).ReleaseStringUTFChars(deviceName,drive);
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
	const char * drive =  (*env).GetStringUTFChars( deviceName, NULL) ; 
	int driveNo =atoi( drive);
	jint cnt = (jint)eject(driveNo) ;
	(*env).ReleaseStringUTFChars(deviceName,drive);
	return JNI_TRUE ;
}

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _injectCompactDiscIntoCompactDiscDrive
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1injectCompactDiscIntoCompactDiscDrive
  (JNIEnv * env, jobject cePtr, jstring deviceName)
{ 
	
	const char * drive =  (*env).GetStringUTFChars( deviceName, NULL) ; 
	int driveNo =atoi( drive);
	jint cnt = (jint)inject(driveNo) ;
	(*env).ReleaseStringUTFChars(deviceName,drive);
	return JNI_TRUE ;
}


/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _isCompactDiscDriveDoorOpen
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1isCompactDiscDriveDoorOpen
  (JNIEnv * env, jobject o, jstring deviceName)
{
	const char * drive =  (*env).GetStringUTFChars( deviceName, NULL) ; 
	int driveNo =atoi( drive);
	BOOL is_open = isOpen(driveNo);
	(*env).ReleaseStringUTFChars(deviceName,drive);
	return is_open ==TRUE? JNI_TRUE  : JNI_FALSE;
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

/*
* Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
* Method:    _ripTrackFromCompactDisc
* Signature: (Ljava/lang/String;ILjava/lang/String;)Z
*/
JNIEXPORT jboolean JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1ripTrackFromCompactDisc
(JNIEnv * env, jobject ce, jstring devName, jint jtrackNo, jstring fPath)
{

	char * fileOutputPath = (char *) (*env).GetStringUTFChars( fPath, NULL);
	char * deviceNameStr =  (char *) (*env).GetStringUTFChars( devName, NULL);
	
//	std::cout<< "the file output path is "<< fileOutputPath << std::endl ; 

	 

	int driveNo =atoi( deviceNameStr);


	size_t origsize = strlen(fileOutputPath) + 1;
    const size_t newsize = 300;
    size_t convertedChars = 0;
    wchar_t wcstring[newsize];
    mbstowcs_s(&convertedChars, wcstring, origsize, fileOutputPath, _TRUNCATE);
    wcscat_s(wcstring, L" (wchar_t *)");
	BOOL worked = ripTrackToFile( driveNo,(int)jtrackNo, (LPCWSTR) wcstring ) ;
	(*env).ReleaseStringUTFChars(devName,deviceNameStr);
	(*env).ReleaseStringUTFChars(fPath,fileOutputPath); 

 
	return worked == TRUE ? JNI_TRUE : JNI_FALSE; 
}
 
/* 
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _getFreeDbCompactDiscDiscId
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1getFreeDbCompactDiscDiscId
  (JNIEnv * env, jobject cePtr, jstring deviceName)
{	 
	const char * drive =  (*env).GetStringUTFChars( deviceName, NULL) ; 
	int driveNo =atoi( drive);
	std::string discId = getDiscId(driveNo) ;
	(*env).ReleaseStringUTFChars(deviceName,drive);
	jstring answer = (*env).NewStringUTF( discId.c_str()) ; 
	return answer ;
}



JNIEXPORT jobjectArray JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1listDiscDevices
(JNIEnv * env, jobject cePtr) 
{ 


	int   size  =  countDevices()  ;

//	std::cout << "arrayOfIds"<< std::endl ;
	//int size=NUM_OF(arrayOfIds);
	//std::cout << "size of arrayOfIds "<<  size << std::endl ;
	/// 
	jobjectArray sArr =(jobjectArray)(*env).NewObjectArray(size, 
		env->FindClass("java/lang/String"),
         env->NewStringUTF("")) ;
	
	// loop through arrayOfIds, convert to string, set the value in the jobjectArray
	int indx =0 ; 
	for(indx =0;indx< size ;indx++){ 
	 int id = indx;
	// std::cout<< "the id is "<< id << std::endl ;
	 
	 char * strForId  =new char[10] ; 
	 
	 strForId = itoa( id,strForId, 10  ) ; ///str=itoa(i, str, 10); // 10 - decimal; 
	 jstring s = (*env).NewStringUTF(strForId );
	 (*env).SetObjectArrayElement(sArr,indx,s);
	 delete strForId; 
	}
	return sArr ;
}

/*
 * Class:     com_joshlong_cdworld_utils_cdda_AudioCompactDisc
 * Method:    _getDiscDeviceDetailsForDisc
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_joshlong_cdworld_utils_cdda_AudioCompactDisc__1getDiscDeviceDetailsForDisc
(JNIEnv * env , jobject cePtr, jstring deviceName) 
{
	const char * drive =  (*env).GetStringUTFChars( deviceName, NULL) ; 
	int driveNo =atoi( drive);
	std::string  name =  getProductNameForDevice(driveNo) ; 
	(*env).ReleaseStringUTFChars(deviceName,drive);
	const char * cStr=name.c_str() ; 

	jstring answer = (*env).NewStringUTF(cStr) ;

	return answer ;
	
}