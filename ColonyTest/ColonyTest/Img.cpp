#include "Img.h"


//======================================================================================
//
//
//
//======================================================================================
const char	*CImg::GetImageInfo()
{
	static char info[200];
	sprintf(info, "no info");

	if (GetImage() != NULL) {
		sprintf(info, "(%dx%d), %s", Width(), Height(), Channel() == 1 ? "Grayscale Image" : "Color Image");
	}

	return info;
}

const char	*CImg::GetShortName()
{
	int len = strlen(m_name);
	const char *ptr = m_name + len - 1;

	for (int i = len - 1; i >= 0; i--, ptr--) {
		if (*ptr == '\\'
			|| *ptr == '/') break;
	}
	ptr++;

	return ptr;
}

const char*	cvGetDirName(const char *path)
{
	static char fullpath[500];
	strcpy(fullpath, path);

	int len = strlen(fullpath);
	char *ptr = fullpath + len - 1;

	for (int i = len - 1; i >= 0; i--, ptr--) {
		if (*ptr == '\\'
			|| *ptr == '/') break;
	}
	*ptr = '\0';
	return fullpath;
}
const char*	cvGetFileName(const char *path)
{
	int len = strlen(path);
	const char *ptr = path + len - 1;

	for (int i = len - 1; i >= 0; i--, ptr--) {
		if (*ptr == '\\'
			|| *ptr == '/') break;
	}
	ptr++;

	return ptr;
}
