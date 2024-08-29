#include <io.h>
#include "GlobalConfigs.h"

//CPU-PreSkinning
#include "Functions_Unity_CPU_PreSkinning.h"

//UE4 CS
#include "Functions_WW.h"
#include "Functions_ToF.h"

//UE4 VS
#include "Functions_WW1.1.h"
#include "Functions_KBY.h"
#include "Functions_SnB.h"

//Unity VS
#include "Functions_HI3.h"
#include "Functions_GI.h"
#include "Functions_HSR.h"
#include "Functions_ZZZ.h"

//Unity CS


#include "MMTTimeUtils.h"


//��ʼ��easylogpp
INITIALIZE_EASYLOGGINGPP
//��ʼ��ȫ������
GlobalConfigs G;
//��ʼ����־
MMTLogger LOG;


std::int32_t wmain(std::int32_t argc, wchar_t* argv[])
{
    //����ʹ������locale
    setlocale(LC_ALL, "Chinese-simplified");

    std::wstring fullPath = argv[0];
    std::wstring applicationLocation = MMTString::GetFolderPathFromFilePath(fullPath);
    
    //��Ϊ������Ҫ�ŵ�PluginsĿ¼��
    applicationLocation = MMTString::GetParentFolderPathFromFolderPath(applicationLocation);

    //���ȳ�ʼ����־���ã��ǳ���Ҫ
    std::string logFileName = "Logs\\" + MMTTime::GetCurrentTimeString() + ".log";
    el::Configurations logConfigurations;
    logConfigurations.setToDefault();
    logConfigurations.set(el::Level::Global, el::ConfigurationType::ToStandardOutput, "false");
    logConfigurations.set(el::Level::Global, el::ConfigurationType::ToFile, "true");
    logConfigurations.set(el::Level::Global, el::ConfigurationType::Filename, logFileName);
    el::Loggers::reconfigureAllLoggers(logConfigurations);

    LOG.Info(L"Running : " + fullPath);
    LOG.NewLine();

    //��ʼ����־��
    LOG = MMTLogger(applicationLocation);
    //��ʼ��ȫ������
    G = GlobalConfigs(applicationLocation);

    //��ʼ������Ϸ�������
    LOG.Info(L"��ӭʹ��MMT-Community����������NicoMico��������Github��Դ��ѣ�https://github.com/StarBobis/MigotoModTool");
    LOG.Info(L"��ǰִ�У�" + G.RunCommand);
    LOG.NewLine();

    //������Ϸ���;�������ʹ���ĸ�������д���
    Functions_Basic* basic_functions = new Functions_Basic();

    if (G.GameName == L"GI") {
        basic_functions = new Functions_GI();
    }
    else if (G.GameName == L"HI3") {
        basic_functions = new Functions_HI3();
    }
    else if (G.GameName == L"HSR") {
        basic_functions = new Functions_HSR();
    }
    else if (G.GameName == L"ZZZ") {
        basic_functions = new Functions_ZZZ();
    }
    else if (G.GameName == L"WW1.1") {
        basic_functions = new Functions_WW11();
    }
    else if (G.GameName == L"SnB") {
        basic_functions = new Functions_SnB();
    }
    else if (G.GameName == L"KBY") {
        basic_functions = new Functions_KBY();
    }
    else if (G.GameName == L"ToF") {
        basic_functions = new Functions_ToF();
    }
    else if (G.GameName == L"WW") {
        basic_functions = new Functions_WW();
    }
    //CPU-PreSkinning����Ϸ
    else if (G.GameName == L"GF2") {
        basic_functions = new Functions_Unity_CPU_PreSkinning();
    }

    //��ʼ����ͼʶ���㷨�б�
    basic_functions->InitializeTextureTypeList();

    //��ʼ������Ϸ��������
    std::wstring GameTypeConfigPath = G.ApplicationRunningLocation + L"\\Configs\\ExtractTypes\\" + G.GameName + L"\\";
    if (G.GameName == L"GF2") {
        //Unity-CPU-PreSkinning
        GameTypeConfigPath = G.ApplicationRunningLocation + L"\\Configs\\ExtractTypes\\Unity-CPU-PreSkinning\\";
    }
    basic_functions->InitializeGameTypeListFromConfig(GameTypeConfigPath);

    //�������������ȡ����������·��
    std::wstring ReverseGameTypeExtraConfigPath = G.ApplicationRunningLocation + L"\\Configs\\ReverseTypes\\";


#ifdef _DEBUG 
    //ע�⣡��ֹʹ��DEBUGģʽ���в��Ժͱ��룡
    LOG.NewLine();
#else
    //������ȡģ��
    if (G.RunCommand == L"merge") {
        //�������ΪMerge����ȷ����������һ��FrameAnalysis�ļ���
        if (G.FrameAnalyseFolder == L"") {
            LOG.Error("Can't find any FrameAnalysis folder in your 3Dmigoto folder,please try press F8 to dump a new one with Hunting open.");
        }
        if (G.GameName == L"WWMI") {
            LOG.Error(L"WWMI������������ȡ�ͽű�ʽһ����������ʹ��WW�������л���WW1.1");
        }
        else {
            basic_functions->ExtractModel();
        }
    }
    //����Mod
    else if (G.RunCommand == L"split") {

        if (G.GameName == L"WWMI") {
            LOG.Error(L"WWMI������������ȡ�ͽű�ʽһ����������ʹ��WW�������л���WW1.1");
        }
        else {
            basic_functions->GenerateMod();
        }
    }
    else {
        LOG.Error(L"δ֪�������������MMT�汾");
    }

    //If we success executed,we will run to here.
    LOG.Success();

    //clean object created by "new" 
    delete basic_functions;
#endif
    return 0;
}
