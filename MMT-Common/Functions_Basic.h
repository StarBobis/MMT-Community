#pragma once

#include <vector>
#include <unordered_map>

#include "D3d11GameType.h"
#include "D3D11Texture.h"
#include "TextureSlotReplace.h"
#include "MMTJsonUtils.h"
#include "DrawIBConfig.h"




//���࣬�������ʹ��븴�ã���Ű���������������ж���ʵ�֡�
class Functions_Basic {
//������������ʵķ����ŵ�protected��������Ź��÷�����
protected:

	//���������������
	std::vector<D3D11GameType> CurrentD3d11GameTypeList;
	std::vector<std::string> Ordered_GPU_Object_D3D11GameTypeList;
	std::unordered_map<std::string, D3D11GameType> GameTypeName_D3d11GameType_Map;

	//��ͼ�������
	std::vector<TextureType> TextureTypeList;
	std::unordered_map<std::string, TextureSlotReplace> PartName_TextureSlotReplace_Map; //���ÿ��DrawIB��֮ǰ�ǵö�����һ�£���ֻ����һ���������ݵ����á�
	virtual void AutoDetectTextureFiles(std::wstring OutputIB, std::unordered_map<std::wstring, std::wstring> MatchFirstIndex_PartName_Map, bool GPUPreSkinning) {};
	virtual void MoveAllUsedTexturesToOutputFolder(std::wstring DrawIB, std::wstring OutputFolder);
	virtual void GenerateHashTextureIni_Deprecated(std::wstring ModOutputFolder);
	virtual void GenerateHashTextureIni_2(std::wstring ModOutputFolder);

	//�Զ���������ʶ���㷨
	virtual std::wstring AutoGameType(std::wstring DrawIB);
	virtual std::wstring AutoGameType_Reverse(std::wstring TrianglelistIBIndex, uint32_t TrianglelistVertexCount);

	void InitializeGameTypeList(std::unordered_map<std::string, nlohmann::json> GameTypeName_GameTypeJson_Map);

//�麯����̬�������ô��ڣ�ÿ����Ϸ�����Լ���ʵ��
public:
	//����ǰ��ǰ�����ó�ʼ�� 
	virtual void InitializeGameTypeListFromConfig(std::wstring GameTypeConfigPath);
	virtual void InitializeGameTypeListFromPreset() {};

	//��ʼ����ͼ�Զ�ʶ���㷨
	virtual void InitializeTextureTypeList() {};

	//���������ȡģ�ͺ�����Mod ��ѡ
	virtual void ExtractModel() { };
	virtual void GenerateMod() { };

	//������ȡ��һ������
	//virtual void ExtractModel_Reverse();
	//virtual void ReverseSingle();
	//virtual void ReverseMerged_Toggle();
	//virtual void ReverseMerged_OutfitCompiler();
	//virtual void ReverseMerged_NameSpace();
	//virtual void Reverse_3DmigotoSimulater();
};

