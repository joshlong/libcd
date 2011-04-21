

/**
 Clean, C-only implementation of the MacIoCtlDisc header.
 
 
 Defintions:
  
   deviceName : 'disk1', for example, on OSX 10.6 on a standard MacBook Pro.
 
   	
 */

 
 
int open_cd_drive_door(char *deviceName);

char * get_raw_device_path(char * deviceName);

char * disc_id(char * deviceName) ;

int track_count( char * deviceName);

void  read_track_to_wav_file( char * deviceName, unsigned track, char * whereToDumpWavData)   ;