#include "lin_dirUtils.h"

char *ccGetDirUserDir(void)
{
	return CC_USER_LOCATION;
}

char *ccGetDirDataDir(void)
{
#ifndef CC_DATA_LOCATION
	char *buf;
	int len;

	buf = malloc(PATH_MAX);
	len = readlink("/proc/self/exe", buf, PATH_MAX);
	if(len > 0){
		buf[len] = '\0';
		buf = dirname(buf);
		strcat(buf, "/");
	}
	return buf;
#else
	return CC_DATA_LOCATION;
#endif
}

char *ccGetDirTempDir(void)
{
	return CC_TEMP_LOCATION;
}
