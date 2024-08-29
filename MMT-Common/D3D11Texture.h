#pragma once
#include <string>
#include <vector>
#include <unordered_map>


class TextureType {
public:
	std::unordered_map<std::string, std::string> PixelSlot_TextureType_Map;

	//���˻��ƣ��ѽ�ɫ���������ͷֿ�
	bool GPUPreSkinning = true;

	TextureType();

	std::vector<int> PsSetShaderResourceTimeList;

	bool TimeInPsSetShaderResourceTimeList(int time);

	uint32_t GetMeaningfulDdsFileCount();

	std::string GetDiffuseMapSlot();

	std::unordered_map<std::string,std::vector<uint32_t>> AliasName_FileSizeList_Map;
};



