# Microsoft Developer Studio Project File - Name="glcylindrix" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=glcylindrix - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "glcylindrix.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "glcylindrix.mak" CFG="glcylindrix - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "glcylindrix - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "glcylindrix - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "glcylindrix - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "GLCYLINDRIX" /FR /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dsound.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "glcylindrix - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "GLCYLINDRIX" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib dsound.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "glcylindrix - Win32 Release"
# Name "glcylindrix - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "system"

# PROP Default_Filter ""
# Begin Group "win32"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\system\win32\sound.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\win32\system.cpp
# End Source File
# Begin Source File

SOURCE=.\src\system\win32\winmain.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\system\system.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\ai.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ai.h
# End Source File
# Begin Source File

SOURCE=.\src\ai_move.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ai_move.h
# End Source File
# Begin Source File

SOURCE=.\src\ai_util.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ai_util.h
# End Source File
# Begin Source File

SOURCE=.\src\base.cpp
# End Source File
# Begin Source File

SOURCE=.\src\base.h
# End Source File
# Begin Source File

SOURCE=.\src\clipping.cpp
# End Source File
# Begin Source File

SOURCE=.\src\clipping.h
# End Source File
# Begin Source File

SOURCE=.\src\collide.cpp
# End Source File
# Begin Source File

SOURCE=.\src\collide.h
# End Source File
# Begin Source File

SOURCE=.\src\commands.cpp
# End Source File
# Begin Source File

SOURCE=.\src\commands.h
# End Source File
# Begin Source File

SOURCE=.\src\config.cpp
# End Source File
# Begin Source File

SOURCE=.\src\config.h
# End Source File
# Begin Source File

SOURCE=.\src\do_state.cpp
# End Source File
# Begin Source File

SOURCE=.\src\do_state.h
# End Source File
# Begin Source File

SOURCE=.\src\energy.cpp
# End Source File
# Begin Source File

SOURCE=.\src\energy.h
# End Source File
# Begin Source File

SOURCE=.\src\events.cpp
# End Source File
# Begin Source File

SOURCE=.\src\events.h
# End Source File
# Begin Source File

SOURCE=.\src\explode.cpp
# End Source File
# Begin Source File

SOURCE=.\src\explode.h
# End Source File
# Begin Source File

SOURCE=.\src\fli.cpp
# End Source File
# Begin Source File

SOURCE=.\src\fli.h
# End Source File
# Begin Source File

SOURCE=.\src\fx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\fx.h
# End Source File
# Begin Source File

SOURCE=.\src\gameloop.cpp
# End Source File
# Begin Source File

SOURCE=.\src\gameloop.h
# End Source File
# Begin Source File

SOURCE=.\src\glcylindrix.cpp
# End Source File
# Begin Source File

SOURCE=.\src\glcylindrix.h
# End Source File
# Begin Source File

SOURCE=.\src\glext.h
# End Source File
# Begin Source File

SOURCE=.\src\glh_genext.h
# End Source File
# Begin Source File

SOURCE=.\src\hud.cpp
# End Source File
# Begin Source File

SOURCE=.\src\hud.h
# End Source File
# Begin Source File

SOURCE=.\src\jonsb.cpp
# End Source File
# Begin Source File

SOURCE=.\src\jonsb.h
# End Source File
# Begin Source File

SOURCE=.\src\keys.cpp
# End Source File
# Begin Source File

SOURCE=.\src\keys.h
# End Source File
# Begin Source File

SOURCE=.\src\level.cpp
# End Source File
# Begin Source File

SOURCE=.\src\level.h
# End Source File
# Begin Source File

SOURCE=.\src\main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\main.h
# End Source File
# Begin Source File

SOURCE=.\src\menu.cpp
# End Source File
# Begin Source File

SOURCE=.\src\menu.h
# End Source File
# Begin Source File

SOURCE=.\src\motor.cpp
# End Source File
# Begin Source File

SOURCE=.\src\motor.h
# End Source File
# Begin Source File

SOURCE=.\src\movement.cpp
# End Source File
# Begin Source File

SOURCE=.\src\movement.h
# End Source File
# Begin Source File

SOURCE=.\src\object.cpp
# End Source File
# Begin Source File

SOURCE=.\src\object.h
# End Source File
# Begin Source File

SOURCE=.\src\omega.cpp
# End Source File
# Begin Source File

SOURCE=.\src\omega.h
# End Source File
# Begin Source File

SOURCE=.\src\path.cpp
# End Source File
# Begin Source File

SOURCE=.\src\path.h
# End Source File
# Begin Source File

SOURCE=.\src\pcx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pcx.h
# End Source File
# Begin Source File

SOURCE=.\src\prim.cpp
# End Source File
# Begin Source File

SOURCE=.\src\prim.h
# End Source File
# Begin Source File

SOURCE=.\src\project.cpp
# End Source File
# Begin Source File

SOURCE=.\src\project.h
# End Source File
# Begin Source File

SOURCE=.\src\pylon.cpp
# End Source File
# Begin Source File

SOURCE=.\src\pylon.h
# End Source File
# Begin Source File

SOURCE=.\src\radar.cpp
# End Source File
# Begin Source File

SOURCE=.\src\radar.h
# End Source File
# Begin Source File

SOURCE=.\src\states.cpp
# End Source File
# Begin Source File

SOURCE=.\src\states.h
# End Source File
# Begin Source File

SOURCE=.\src\stats.cpp
# End Source File
# Begin Source File

SOURCE=.\src\stats.h
# End Source File
# Begin Source File

SOURCE=.\src\stub.cpp
# End Source File
# Begin Source File

SOURCE=.\src\stub.h
# End Source File
# Begin Source File

SOURCE=.\src\system.h
# End Source File
# Begin Source File

SOURCE=.\src\tanks.cpp
# End Source File
# Begin Source File

SOURCE=.\src\tanks.h
# End Source File
# Begin Source File

SOURCE=.\src\text.cpp
# End Source File
# Begin Source File

SOURCE=.\src\text.h
# End Source File
# Begin Source File

SOURCE=.\src\types.h
# End Source File
# Begin Source File

SOURCE=.\src\util.cpp
# End Source File
# Begin Source File

SOURCE=.\src\util.h
# End Source File
# Begin Source File

SOURCE=.\src\voices.cpp
# End Source File
# Begin Source File

SOURCE=.\src\voices.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
