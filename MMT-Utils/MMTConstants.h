#pragma once
#include <string>
//ʹ��const std::wstring��namespace���������������Ϳ���ȫ��ʹ��һ�ݳ�����


enum class VertexCountType {
	TxtFileShow,
	TxtFileCountLine,
	BufFileTotal,
	BufFileRemoveNull
};


enum class IniSectionType {
	TextureOverrideIB,
	TextureOverrideVB,
	TextureOverrideTexture,
	IBSkip,
	ResourceVB,
	ResourceIB,
	ResourceTexture,
	CreditInfo
};



namespace MMT_Tips {
	const std::wstring TIP_GenerateMod_NoAnyExportModelDetected = LR"(
�����Ĵ�Blender��������DrawIB��Hash�ļ����У�δ�ҵ��κ�.ib .vb .fmt��ʽ��3Dmigotoģ���ļ�
���ܵĽ���������£�
1,�������Ƿ���ȷ��ʹ��MMT��������˵�����
2,�������������ļ���ʽ�Ƿ�Ϊ[����.��׺]��ʽ����1.ib 1.vb 1.fmt��
3,�������Ƿ񵼳��˴���Ϸ��Dump��ȫ����λ������������޸������˵�Modģ�Ϳ������ǵ�ģ�Ͳ�λ�뵱ǰ��Ϸ��ȡ���Ĳ�λ��һ�£�
�벹�䵼����Ӧ��λ��������Mod��ȡ��Mod��ini�ж�Ӧ��λ��TextureOverrideIB�е�drawindexed = auto������ib = null
4,�練�����ȷ���Ѿ������ɹ����Ҹ�ʽȫ����ȷ������ϵNicoMico��ȡ����֧�֡�)";

	const std::wstring TIP_AutoGameType_NoGameTypeDetected = LR"(
�����������͵��Զ�����ʶ���޷�ʶ�����ǰIB����������
���ܵĽ���������£�
1,����ϵNicoMico���Բ���Ӵ���������֧�֣�����Ҫ���������ж�ȡ������FrameAnalysis�ļ����Լ���ȡ�õ�IB��Hashֵһ���͸�NicoMico���������Բ����°汾�����֧�֡�
2,������Ƿ��ڸ��¹���ʱδ����ConfigĿ¼�µ�ExtractTypes�ļ��У�������Ϸ�汾���º��������ͺ�MMT�������������Ͳ�ƥ�䣬�����޷�ʶ��
3,�������֮ǰ�İ汾����������ȡ������ǰ�汾�޷���ȡ������ϵNicoMico�޸���BUG��
)";
}


namespace DXGI_FORMAT {
	const std::string R8G8B8A8_UNORM = "R8G8B8A8_UNORM";
	const std::string R32G32B32A32_FLOAT = "R32G32B32A32_FLOAT";
	const std::string DXGI_FORMAT_R16_UINT = "DXGI_FORMAT_R16_UINT";
	const std::string DXGI_FORMAT_R32_UINT = "DXGI_FORMAT_R32_UINT";

}



