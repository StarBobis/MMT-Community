#include "GlobalConfigs.h"
#include "FrameAnalysisData.h"


//���������ȷ��ȫ��ΨһFAData
FrameAnalysisData GlobalConfigs::GetFrameAnalysisData() {
    //���Ϊ��˵��û��ʼ��,��Ȼֱ�ӷ����Ѿ���ʼ���õ�
    if (this->FAData.WorkFolder == L"") {
        FrameAnalysisData tmpFAData(this->WorkFolder);
        this->FAData = tmpFAData;
    }
    return this->FAData;
}

FrameAnalysisData GlobalConfigs::GetFrameAnalysisData(std::wstring DrawIB) {
    //���Ϊ��˵��û��ʼ��,��Ȼֱ�ӷ����Ѿ���ʼ���õ�
    if (this->FAData.WorkFolder == L"") {
        FrameAnalysisData tmpFAData(this->WorkFolder, DrawIB);
        this->FAData = tmpFAData;
    }
    return this->FAData;
}


FrameAnalysisLog GlobalConfigs::GetFrameAnalysisLog() {
    if (this->FALog.WorkFolder == L"") {
        FrameAnalysisLog tmpFALog(this->WorkFolder);
        this->FALog = tmpFALog;
    }
    return this->FALog;
}


//���ṩ�������е�·��
GlobalConfigs::GlobalConfigs(std::wstring ApplicationLocation) {
    //��ʼ��GIMI��ʽ��Ҫ����Ƭ����
    this->GIMIPartNameAliasMap[L"1"] = L"Head";
    this->GIMIPartNameAliasMap[L"2"] = L"Body";
    this->GIMIPartNameAliasMap[L"3"] = L"Dress";
    this->GIMIPartNameAliasMap[L"4"] = L"Extra";
    this->GIMIPartNameAliasMap[L"5"] = L"Extra1";
    this->GIMIPartNameAliasMap[L"6"] = L"Extra2";
    this->GIMIPartNameAliasMap[L"7"] = L"Extra3";
    this->GIMIPartNameAliasMap[L"8"] = L"Extra4";
    this->GIMIPartNameAliasMap[L"9"] = L"Extra5";
    this->GIMIPartNameAliasMap[L"10"] = L"Extra6";
    this->GIMIPartNameAliasMap[L"12"] = L"Extra7";
    this->GIMIPartNameAliasMap[L"13"] = L"Extra8";
    this->GIMIPartNameAliasMap[L"14"] = L"Extra9";
    this->GIMIPartNameAliasMap[L"15"] = L"Extra10";
    this->GIMIPartNameAliasMap[L"16"] = L"Extra11";
    this->GIMIPartNameAliasMap[L"17"] = L"Extra12";
    this->GIMIPartNameAliasMap[L"18"] = L"Extra13";

    //��ʼ������·��
    this->ApplicationRunningLocation = ApplicationLocation;
    LOG.Info(L"ApplicationLocation: " + ApplicationLocation);
    LOG.Info(L"Start to initialize global config:");

    //��ʼ������·��
    this->Path_MainJson = this->ApplicationRunningLocation + L"\\Configs\\Main.json";

    //��ȡ��ǰ��������Ϸ����
    LOG.Info(L"Start to read from Main.json:");
    LOG.Info(L"Main.json Path:" + this->Path_MainJson);
    nlohmann::json MainJsonData = MMTJson::ReadJsonFromFile(this->Path_MainJson);
    this->GameName = MMTString::ToWideString(MainJsonData["GameName"]);
    LOG.Info(L"GameName:" + GameName);
    LOG.NewLine();

    //������Ϸ���ƣ������GamesĿ¼�µľ�����ϷĿ¼
    this->Path_Game_Folder = this->ApplicationRunningLocation + L"\\Games\\" + GameName + L"\\";
    this->Path_Game_3DmigotoFolder = Path_Game_Folder + L"3Dmigoto\\";
    this->Path_Game_VSCheckJson = Path_Game_Folder + L"VSCheck.json";
    this->Path_Game_ConfigJson = Path_Game_Folder + L"Config.json";
    this->Path_Game_SettingJson = this->ApplicationRunningLocation + L"\\Configs\\" + L"Setting.json";
    

    //�ļ����������
    this->OutputFolder = this->Path_Game_3DmigotoFolder + L"Mods\\output\\";
    LOG.Info(L"OutputFolder:" + OutputFolder);
    std::filesystem::create_directories(OutputFolder);

    std::wstring timeStr = MMTString::GetFormattedDateTimeForFilename().substr(0, 10);
    this->TimeOutputFolder = this->OutputFolder + timeStr + L"/";

    this->LoaderFolder = this->Path_Game_3DmigotoFolder;
    LOG.Info(L"LoaderFolder:" + LoaderFolder);
    
    std::wstring latestFrameAnalyseFolder = MMTFile::FindMaxPrefixedDirectory(LoaderFolder, L"FrameAnalysis-");
    if (latestFrameAnalyseFolder == L"") {
        LOG.Warning(L"Can't find any FrameAnalysisFolder.");
    }
    this->FrameAnalyseFolder = latestFrameAnalyseFolder;
    LOG.Info(L"FrameAnalyseFolder:" + FrameAnalyseFolder);

    this->WorkFolder = LoaderFolder + FrameAnalyseFolder + L"\\";
    LOG.Info(L"WorkFolder:" + WorkFolder);
    LOG.NewLine();

    //��ȡ��ǰ�������������
    this->Path_RunInputJson = this->ApplicationRunningLocation + L"\\Configs\\RunInput.json";
    nlohmann::json runInputJson = MMTJson::ReadJsonFromFile(this->Path_RunInputJson);
    std::string runCommand = runInputJson["RunCommand"];
    this->RunCommand = MMTString::ToWideString(runCommand);

    //��ȡConfigs\\Setting.json
    if (std::filesystem::exists(this->Path_Game_SettingJson)) {
        nlohmann::json configJsonData = MMTJson::ReadJsonFromFile(this->Path_Game_SettingJson);
        this->ShareSourceModel = (bool)configJsonData["ShareSourceModel"];
        LOG.Info(L"Share Source Model: " + std::to_wstring(this->ShareSourceModel));

        this->Author = MMTString::ToWideString((std::string)configJsonData["Author"]);
        LOG.Info(L"Author: " + this->Author);

        this->AuthorLink = MMTString::ToWideString((std::string)configJsonData["AuthorLink"]);
        LOG.Info(L"AuthorLink: " + this->AuthorLink);

        LOG.NewLine();
    }

    //��ȡGames\\[��Ӧ��Ϸ����]\\Config.json����
    if (std::filesystem::exists(this->Path_Game_ConfigJson)) {
        //������Ϸ���ͣ���ȡconfigsĿ¼�¶�Ӧ�������ļ�
        nlohmann::json configJsonData = MMTJson::ReadJsonFromFile(this->Path_Game_ConfigJson);
        for (const auto& obj : configJsonData) {
            DrawIBConfig config;
            //�Ȱ�DrawIB��ȡ����
            config.DrawIB = MMTString::ToWideString(obj["DrawIB"]);
            LOG.Info(L"Start to read config for DrawIB:" + config.DrawIB);
            
            config.GameType = MMTString::ToWideString(obj["GameType"]);
            LOG.Info(L"GameType:" + config.GameType);

            config.TangentAlgorithm = MMTString::ToWideString(obj["TangentAlgorithm"]);
            LOG.Info(L"TangentAlgorithm:" + config.TangentAlgorithm);

            config.ColorAlgorithm = MMTString::ToWideString(obj["ColorAlgorithm"]);
            LOG.Info(L"ColorAlgorithm:" + config.ColorAlgorithm);

            config.ForbidAutoTexture = obj["ForbidAutoTexture"];
            LOG.Info(L"ForbidAutoTexture:" + std::to_wstring(config.ForbidAutoTexture));

            //UseHashTexture
            config.UseHashTexture = obj["UseHashTexture"];
            LOG.Info(L"UseHashTexture:" + std::to_wstring(config.UseHashTexture));

            //color map
            config.ColorMap = obj["Color"];
            LOG.Info(L"ColorMap:");
            for (const auto& pair : config.ColorMap) {
                LOG.Info(L"Color: " + MMTString::ToWideString(pair.first) + L"  MMTString::value: " + MMTString::ToWideString(pair.second));
            }

            //Reverse Normal and Tangent
            config.NormalReverseX = obj["NormalReverseX"];
            config.NormalReverseY = obj["NormalReverseY"];
            config.NormalReverseZ = obj["NormalReverseZ"];

            config.TangentReverseX = obj["TangentReverseX"];
            config.TangentReverseY = obj["TangentReverseY"];
            config.TangentReverseZ = obj["TangentReverseZ"];
            config.TangentReverseW = obj["TangentReverseW"];
            LOG.NewLine();

            //�������ж��Ƿ����tmpConfig,ֻ���ڴ��ڵ�����²Ż���ж�ȡ�������ֹ�ָ��
            std::wstring tmpConfigPath = OutputFolder + config.DrawIB + L"\\tmp.json";
            LOG.Info(tmpConfigPath);
            if (std::filesystem::exists(tmpConfigPath)) {
                nlohmann::json tmpJsonData = MMTJson::ReadJsonFromFile(tmpConfigPath);

                //Read the config Merge generated.

                LOG.Info(L"Start to read tmp config.");
                config.VertexLimitVB = tmpJsonData["VertexLimitVB"];
                LOG.Info(L"VertexLimitVB:" + MMTString::ToWideString(config.VertexLimitVB));

                config.WorkGameType = tmpJsonData["WorkGameType"];
                LOG.Info(L"WorkGameType:" + MMTString::ToWideString(config.WorkGameType));

                config.TmpElementList = tmpJsonData["TmpElementList"];
                LOG.Info(L"TmpElementList:");
                for (const auto& pair : config.TmpElementList) {
                    LOG.Info(MMTString::ToWideString(pair));
                }

                config.CategoryHashMap = tmpJsonData["CategoryHash"];
                LOG.Info(L"CategoryHash:");
                for (const auto& pair : config.CategoryHashMap) {
                    LOG.Info(L"Category: " + MMTString::ToWideString(pair.first) + L"  Hash: " + MMTString::ToWideString(pair.second));
                }
                config.MatchFirstIndexList = tmpJsonData["MatchFirstIndex"];
                LOG.Info(L"MatchFirstIndex:");
                for (const auto& pair : config.MatchFirstIndexList) {
                    LOG.Info(MMTString::ToWideString(pair));
                }
                config.PartNameList = tmpJsonData["PartNameList"];
                LOG.Info(L"PartNameList:");
                for (const auto& pair : config.PartNameList) {
                    LOG.Info(MMTString::ToWideString(pair));
                }

                config.PartName_TextureSlotReplace_Map = tmpJsonData["PartNameTextureResourceReplaceList"];

                LOG.NewLine();
            }

            DrawIB_ExtractConfig_Map[config.DrawIB] = config;
        }
        LOG.Info(L"Read config for every DrawIB success.");
        LOG.NewLine();


    }
    else {
        LOG.Info(L"Can't find config file: " + this->Path_Game_ConfigJson);
    }

}


//�����ṩ�޲ι��죬������������ȫ�ֱ������ں������г�ʼ����
GlobalConfigs::GlobalConfigs() {
    
}





std::unordered_map<std::wstring, std::wstring> GlobalConfigs::Get_Hash_TextureFileNameMap() {



}