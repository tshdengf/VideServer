#include "CommonUtils.h"
#include <sys/stat.h>  

//用于json和普通数据之间的转换
unsigned long CommonUtils::get_file_size(const char * path)
{
	unsigned long filesize = -1;
	struct stat statbuff;
	if (stat(path, &statbuff) < 0) {
		return filesize;
	}
	else {
		filesize = statbuff.st_size;
	}
	return filesize;
}
