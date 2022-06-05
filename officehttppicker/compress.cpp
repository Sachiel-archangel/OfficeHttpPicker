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

	// �󂯎��I�u�W�F�N�g��ݒ�
	pDecompressedData = new DataContainer;
	if (pDecompressedData == NULL)
		return NULL;

	for (;;) {
		// �u���[�N����
		if (dwOffset + sizeof(ZipHeader) >= (DWORD)pData->GetCurrentDataSize()) {
			break;
		}

		// �̈�̃N���A
		objZipHeader.Clear();
		memset(&objStream, 0x00, sizeof(z_stream));

		// ZIP�w�b�_���̓W�J
		if (objZipHeader.ExpandZipHeader((char *)pData->GetDataPointer() + dwOffset) < 0){
			break;
		}

		// �I�t�Z�b�g�v�Z
		// �ȉ��̒������I�t�Z�b�g����B
		// �w�b�_�Œ�T�C�Y + �t�@�C�����̒��� + �g���t�B�[���h�̒���
		dwOffset += objZipHeader.GetHeaderSize();

		// �𓀑Ώۂ̏ꍇ�A�𓀏������s���B
		if (objZipHeader.GetInflateFlg() && objZipHeader.GetDecompSize() > 0) {

			// �𓀃f�[�^��̗̈�m��
			if (objReceive.CreateDataObject(objZipHeader.GetDecompSize()) != DATACONT_SUCCESS) {
				break;
			}

			// objStream�ւ̃p�����[�^�ݒ�
			objStream.next_in = (Bytef *)((char *)pData->GetDataPointer() + dwOffset);
			objStream.avail_in = objZipHeader.GetCompSize();
			objStream.next_out = (Bytef *)objReceive.GetDataPointer();
			objStream.avail_out = objZipHeader.GetDecompSize();

			// �𓀏���(zlib�̊֐��𗘗p)
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

			// �𓀌��ʂ��i�[����B
			if (pDecompressedData->ReallocDataObject(iTotalSize) != DATACONT_SUCCESS) {
				delete pDecompressedData;
				pDecompressedData = NULL;
				break;
			}
			memcpy((char *)pDecompressedData->GetDataPointer() + ipDecompressedDataOffset, objReceive.GetDataPointer(), objZipHeader.GetDecompSize());
			ipDecompressedDataOffset += objZipHeader.GetDecompSize();

			// �I�t�Z�b�g�v�Z
			// �ȉ��̒������I�t�Z�b�g����B
			// (�w�b�_�Œ�T�C�Y + �t�@�C�����̒��� + �g���t�B�[���h�̒��� = ���Z�ς�)+ ���k�T�C�Y
			dwOffset += objZipHeader.GetCompSize();
		}
	}

	// ������NULL���������Ă����B
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

