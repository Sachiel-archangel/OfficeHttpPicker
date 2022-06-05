#include "Fifo.h"



Fifo::Fifo()
{
	pNext = NULL;
	pPrev = NULL;
	pData = NULL;
}


Fifo::~Fifo()
{
	pNext = NULL;
	pPrev = NULL;
	pData = NULL;
}

int Fifo::Append(Fifo *pAppendedFifo)
{
	if (pAppendedFifo->pNext != NULL) {
		(pAppendedFifo->pNext)->Append(pAppendedFifo);
		return 0;
	}

	this->pNext = pAppendedFifo;
	pAppendedFifo->pPrev = this;

	return 0;
}

int Fifo::SetData(DataContainer *pSetData)
{
	pData = pSetData;

	return 0;
}

DataContainer * Fifo::GetData()
{
	return pData;
}


