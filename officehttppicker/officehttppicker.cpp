// officehttppicker.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include "pch.h"
#include <iostream>
#include <Shlwapi.h>
#include "DataContainer.h"
#include "compress.h"
#include "Fifo.h"

void DestroyFifo(Fifo *pFifo)
{
	Fifo *pCurrent;
	Fifo *pNext;

	pCurrent = pFifo;
	for (;;) {
		if (pCurrent == NULL)
			break;

		pNext = pCurrent->GetNext();
		delete pCurrent->GetData();
		delete pCurrent;
		pCurrent = pNext;
	}

	return;
}


Fifo* PickupHttp(DataContainer *pData)
{
	Fifo* objStart = NULL;
	Fifo* objCurrent = NULL;
	char* pDataPos = NULL;
	char* pHttpPos = NULL;
	int iLenCount = 0;

	Fifo *pWorkFifo = NULL;
	DataContainer *pWorkDataCont = NULL;

	if (pData == NULL)
		return NULL;

	pDataPos = (char *)pData->GetDataPointer();
	if(pDataPos == NULL)
		return NULL;

	try {
		for (;;) {

			// httpの文字列を探す
			// 暫定
			// 後で、バッファサイズをパラメータとしてその末尾まで検索するような処理を作り直すことを考慮。
			// (不測のNULL文字対策)
			pHttpPos = StrStrIA(pDataPos, "http");

			if (pHttpPos == NULL)
				break;

			iLenCount = 0;
			for (;;) {
				if (pHttpPos[iLenCount] == '\"' || pHttpPos[iLenCount] == '>' || pHttpPos[iLenCount] == ' ')
					break;
				iLenCount++;
			}

			// 見つかった文字列を格納
			pWorkDataCont = new DataContainer();
			if (pWorkDataCont == NULL) {
				throw;
			}

			if (pWorkDataCont->CreateDataObject(iLenCount + 1) != DATACONT_SUCCESS) {
				delete pWorkDataCont;
				throw;
			}

			memcpy(pWorkDataCont->GetDataPointer(), pHttpPos, iLenCount);
			pWorkDataCont->SetCurrentDataSize(iLenCount);

			// Fifoキューオブジェクトを作成して設定。
			pWorkFifo = new Fifo;
			if (pWorkFifo == NULL) {
				throw;
			}

			pWorkFifo->SetData(pWorkDataCont);


			// Fifoキューの結合処理
			if (objStart == NULL) {
				// 初回処理
				objStart = pWorkFifo;
				objCurrent = pWorkFifo;
			}
			else {
				objCurrent->Append(pWorkFifo);
				objCurrent = pWorkFifo;
			}

			// 次の位置へ
			pDataPos = pHttpPos + iLenCount;

		}
	}
	catch (...) {
		printf("Unexpected error at PickupHttp\r\n");
		DestroyFifo(objStart);
		return NULL;
	}

	return objStart;
}

Fifo *OpenWhitelist(LPCWSTR strWhitelistName)
{
	Fifo *pWhitelist = NULL;
	Fifo *pWhitelistCurrent = NULL;
	HANDLE hWhitelistFile = NULL;
	DWORD dwWhitelistFileSize = 0;
	DataContainer *WhitelistFileData = NULL;
	DWORD dwReadSize = 0;
	char *pData = NULL;
	int i, iTop;

	DataContainer *pWorkContainer = NULL;
	Fifo *pWorkFifo = NULL;

	try {
		if (strWhitelistName == NULL)
			return NULL;

		// ファイルの存在チェック
		if (!PathFileExists(strWhitelistName)) {
			printf("Whitelist not found.\r\nWhitelist is not applied.\r\n");
			return NULL;
		}

		// ファイルの読み込み
		hWhitelistFile = CreateFile(strWhitelistName, GENERIC_READ, 0x0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hWhitelistFile == INVALID_HANDLE_VALUE) {
			printf("Whitelist open error.\r\nPlease check whitelist file path or permission.\r\n");
			return NULL;
		}

		dwWhitelistFileSize = GetFileSize(hWhitelistFile, NULL);

		WhitelistFileData = new DataContainer;
		WhitelistFileData->CreateDataObject((int)dwWhitelistFileSize);

		if (ReadFile(hWhitelistFile, WhitelistFileData->GetDataPointer(), dwWhitelistFileSize, &dwReadSize, NULL) == 0) {
			printf("Read whitefile error.\r\n");
			return NULL;
		}
		WhitelistFileData->SetCurrentDataSize(dwWhitelistFileSize);
		pData = (char *)WhitelistFileData->GetDataPointer();

		iTop = 0;
		for (i = 0; i < (int)dwWhitelistFileSize; i++) {
			if (pData[i] == '\r' || pData[i] == '\n') {

				// '\r' や '\n' だけにならないようにチェック
				if (i - iTop > 1) {
					pWorkContainer = new DataContainer;
					if (pWorkContainer == NULL)
						throw;

					if (pWorkContainer->CreateDataObject(i - iTop + 1) != DATACONT_SUCCESS) {
						delete pWorkContainer;
						throw;
					}

					memcpy(pWorkContainer->GetDataPointer(), &pData[iTop], i - iTop);
					pWorkContainer->SetCurrentDataSize(i - iTop);

					pWorkFifo = new Fifo;
					if (pWorkFifo == NULL)
						throw;
					pWorkFifo->SetData(pWorkContainer);

					if (pWhitelist == NULL) {
						pWhitelist = pWorkFifo;
						pWhitelistCurrent = pWorkFifo;
					}
					else {
						pWhitelistCurrent->Append(pWorkFifo);
						pWhitelistCurrent = pWorkFifo;
					}
				}

				iTop = i + 1;
			}
		}

		// 最終レコードチェック
		if (i - iTop > 1) {
			pWorkContainer = new DataContainer;
			if (pWorkContainer == NULL)
				throw;
			if (pWorkContainer->CreateDataObject(i - iTop + 1) != DATACONT_SUCCESS) {
				delete pWorkContainer;
				throw;
			}
			memcpy(pWorkContainer->GetDataPointer(), &pData[iTop], i - iTop);
			pWorkContainer->SetCurrentDataSize(i - iTop);

			pWorkFifo = new Fifo;
			if (pWorkFifo == NULL)
				throw;
			pWorkFifo->SetData(pWorkContainer);

			if (pWhitelist == NULL) {
				pWhitelist = pWorkFifo;
				pWhitelistCurrent = pWorkFifo;
			}
			else {
				pWhitelistCurrent->Append(pWorkFifo);
				pWhitelistCurrent = pWorkFifo;
			}
		}
	}
	catch (...) {
		DestroyFifo(pWhitelist);
		return NULL;
	}

	return pWhitelist;
}

Fifo *CleansingByWhiteList(Fifo *pDataList, LPCWSTR strWhitelistName)
{
	Fifo *pOutputList = NULL;
	Fifo *pOutCurrent = NULL;
	Fifo *pCurrent = pDataList;
	DataContainer *pWorkContainer = NULL;
	DataContainer *pWorkContainer2 = NULL;
	Fifo *pWorkFifo = NULL;

	Fifo *pWhiteList = NULL;
	Fifo *pCurrentWhiteList = NULL;

	if (strWhitelistName == NULL) {
		printf("Unexpected error.Whitelist file name is NULL\r\n");
		return NULL;
	}

	if (pDataList == NULL)
		return NULL;

	// ホワイトリストの読み込み
	// ホワイトリストのオブジェクトがNULLの場合、「ホワイトリストなし」として処理する。
	pWhiteList = OpenWhitelist(strWhitelistName);

	try {
		for (;;)
		{
			if (pCurrent == NULL)
				break;

			pWorkContainer = pCurrent->GetData();

			pCurrentWhiteList = pWhiteList;

			for (;;)
			{
				// ホワイトリストに合致しなかった場合
				if (pCurrentWhiteList == NULL) {
					pWorkContainer2 = new DataContainer;
					if (pWorkContainer2 == NULL)
						throw;
					if (pWorkContainer2->CreateDataObject(pWorkContainer->GetCurrentDataSize() + 1) != DATACONT_SUCCESS) {
						delete pWorkContainer2;
						throw;
					}
					memcpy(pWorkContainer2->GetDataPointer(), pWorkContainer->GetDataPointer(), pWorkContainer->GetCurrentDataSize());
					pWorkContainer2->SetCurrentDataSize(pWorkContainer->GetCurrentDataSize());

					pWorkFifo = new Fifo;
					if (pWorkFifo == NULL)
						throw;
					pWorkFifo->SetData(pWorkContainer2);

					if (pOutputList == NULL) {
						pOutputList = pWorkFifo;
						pOutCurrent = pWorkFifo;
					}
					else {
						pOutCurrent->Append(pWorkFifo);
						pOutCurrent = pWorkFifo;
					}
					break;
				}

				// ホワイトリストと付き合わせ
				if (strstr((char *)pWorkContainer->GetDataPointer(), (char *)pCurrentWhiteList->GetData()->GetDataPointer()) != NULL)
					break;

				pCurrentWhiteList = pCurrentWhiteList->GetNext();
			}

			pCurrent = pCurrent->GetNext();
		}

		// ホワイトリストの開放
		DestroyFifo(pWhiteList);
	}
	catch (...) {
		printf("Unexpected error.Check will be uncompleted.\r\n");
		DestroyFifo(pWhiteList);
		DestroyFifo(pOutputList);
		return NULL;
	}

	// 結果のリターン
	return pOutputList;
}

LPCWSTR CreateOutputFileName(LPCWSTR strFileName)
{
	// 暫定：入力ファイル名を加工して出力する。
	// それよりも、Unixタイム等でファイル名を作ったほうがいいかもしれない？
	LPCWSTR strOutputFileName = NULL;
	LPCWSTR pExtPos = NULL;
	
	int iFilenameLen = 0;
	int iOutBufferlen = 0;

	if (strFileName == NULL) {
		strOutputFileName = new TCHAR[15 * 2];

		if (strOutputFileName == NULL)
			return NULL;

		memset((void *)strOutputFileName, 0x00, iOutBufferlen);
		memcpy((void *)strOutputFileName, L"suspicious.txt", 14 * 2);
		return strOutputFileName;
	}

	iFilenameLen = wcslen(strFileName);

	iOutBufferlen = (iFilenameLen + 15) * 2;
	strOutputFileName = new TCHAR[iOutBufferlen];

	if (strOutputFileName == NULL)
		return NULL;

	memset((void *)strOutputFileName, 0x00, iOutBufferlen);
	memcpy((void *)strOutputFileName, strFileName, iFilenameLen * 2);

	pExtPos = PathFindExtension(strFileName);
	if (pExtPos != NULL) {
		memset((void *)pExtPos, 0x00, 2);
	}
	wcscat_s((LPWSTR)strOutputFileName, iOutBufferlen, L".suspicious.txt");

	return strOutputFileName;
}

int OutputData(Fifo *pOutputFifo, LPCWSTR strOutFileName)
{
	HANDLE hOutFile = 0;
	Fifo *pCurrent = NULL;
	DWORD dwWittenSize = 0;

	if (pOutputFifo == NULL)
		return -1;

	// ファイルの読み込み
	hOutFile = CreateFile(strOutFileName, GENERIC_WRITE, 0x0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOutFile == INVALID_HANDLE_VALUE)
		return -1;

	pCurrent = pOutputFifo;
	for (;;)
	{
		WriteFile(hOutFile, pCurrent->GetData()->GetDataPointer(), pCurrent->GetData()->GetCurrentDataSize(), &dwWittenSize, NULL);
		WriteFile(hOutFile, "\r\n", 2, &dwWittenSize, NULL);

		pCurrent = pCurrent->GetNext();
		if (pCurrent == NULL)
			break;
	}

	// 後処理
	CloseHandle(hOutFile);

	return 0;
}


int wmain(int argc, wchar_t *argv[])
{
	LPCWSTR strFileName = NULL;
	LPCWSTR strWhitelistName = NULL;
	LPCWSTR strOutFileName = NULL;
	int iWhitelistNameLen = 0;
	HANDLE hOfficeFile = NULL;
	DWORD dwOfficeFileSize = 0;
	DWORD dwReadSize = 0;
	DataContainer *OfficeFileData = NULL;
	DataContainer *DecompressedData = NULL;
	Fifo* pHttpDataFifo = NULL;
	Fifo* pOutputFifo = NULL;

	try {
		// 引数の数をチェック
		if (argc <= 1) {
			printf("Copylight(c) 2022 Sachiel\r\n");
			printf("Released under the MIT license.\r\n");
			printf("https://opensource.org/licenses/mit-license.php\r\n\r\n");
			printf("Need argment(s).\r\nSee help or document.\r\n");
			return 0;
		}

		// 引数の分解

		// [To do]
		// 暫定で、第1引数はファイル名で固定
		strFileName = (LPCWSTR)argv[1];

		if (argc >= 3) {
			iWhitelistNameLen = wcslen(argv[2]);
			strWhitelistName = new TCHAR[(iWhitelistNameLen + 1) * 2];
			memset((void *)strWhitelistName, 0x00, (iWhitelistNameLen + 1) * 2);
			memcpy((void *)strWhitelistName, argv[2], iWhitelistNameLen * 2);
		}
		else {
			iWhitelistNameLen = wcslen(L"whitelist.txt");
			strWhitelistName = new TCHAR[(iWhitelistNameLen + 1) * 2];
			memset((void *)strWhitelistName, 0x00, (iWhitelistNameLen + 1) * 2);
			memcpy((void *)strWhitelistName, L"whitelist.txt", iWhitelistNameLen * 2);
		}


		// チェック対象ファイルの存在チェック
		if (!PathFileExists(strFileName)) {
			printf("File not found.\r\nPlease check file path or permission.\r\n");
			return 0;
		}

		// チェック対象ファイルの読み込み
		hOfficeFile = CreateFile(strFileName, GENERIC_READ, 0x0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hOfficeFile == INVALID_HANDLE_VALUE) {
			printf("File open error.\r\nPlease check file path or permission.\r\n");
			return 0;
		}

		dwOfficeFileSize = GetFileSize(hOfficeFile, NULL);

		OfficeFileData = new DataContainer;
		OfficeFileData->CreateDataObject((int)dwOfficeFileSize);

		if (ReadFile(hOfficeFile, OfficeFileData->GetDataPointer(), dwOfficeFileSize, &dwReadSize, NULL) == 0) {
			printf("Read file error.\r\n");
			return 0;
		}
		OfficeFileData->SetCurrentDataSize(dwOfficeFileSize);

		// データの解凍
		// Officeファイルはzip圧縮されているため、これを解凍する。

		compress objCompress;

		DecompressedData = objCompress.DecompressZip(OfficeFileData);
		if (DecompressedData == NULL) {
			printf("Decompress error.\r\n");
			return 0;
		}

		// 「http:」の文字列を抽出
		pHttpDataFifo = PickupHttp(DecompressedData);

		if (pHttpDataFifo != NULL) {
			// ホワイトリスト対象を削除
			pOutputFifo = CleansingByWhiteList(pHttpDataFifo, strWhitelistName);

			if (pOutputFifo != NULL) {
				// 出力ファイル名の生成
				strOutFileName = CreateOutputFileName(strFileName);
				if (strOutFileName == NULL) {
					printf("Output file name creation error. \r\n");
				}

				// 残ったレコードをファイル出力
				if (OutputData(pOutputFifo, strOutFileName) < 0) {
					printf("Suspicious record(s) was found, but there was an error in the file output. \r\n");
				}

				// キューデータの削除
				DestroyFifo(pOutputFifo);
				pOutputFifo = NULL;
			}

			// キューデータの削除
			DestroyFifo(pHttpDataFifo);
			pHttpDataFifo = NULL;
		}

		// 後処理
		CloseHandle(hOfficeFile);
		if (OfficeFileData != NULL) {
			delete OfficeFileData;
			OfficeFileData = NULL;
		}
		if (DecompressedData != NULL) {
			delete DecompressedData;
			DecompressedData = NULL;
		}

		if (strWhitelistName != NULL) {
			delete strWhitelistName;
			strWhitelistName = NULL;
		}

		if (strOutFileName != NULL) {
			delete strOutFileName;
			strOutFileName = NULL;
		}
	}
	catch(...){
		if(hOfficeFile == NULL)
			CloseHandle(hOfficeFile);

		if (OfficeFileData != NULL) {
			delete OfficeFileData;
			OfficeFileData = NULL;
		}
		if (DecompressedData != NULL) {
			delete DecompressedData;
			DecompressedData = NULL;
		}

		if (strWhitelistName != NULL) {
			delete strWhitelistName;
			strWhitelistName = NULL;
		}

		if (strOutFileName != NULL) {
			delete strOutFileName;
			strOutFileName = NULL;
		}
		DestroyFifo(pOutputFifo);
		DestroyFifo(pHttpDataFifo);
	}

	return 0;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
