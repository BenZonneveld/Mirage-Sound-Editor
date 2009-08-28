================================================================================
    MICROSOFT FOUNDATION CLASS LIBRARY : Mirage Editor Project Overview
===============================================================================

The application wizard has created this Mirage Editor application for 
you.  This application not only demonstrates the basics of using the Microsoft 
Foundation Classes but is also a starting point for writing your application.

This file contains a summary of what you will find in each of the files that
make up your Mirage Editor application.

Mirage Editor.vcproj
    This is the main project file for VC++ projects generated using an application wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    application wizard.

Mirage Editor.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CMirageEditorApp application class.

Mirage Editor.cpp
    This is the main application source file that contains the application
    class CMirageEditorApp.

Mirage Editor.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
    Visual C++. Your project resources are in 1033.

res\Mirage Editor.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file Mirage Editor.rc.

res\MirageEditor.rc2
    This file contains resources that are not edited by Microsoft 
    Visual C++. You should place all resources not editable by
    the resource editor in this file.

Mirage Editor.reg
    This is an example .reg file that shows you the kind of registration
    settings the framework will set for you.  You can use this as a .reg
    file to go along with your application or just delete it and rely
    on the default RegisterShellFileTypes registration.


/////////////////////////////////////////////////////////////////////////////

For the main frame window:
    The project includes a standard MFC interface.

MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CMDIFrameWnd and controls all MDI frame features.

res\Toolbar.bmp
    This bitmap file is used to create tiled images for the toolbar.
    The initial toolbar and status bar are constructed in the CMainFrame
    class. Edit this toolbar bitmap using the resource editor, and
    update the IDR_MAINFRAME TOOLBAR array in Mirage Editor.rc to add
    toolbar buttons.
/////////////////////////////////////////////////////////////////////////////

For the child frame window:

ChildFrm.h, ChildFrm.cpp
    These files define and implement the CChildFrame class, which
    supports the child windows in an MDI application.

/////////////////////////////////////////////////////////////////////////////

The application wizard creates one document type and one view:

Mirage EditorDoc.h, Mirage EditorDoc.cpp - the document
    These files contain your CMirageEditorDoc class.  Edit these files to
    add your special document data and to implement file saving and loading
    (via CMirageEditorDoc::Serialize).
    The Document will have the following strings:
        File extension:      emd
        File type ID:        MirageSampleDump.Document
        Main frame caption:  Mirage Editor
        Doc type name:       MirageSampDump
        Filter name:         Mirage Editor Files (*.emd)
        File new short name: Mirage Editor
        File type long name: Mirage Sample Dump.Document

Mirage EditorView.h, Mirage EditorView.cpp - the view of the document
    These files contain your CMirageEditorView class.
    CMirageEditorView objects are used to view CMirageEditorDoc objects.

res\Mirage EditorDoc.ico
    This is an icon file, which is used as the icon for MDI child windows
    for the CMirageEditorDoc class.  This icon is included by the main
    resource file Mirage Editor.rc.


/////////////////////////////////////////////////////////////////////////////

The application wizard has also created classes specific to OLE

CntrItem.h, CntrItem.cpp
    These files contain your CMirageEditorCntrItem class. This class is used to 
    manipulate OLE objects.  OLE objects are usually displayed by your
    CMirageEditorView class and serialized as part of your CMirageEditorDoc class.

    The project has support for Compound files. The Compound-file format stores a document 
    that contains one or more Automation objects to one file and still allows access to 
    the it for the individual objects.

/////////////////////////////////////////////////////////////////////////////

Help Support:

hlp\MirageEditor.hhp
    This file is a help project file. It contains the data needed to
    compile the help files into a .chm file.

hlp\MirageEditor.hhc
    This file lists the contents of the help project.

hlp\MirageEditor.hhk
    This file contains an index of the help topics.

hlp\afxcore.htm
    This file contains the standard help topics for standard MFC
    commands and screen objects. Add your own help topics to this file.

makehtmlhelp.bat
    This file is used by the build system to compile the help files.

hlp\Images\*.gif
    These are bitmap files required by the standard help file topics for
    Microsoft Foundation Class Library standard commands.


/////////////////////////////////////////////////////////////////////////////

Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named Mirage Editor.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

Mirage Editor.manifest
	Application manifest files are used by Windows XP to describe an applications 
	dependency on specific versions of Side-by-Side assemblies. The loader uses this 
	information to load the appropriate assembly from the assembly cache or private 
	from the application. The Application manifest  maybe included for redistribution 
	as an external .manifest file that is installed in the same folder as the application 
	executable or it may be included in the executable in the form of a resource. 
/////////////////////////////////////////////////////////////////////////////

Other notes:

The application wizard uses "TODO:" to indicate parts of the source code you
should add to or customize.

If your application uses MFC in a shared DLL, you will need 
to redistribute the MFC DLLs. If your application is in a language 
other than the operating system's locale, you will also have to 
redistribute the corresponding localized resources MFC80XXX.DLL. 
For more information on both of these topics, please see the section on 
redistributing Visual C++ applications in MSDN documentation. 

/////////////////////////////////////////////////////////////////////////////
