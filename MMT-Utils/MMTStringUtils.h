#pragma once
#include <iostream>
#include <Windows.h>
#include <vector>

//�ַ�������boost����
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>

namespace MMTString {
	//�ַ���ת��
	std::wstring ToWideString(std::string input);
	std::string	ToByteString(std::wstring input);
	std::wstring ToLowerCase(std::wstring str);
	std::string ToLowerCase(std::string str);
	std::wstring ToUpperCase(std::wstring str);
	std::string ToUpperCase(std::string str);

	//��ȡ��ǰ����·��
	std::wstring GetCurrentWorkingDirectoryPath();

	//���ݵ�ǰĿ¼·������ȡ����Ŀ¼·��
	std::wstring GetParentFolderPathFromFolderPath(std::wstring FolderPath);

	// �����ļ�����·����ȡ�ļ�����Ŀ¼·��,��β�����Զ�����/
	std::wstring GetFolderPathFromFilePath(std::wstring filePath);



	//����UUID
	std::wstring GenerateUUIDW();

	//���������ַ���
	std::wstring GetFormattedDateTimeForFilename();

	//��C++ʵ�ַָ��ַ�������ΪBoost���޷�������L"=="��Ϊ�ָ���
	//ԭʼ�ַ��������ڷָ���ַ���
	std::vector<std::wstring> SplitString(std::wstring originalStr, std::wstring delimiter);
	std::vector <std::wstring> SplitStringOnlyMatchFirst(std::wstring originalStr, std::wstring delimiter);

	//ͨ���ļ�·����ȡ�ļ���
	std::wstring GetFileNameFromFilePath(std::wstring filePath);

	std::wstring GetFileNameWithOutSuffix(std::wstring filePath);




	LPSTR WCHAR_TO_LPSTR(WCHAR wStr[]);

	//��ȡ�ļ����а���������
	std::wstring GetFileHashFromFileName(std::wstring input);
	std::wstring GetPSHashFromFileName(const std::wstring& input);
	std::wstring GetVSHashFromFileName(const std::wstring& input);
	std::wstring GetPixelSlotFromTextureFileName(std::wstring TextureFileName);

	//�Ƴ��ַ����е�����
	std::wstring RemoveSquareBrackets(std::wstring inputStr);
	std::wstring Remove$Prefix(std::wstring VariableName);
}
