#include "Functions_HI3.h"

#include "MMTLogUtils.h"
#include "MMTStringUtils.h"
#include "GlobalConfigs.h"
#include "FrameAnalysisData.h"
#include "FrameAnalysisLog.h"
#include "IndexBufferTxtFile.h"
#include <filesystem>
#include <set>
#include "MMTFileUtils.h"


void Functions_HI3::AutoDetectTextureFiles(std::wstring OutputIB, std::unordered_map<std::wstring, std::wstring> MatchFirstIndex_PartName_Map, bool GPUPreSkinning) {
    LOG.Info(L"��ʼHI3��ͼ�Զ�����ʶ��:");
    FrameAnalysisLog FALog = G.GetFrameAnalysisLog();
    FrameAnalysisData FAData = G.GetFrameAnalysisData();

    bool OnlyMatchFirst = true;
    if (MatchFirstIndex_PartName_Map.size() > 1) {
        OnlyMatchFirst = false;
    }

    std::vector<std::wstring> MatchedFirstIndexList;//����б�������ֹ�ظ�ƥ��

    //����Ϊ�˼���������ȡ������ʹ�ô�Log���ȡ����ʵ����Index�б�
    std::vector<std::wstring> trianglelistIndexList = FAData.ReadRealTrianglelistIndexListFromLog(OutputIB);
    //��һ����¼��һ��ƥ��ɹ����ļ�������map
    std::unordered_map<std::wstring, int> PartName_LastMatchFileNumber_Map;
    for (std::wstring index : trianglelistIndexList) {
        LOG.Info(L"��ǰʶ���TrianglelistIndex: " + index);

        //1.�жϵ�ǰIndex�Ƿ���IB�ļ���
        std::wstring MatchFirstIndex = FAData.GetIBMatchFirstIndexByIndex(G.WorkFolder, index);
        if (MatchFirstIndex == L"") {
            LOG.Info(L"��ǰIndexδ�ҵ�IB�ļ�����������ЧIndex���Զ�������");
            LOG.NewLine();
            continue;
        }
        LOG.Info(L"��ǰ��ӦPartName: " + MatchFirstIndex_PartName_Map[MatchFirstIndex]);
        //LOG.Info(L"PartName���");
        //2.�жϵ�ǰMatchFirstIndex�Ƿ��Ѿ��������
        bool findExistsFirstIndex = false;
        for (std::wstring match_firstIndex : MatchedFirstIndexList) {
            if (MatchFirstIndex == match_firstIndex) {
                findExistsFirstIndex = true;
            }
        }

        std::vector<std::wstring> pixelShaderTextureDDSFileNameList = FAData.FindFrameAnalysisFileNameListWithCondition(index + L"-ps-t", L".dds");
        std::vector<std::wstring> pixelShaderTextureJPGFileNameList = FAData.FindFrameAnalysisFileNameListWithCondition(index + L"-ps-t", L".jpg");

        //�������е�TextureType
        bool findMatched = false;
        for (TextureType textureType : this->TextureTypeList) {
            //���ݵ�ǰ���������Ƿ�ΪGPUPreSkinning�����й���
            if (textureType.GPUPreSkinning != GPUPreSkinning) {
                //LOG.Info("Test");
                LOG.Info(L"��ǰʶ����������͵�GPUPreSkinning�趨�뵱ǰ��ͼ�����趨����������ʶ��.");
                LOG.NewLine();
                continue;
            }


            //�����Ƿ�ʶ��jpg�������������
            std::vector<std::wstring> pixelShaderTextureAllFileNameList;
            pixelShaderTextureAllFileNameList.insert(pixelShaderTextureAllFileNameList.end(), pixelShaderTextureDDSFileNameList.begin(), pixelShaderTextureDDSFileNameList.end());
            pixelShaderTextureAllFileNameList.insert(pixelShaderTextureAllFileNameList.end(), pixelShaderTextureJPGFileNameList.begin(), pixelShaderTextureJPGFileNameList.end());

            //int IndexCallPsSetShaderResourcesTime = FALog.GetIndexDrawCallPsSetShaderResourcesTime(index);
            //LOG.Info(L"����PsSetShaderResources�Ĵ���:" + std::to_wstring(IndexCallPsSetShaderResourcesTime));

            //����ÿ����ͼ���Ʋ���ps-t*����һ��Set��������Set����Ϊ����ͬ����dds��jpg���ͬһ����λ���ƥ������
            std::set<std::wstring> PixelSlotSet;
            for (std::wstring PixelShaderTextureFileName : pixelShaderTextureAllFileNameList) {
                std::wstring PixelSlot = MMTString::GetPixelSlotFromTextureFileName(PixelShaderTextureFileName);
                PixelSlotSet.insert(PixelSlot);
                LOG.Info(L"Detect Slot: " + PixelSlot);
            }
            std::unordered_map<std::string, std::string> PixelSlot_TextureType_Map = textureType.PixelSlot_TextureType_Map;

            //�����λ���������ֱ���˳�����
            if (PixelSlotSet.size() != PixelSlot_TextureType_Map.size()) {
                LOG.Info("All slot matched but total slot number not equal, skip this.");
                LOG.NewLine();
                continue;
            }
            else {
                //�����ȵĻ��������ǰ�Ĳ�λ��Ϣ������DEBUG
                std::wstring slotPairStr;
                for (const auto& slotPair : PixelSlot_TextureType_Map) {
                    slotPairStr = slotPairStr + MMTString::ToWideString(slotPair.first) + L" ";
                }
                LOG.Info(L"Current Slots:" + slotPairStr);
            }

            /*if (!textureType.TimeInPsSetShaderResourceTimeList(IndexCallPsSetShaderResourcesTime)) {
                LOG.Info(L"����PsSetShaderResources�Ĵ����͵�ǰTextureType���õĿ����е�PsSetShaderResourceTime�������������");
                LOG.NewLine();
                continue;
            }*/

            //���һ���Ҫ�жϵ�ǰ��Slot�Ƿ���TextureType��Ԥ���г��ֹ���ֻҪ��һ��û���ֶ����ϸ�
            bool allMatch = true;
            for (std::wstring PixelSlot : PixelSlotSet) {
                if (!PixelSlot_TextureType_Map.contains(MMTString::ToByteString(PixelSlot))) {
                    LOG.Info(L"Can't match for slot: " + PixelSlot + L"  " + MMTString::ToWideString(PixelSlot_TextureType_Map[MMTString::ToByteString(PixelSlot)]));
                    allMatch = false;
                    break;
                }
                else {
                    LOG.Info(PixelSlot + L" matched!");
                }
            }

            if (!allMatch) {
                LOG.Info("Can't match all slot, skip this.");
                LOG.NewLine();
                continue;

            }
            else {
                LOG.Info("All Matched!");
            }

            //TODO ����Ҫ����һ�£�������и�ʲôDDS��ʽ��ȡ�����ܹ�ֱ�Ӷ�ȡ���жϸ���ͨ���е����ݡ�
            //�Դ�����׼ȷ������ǲ���Diffuse��ͼ��


            //����ж�Diffuse��λ����ͼ�ĸ�ʽ��Deduped���ǲ��Ǹ���ͼ����ָ���ĸ�ʽ
            //���в�λ�жϣ�����Ѱ��DiffuseMap.dds
            //Ѱ��diffuse slot
            std::wstring diffuse_slot = L"";
            for (const auto& slotPair : textureType.PixelSlot_TextureType_Map) {
                std::string slot = slotPair.first;
                std::string textureFileName = slotPair.second;
                if (textureFileName == "DiffuseMap.dds") {
                    diffuse_slot = MMTString::ToWideString(slot);
                }
            }
            //�ȴ�deduped�ҵ���ǰ��λ��Ӧ��ͼƬ���ж��Ƿ����ΪDiffuseMap
            //����ÿ����Ϸ����Ҫ��һ��Ԥ��
            bool findDiffuseMap = false;
            for (std::wstring PixelShaderTextureFileName : pixelShaderTextureAllFileNameList) {
                if (PixelShaderTextureFileName.find(L"-" + diffuse_slot + L"=") != std::wstring::npos) {
                    LOG.Info(L"��ǰƥ���PS��ͼ�ļ���" + PixelShaderTextureFileName);
                    std::wstring textureFileName = FAData.FindDedupedTextureName(G.WorkFolder, PixelShaderTextureFileName);
                    if (textureFileName == L"") {
                        LOG.Info(L"�����޷���Deduped�ļ����ҵ�����ͼ��deduped�ļ��������Դ�Dump log�ж�ȡ");
                        textureFileName = FALog.FindRealDedupedResourceFileNameFromDumpCallLog(index, PixelShaderTextureFileName);
                        LOG.Info(L"��Dump Log�ж�ȡ����deduped�ļ���Ϊ��" + textureFileName);
                    }

                    //����ÿ����Ϸ����׼ȷ�ĸ�ʽ�������ʽ�������޷�ƥ��
                    //����ʹ��find������ends_with���ӿ��ɣ��Ͼ��ر���ҪSRGB�Ļ�����ǿ��ָ��
                    //��������ر���Ҫ����ģ��һ��Ļ���ʹ��Ĭ�ϵ�BC7_UNORMҲ�ܴպϣ�����find��ͨ�ü����Ը��á�


                    //��������DiffuseMap��LightMap����BC7_UNORM�������ƺ��ô�����
                    if (textureFileName.find(L"BC7_UNORM") != std::wstring::npos) {
                        LOG.Info(L"�ļ������ҵ���BC7_UNORM��ƥ��DiffuseMap��λ�ɹ�");
                        findDiffuseMap = true;
                        break;
                    }
                }
            }
            if (!findDiffuseMap) {
                LOG.Warning(L"HI3:����DiffuseMap��λδ�ҵ�BC7_UNORM��ʽdds��ͼ�ļ�������ͼ���Ͳ���ȷ���Զ�������");
                LOG.NewLine();
                continue;
            }




            //��ִ�е�����˵�����е�ǰ��������ƥ��ɹ���
            findMatched = true;
            LOG.NewLine();


            //��Ϊ��������ظ�ƥ�䣬�ڶ��θ��ǵ�һ�ε��������������Ҫ�ж�
            //ֻ�еڶ��εõ����滻�ļ��������ڵ�һ�εõ��Ĳ��ܸ���

            int CurrentValidFileNumber = 0;
            for (std::wstring PixelShaderTextureFileName : pixelShaderTextureAllFileNameList) {
                std::wstring PixelSlot = MMTString::GetPixelSlotFromTextureFileName(PixelShaderTextureFileName);
                std::wstring TextureAliasName = MMTString::ToWideString(textureType.PixelSlot_TextureType_Map[MMTString::ToByteString(PixelSlot)]);
                if (TextureAliasName == L"NicoMico") {
                    continue;
                }
                else {
                    CurrentValidFileNumber++;
                }
            }

            std::wstring PartName = MatchFirstIndex_PartName_Map[MatchFirstIndex];
            if (PartName_LastMatchFileNumber_Map.contains(PartName)) {

               /* int LastValidFileNumber = PartName_LastMatchFileNumber_Map[PartName];
                LOG.Info(L"��ǰ���ǵ��ļ����� " + std::to_wstring(CurrentValidFileNumber)
                    + L"��һ��ʶ��ɹ��������ļ�����:"
                    + std::to_wstring(LastValidFileNumber)
                );

                if (CurrentValidFileNumber <= LastValidFileNumber) {
                    LOG.Info(L"Ϊ�����ظ�����Ե�ǰ���ǵ��ļ����� " + std::to_wstring(CurrentValidFileNumber)
                        + L"�����жϣ�����С�ڵ�����һ��ʶ��ɹ��������ļ�����:"
                        + std::to_wstring(LastValidFileNumber)
                        + L" ���������˴�����"
                    );
                    LOG.NewLine();
                    continue;
                }*/
                LOG.Info(L"����HI3���ڵ�һ��Drawʱ����ͼȫ�����룬����Shader��Draw����֮ǰ��λ�����ݶ����������ύ�����Բ�������ܵ��´�����ظ�ʶ��");
                LOG.NewLine();
                continue;

            }
            else {
                PartName_LastMatchFileNumber_Map[PartName] = CurrentValidFileNumber;
            }

            //�����¸�ֵ֮ǰ��Ҫ�Ѿɵ���շ�ֹӰ�췢��
            PartName_TextureSlotReplace_Map[MMTString::ToByteString(PartName)] = {};
            //����������ʹ�õ�ǰ��TextureType�����ϢΪÿ����λ����ͼ�ƶ������������ˡ�
            for (std::wstring PixelShaderTextureFileName : pixelShaderTextureAllFileNameList) {
                //��ȡ�ļ����е���ͼ��λ
                std::wstring PixelSlot = MMTString::GetPixelSlotFromTextureFileName(PixelShaderTextureFileName);
                //��ȡ����ͼ�ڵ�ǰ��ͼ�����еı���
                std::wstring TextureAliasName = MMTString::ToWideString(textureType.PixelSlot_TextureType_Map[MMTString::ToByteString(PixelSlot)]);
                if (TextureAliasName == L"NicoMico") {
                    continue;
                }
                //��ȡ����ͼ��Hashֵ
                std::wstring Hash = MMTString::GetFileHashFromFileName(PixelShaderTextureFileName);
                //ƴ��Ҫ������ļ��������Դ�ļ���.jpg���׺��Ҫ��Ϊ.jpg
                std::wstring outputTextureName = OutputIB + L"-" + Hash + L"-" + PartName + L"-" + TextureAliasName;
                if (PixelShaderTextureFileName.ends_with(L".jpg")) {
                    outputTextureName = outputTextureName.substr(0, outputTextureName.length() - 4) + L".jpg";
                }

                //��ȡ��ǰ�Ѳ������滻�б�
                TextureSlotReplace slotReplace = this->PartName_TextureSlotReplace_Map[MMTString::ToByteString(PartName)];
                bool findExists = false;

                for (const auto& slotFileNamePair : slotReplace.In_SlotName_ResourceFileName_Map) {
                    LOG.Info(outputTextureName + L" == " + slotFileNamePair.second + L" ?");
                    if (outputTextureName == slotFileNamePair.second) {
                        LOG.Info(L"�ҵ����ڵ��ˣ����������");
                        findExists = true;
                    }
                    else {
                        if (PixelSlot == slotFileNamePair.first && slotFileNamePair.second.ends_with(L".dds") && outputTextureName.ends_with(L".jpg")) {
                            LOG.Warning(L"�Ѿ�����dds��ʽ��ͼ���������˶�Ӧ��ͼ��jpg��ʽ�����������jpg��ʽ��");
                            findExists = true;
                        }
                    }
                }

                //û���ֹ��������
                if (!findExists) {
                    slotReplace.In_SlotName_ResourceFileName_Map[PixelSlot] = outputTextureName;
                    slotReplace.In_SlotName_ResourceName_Map[PixelSlot] = L"Resource_" + outputTextureName;
                    slotReplace.SetSerializedList();
                    LOG.Info(L"��ӣ�" + PixelSlot + L" = " + outputTextureName);
                    PartName_TextureSlotReplace_Map[MMTString::ToByteString(PartName)] = slotReplace;

                    LOG.Info(L"Try Copy Original File: " + G.WorkFolder + PixelShaderTextureFileName);
                    if (!std::filesystem::exists(G.WorkFolder + PixelShaderTextureFileName)) {
                        LOG.Warning(L"Can't find original file: " + G.WorkFolder + PixelShaderTextureFileName);
                    }
                    else {
                        std::filesystem::copy_file(G.WorkFolder + PixelShaderTextureFileName, G.OutputFolder + OutputIB + L"\\" + outputTextureName, std::filesystem::copy_options::overwrite_existing);
                        LOG.Info(L"Copy To: " + outputTextureName);
                    }
                }

            }

            //ÿ��Indexֻ����һ��TextureType���㲢ʶ��ɹ������Ա���break
            LOG.Info(L"��ǰPartName: " +PartName + L" ʶ��ɹ�");
            MatchedFirstIndexList.push_back(MatchFirstIndex);
            LOG.NewLine();
            break;
        }
        LOG.NewLine();
        LOG.NewLine();

        //�������1��MatchFirstIndex�Ļ���ֱ��break������ж���Ļ���������ƥ��
        if (MatchFirstIndex_PartName_Map.size() <= 1 && findMatched) {
            break;
        }

    }

}