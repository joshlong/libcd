Name "CDex"
Caption "Instalaci�n de CDex"
ComponentText "Seleccione los componentes a ser instalados" 

OutFile "cdex_setup.exe" 
InstallDir $PROGRAMFILES\CDex_140b9 
ShowInstDetails show

icon CDex.ico 
UninstallIcon Uninstall.ico
EnabledBitmap cdex1.bmp 
DisabledBitmap cdex2.bmp 

UninstallText "Desinstalar CDex, pulse el bot�n SIGUIENTE para continuar "
UninstallExeName "uninstall.exe" 
InstallDirRegKey HKEY_LOCAL_MACHINE "Software\CDex" "Instpath"

LicenseText "Por favor, lea esto antes de continuar:"
LicenseData "CDex.txt" 

InstType "CDex Completo (recomendado)"
InstType "Sin Accesos Directos"

SetOverwrite on

ComponentText "Instalaci�n de CDex, por favor seleccione abajo los componentes a ser instalados." 
DirText "Seleccionar archivo(s) a instalar:"
AutoCloseWindow true
SetDatablockOptimize on
SetCompress force

Section "CDex (requisito)"
	SectionIn 1 2
	
DetailPrint " - Instalaci�n de archivos base"

FindWindowByTitle close "CDex versi�n 1.30 Final"
FindWindowByTitle close "CDex Versi�n 1.40 Beta 4"
FindWindowByTitle close "CDex Versi�n 1.40 Beta 5"
FindWindowByTitle close "CDex Versi�n 1.40 Beta 6"
FindWindowByTitle close "CDex Versi�n 1.40 Beta 7"
FindWindowByTitle close "CDex Versi�n 1.40 Beta 8"
FindWindowByTitle close "CDex Versi�n 1.40 Beta 9"
FindWindowByTitle close "CDex Versi�n 1.40"

IfFileExists $INSTDIR\CDex.exe exexist
goto noexe
exexist:
	MessageBox MB_YESNO|MB_ICONEXCLAMATION|MB_TOPMOST \
	"Fu� detectada una instalaci�n antigua de CDex. Esto normalmente no plantea ning�n problema, \
	$\r$\r�Quiere sobreescribir la instalaci�n antigua?"\
	IDYES noexe 
	
MessageBox MB_OKCANCEL|MB_ICONSTOP|MB_TOPMOST "� Continuar con la instalaci�n ?" IDCANCEL noexe
Abort

noexe:
	SetOutPath $INSTDIR
	File "CDex\CDex.exe"
	File "CDex\CDRip.DLL"
	File "CDex\LibSnd.DLL"
	File "CDex\LangDll.DLL"
	File "CDex\CDex.cnt" 
	File "CDex\CDex.hlp"
	File "CDex\changes.txt"
	File "CDex\readme.txt"
	
	WriteRegStr HKEY_LOCAL_MACHINE "Software\CDex" "Instpath" "$INSTDIR" 
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\CDex" "DisplayName" "CDex extraction audio"
	WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\CDex" "UninstallString" '"$INSTDIR\uninstall.exe"'
	SectionEnd
	
Section "Compresi�n DLL(s) (recomendado)"
	SectionIn 1 2
DetailPrint " - Instalaci�n de la compresi�n DLL(s)"
	SetOutPath $INSTDIR
	File "CDex\lame_enc.dll"
	File "CDex\Vorbis.DLL"
	File "CDex\MP2Enc.DLL"
SectionEnd
	
Section "Plugins de Winamp (recomendado)"
	SectionIn 1 2
DetailPrint " - Instalaci�n de Plugins de WinAmp"
	SetOutPath $INSTDIR\Plugins
	File "CDex\Plugins\in_mp3.dll"
	File "CDex\Plugins\in_vorbis.dll"
	File "CDex\Plugins\in_vorbis.txt"
	SetOutPath $INSTDIR
SectionEnd

Section "Configuraci�n Autom�tica"
	SectionIn 1
DetailPrint " - Configuraci�n en curso"

IfFileExists $INSTDIR\cdex.ini iniexist
goto noini
iniexist:
	MessageBox MB_YESNO|MB_ICONEXCLAMATION|MB_TOPMOST \
	"Fu� encontrado un fichero de configuraci�n.$\r$\r�Quiere borrar el fichero de configuraci�n antiguo?" \
	IDNO noauto
delete $INSTDIR\cdex.ini 
	
noini:
	ReadRegStr $0 HKEY_CURRENT_USER "Software\Microsoft\Windows\CurrentVersion\Explorer\Shell Folders" "Personal"
	SetOutPath "$INSTDIR"
	File "CDex\CDex.INI"
	WriteINIStr "$INSTDIR\CDex.ini" "General" "MP3OutputDir" $0\MP3
	WriteINIStr "$INSTDIR\CDex.ini" "General" "CnvOutputDir" $0\MP3
	WriteINIStr "$INSTDIR\CDex.ini" "General" "PlayDir" $0\MP3
	WriteINIStr "$INSTDIR\CDex.ini" "General" "WAVInputDir" $0\MP3\WAV
	WriteINIStr "$INSTDIR\CDex.ini" "General" "CDDBPath" $0\MP3\CDDB
	WriteINIStr "$INSTDIR\CDex.ini" "General" "WAVInputDir" $0\MP3\WAV
	WriteINIStr "$INSTDIR\CDex.ini" "General" "MP3InputDir" $0\MP3
noauto: 
SectionEnd 

SectionDivider

Section "Creando acceso directo en Escritorio" 
	SectionIn 1
DetailPrint " - Creando acceso directo en Escritorio"
	SetOutPath $INSTDIR
	CreateShortCut "$DESKTOP\CDex.lnk" "$INSTDIR\CDex.exe"
SectionEnd


Section "Creando acceso directo en Men� Inicio"
	SectionIn 1 
DetailPrint " - Creando acceso directo en Men� Inicio"
	CreateDirectory $SMPROGRAMS\CDex
	SetOutPath $INSTDIR
	CreateShortCut "$SMPROGRAMS\CDex\CDex.lnk" "$INSTDIR\CDex.exe"
	CreateShortCut "$SMPROGRAMS\CDex\CDex Help.lnk" "$INSTDIR\CDex.hlp"
	CreateShortCut "$SMPROGRAMS\CDex\CDex Uninstall.lnk" "$INSTDIR\uninstall.exe"
	CreateShortCut "$SMPROGRAMS\CDex\CDex Site.lnk" "http://cdexos.sourceforge.net/"
SectionEnd

Function .onInstSuccess
	MessageBox MB_YESNO|MB_ICONQUESTION|MB_TOPMOST "� Enhorabuena ! la instalaci�n de CDex ha sido completada.$\r$\r� Ver el archivo readme (recomendado) ?" IDNO nohelp 
	ExecShell open "$INSTDIR\readme.txt"
goto fxend
	nohelp:
		fxend:
FunctionEnd

Section "Desinstalar"

FindWindowByTitle close "CDex versi�n 1.30 Final"
FindWindowByTitle close "CDex Versi�n 1.40 Beta 4"
FindWindowByTitle close "CDex Versi�n 1.40 Beta 5"
FindWindowByTitle close "CDex Versi�n 1.40 Beta 6"
FindWindowByTitle close "CDex Versi�n 1.40 Beta 7"
FindWindowByTitle close "CDex Versi�n 1.40 Beta 8"
FindWindowByTitle close "CDex Versi�n 1.40 Beta 9"
FindWindowByTitle close "CDex Versi�n 1.40 Final"

DetailPrint " - Los siguientes archivos son suprimidos ..."
	Delete /rebootok "$INSTDIR\CDex.exe"
	Delete /rebootok "$INSTDIR\CDRip.DLL"
	Delete /rebootok "$INSTDIR\lame_enc.dll"
	Delete /rebootok "$INSTDIR\LibSnd.DLL"
	Delete /rebootok "$INSTDIR\LangDll.DLL"
	Delete /rebootok "$INSTDIR\MP2Enc.DLL"
	Delete /rebootok "$INSTDIR\CDex.CNT"
	Delete /rebootok "$INSTDIR\CDex.HLP"
	Delete /rebootok "$INSTDIR\Changes.TXT"
	Delete /rebootok "$INSTDIR\Readme.TXT"
	Delete /rebootok "$INSTDIR\Vorbis.DLL"
	Delete /rebootok "$INSTDIR\Plugins\in_mp3.dll"
	Delete /rebootok "$INSTDIR\Plugins\in_vorbis.dll"
	Delete /rebootok "$INSTDIR\Plugins\in_vorbis.txt"
	DetailPrint " - Los accesos directos son suprimidos ..."
	Delete /rebootok "$DESKTOP\CDex.lnk"
	Delete /rebootok "$SMPROGRAMS\CDex\CDex.lnk"
	Delete /rebootok "$SMPROGRAMS\CDex\CDex Help.lnk"
	Delete /rebootok "$SMPROGRAMS\CDex Uninstall.lnk"
	Delete /rebootok "$SMPROGRAMS\CDex\Internet\CDex Site.lnk"
	
	rmdir /r "$SMPROGRAMS\CDex"
	
DetailPrint " - Las entradas de registro son suprimidas ..."
	DeleteRegValue HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\CDex" "DisplayName"
	DeleteRegValue HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\CDex" "UninstallString"

MessageBox MB_YESNO|MB_ICONQUESTION|MB_TOPMOST \
"La desinstalaci�n ha finalizado. �Desea borrar todos los contenidos del archivo de CDex as� como tambi�n los archivos de configuraci�n? (no recomendado)" \ 
IDNO nodel 

DetailPrint " - El archivo(s) CDex y sus preferencias son suprimidas ..." 


	rmdir /r "$INSTDIR"
	Delete /rebootok "$WINDIR\CDex.ini" 

nodel:

DetailPrint " - � La desinstalaci�n ha finalizado !" 


SectionEnd  
