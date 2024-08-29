#pragma once

#include <string>
#include <unordered_map>

#include "GlobalConfigs.h"
#include "D3d11GameType.h"

#include "IniBuilder.h"

namespace BufferUtils {
	IniSection GetCreditInfoIniSection();
	void AddCreditREADME();

	//��������Դģ��
	void ShareSourceModel(DrawIBConfig IBConfig);

	//COLOR�ؼ��㣬ʹ�û�������ƽ����AverageNormal�ؼ��㷽��
	void Unity_COLOR_AverageNormal(std::unordered_map<std::wstring, std::vector<std::byte>>& finalVBCategoryDataMap, DrawIBConfig basicConfig, D3D11GameType d3d11GameType);
	//TANGENTʹ�û���Vector��AverageNormal��һ���ؼ��㷽��
	void Unity_TANGENT_AverageNormal(std::unordered_map<std::wstring, std::vector<std::byte>> &finalVBCategoryDataMap, D3D11GameType d3d11GameType);
	//COLORֱֵ�Ӹ���
	void Unity_Reset_COLOR(std::unordered_map<std::wstring, std::vector<std::byte>>& finalVBCategoryDataMap, DrawIBConfig basicConfig, D3D11GameType d3d11GameType);
	//��תNORMAL��TANGENT��ֵ
	void Unity_Reverse_NORMAL_TANGENT(std::unordered_map<std::wstring, std::vector<std::byte>>& finalVBCategoryDataMap, DrawIBConfig basicConfig, D3D11GameType d3d11GameType);

	//����Mod�õ�---------------------------------------------
	uint32_t GetSum_DrawNumber_FromVBFiles( DrawIBConfig IBConfig);
	std::unordered_map<std::wstring, std::vector<std::byte>> Read_FinalVBCategoryDataMap( DrawIBConfig IBConfig);
	void CopySlotDDSTextureFromOutputFolder( DrawIBConfig IBConfig);
	void Read_Convert_Output_IBBufferFiles( DrawIBConfig IBConfig);
	void OutputCategoryBufferFiles( DrawIBConfig IBConfig);
}