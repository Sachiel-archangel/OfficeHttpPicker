#pragma once
#include "DataContainer.h"

class compress
{
public:
	compress();
	~compress();
	DataContainer * DecompressZip(DataContainer *pData);
};

