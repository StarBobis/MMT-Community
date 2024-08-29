#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <istream>
#include <fstream>

namespace MMTFile{
	//�ƶ��ļ�������վ������ֱ��ɾ������ֹ���ݶ�ʧ
	//ʵ�ʲ��Է�����BUG���е�ʱ���޷��ƶ��ļ�������վ�����Բ��Ƽ�ʹ��
	bool MoveFileToRecycleBin_Deprecated(const std::wstring& filePath);

	std::vector<std::wstring> FindFileNameListWithCondition(std::wstring SearchFolderPath, std::wstring searchStr, std::wstring endStr);
	std::vector<std::wstring> FindTextureFileList(std::wstring SearchFolderPath, std::wstring FilterStr);

	std::vector<std::wstring> ReadIniFileLineList(std::wstring filePath);

	std::vector<std::wstring> GetFilePathListRecursive(std::wstring directory);

	//��ȡ�ļ��������У�һ������ini��ȡ�����
	std::vector<std::wstring> ReadAllLinesW(std::wstring filePath);

	//ɾ��ָ��Ŀ¼��ָ����׺���ļ�������������һ�����ɵ�Mod�ļ���
	void DeleteFilesWithSuffix(std::wstring directory, std::wstring suffix);

	std::unordered_map<uint64_t, std::vector<std::byte>> ReadBufMapFromFile(std::wstring readPathW, uint64_t vertexNumber);

	//��ȡ�ļ���С
	uint64_t GetFileSize(std::wstring FileName);

	//��ȡ����Ľ�β������ر��0���ļ�����ʵ�Ĳ�Ϊ0���ֵ��ļ���С
	uint64_t GetRealFileSize_NullTerminated(std::wstring FileName);

	std::wstring FindMaxPrefixedDirectory(const std::wstring& directoryPath, const std::wstring& prefix);

	//���ļ��в���ָ��3Dmigoto��ini�������ԣ��������format,���ṩ����·������ֱ�ӷ��������ļ���������������
	std::wstring FindMigotoIniAttributeInFile(const std::wstring& filePath, const std::wstring& attributeName);

	bool IsValidFilename(std::string filename);

	void ReadDirectoryRecursively(const std::string& directory, std::vector<std::string>& filePaths);
}
