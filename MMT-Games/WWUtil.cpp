#include "WWUtil.h"
#include "MMTJsonUtils.h"
#include "MMTLogUtils.h"


void WuwaCSInfoJsonObject::saveToJsonFile(std::wstring outputFolder) {
    std::wstring jsonFilePath = outputFolder + L"wwinfo.json";
    //д��ԭʼ�Ķ�������Ӧ������CS��Ϣ
    nlohmann::json wuwaCsInfoJson;
    for (const auto& pair : this->PartNameWuwaCSInfoMap) {
        WuwaCSInfo csInfo = pair.second;
        LOG.Info("VertexCount: " + std::to_string(csInfo.CalculateTime) + "  Offset:" + std::to_string(csInfo.Offset) + "  ComputeShaderHash:" + csInfo.ComputeShaderHash);
        nlohmann::json csInfoJson;
        csInfoJson["CalculateTime"] = std::to_string(csInfo.CalculateTime);
        csInfoJson["Offset"] = std::to_string(csInfo.Offset);
        csInfoJson["ComputeShaderHash"] = csInfo.ComputeShaderHash;

        //����key��partName
        wuwaCsInfoJson[pair.first] = csInfoJson;
    }
    //д�����ļ�
    if (std::filesystem::exists(jsonFilePath)) {
        std::filesystem::remove(jsonFilePath);
    }
    std::ofstream file(jsonFilePath);
    std::string content = wuwaCsInfoJson.dump(4); // ʹ��������ʽ���浽�ļ�������Ϊ4���ո�
    file << content << "\n";
    file.close();
}


WuwaCSInfoJsonObject::WuwaCSInfoJsonObject() {

}


WuwaCSInfoJsonObject::WuwaCSInfoJsonObject(std::wstring readFolderPath) {
    std::wstring readJsonPath = readFolderPath + L"wwinfo.json";
    if (std::filesystem::exists(readJsonPath)) {
        nlohmann::json wwcsinfoJson = MMTJson::ReadJsonFromFile(readJsonPath);
        WuwaCSInfo wwcsInfo;
        for (const auto& item : wwcsinfoJson.items()) {
            std::string CalculateTime = item.value()["CalculateTime"];
            std::string Offset = item.value()["Offset"];
            std::string ComputeShaderHash = item.value()["ComputeShaderHash"];
            LOG.Info("CalculateTime: " + CalculateTime);
            LOG.Info("Offset: " + Offset);
            LOG.Info("ComputeShaderHash: " + ComputeShaderHash);
            wwcsInfo.CalculateTime = std::stoi(CalculateTime);
            wwcsInfo.Offset = std::stoi(Offset);
            wwcsInfo.ComputeShaderHash = ComputeShaderHash;
            this->PartNameWuwaCSInfoMap[item.key()] = wwcsInfo;
        }
    }
}
