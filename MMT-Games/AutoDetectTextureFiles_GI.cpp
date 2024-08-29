#include "Functions_GI.h"

#include "MMTLogUtils.h"
#include "MMTStringUtils.h"
#include "GlobalConfigs.h"
#include "FrameAnalysisData.h"
#include "FrameAnalysisLog.h"
#include "IndexBufferTxtFile.h"
#include <filesystem>
#include <set>
#include "MMTFileUtils.h"


void Functions_GI::AutoDetectTextureFiles(std::wstring OutputIB, std::unordered_map<std::wstring, std::wstring> MatchFirstIndex_PartName_Map,bool GPUPreSkinning) {
	LOG.Info(L"��ʼGI��ͼ�Զ�����ʶ��:");
    FrameAnalysisLog FALog = G.GetFrameAnalysisLog();
    FrameAnalysisData FAData = G.GetFrameAnalysisData();

    //����б�������ֹ�ظ�ƥ��
    std::vector<std::wstring> MatchedFirstIndexList;

    //����Ϊ�˼���������ȡ������ʹ�ô�Log���ȡ����ʵ����Index�б�
    std::vector<std::wstring> trianglelistIndexList = FAData.ReadRealTrianglelistIndexListFromLog(OutputIB);

    //��Ϊԭ��տ�ʼDraw�����ݿ϶��������⣬����000020��Index���ܶ���Ӱ�ӵ�Shader���Կ���û��NormalMap
    //�����������������⴦���Ȱ�000020���ϵķ�ǰ��ƥ�䣬���µķź���ƥ��
    std::vector<std::wstring> ReorderedTrianglelistIndexList;
    for (std::wstring Index: trianglelistIndexList) {
        if (std::stoi(Index) >= 20) {
            ReorderedTrianglelistIndexList.push_back(Index);
        }
    }
    for (std::wstring Index : trianglelistIndexList) {
        if (std::stoi(Index) < 20) {
            ReorderedTrianglelistIndexList.push_back(Index);
        }
    }

    //��һ����¼��һ��ƥ��ɹ����ļ�������map
    std::unordered_map<std::wstring,int> PartName_LastMatchFileNumber_Map;


    int lastMatchNumber = 0;
    for (std::wstring index : ReorderedTrianglelistIndexList) {
        LOG.Info(L"��ǰʶ��Index: " + index);

        //2.�жϵ�ǰIndex�Ƿ���IB�ļ���
        std::wstring MatchFirstIndex = FAData.GetIBMatchFirstIndexByIndex(G.WorkFolder, index);
        if (MatchFirstIndex == L"") {
            LOG.Info(L"��ǰIndexδ�ҵ�IB�ļ�����������ЧIndex���Զ�������");
            LOG.NewLine();
            continue;
        }

        LOG.Info(L"��ǰ��ӦPartName: " + MatchFirstIndex_PartName_Map[MatchFirstIndex]);
        

        //3.�жϵ�ǰMatchFirstIndex�Ƿ��Ѿ��������
        bool findExistsFirstIndex = false;
        for (std::wstring match_firstIndex : MatchedFirstIndexList) {
            if (MatchFirstIndex == match_firstIndex) {
                findExistsFirstIndex = true;
            }
        }

        //����ÿ����ͼ���Ʋ���ps-t*����һ��Set��������Set����Ϊ����ͬ����dds��jpg���ͬһ����λ���ƥ������
         //�����Ƿ�ʶ��jpg�������������
        std::vector<std::wstring> pixelShaderTextureAllFileNameList = MMTFile::FindTextureFileList(G.WorkFolder, index + L"-ps-t");
        std::set<std::wstring> PixelSlotSet;
        for (std::wstring PixelShaderTextureFileName : pixelShaderTextureAllFileNameList) {
            std::wstring PixelSlot = MMTString::GetPixelSlotFromTextureFileName(PixelShaderTextureFileName);
            
            if (std::stoi(PixelSlot.substr(4)) > 20 ) {
                LOG.Warning(L"��ǰSlotֵ���������ЧSlotֵ20����������: " + PixelSlot);
            }
            else {
                PixelSlotSet.insert(PixelSlot);
                LOG.Info(L"Detect Slot: " + PixelSlot);
            }
        }

        //�������е�TextureType
        bool findMatched = false;
        for (TextureType textureType : this->TextureTypeList) {
            //���ݵ�ǰ���������Ƿ�ΪGPUPreSkinning�����й���
            if (textureType.GPUPreSkinning != GPUPreSkinning) {
                LOG.Info(L"��ǰʶ����������͵�GPUPreSkinning�趨�뵱ǰ��ͼ�����趨����������ʶ��.");
                LOG.NewLine();
                continue;
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
                if (PixelShaderTextureFileName.find(diffuse_slot) != std::wstring::npos) {
                    std::wstring textureFileName = FAData.FindDedupedTextureName(G.WorkFolder, PixelShaderTextureFileName);
                    if (textureFileName == L"") {
                        LOG.Info(L"�����޷���Deduped�ļ����ҵ�����ͼ��deduped�ļ��������Դ�Dump log�ж�ȡ");
                        textureFileName = FALog.FindRealDedupedResourceFileNameFromDumpCallLog(index, PixelShaderTextureFileName);
                        LOG.Info(L"��Dump Log�ж�ȡ����deduped�ļ���Ϊ��" + textureFileName);
                    }

                    //����ÿ����Ϸ����׼ȷ�ĸ�ʽ�������ʽ�������޷�ƥ��
                    //����ʹ��find������ends_with���ӿ��ɣ��Ͼ��ر���ҪSRGB�Ļ�����ǿ��ָ��
                    //��������ر���Ҫ����ģ��һ��Ļ���ʹ��Ĭ�ϵ�BC7_UNORMҲ�ܴպϣ�����find��ͨ�ü����Ը��á�
                    if (textureFileName.find(L"BC7_UNORM_SRGB") != std::wstring::npos) {
                        findDiffuseMap = true;
                        break;
                    }
                }
            }
            if (!findDiffuseMap) {
                LOG.Warning(L"����DiffuseMap��λδ�ҵ�BC7_UNORM_SRGB��ʽdds��ͼ�ļ�������ͼ���Ͳ���ȷ���Զ�������");
                LOG.NewLine();
                continue;
            }


            LOG.Info(L"�ܲ�λ������" + std::to_wstring(pixelShaderTextureAllFileNameList.size()) + L" ֮ǰ���ɵ���ͼ�滻��λ������" + std::to_wstring(lastMatchNumber));
            std::wstring PartName = MatchFirstIndex_PartName_Map[MatchFirstIndex];
            if (findExistsFirstIndex) {
                //GI���������������Drawһ��ֻ����DiffuseMap��LightMap��
                //�����Draw�ŵ��ô���NormalMap�ģ�����ʶ��ʱ��ʶ���λ�ٵĻᵼ�´���ʶ��

                //�ж�ʶ�𵽵���ͼ�����������֮ǰ���е���������������滻������ֱ��continue
                if (pixelShaderTextureAllFileNameList.size() < lastMatchNumber) {
                    LOG.Info(L"��ǰmatch_first_index��ʶ����ͼ���ͣ��ҵ�ǰʶ�����ͼ���Ͳ�λ��������֮ǰʶ�𵽵Ķ࣬��������������");
                    continue;
                }
                else {
                    LOG.Info(L"�����ǲ���");
                    
                }
            }
            lastMatchNumber = (int)pixelShaderTextureAllFileNameList.size();

            
            //ԭ����е���ͼ��С��⣬ʵ��û�취�ˣ�ֻ��һ��������!
            //ֻ��ʹ��GPU-PreSkinningʱ����
            int TextureStandardSize = 1048724;
            if (!GPUPreSkinning) {
                TextureStandardSize = 262292;
            }
            bool AllTextureSizeValid = true;
            for (std::wstring PixelShaderTextureFileName : pixelShaderTextureAllFileNameList) {
                std::wstring PixelSlot = MMTString::GetPixelSlotFromTextureFileName(PixelShaderTextureFileName);
                std::wstring TextureAliasName = MMTString::ToWideString(textureType.PixelSlot_TextureType_Map[MMTString::ToByteString(PixelSlot)]);

                //1048724��С�жϣ�ԭ���Dump��������ͼ������������ô��Ĳ��С�
                int filesize = MMTFile::GetFileSize(G.WorkFolder + PixelShaderTextureFileName);
                if (TextureAliasName == L"DiffuseMap.dds") {
                    if (filesize < TextureStandardSize) {
                        AllTextureSizeValid = false;
                    }
                }
                else if (TextureAliasName == L"NormalMap.dds") {
                    if (filesize < TextureStandardSize) {
                        AllTextureSizeValid = false;
                    }
                }
                else if (TextureAliasName == L"LightMap.dds") {
                    if (filesize < TextureStandardSize) {
                        AllTextureSizeValid = false;
                    }
                }
                else if (TextureAliasName == L"HighLightMap.dds") {
                    if (filesize < TextureStandardSize) {
                        AllTextureSizeValid = false;
                    }
                }

            }

            if (!AllTextureSizeValid) {
                LOG.Info(L"����������ͼ�Ĵ�С������ʶ��Ҫ����������");
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

            if (PartName_LastMatchFileNumber_Map.contains(PartName)) {
                
                int LastValidFileNumber = PartName_LastMatchFileNumber_Map[PartName];
                LOG.Info(L"��ǰ���ǵ��ļ����� " + std::to_wstring(CurrentValidFileNumber)
                    + L"��һ��ʶ��ɹ��������ļ�����:"
                    + std::to_wstring(LastValidFileNumber)
                );

                if (CurrentValidFileNumber < LastValidFileNumber) {
                    LOG.Info(L"Ϊ�����ظ�����Ե�ǰ���ǵ��ļ����� " + std::to_wstring(CurrentValidFileNumber)
                        + L"�����жϣ�����С����һ��ʶ��ɹ��������ļ�����:"
                        + std::to_wstring(LastValidFileNumber)
                        + L" ���������˴�����"
                    );
                    LOG.NewLine();
                    continue;
                }
            }
            else {
                PartName_LastMatchFileNumber_Map[PartName] = CurrentValidFileNumber;
            }
            
            //�����¸�ֵ֮ǰ��Ҫ�Ѿɵ���շ�ֹӰ�췢��
            PartName_TextureSlotReplace_Map[MMTString::ToByteString(PartName)] = {};
            //����������ʹ�õ�ǰ��TextureType�����ϢΪÿ����λ����ͼ�ƶ������������ˡ�
            for (std::wstring PixelShaderTextureFileName : pixelShaderTextureAllFileNameList) {
                std::wstring PixelSlot = MMTString::GetPixelSlotFromTextureFileName(PixelShaderTextureFileName);
                std::wstring TextureAliasName = MMTString::ToWideString(textureType.PixelSlot_TextureType_Map[MMTString::ToByteString(PixelSlot)]);
                if (TextureAliasName == L"NicoMico") {
                    continue;
                }
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
                    if (outputTextureName == slotFileNamePair.second) {
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
                    std::filesystem::copy_file(G.WorkFolder + PixelShaderTextureFileName, G.OutputFolder + OutputIB + L"\\" + outputTextureName, std::filesystem::copy_options::overwrite_existing);
                    LOG.Info(L"Copy To: " + outputTextureName);
                }

               
            }

            //ÿ��Indexֻ����һ��TextureType���㲢ʶ��ɹ������Ա���break
            LOG.Info(L"ʶ��ɹ�");
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