#include "IniBuilder.h"

#include "MMTFileUtils.h"

void IniBuilder::AppendSectionLine(IniSectionType InputIniSectionType) {
	for (IniSection iniSection : this->IniSectionList) {
		if (iniSection.SectionType == InputIniSectionType) {
			for (std::wstring iniLine : iniSection.SectionLineList) {
				LineList.push_back(iniLine);
			}
			LineList.push_back(L";----------------------------------------------------------\n");
		}
	}
}


void IniBuilder::AppendSection(IniSection InputIniSection) {
	this->IniSectionList.push_back(InputIniSection);
}


void IniBuilder::SaveToFile(std::wstring FilePath) {
	//���ﲢ���Ǽ򵥵���������ˣ����Ǹ���Section�ж����������к������
	// �����ⲿ���������AppendSectionLine�������ⲿֻ��������Section����Section����ָ�����͡�
	this->AppendSectionLine(IniSectionType::IBSkip);

	this->AppendSectionLine(IniSectionType::TextureOverrideIB);
	this->AppendSectionLine(IniSectionType::TextureOverrideVB);

	this->AppendSectionLine(IniSectionType::TextureOverrideTexture);

	this->AppendSectionLine(IniSectionType::ResourceIB);
	this->AppendSectionLine(IniSectionType::ResourceVB);
	this->AppendSectionLine(IniSectionType::ResourceTexture);

	this->AppendSectionLine(IniSectionType::CreditInfo);

	std::wofstream outputIniFile(FilePath);
	for (std::wstring Line: this->LineList) {
		outputIniFile << Line << L"\n";
	}
	outputIniFile.close();
}