#include "win_file.h"

#define USERHOME "%HOMEPATH%/"

static char *userDir = NULL;
static char *dataDir;
static char *tempDir;

static void scanDirs(void)
{
	HMODULE hModule = GetModuleHandleW(NULL);
	char path[MAX_PATH];

	//Fetch absolue .exe path
	GetModuleFileName(hModule, path, MAX_PATH);
	
	dataDir = malloc(strlen(path) + 1);
	strcpy(dataDir, path);
	ccStringTrimToChar(dataDir, '\\', true);
	ccStringReplaceChar(dataDir, '\\', '/');

	//User dir
	userDir = USERHOME;

	//Temp directory
	GetTempPath(MAX_PATH, path);

	tempDir = malloc(strlen(path) + 1);
	strcpy(tempDir, path);
	ccStringReplaceChar(tempDir, '\\', '/');
}

char *ccFileGetUserDir(void)
{
	if(userDir == NULL) scanDirs();
	return userDir;
}

char *ccFileGetDataDir(void)
{
	if(userDir == NULL) scanDirs();
	return dataDir;
}

char *ccFileGetTempDir(void)
{
	if(userDir == NULL) scanDirs();
	return tempDir;
}

ccFileInfo ccFileGetInfo(char *file)
{
	struct _stat sb;
	ccFileInfo info;

	if(_stat(file, &sb) != 0) {
		ccErrorPush(CC_ERROR_FILE_OPEN);
		info.size = 0;
		info.modified = 0;
		return info;
	}

	info.size = (unsigned long)sb.st_size;
	info.modified = (time_t)sb.st_mtime;

	return info;
}

void _ccFileFree(void)
{
	if(userDir == NULL) return;
	free(dataDir);
	free(tempDir);
	userDir = NULL;
}