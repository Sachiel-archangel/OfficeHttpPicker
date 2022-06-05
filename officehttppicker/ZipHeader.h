#pragma once
#include "DataContainer.h"

class ZipHeader
{
private:
	char pSignature[4];
	WORD wVer;
	WORD wFlag;
	WORD wMethod;
	WORD wUpdTime;
	WORD wUpdDate;
	DWORD dwCRC32;
	DWORD dwCompSize;
	DWORD dwDecompSize;
	WORD wFileNameLen;
	WORD wExtendFieldLen;
	DataContainer *strFileName;
	DataContainer *pExtendField;

	WORD wDevVer;
	WORD wCommentLen;
	WORD wDiskNum;
	WORD wInsideFileAttr;
	DWORD dwOutsideFileAttr;
	DWORD dwLclFileHdOffset;
	DataContainer *strFileComment;

	int iHeaderSize;
	int iInflateFlg;

public:
	ZipHeader();
	~ZipHeader();
	void Clear();
	int ExpandZipHeader(void *pData);

	char *GetSignature() { return pSignature; };
	WORD GetVer() { return wVer; };
	WORD GetFlag() { return wFlag; };
	WORD GetMethod() { return wMethod; };
	WORD GetUpdTime() { return wUpdTime; };
	WORD GetUpdDate() { return wUpdDate; };
	DWORD GetCRC32() { return dwCRC32; };
	DWORD GetCompSize() { return dwCompSize; };
	DWORD GetDecompSize() { return dwDecompSize; };
	WORD GetFileNameLen() { return wFileNameLen; };
	WORD GetExtendFieldLen() { return wExtendFieldLen; };
	DataContainer *GetFileName() { return strFileName; };
	DataContainer *GetExtendField() { return pExtendField; };

	WORD GetDevVer() { return wDevVer; };
	WORD GetCommentLen() { return wCommentLen; };

	int GetHeaderSize() { return iHeaderSize; };
	int GetInflateFlg() { return iInflateFlg; };
};

