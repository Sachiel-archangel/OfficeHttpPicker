#include "ZipHeader.h"



ZipHeader::ZipHeader()
{
	memset(pSignature, 0x00, 4);
	wVer = 0;
	wFlag = 0;
	wMethod = 0;
	wUpdTime = 0;
	wUpdDate = 0;
	dwCRC32 = 0;
	dwCompSize = 0;
	dwDecompSize = 0;
	wFileNameLen = 0;
	wExtendFieldLen = 0;
	strFileName = NULL;
	pExtendField = NULL;

	wDevVer = 0;
	iHeaderSize = 0;
	wCommentLen = 0;
	wDevVer = 0;
	wCommentLen = 0;
	wDiskNum = 0;
	wInsideFileAttr = 0;
	dwOutsideFileAttr = 0;
	dwLclFileHdOffset = 0;
	strFileComment = NULL;

	iHeaderSize = 0;
	iInflateFlg = 0;
}


ZipHeader::~ZipHeader()
{
	Clear();
}

void ZipHeader::Clear()
{
	memset(pSignature, 0x00, 4);
	wVer = 0;
	wFlag = 0;
	wMethod = 0;
	wUpdTime = 0;
	wUpdDate = 0;
	dwCRC32 = 0;
	dwCompSize = 0;
	dwDecompSize = 0;
	wFileNameLen = 0;
	wExtendFieldLen = 0;
	if (strFileName != NULL) {
		delete strFileName;
		strFileName = NULL;
	}
	if (pExtendField != NULL) {
		delete pExtendField;
		pExtendField = NULL;
	}

	wDevVer = 0;
	iHeaderSize = 0;
	wCommentLen = 0;
	wDevVer = 0;
	wCommentLen = 0;
	wDiskNum = 0;
	wInsideFileAttr = 0;
	dwOutsideFileAttr = 0;
	dwLclFileHdOffset = 0;
	if (strFileComment != NULL) {
		delete strFileComment;
		strFileComment = NULL;
	}

	iHeaderSize = 0;
	iInflateFlg = 0;
}

int ZipHeader::ExpandZipHeader(void *pData)
{
	char buf1[] = { 0x50 , 0x4B ,0x03, 0x04 };
	char buf2[] = { 0x50 , 0x4B ,0x01, 0x02 };
	char buf3[] = { 0x50 , 0x4B ,0x05, 0x06 };

	// ポインタのデータチェック
	if (pData == NULL)
		return -1;

	memcpy(pSignature, pData, 4);

	// "PK+0x0304 の場合
	if (memcmp(buf1, pSignature, 4) == 0)
	{
		wVer = *((WORD *)((char *)pData + 4));
		wFlag = *((WORD *)((char *)pData + 6));
		wMethod = *((WORD *)((char *)pData + 8));
		wUpdTime = *((WORD *)((char *)pData + 10));
		wUpdDate = *((WORD *)((char *)pData + 12));
		dwCRC32 = *((DWORD *)((char *)pData + 14));
		dwCompSize = *((DWORD *)((char *)pData + 18));
		dwDecompSize = *((DWORD *)((char *)pData + 22));
		wFileNameLen = *((WORD *)((char *)pData + 26));
		wExtendFieldLen = *((WORD *)((char *)pData + 28));

		if (wFileNameLen > 0)
		{
			strFileName = new DataContainer;
			if (strFileName == NULL)
				return -1;
			if (strFileName->CreateDataObject((int)wFileNameLen + 1) != DATACONT_SUCCESS)
				return -1;

			strFileName->ClearDataObject();
			memcpy(strFileName->GetDataPointer(), ((char *)pData + 30), wFileNameLen);
		}

		if (wExtendFieldLen > 0)
		{
			pExtendField = new DataContainer;
			if (pExtendField == NULL)
				return -1;
			if (pExtendField->CreateDataObject((int)wExtendFieldLen) != DATACONT_SUCCESS)
				return -1;

			pExtendField->ClearDataObject();
			memcpy(pExtendField->GetDataPointer(), ((char *)pData + 30 + wFileNameLen), wExtendFieldLen);
		}

		iHeaderSize = 30 + wFileNameLen + wExtendFieldLen;
		iInflateFlg = 1;
	}
	else if (memcmp(buf2, pSignature, 4) == 0)
	{
		wDevVer = *((WORD *)((char *)pData + 4));
		wVer = *((WORD *)((char *)pData + 6));
		wFlag = *((WORD *)((char *)pData + 8));
		wMethod = *((WORD *)((char *)pData + 10));
		wUpdTime = *((WORD *)((char *)pData + 12));
		wUpdDate = *((WORD *)((char *)pData + 14));
		dwCRC32 = *((DWORD *)((char *)pData + 16));
		dwCompSize = *((DWORD *)((char *)pData + 20));
		dwDecompSize = *((DWORD *)((char *)pData + 24));
		wFileNameLen = *((WORD *)((char *)pData + 28));
		wExtendFieldLen = *((WORD *)((char *)pData + 30));
		wCommentLen = *((WORD *)((char *)pData + 32));
		wDiskNum = *((WORD *)((char *)pData + 34));
		wInsideFileAttr = *((WORD *)((char *)pData + 36));
		dwOutsideFileAttr = *((DWORD *)((char *)pData + 38));
		dwLclFileHdOffset = *((DWORD *)((char *)pData + 42));

		if (wFileNameLen > 0)
		{
			strFileName = new DataContainer;
			if (strFileName == NULL)
				return -1;
			if (strFileName->CreateDataObject((int)wFileNameLen + 1) != DATACONT_SUCCESS)
				return -1;

			strFileName->ClearDataObject();
			memcpy(strFileName->GetDataPointer(), ((char *)pData + 46), wFileNameLen);
		}

		if (wExtendFieldLen > 0)
		{
			pExtendField = new DataContainer;
			if (pExtendField == NULL)
				return -1;
			if (pExtendField->CreateDataObject((int)wExtendFieldLen) != DATACONT_SUCCESS)
				return -1;

			pExtendField->ClearDataObject();
			memcpy(pExtendField->GetDataPointer(), ((char *)pData + 46 + wFileNameLen), wExtendFieldLen);
		}

		if (wCommentLen > 0)
		{
			strFileComment = new DataContainer;
			if (strFileComment == NULL)
				return -1;
			if (strFileComment->CreateDataObject((int)wCommentLen + 1) != DATACONT_SUCCESS)
				return -1;

			strFileName->ClearDataObject();
			memcpy(strFileName->GetDataPointer(), ((char *)pData + 46 + wFileNameLen + wExtendFieldLen), wCommentLen);
		}

		iHeaderSize = 46 + wFileNameLen + wExtendFieldLen + wCommentLen;
	}
	else if (memcmp(buf3, pSignature, 4) == 0)
	{
		// 今回は使わないので、サイズ分移動させる
		iHeaderSize = 22 + *((WORD *)((char *)pData + 20));
	}
	else 
	{
		return -1;
	}

	return 0;
}
