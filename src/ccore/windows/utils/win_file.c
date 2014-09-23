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

char *ccFileUserDirGet(void)
{
	if(userDir == NULL) scanDirs();
	return userDir;
}

char *ccFileDataDirGet(void)
{
	if(userDir == NULL) scanDirs();
	return dataDir;
}

char *ccFileTempDirGet(void)
{
	if(userDir == NULL) scanDirs();
	return tempDir;
}

void _ccFileFree(void)
{
	if(userDir == NULL) return;
	free(dataDir);
	free(tempDir);
	userDir = NULL;
}