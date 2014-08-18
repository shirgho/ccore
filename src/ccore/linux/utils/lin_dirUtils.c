#include "lin_dirUtils.h"

char *ccGetDirUserDir()
{
	return CC_USER_LOCATION;
}

char *ccGetDirDataDir()
{
#ifndef CC_DATA_LOCATION
	char *buf;

	buf = malloc(PATH_MAX);
	readlink("/proc/self/exe", buf, PATH_MAX);

	buf = dirname(buf);
	strcat(buf, "/");
	ccStrTrim(buf);

	return buf;
#else
	return CC_DATA_LOCATION;
#endif
}

char *ccGetDirTempDir()
{
	return CC_TEMP_LOCATION;
}
