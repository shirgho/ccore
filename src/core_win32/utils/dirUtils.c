#include "dirUtils.h"

#define USERHOME "%HOMEPATH%/"

static char *userDir = NULL;
static char *dataDir;
static char *tempDir;

static void scanDirs()
{
	HMODULE hModule = GetModuleHandleW(NULL);
	char path[MAX_PATH];

	//Fetch absolue .exe path
	GetModuleFileName(hModule, path, MAX_PATH);
	
	dataDir = malloc(strlen(path));
	ccStrCopy(path, dataDir);
	ccStrTrimToChar(dataDir, '\\', true);
	ccStrReplaceChar(dataDir, '\\', '/');

	//User dir
	userDir = USERHOME;

	//Temp directory
	GetTempPath(MAX_PATH, path);

	tempDir = malloc(strlen(path));
	ccStrCopy(path, tempDir);
	ccStrReplaceChar(tempDir, '\\', '/');
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