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
#include <openssl/md5.h>

#define BUFSIZE MAX_PATH

unsigned char virus_hash[] = "\xaf\x28\x7a\x22\x85\x98\x1f\xf3\x4d\xb7\x02\xa7\xef\x07\x44\xee";

bool check_if_virus(TCHAR *exe_file) {
    FILE *exe_handle = _tfopen(exe_file, L"rb");

    fseek(exe_handle, 0L, SEEK_END);
    unsigned long long exe_size = ftell(exe_handle);
    unsigned char *buf = (unsigned char*)malloc(exe_size);

    fseek(exe_handle, 0L, SEEK_SET);
    fread(buf, exe_size, 1, exe_handle);
    fclose(exe_handle);

    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5(buf, exe_size, (unsigned char*)&digest);

    /*
    for(int i = 0; i < 16; ++i) {
        printf("%02x", digest[i]);
    }
    printf("\n");
    */

    if (strncmp((char*)virus_hash, (char*)digest, MD5_DIGEST_LENGTH) == 0) {
        return true;
    }

    return false;
}

void CLEAN_IT(TCHAR *root) {
    TCHAR root_find[BUFSIZE];
    _tcscpy(root_find, root);
    _tcscat(root_find, L"\\*");

    _tprintf(L"Cleaning directory: %s\n", root_find);

    WIN32_FIND_DATA data;
    HANDLE hFind = FindFirstFile(root_find, &data);

    if ( hFind != INVALID_HANDLE_VALUE ) {
        do {
            //_tprintf(L"%s\n", data.cFileName);

            if (_tcscmp(data.cFileName, L".") == 0 || _tcscmp(data.cFileName, L"..") == 0) {
                continue;
            }

            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                TCHAR next_root[BUFSIZE] = L"";
                _tcscpy(next_root, root);
                _tcscat(next_root, L"\\");
                _tcscat(next_root, data.cFileName);
                //_tcscat(next_root, L"\\*");
                //_tprintf(L"%s\n", next_root);
                CLEAN_IT(next_root);
                continue;
            }

            int fname_len = _tcslen(data.cFileName);
            //printf("%d\n", fname_len);
            if (fname_len <= 4)
                continue;

            if (_tcscmp(data.cFileName + fname_len - 4, L".txt") == 0) {
                TCHAR txt_file[BUFSIZE] = L"";
                _tcscpy(txt_file, root);
                _tcscat(txt_file, L"\\");
                _tcscat(txt_file, data.cFileName);
                _tprintf(L"Found txt file, revealing: %s\n", txt_file);

                TCHAR exe_file[BUFSIZE] = L"";
                _tcscpy(exe_file, root);
                _tcscat(exe_file, L"\\");
                _tcsncat(exe_file, data.cFileName, fname_len - 4);
                _tcscat(exe_file, L".exe");
                //_tprintf(L"EXE: %s\n", exe_file);

                if (PathFileExists(exe_file)) {
                    //printf("hello!\n");

                    if (check_if_virus(exe_file)) {
                        _tprintf(L"FOUND VIRUS, REMOVING: %s\n", exe_file);
                        DeleteFile(exe_file);
                    }
                }

                SetFileAttributes(txt_file, GetFileAttributes(txt_file) & ~FILE_ATTRIBUTE_HIDDEN);

            } else if (_tcscmp(data.cFileName + fname_len - 4, L".exe") == 0) {
                TCHAR exe_file[BUFSIZE] = L"";
                _tcscpy(exe_file, root);
                _tcscat(exe_file, L"\\");
                _tcscat(exe_file, data.cFileName);
                //_tprintf(L"EXE: %s\n", exe_file);

                if (check_if_virus(exe_file)) {
                    _tprintf(L"FOUND VIRUS, REMOVING: %s\n", exe_file);
                    DeleteFile(exe_file);
                }

                TCHAR txt_file[BUFSIZE] = L"";
                _tcscpy(txt_file, root);
                _tcscat(txt_file, L"\\");
                _tcsncat(txt_file, data.cFileName, fname_len - 4);
                _tcscat(txt_file, L".txt");
                //_tprintf(L"TXT: %s\n", txt_file);
                _tprintf(L"Found txt file, revealing: %s\n", txt_file);

                if (PathFileExists(txt_file)) {
                    SetFileAttributes(txt_file, GetFileAttributes(txt_file) & ~FILE_ATTRIBUTE_HIDDEN);
                }
            }
        } while (FindNextFile(hFind, &data));
        FindClose(hFind);
    }
}

extern "C"
int _tmain(int argc, TCHAR *argv[]) {
    if (argc < 2) {
        _tprintf(L"Usage: %s cleaner_root_directory\n", argv[0]);
        return 0;
    }

    if (!PathFileExists(argv[1]) || !(GetFileAttributes(argv[1]) & FILE_ATTRIBUTE_DIRECTORY)) {
        _tprintf(L"%s is not valid directory!\n", argv[1]);
        return 0;
    }

    CLEAN_IT(argv[1]);
}
