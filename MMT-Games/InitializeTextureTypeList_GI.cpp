#include "Functions_GI.h"


void Functions_GI::InitializeTextureTypeList() {
	
	std::vector<TextureType> TmpTextureTypeList;
	//- ԭ��Diffuse��ͼ��ʽ���Ͻ����ϸ�ʹ��BC7_UNORM_SRGB�����Ը����������һ����Сʶ��Χ��

	TextureType Body_T1_01234;
	Body_T1_01234.PixelSlot_TextureType_Map["ps-t0"] = "NormalMap.dds";
	Body_T1_01234.PixelSlot_TextureType_Map["ps-t1"] = "DiffuseMap.dds";
	Body_T1_01234.PixelSlot_TextureType_Map["ps-t2"] = "LightMap.dds";
	Body_T1_01234.PixelSlot_TextureType_Map["ps-t3"] = "NicoMico";
	Body_T1_01234.PixelSlot_TextureType_Map["ps-t4"] = "NicoMico";
	TmpTextureTypeList.push_back(Body_T1_01234);

	TextureType Body_T1_012345;
	Body_T1_012345.PixelSlot_TextureType_Map["ps-t0"] = "NormalMap.dds";
	Body_T1_012345.PixelSlot_TextureType_Map["ps-t1"] = "DiffuseMap.dds";
	Body_T1_012345.PixelSlot_TextureType_Map["ps-t2"] = "LightMap.dds";
	Body_T1_012345.PixelSlot_TextureType_Map["ps-t3"] = "NicoMico";
	Body_T1_012345.PixelSlot_TextureType_Map["ps-t4"] = "NicoMico";
	Body_T1_012345.PixelSlot_TextureType_Map["ps-t5"] = "NicoMico";
	Body_T1_012345.PsSetShaderResourceTimeList.push_back(2);
	Body_T1_012345.PsSetShaderResourceTimeList.push_back(3);
	Body_T1_012345.PsSetShaderResourceTimeList.push_back(4);
	Body_T1_012345.PsSetShaderResourceTimeList.push_back(6);
	Body_T1_012345.PsSetShaderResourceTimeList.push_back(0);
	TmpTextureTypeList.push_back(Body_T1_012345);

	TextureType Body_T0_012345;
	Body_T0_012345.PixelSlot_TextureType_Map["ps-t0"] = "DiffuseMap.dds";
	Body_T0_012345.PixelSlot_TextureType_Map["ps-t1"] = "LightMap.dds";
	Body_T0_012345.PixelSlot_TextureType_Map["ps-t2"] = "NicoMico";
	Body_T0_012345.PixelSlot_TextureType_Map["ps-t3"] = "NicoMico";
	Body_T0_012345.PixelSlot_TextureType_Map["ps-t4"] = "NicoMico";
	Body_T0_012345.PixelSlot_TextureType_Map["ps-t5"] = "NicoMico";
	Body_T0_012345.PsSetShaderResourceTimeList.push_back(2);
	Body_T0_012345.PsSetShaderResourceTimeList.push_back(3);
	Body_T0_012345.PsSetShaderResourceTimeList.push_back(4);
	Body_T0_012345.PsSetShaderResourceTimeList.push_back(6);
	Body_T0_012345.PsSetShaderResourceTimeList.push_back(0);
	TmpTextureTypeList.push_back(Body_T0_012345);

	//�����000020���µı��׻��ƣ����ƥ�䲻���Żᴥ�����
	TextureType Hair_T0_01234567;
	Hair_T0_01234567.PixelSlot_TextureType_Map["ps-t0"] = "DiffuseMap.dds";
	Hair_T0_01234567.PixelSlot_TextureType_Map["ps-t1"] = "LightMap.dds";
	Hair_T0_01234567.PixelSlot_TextureType_Map["ps-t2"] = "NicoMico";
	Hair_T0_01234567.PixelSlot_TextureType_Map["ps-t3"] = "NicoMico";
	Hair_T0_01234567.PixelSlot_TextureType_Map["ps-t4"] = "NicoMico";
	Hair_T0_01234567.PixelSlot_TextureType_Map["ps-t5"] = "NicoMico";
	Hair_T0_01234567.PixelSlot_TextureType_Map["ps-t6"] = "NicoMico";
	Hair_T0_01234567.PixelSlot_TextureType_Map["ps-t7"] = "NicoMico";
	Hair_T0_01234567.PsSetShaderResourceTimeList.push_back(2);
	Hair_T0_01234567.PsSetShaderResourceTimeList.push_back(0);
	TmpTextureTypeList.push_back(Hair_T0_01234567);

	TextureType Body_T2_0123456;
	Body_T2_0123456.PixelSlot_TextureType_Map["ps-t0"] = "HighLightMap.dds";
	Body_T2_0123456.PixelSlot_TextureType_Map["ps-t1"] = "NormalMap.dds";
	Body_T2_0123456.PixelSlot_TextureType_Map["ps-t2"] = "DiffuseMap.dds";
	Body_T2_0123456.PixelSlot_TextureType_Map["ps-t3"] = "LightMap.dds";
	Body_T2_0123456.PixelSlot_TextureType_Map["ps-t4"] = "NicoMico";
	Body_T2_0123456.PixelSlot_TextureType_Map["ps-t5"] = "NicoMico";
	Body_T2_0123456.PixelSlot_TextureType_Map["ps-t6"] = "NicoMico";
	Body_T2_0123456.PsSetShaderResourceTimeList.push_back(7);
	Body_T2_0123456.PsSetShaderResourceTimeList.push_back(3);
	Body_T2_0123456.PsSetShaderResourceTimeList.push_back(0);
	TmpTextureTypeList.push_back(Body_T2_0123456);

	TextureType Body_T1_0123456; //������
	Body_T1_0123456.PixelSlot_TextureType_Map["ps-t0"] = "NormalMap.dds";
	Body_T1_0123456.PixelSlot_TextureType_Map["ps-t1"] = "DiffuseMap.dds";
	Body_T1_0123456.PixelSlot_TextureType_Map["ps-t2"] = "LightMap.dds";
	Body_T1_0123456.PixelSlot_TextureType_Map["ps-t3"] = "NicoMico";
	Body_T1_0123456.PixelSlot_TextureType_Map["ps-t4"] = "NicoMico";
	Body_T1_0123456.PixelSlot_TextureType_Map["ps-t5"] = "NicoMico";
	Body_T1_0123456.PixelSlot_TextureType_Map["ps-t6"] = "NicoMico";
	Body_T1_0123456.PsSetShaderResourceTimeList.push_back(7);
	Body_T1_0123456.PsSetShaderResourceTimeList.push_back(3);
	Body_T1_0123456.PsSetShaderResourceTimeList.push_back(0);
	TmpTextureTypeList.push_back(Body_T1_0123456);


	TextureType Body_T0_01234; //���
	Body_T0_01234.PixelSlot_TextureType_Map["ps-t0"] = "DiffuseMap.dds";
	Body_T0_01234.PixelSlot_TextureType_Map["ps-t1"] = "LightMap.dds";
	Body_T0_01234.PixelSlot_TextureType_Map["ps-t2"] = "NicoMico";
	Body_T0_01234.PixelSlot_TextureType_Map["ps-t3"] = "NicoMico";
	Body_T0_01234.PixelSlot_TextureType_Map["ps-t4"] = "NicoMico";
	Body_T0_01234.PsSetShaderResourceTimeList.push_back(3);
	Body_T0_01234.PsSetShaderResourceTimeList.push_back(4);
	Body_T0_01234.PsSetShaderResourceTimeList.push_back(5);
	Body_T0_01234.PsSetShaderResourceTimeList.push_back(0);
	TmpTextureTypeList.push_back(Body_T0_01234);


	TextureType Weapon_T0_0123456; //������
	Weapon_T0_0123456.GPUPreSkinning = false;
	Weapon_T0_0123456.PixelSlot_TextureType_Map["ps-t0"] = "DiffuseMap.dds";
	Weapon_T0_0123456.PixelSlot_TextureType_Map["ps-t1"] = "LightMap.dds";
	Weapon_T0_0123456.PixelSlot_TextureType_Map["ps-t2"] = "NicoMico";
	Weapon_T0_0123456.PixelSlot_TextureType_Map["ps-t3"] = "NicoMico";
	Weapon_T0_0123456.PixelSlot_TextureType_Map["ps-t4"] = "NicoMico";
	Weapon_T0_0123456.PixelSlot_TextureType_Map["ps-t5"] = "NicoMico";
	Weapon_T0_0123456.PixelSlot_TextureType_Map["ps-t6"] = "NicoMico";
	Weapon_T0_0123456.PsSetShaderResourceTimeList.push_back(7);
	Weapon_T0_0123456.PsSetShaderResourceTimeList.push_back(3);
	Weapon_T0_0123456.PsSetShaderResourceTimeList.push_back(0);
	TmpTextureTypeList.push_back(Weapon_T0_0123456);

	this->TextureTypeList = TmpTextureTypeList;
}