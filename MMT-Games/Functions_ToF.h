#pragma once


#include "Functions_Basic.h"

//TODO Ҫ��ӻ���֧�ֵĻ����������ѧ��WWMI�����м��ɣ��������ϰ汾�ĵ�һ��WWMI�汾���С�
class Functions_ToF : public Functions_Basic {
private:
	void Extract_GPU_PreSkinning_WW(std::wstring DrawIB, std::wstring GameType);
	void Extract_Object_WW(std::wstring DrawIB, std::wstring GameType);
public:
	void ExtractModel() override;
	void GenerateMod() override;
	void InitializeTextureTypeList() override;
};

