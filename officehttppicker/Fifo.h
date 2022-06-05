#pragma once
#include "DataContainer.h"
class Fifo
{
private:
	Fifo *pNext;
	Fifo *pPrev;
	DataContainer *pData;

public:
	Fifo();
	~Fifo();
	int Append(Fifo *pAppendedFifo);
	int SetData(DataContainer *pSetData);
	DataContainer *GetData();

	Fifo *GetNext() { return pNext; };
	Fifo *GetPrev() { return pPrev; };
};

