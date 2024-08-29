#pragma once
#include <iostream>
#include <cstdint>
#include <vector>

class IndexBufferBufFile {
public:
	std::wstring Index;
	std::wstring MatchFirstIndex;
	uint32_t ReadDrawNumber = 0;

	//��С�Ķ�����ֵ
	uint32_t MinNumber = 0;

	//���Ķ�����ֵ
	uint32_t MaxNumber = 0;

	//�ܹ��м�������
	uint32_t NumberCount = 0;

	//�ܹ��õ��Ķ�����������
	uint32_t UniqueVertexCount = 0;

	//�����б�
	std::vector<uint32_t> NumberList;

	IndexBufferBufFile();

	//IB�ļ�����·���������ִ�Сд�ĸ�ʽ����DXGI_FORMAT_R32_UINT
	IndexBufferBufFile(std::wstring FileReadPath,std::wstring Format);

	void SelfDivide(int FirstIndex,int IndexCount);

	void SaveToFile_UINT32(std::wstring FileWritePath, uint32_t Offset);
	void SaveToFile_UINT16(std::wstring FileWritePath, uint16_t Offset);

	void SaveToFile_MinSize(std::wstring FileWritePath, uint32_t Offset);
	std::string GetSelfMinFormat();
};