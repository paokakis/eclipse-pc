#include "Resource.hpp"
#include <sys/stat.h>

Resource::Resource(const char* pPath) :
	mAsset()
{
	strcpy(mPath, pPath);
}

status Resource::open() 
{
	mAsset.open(mPath, std::ios::ate | std::ios::binary);

	return STATUS_OK;
}

void Resource::close() 
{
	if (mAsset.is_open()) 
	{
		mAsset.close();
	}
}

status Resource::read(void* pBuffer, size_t pCount) 
{
	mAsset.read((char*)pBuffer, pCount);

	return STATUS_OK;
}

off_t Resource::getLength()
{
	off_t fsize , tmpSz;

	tmpSz = mAsset.tellg();
	mAsset.seekg(0, std::ios::end);
	fsize = mAsset.tellg();
	fsize -= tmpSz;

	return fsize;
}

bool Resource::operator==(const Resource& pOther) 
{
	return !strcmp(mPath, pOther.mPath);
}