#pragma once

#include "Functions_Basic.h"


//TODO ������������ȡ����CS���㣬����������Щ����ֻ�вο������ˡ�
class Functions_WW: public Functions_Basic{
private:
	void Extract_GPU_PreSkinning_WW(std::wstring DrawIB, std::wstring GameType);
	void Extract_Object_WW(std::wstring DrawIB, std::wstring GameType);

public:
	void ExtractModel() override;
	void GenerateMod() override;
	void InitializeTextureTypeList() override;
};
