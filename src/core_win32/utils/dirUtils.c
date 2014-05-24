#include "dirUtils.h"

static char *userDir = NULL;
static char *dataDir;
static char *tempDir;

static void scanDirs()
{
	HMODULE hModule = GetModuleHandleW(NULL);
	WCHAR path[MAX_PATH];
	int l;
	GetModuleFileName(hModule, path, MAX_PATH);
	
	dataDir = malloc(ccStrLength(path));
	ccStrCopy(path, dataDir);
	ccStrTrimToChar(dataDir, '\\', true);
	ccStrReplaceChar(dataDir, '\\', '/');
}

char *ccGetUserDir()
{
	if(userDir == NULL) scanDirs();
	return userDir;
}

char *ccGetDataDir()
{
	if(userDir == NULL) scanDirs();
	return dataDir;
}

char *ccGetTempDir()
{
	if(userDir == NULL) scanDirs();
	return tempDir;
}