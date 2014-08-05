#include <ccore/dirUtils.h>

char *ccGetUserDir()
{
	return CC_USER_LOCATION;
}

char *ccGetDataDir()
{
#ifndef CC_DATA_LOCATION
	char *buf;

	buf = malloc(4096);
	readlink("/proc/self/exe", buf, 4096);

	buf = dirname(buf);
	strcat(buf, "/");
	ccStrTrim(buf);

	return buf;
#else
	return CC_DATA_LOCATION;
#endif
}

char *ccGetTempDir()
{
	return CC_TEMP_LOCATION;
}
