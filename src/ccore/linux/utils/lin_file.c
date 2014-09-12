#include "lin_file.h"

static char *dataDir = NULL;

char *ccFileGetUserDir(void)
{
	return CC_USER_LOCATION;
}

char *ccFileGetDataDir(void)
{
#ifndef CC_DATA_LOCATION
	int len;

	if(dataDir == NULL){
		dataDir = malloc(PATH_MAX);
		len = readlink("/proc/self/exe", dataDir, PATH_MAX);
		if(len > 0){
			dataDir[len] = '\0';
			dataDir = dirname(dataDir);
			strcat(dataDir, "/");
		}
	}
	return dataDir;
#else
	return CC_DATA_LOCATION;
#endif
}

char *ccFileGetTempDir(void)
{
	return CC_TEMP_LOCATION;
}

ccReturn ccFileGetSize(char *file, unsigned long *size)
{
	struct stat sb;

	if(stat(file, &sb) != 0){
		ccErrorPush(CC_ERROR_FILE_OPEN);
		return CC_FAIL;
	}
	
	*size = sb.st_size;

	return CC_SUCCESS;
}

void _ccFileFree(void)
{

}
