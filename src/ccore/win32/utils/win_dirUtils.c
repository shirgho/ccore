#include "win_dirUtils.h"

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
	
	dataDir = malloc(strlen(path));
	strcpy(dataDir, path);
	ccStrTrimToChar(dataDir, '\\', true);
	ccStrReplaceChar(dataDir, '\\', '/');

	//User dir
	userDir = USERHOME;

	//Temp directory
	GetTempPath(MAX_PATH, path);

	tempDir = malloc(strlen(path));
	strcpy(tempDir, path);
	ccStrReplaceChar(tempDir, '\\', '/');
}

char *ccGetDirUserDir(void)
{
	if(userDir == NULL) scanDirs();
	return userDir;
}

char *ccGetDirDataDir(void)
{
	if(userDir == NULL) scanDirs();
	return dataDir;
}

char *ccGetDirTempDir(void)
{
	if(userDir == NULL) scanDirs();
	return tempDir;
}