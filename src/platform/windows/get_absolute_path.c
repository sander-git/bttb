#include "platform_windows.h"

#include <io.h>
#include <search.h>
#include <stdio.h>
#include <string.h>

// convert relative path to absolute path
int get_absolute_path(char *absolute_path, const char *relative_path, size_t max)
{
	char *tmp_p;
	size_t len;

	// MSVC
	struct _finddatai64_t c_file;
	intptr_t handle;

	// This function replaces "/" to "\" automatically.
	if (_fullpath(absolute_path, relative_path, max) == NULL){
		perror("Failed to make absolute path");
		return 1;
	}

	// When the file exists, check each path component.
	handle = _findfirst64(absolute_path, &c_file);
	if (handle != (intptr_t) -1){
		_findclose(handle);

		// Even when case insensitive, use the original case for path component.
		len = strlen(c_file.name);
		tmp_p = strrchr(absolute_path, '\\');
		if (tmp_p != NULL){
			memcpy(tmp_p + 1, c_file.name, len);
		}

		// Check drive letter.
		tmp_p = absolute_path;
		if (tmp_p[1] == ':'){
			if ( (tmp_p[0] >= 'a') && (tmp_p[0] <= 'z') ){
				// Convert from lower case to upper case.
				tmp_p[0] -= 'a' - 'A';
			}
			tmp_p = strchr(tmp_p, '\\');
			if (tmp_p != NULL){
				tmp_p[0] = '/';
				tmp_p++;
			}
		}

		// Check each path component.
		tmp_p = strchr(tmp_p, '\\');
		while (tmp_p != NULL){
			tmp_p[0] = 0;

			//printf("find = %s\n", absolute_path);
			handle = _findfirst64(absolute_path, &c_file);
			if (handle != (intptr_t) -1){
				_findclose(handle);

				//printf("component = %s\n", c_file.name);
				len = strlen(c_file.name);
				memcpy(tmp_p - len, c_file.name, len);
			}

			// Replace directory mark from Windows OS style "\" to UNIX style "/" for compatibility.
			tmp_p[0] = '/';
			tmp_p = strchr(tmp_p + 1, '\\');
		}

	} else {
		// Even when the file doesn't exist, replace directory mark.
		tmp_p = absolute_path;
		tmp_p = strchr(tmp_p, '\\');
		while (tmp_p != NULL){
			tmp_p[0] = '/';
			tmp_p = strchr(tmp_p + 1, '\\');
		}
	}

	return 0;
}

#include <windows.h>
#include <stdint.h>
#include <sys/stat.h>

int win32_stat64(const char *path, struct _stat64 *st)
{
	// Convert path to UTF-16
	int len = MultiByteToWideChar(CP_UTF8, 0, path, -1, NULL, 0);
	if (len <= 0) return -1;
	wchar_t *wpath = (wchar_t *)malloc(len * sizeof(wchar_t));
	if (!wpath) return -1;
	MultiByteToWideChar(CP_UTF8, 0, path, -1, wpath, len);

	// Try standard _wstat64 first
	int ret = _wstat64(wpath, st);
	if (ret == 0) {
		free(wpath);
		return 0;
	}

	// Open file or directory following symlinks
	HANDLE hFile = CreateFileW(
		wpath,
		FILE_READ_ATTRIBUTES,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);

	if (hFile == INVALID_HANDLE_VALUE) {
		// Try opening the symlink itself (without following it)
		hFile = CreateFileW(
			wpath,
			FILE_READ_ATTRIBUTES,
			FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,
			FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT,
			NULL
		);
	}

	if (hFile != INVALID_HANDLE_VALUE) {
		BY_HANDLE_FILE_INFORMATION fi;
		if (GetFileInformationByHandle(hFile, &fi)) {
			memset(st, 0, sizeof(*st));
			st->st_size = ((int64_t)fi.nFileSizeHigh << 32) | fi.nFileSizeLow;

			ULARGE_INTEGER ull;
			ull.LowPart = fi.ftLastWriteTime.dwLowDateTime;
			ull.HighPart = fi.ftLastWriteTime.dwHighDateTime;
			st->st_mtime = (ull.QuadPart - 116444736000000000ULL) / 10000000ULL;

			ull.LowPart = fi.ftLastAccessTime.dwLowDateTime;
			ull.HighPart = fi.ftLastAccessTime.dwHighDateTime;
			st->st_atime = (ull.QuadPart - 116444736000000000ULL) / 10000000ULL;

			ull.LowPart = fi.ftCreationTime.dwLowDateTime;
			ull.HighPart = fi.ftCreationTime.dwHighDateTime;
			st->st_ctime = (ull.QuadPart - 116444736000000000ULL) / 10000000ULL;

			if (fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				st->st_mode = _S_IFDIR | _S_IREAD | _S_IWRITE | _S_IEXEC;
			} else {
				st->st_mode = _S_IFREG | _S_IREAD | _S_IWRITE;
			}
			CloseHandle(hFile);
			free(wpath);
			return 0;
		}
		CloseHandle(hFile);
	}

	free(wpath);
	return -1;
}