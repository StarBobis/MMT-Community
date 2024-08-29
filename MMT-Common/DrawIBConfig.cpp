#include "DrawIBConfig.h"

#include "MMTLogUtils.h"
#include "MMTJsonUtils.h"
#include "MMTFileUtils.h"
#include "MMTStringUtils.h"


bool DrawIBConfig::Initialize(std::wstring OutputFolder, D3D11GameType InputD3d11GameType) {
    //��ȡ��ǰ���� YYYY_MM_DD
    std::wstring timeStr = MMTString::GetFormattedDateTimeForFilename().substr(0, 10);
    this->ModOutputFolder = OutputFolder + timeStr;

    // ָ�����Ŀ¼
    this->BufferReadFolder = OutputFolder + this->DrawIB + L"\\";
    this->BufferOutputFolder = this->ModOutputFolder + L"\\Buffer\\";
    this->TextureOutputFolder = this->ModOutputFolder + L"\\Texture\\";
    this->ModelFolder = this->ModOutputFolder + L"\\Model\\";
    this->CreditOutputFolder = this->ModOutputFolder + L"\\Credit\\";

    this->d3d11GameType = InputD3d11GameType;

    //�ж��Ƿ����1.vbʲô�����жϵ�ǰDrawIB�Ƿ񵼳���ģ���ļ�
    bool findVBModel = false;
    for (std::string partName : this->PartNameList) {
        std::wstring VBFileName = MMTString::ToWideString(partName) + L".vb";
        if (std::filesystem::exists(BufferReadFolder + VBFileName)) {
            findVBModel = true;
        }
    }

    if (findVBModel) {
        //��ɾ����һ�����ɵ�����Buffer�ļ�
        //std::filesystem::remove_all(BufferOutputFolder);

        //�����´���Buffer�ļ���
        std::filesystem::create_directories(BufferOutputFolder);
        std::filesystem::create_directories(TextureOutputFolder);
        std::filesystem::create_directories(ModelFolder);
        //std::filesystem::create_directories(CreditOutputFolder);
        return true;
    }
    else {
        LOG.Warning(L"Detect didn't export vb file for DrawIB: " + this->DrawIB + L" , so skip this DrawIB's mod generate.");
        return false;
    }


}


//����outputFolder�������浽outputfolder�����DrawIBΪ���Ƶ��ļ�����
void DrawIBConfig::SaveTmpJsonConfigs(std::wstring outputPath) {
    LOG.Info(L"Start to save tmp config");

    //write to json file.
    nlohmann::json tmpJsonData;
    tmpJsonData["VertexLimitVB"] = VertexLimitVB;
    tmpJsonData["TmpElementList"] = TmpElementList;
    tmpJsonData["CategoryHash"] = CategoryHashMap;
    tmpJsonData["MatchFirstIndex"] = MatchFirstIndexList;
    tmpJsonData["PartNameList"] = PartNameList;
    tmpJsonData["WorkGameType"] = WorkGameType;

    tmpJsonData["PartNameTextureResourceReplaceList"] = this->PartName_TextureSlotReplace_Map;

    std::wstring configCompletePath = outputPath + L"tmp.json";
    LOG.Info(L"configCompletePath: " + configCompletePath);
    // save json config into file.
    // ��������ļ������ڣ�����Զ������µ��ļ���д�룬���Բ��õ��ġ�
    if (std::filesystem::exists(configCompletePath)) {
        std::filesystem::remove(configCompletePath);
    }
    std::ofstream file(configCompletePath);
    std::string content = tmpJsonData.dump(4); // ʹ��������ʽ���浽�ļ�������Ϊ4���ո�
    file << content << "\n";
    file.close();
    LOG.Info(L"Json config has saved into file.");

    LOG.NewLine();
}
