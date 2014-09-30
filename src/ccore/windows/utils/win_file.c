#include "win_file.h"

#define USERHOME "%HOMEPATH%/"

static char *userDir = NULL;
static char *dataDir;
static char *tempDir;

static void scanDirs(void)
{
	HMODULE hModule = GetModuleHandleW(NULL);
	char path[MAX_PATH];
	int pathlength;

	//Fetch absolue .exe path
	pathlength = GetModuleFileName(hModule, path, MAX_PATH);
	
	dataDir = calloc(pathlength + 1, sizeof(char));
	strcpy_s(dataDir, pathlength, path);
	ccStringTrimToChar(dataDir, '\\', true);
	ccStringReplaceChar(dataDir, '\\', '/');

	//User dir
	userDir = USERHOME;

	//Temp directory
	pathlength = GetTempPath(MAX_PATH, path);

	tempDir = calloc(pathlength + 1, sizeof(char));
	strcpy_s(tempDir, pathlength, path);
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

void _ccFileFree(void)
{
	if(userDir == NULL) return;
	free(dataDir);
	free(tempDir);
	userDir = NULL;
}