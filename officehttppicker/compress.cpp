#include "pch.h"
#include "compress.h"
#include "zlib.h"
#include "ZipHeader.h"

compress::compress()
{
}


compress::~compress()
{
}

DataContainer *  compress::DecompressZip(DataContainer *pData)
{
	int iRet = 0;
	int iTotalSize = 0;
	z_stream objStream;
	ZipHeader objZipHeader;
	DataContainer *pDecompressedData = NULL;
	int ipDecompressedDataOffset = 0;
	DataContainer objReceive;
	DWORD dwOffset = 0;


	if (pData == NULL)
		return NULL;

	// 受け取りオブジェクトを設定
	pDecompressedData = new DataContainer;
	if (pDecompressedData == NULL)
		return NULL;

	for (;;) {
		// ブレーク判定
		if (dwOffset + sizeof(ZipHeader) >= (DWORD)pData->GetCurrentDataSize()) {
			break;
		}

		// 領域のクリア
		objZipHeader.Clear();
		memset(&objStream, 0x00, sizeof(z_stream));

		// ZIPヘッダ情報の展開
		if (objZipHeader.ExpandZipHeader((char *)pData->GetDataPointer() + dwOffset) < 0){
			break;
		}

		// オフセット計算
		// 以下の長さをオフセットする。
		// ヘッダ固定サイズ + ファイル名の長さ + 拡張フィールドの長さ
		dwOffset += objZipHeader.GetHeaderSize();

		// 解凍対象の場合、解凍処理を行う。
		if (objZipHeader.GetInflateFlg() && objZipHeader.GetDecompSize() > 0) {

			// 解凍データ受領領域確保
			if (objReceive.CreateDataObject(objZipHeader.GetDecompSize()) != DATACONT_SUCCESS) {
				break;
			}

			// objStreamへのパラメータ設定
			objStream.next_in = (Bytef *)((char *)pData->GetDataPointer() + dwOffset);
			objStream.avail_in = objZipHeader.GetCompSize();
			objStream.next_out = (Bytef *)objReceive.GetDataPointer();
			objStream.avail_out = objZipHeader.GetDecompSize();

			// 解凍処理(zlibの関数を利用)
			iRet = inflateInit2(&objStream, -MAX_WBITS);
			if (iRet != Z_OK) {
				delete pDecompressedData;
				return NULL;
			}

			iRet = inflate(&objStream, Z_SYNC_FLUSH);
			if ((iRet >= 0) && (objStream.msg != NULL)) {
				delete pDecompressedData;
				return NULL;
			}

			iRet = inflateEnd(&objStream);
			if (iRet != Z_OK) {
				delete pDecompressedData;
				return NULL;
			}

			iTotalSize += objZipHeader.GetDecompSize();

			// 解凍結果を格納する。
			if (pDecompressedData->ReallocDataObject(iTotalSize) != DATACONT_SUCCESS) {
				delete pDecompressedData;
				pDecompressedData = NULL;
				break;
			}
			memcpy((char *)pDecompressedData->GetDataPointer() + ipDecompressedDataOffset, objReceive.GetDataPointer(), objZipHeader.GetDecompSize());
			ipDecompressedDataOffset += objZipHeader.GetDecompSize();

			// オフセット計算
			// 以下の長さをオフセットする。
			// (ヘッダ固定サイズ + ファイル名の長さ + 拡張フィールドの長さ = 加算済み)+ 圧縮サイズ
			dwOffset += objZipHeader.GetCompSize();
		}
	}

	// 末尾にNULL文字を入れておく。
	if(pDecompressedData->ReallocDataObject(iTotalSize + 1) != DATACONT_SUCCESS) {
		delete pDecompressedData;
		pDecompressedData = NULL;
	}
	else {
		char *tmp = (char *)pDecompressedData->GetDataPointer();
		tmp[iTotalSize] = '\0';

		pDecompressedData->SetCurrentDataSize(iTotalSize + 1);
	}

	return pDecompressedData;
}

