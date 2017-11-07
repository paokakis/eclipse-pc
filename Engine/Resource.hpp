#ifndef RESOURCE_HPP
#define RESOURCE_HPP
#include <iostream>
#include <fstream>
#include "Tools/Types.hpp"

struct ResourceDescriptor {
	int32_t mDescriptor;
	off_t mStart;
	off_t mLength;
};

class Resource {
public:
	explicit Resource(const char* pPath);
	const char* getPath() const { return mPath; }

	status open();
	void close();
	status read(void* pBuffer, size_t pCount);
	
	off_t getLength();

	bool operator==(const Resource& pOther);
private:
	char mPath[1000];
	std::fstream mAsset;
};

#endif