#pragma once
//#include "GlobalConfigs.h"
#include "D3d11GameType.h"

class FmtFile {
public:
	std::wstring Topology = L"";
	std::vector<D3D11Element> d3d11ElementList;


	//���������������������Ҫ�ֶ�ƴ��һ��FMT�ļ������ʱ�������õĶ���
	std::wstring Format = L"";
	D3D11GameType d3d11GameType;
	std::vector<std::string> ElementNameList;
	int Stride = 0;


	FmtFile();
	FmtFile(std::wstring readFmtPath);
	void OutputFmtFile(std::wstring OutputFmtPath);

	bool IsD3d11ElementListMatch(std::vector<std::string> TargetElementList);
};


