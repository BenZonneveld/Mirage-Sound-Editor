// UpdateCheck.cpp
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "wininet.h"
#include "UpdateCheck.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUpdateCheck::CUpdateCheck()
{

}

CUpdateCheck::~CUpdateCheck()
{
}

BOOL CUpdateCheck::GetFileVersion(DWORD &dwMS, DWORD &dwLS)
{
	char szModuleFileName[MAX_PATH];

    LPBYTE  lpVersionData; 

	if (GetModuleFileName(AfxGetInstanceHandle(), szModuleFileName, sizeof(szModuleFileName)) == 0) return FALSE;

    DWORD dwHandle;     
    DWORD dwDataSize = ::GetFileVersionInfoSize(szModuleFileName, &dwHandle); 
    if ( dwDataSize == 0 ) 
        return FALSE;

    lpVersionData = new BYTE[dwDataSize]; 
    if (!::GetFileVersionInfo(szModuleFileName, dwHandle, dwDataSize, (void**)lpVersionData) )
    {
		delete [] lpVersionData;
        return FALSE;
    }

    ASSERT(lpVersionData != NULL);

    UINT nQuerySize;
	VS_FIXEDFILEINFO* pVsffi;
    if ( ::VerQueryValue((void **)lpVersionData, _T("\\"),
                         (void**)&pVsffi, &nQuerySize) )
    {
		dwMS = pVsffi->dwFileVersionMS;
		dwLS = pVsffi->dwFileVersionLS;
		delete [] lpVersionData;
        return TRUE;
    }

	delete [] lpVersionData;
    return FALSE;

}

void CUpdateCheck::Check(UINT uiURL)
{
	CString strURL(MAKEINTRESOURCE(uiURL));
	Check(strURL);
}

void CUpdateCheck::Check(const CString& strURL)
{
	DWORD dwMS, dwLS;
	if (!GetFileVersion(dwMS, dwLS))
	{
		ASSERT(FALSE);
		return;
	}

	CWaitCursor wait;
	HINTERNET hInet = InternetOpen(UPDATECHECK_BROWSER_STRING, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, NULL);
	HINTERNET hUrl = InternetOpenUrl(hInet, strURL, NULL, -1L,
										 INTERNET_FLAG_RELOAD | INTERNET_FLAG_PRAGMA_NOCACHE |
										 INTERNET_FLAG_NO_CACHE_WRITE |WININET_API_FLAG_ASYNC, NULL);
	if (hUrl)
	{
		char szBuffer[65536];
		DWORD dwRead;
		if (InternetReadWholeFile(hUrl, szBuffer, sizeof(szBuffer), &dwRead))
		{
			if (dwRead > 0)
			{
				szBuffer[dwRead] = 0;
				CString strSubMS1;
				CString strSubMS2;
				CString strSubMS3;
				CString strSub;
				DWORD dwMSWeb;
				WORD wMSWeb;
				DWORD dwMSFile;
				AfxExtractSubString(strSubMS1, szBuffer, 0, '.');
				AfxExtractSubString(strSubMS2, szBuffer, 1, '.');
				AfxExtractSubString(strSubMS3, szBuffer, 2, '.');
				wMSWeb = MAKEWORD((BYTE) atoi(strSubMS2),(BYTE) atoi(strSubMS1));
				dwMSWeb = MAKELONG((WORD) atol(strSubMS3), wMSWeb);

				dwMSFile = MAKELONG(HIWORD(dwLS),
														MAKEWORD((BYTE) LOWORD(dwMS), (BYTE) HIWORD(dwMS))); 

				if (dwMSWeb > dwMSFile)
				{
					MsgUpdateAvailable(dwMS, dwLS, dwMSWeb, 0, strURL);
				}
/*
 * When there are no updates don't give a messagebox.
 *
				else
					MsgUpdateNotAvailable(dwMS, dwLS);
*/
			}
			else
				MsgUpdateNoCheck(dwMS, dwLS);

		}
		InternetCloseHandle(hUrl);
	}
	else
		MsgUpdateNoCheck(dwMS, dwLS);

	InternetCloseHandle(hInet);
}

HINSTANCE CUpdateCheck::GotoURL(LPCTSTR url, int showcmd)
{
    TCHAR key[MAX_PATH + MAX_PATH];

    // First try ShellExecute()
    HINSTANCE result = ShellExecute(NULL, _T("open"), url, NULL,NULL, showcmd);

    // If it failed, get the .htm regkey and lookup the program
    if ((UINT)result <= HINSTANCE_ERROR) 
	{

        if (GetRegKey(HKEY_CLASSES_ROOT, _T(".htm"), key) == ERROR_SUCCESS) 
		{
            lstrcat(key, _T("\\shell\\open\\command"));

            if (GetRegKey(HKEY_CLASSES_ROOT,key,key) == ERROR_SUCCESS) 
			{
                TCHAR *pos;
                pos = _tcsstr(key, _T("\"%1\""));
                if (pos == NULL) {                     // No quotes found
                    pos = _tcsstr(key, _T("%1"));      // Check for %1, without quotes 
                    if (pos == NULL)                   // No parameter at all...
                        pos = key+lstrlen(key)-1;
                    else
                        *pos = '\0';                   // Remove the parameter
                }
                else
                    *pos = '\0';                       // Remove the parameter

                lstrcat(pos, _T(" "));
                lstrcat(pos, url);

                result = (HINSTANCE) WinExec(key,showcmd);
            }
        }
    }

    return result;
}

LONG CUpdateCheck::GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata)
{
    HKEY hkey;
    LONG retval = RegOpenKeyEx(key, subkey, 0, KEY_QUERY_VALUE, &hkey);

    if (retval == ERROR_SUCCESS) 
	{
        long datasize = MAX_PATH;
        TCHAR data[MAX_PATH];
        RegQueryValue(hkey, NULL, data, &datasize);
        lstrcpy(retdata,data);
        RegCloseKey(hkey);
    }

    return retval;
}


void CUpdateCheck::MsgUpdateAvailable(DWORD dwMSlocal, DWORD dwLSlocal, DWORD dwMSWeb, DWORD dwLSWeb, const CString& strURL)
{
	CString strMessage;
	strMessage.Format(IDS_UPDATE_AVAILABLE, 
										HIWORD(dwMSlocal),
										LOWORD(dwMSlocal),
										HIWORD(dwLSlocal),
										HIBYTE(HIWORD(dwMSWeb)),
										LOBYTE(HIWORD(dwMSWeb)),
										LOWORD(dwMSWeb));

	if (AfxMessageBox(strMessage, MB_YESNO|MB_ICONINFORMATION) == IDYES)
		GotoURL(strURL, SW_SHOW);
}

void CUpdateCheck::MsgUpdateNotAvailable(DWORD dwMSlocal, DWORD dwLSlocal)
{
	AfxMessageBox(IDS_UPDATE_NO, MB_OK|MB_ICONINFORMATION);
}

void CUpdateCheck::MsgUpdateNoCheck(DWORD dwMSlocal, DWORD dwLSlocal)
{
	AfxMessageBox(IDS_UPDATE_NOCHECK, MB_OK|MB_ICONINFORMATION);
}

/*******************************************************/
bool CUpdateCheck::InternetReadWholeFile(HINTERNET hUrl,
													 LPVOID lpBuffer,
													 DWORD dwNumberOfBytesToRead,
													 LPDWORD lpNumberOfBytesRead)
{
	char * strpfound=0;
	char * strpextension=0;
	DWORD dwRead=0;
	BYTE szBuffer[65536];
	memset(lpBuffer,0,dwNumberOfBytesToRead);
	*lpNumberOfBytesRead = 0;
	do{
		if (!InternetReadFile(hUrl,szBuffer,sizeof(szBuffer),&dwRead))
			dwRead = 0;
		if (dwRead!=0)
		{
			if (*lpNumberOfBytesRead + dwRead >	dwNumberOfBytesToRead) return false;//too much data for the buffer
			memcpy(&((char *)lpBuffer)[*lpNumberOfBytesRead],szBuffer,dwRead);
			strpfound=strstr((char *)lpBuffer,"MirageEdit");
			if(strpfound!=NULL)
			{
				strpfound=strstr(strstr(strpfound,"/"),"-")+1;
				strpextension=strstr(strpfound,".exe");
				*strpextension='\0';
				*lpNumberOfBytesRead+=dwRead;
				memcpy(lpBuffer,strpfound,1+(strpextension - strpfound));
				break;
			}
			*lpNumberOfBytesRead+=dwRead;
		}
	} while (dwRead != 0 && strchr((char *)lpBuffer,EOF)==NULL);
	return strchr((char *)lpBuffer,EOF)==NULL;
}