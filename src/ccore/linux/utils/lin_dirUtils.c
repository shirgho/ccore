#include "lin_dirUtils.h"

char *ccGetDirUserDir(void)
{
	return CC_USER_LOCATION;
}

char *ccGetDirDataDir(void)
{
#ifndef CC_DATA_LOCATION
	char *buf;

	buf = malloc(PATH_MAX);
	readlink("/proc/self/exe", buf, PATH_MAX);

	buf = dirname(buf);
	strcat(buf, "/");

	/* TODO discuss if the last / needs te be removed
	if(buf[strlen(buf) - 1] == '/'){
		buf[strlen(buf) - 1] = '\0';
	}
	*/

	return buf;
#else
	return CC_DATA_LOCATION;
#endif
}

char *ccGetDirTempDir(void)
{
	return CC_TEMP_LOCATION;
}
