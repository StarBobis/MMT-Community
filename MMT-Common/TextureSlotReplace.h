#pragma once

#include <string>
#include <unordered_map>

class TextureSlotReplace {
public:
	std::unordered_map<std::wstring, std::wstring> In_SlotName_ResourceName_Map;
	std::unordered_map<std::wstring, std::wstring> In_SlotName_ResourceFileName_Map;

	//�����������������������ͼ̽��ʱ���ɵ����ݣ��Զ�ƴ�ӳ����ڳ־û���SlotName_Equal_ResourceFileName_List
	void SetSerializedList();

	std::vector<std::string> Save_SlotName_Equal_ResourceFileName_List;

	std::vector<std::wstring> Out_SlotReplaceStrList;
	std::unordered_map<std::wstring,std::wstring> Out_ResourceName_FileName_Map;

	TextureSlotReplace();

	//���ݳ־û���ȡ�������ݣ��Զ�ƴ�ӳ����ʱʹ�õ�����
	TextureSlotReplace(std::vector<std::string> Input_SlotName_Equal_ResourceFileName_List);
};