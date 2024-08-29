#include "Functions_HSR.h"

#include "MMTStringUtils.h"
#include "GlobalConfigs.h"
#include "FrameAnalysisData.h"
#include "FrameAnalysisLog.h"
#include "IndexBufferTxtFile.h"
#include <filesystem>
#include <set>
#include "MMTLogUtils.h"
#include "MMTFileUtils.h"


void Functions_HSR::AutoDetectTextureFiles(std::wstring OutputIB, std::unordered_map<std::wstring, std::wstring> MatchFirstIndex_PartName_Map, bool GPUPreSkinning) {
    LOG.Info(L"��ʼ�����Զ���ͼʶ���㷨��");
    FrameAnalysisLog FALog = G.GetFrameAnalysisLog();
    FrameAnalysisData FAData = G.GetFrameAnalysisData();

    //����б�������ֹ�ظ�ƥ��
    std::vector<std::wstring> MatchedFirstIndexList;

    std::vector<std::wstring> trianglelistIndexList = FAData.ReadRealTrianglelistIndexListFromLog(OutputIB);

    //���ڱ���ǰ������Ⱦ����Ⱦ����ģ�����Ҫ����ƥ��
    // TODO ���Ǻ�ѻ��˿���ʸ���ͼֻ��ǰ������Ⱦ��������̫���ˣ��о����ȵķ������ǻ���Shader��Hashֵ����
    // �ܷ�֪��ÿ����ͬ����Shader����ʵ��Hashֵ����Ϊ���ںܶ�Shader��Hashֵ��ͬ�������õ��Ĳ�λ����ͬ�ġ�
    // TODO �Ⱥ���ʵ���Ƕ�ʶ�𲻶���ʱ���ٻ���Shaderʶ��ɡ�
     // ��ת vector
    std::reverse(trianglelistIndexList.begin(), trianglelistIndexList.end());


    int lastMatchNumber = 0;
    for (std::wstring index : trianglelistIndexList) {
        LOG.Info(L"��ǰʶ��Index: " + index);

        //2.�жϵ�ǰIndex�Ƿ���IB�ļ���
        std::wstring MatchFirstIndex = FAData.GetIBMatchFirstIndexByIndex(G.WorkFolder, index);
        if (MatchFirstIndex == L"") {
            LOG.Info(L"��ǰIndexδ�ҵ�IB�ļ�����������ЧIndex���Զ�������");
            LOG.NewLine();
            continue;
        }

        //3.�жϵ�ǰMatchFirstIndex�Ƿ��Ѿ��������
        bool findExistsFirstIndex = false;
        for (std::wstring match_firstIndex : MatchedFirstIndexList) {
            if (MatchFirstIndex == match_firstIndex) {
                findExistsFirstIndex = true;
            }
        }

        int PartNameCount = std::stoi(MatchFirstIndex_PartName_Map[MatchFirstIndex]);
        LOG.Info(L"��ǰMatch_First_index: " + MatchFirstIndex);
        LOG.Info(L"��ǰPartName: " + std::to_wstring(PartNameCount));

        std::vector<std::wstring> pixelShaderTextureAllFileNameList = MMTFile::FindTextureFileList(G.WorkFolder, index + L"-ps-t");
        std::set<std::wstring> PixelSlotSet;
        for (std::wstring PixelShaderTextureFileName : pixelShaderTextureAllFileNameList) {
            std::wstring PixelSlot = MMTString::GetPixelSlotFromTextureFileName(PixelShaderTextureFileName);
            PixelSlotSet.insert(PixelSlot);
            LOG.Info(L"Detect Slot: " + PixelSlot);
        }

        //�������е�TextureType
        bool findMatched = false;
        for (TextureType textureType : this->TextureTypeList) {
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


            //���ݲ�λ��Ӧ���ļ���С������
            bool AllSizeMatch = true;
            for (std::wstring PixelShaderTextureFileName : pixelShaderTextureAllFileNameList) {
                std::wstring PixelSlot = MMTString::GetPixelSlotFromTextureFileName(PixelShaderTextureFileName);
                std::string AliasName = textureType.PixelSlot_TextureType_Map[MMTString::ToByteString(PixelSlot)];

                if (textureType.AliasName_FileSizeList_Map.contains(AliasName)) {
                    std::vector<uint32_t> StandardSizeList = textureType.AliasName_FileSizeList_Map[AliasName];
                    uint64_t FileSize = MMTFile::GetFileSize(G.WorkFolder + PixelShaderTextureFileName);
                    bool findValidSize = false;

                    for (uint32_t StandardSize: StandardSizeList) {
                        if (StandardSize == FileSize) {
                            findValidSize = true;
                            break;
                        }
                    }

                    if (!findValidSize) {
                        AllSizeMatch = false;
                        LOG.Info(L"��ǰʶ��" + MMTString::ToWideString(AliasName) + L"  ԭ�ļ�����" + PixelShaderTextureFileName);
                        LOG.Info(L"  ��ǰ�ļ���С��" + std::to_wstring(FileSize));
                        LOG.Info(MMTString::ToWideString(AliasName) + L" ��ͼ��С�޷�ƥ�䡣");
                    }
                    
                }
            }
            if (!AllSizeMatch) {
                LOG.Warning(L"��ͼ�ļ���Сƥ���޷�����Ҫ����������");
                LOG.NewLine();
                continue;
            }


            //�������������ͼ��Hashֵ�������ظ�
            std::unordered_map<std::wstring, std::wstring> HashValue_PixelSlot_Map;
            bool RepeatHash = false;
            for (std::wstring PixelShaderTextureFileName : pixelShaderTextureAllFileNameList) {
                std::wstring PixelSlot = MMTString::GetPixelSlotFromTextureFileName(PixelShaderTextureFileName);
                std::wstring HashValue = MMTString::GetFileHashFromFileName(PixelShaderTextureFileName);
                std::string AliasName = textureType.PixelSlot_TextureType_Map[MMTString::ToByteString(PixelSlot)];
                if (AliasName != "NicoMico") {
                    if (HashValue_PixelSlot_Map.contains(HashValue)) {
                        LOG.Info(L"PixelSlot:" + PixelSlot + L" HashValue:" + HashValue);
                        LOG.Info(L"Repeat Hash: " + HashValue);
                        RepeatHash = true;
                        break;
                    }
                    else {
                        HashValue_PixelSlot_Map[HashValue] = PixelSlot;
                    }
                }
                
            }
            if (RepeatHash) {
                LOG.Warning(L"ͬһ����Ч��ͼ�����ڶ����λ�ϣ���Draw Index����������");
                LOG.NewLine();
                continue;
            }


            //TODO ���ﷲ��dds�Ĳ���ΪGPU-PreSkinning�ģ���Ҫʶ����ͼ
            //�ȴ�deduped�ҵ���ǰ��λ��Ӧ��ͼƬ���ж��Ƿ����ΪDiffuseMap
            bool findDiffuseMap = false;
            for (std::wstring PixelShaderTextureFileName : pixelShaderTextureAllFileNameList) {
                if (PixelShaderTextureFileName.find(MMTString::ToWideString(textureType.GetDiffuseMapSlot())) != std::wstring::npos) {
                    std::wstring textureFileName = FAData.FindDedupedTextureName(G.WorkFolder, PixelShaderTextureFileName);
                    //����ÿ����Ϸ����׼ȷ�ĸ�ʽ�������ʽ�������޷�ƥ��
                    //����ʹ��find������ends_with���ӿ��ɣ��Ͼ��ر���ҪSRGB�Ļ�����ǿ��ָ��
                    //��������ر���Ҫ����ģ��һ��Ļ���ʹ��Ĭ�ϵ�BC7_UNORMҲ�ܴպϣ�����find��ͨ�ü����Ը��á�
                    if (textureFileName.find(L"BC7_UNORM") != std::wstring::npos) {
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


            //��ִ�е�����˵���ܲ�λ��Diffuse�ĸ�ʽ��ƥ��ɹ���
            findMatched = true;
            LOG.NewLine();


            LOG.Info(L"�ܲ�λ������" + std::to_wstring(pixelShaderTextureAllFileNameList.size()) + L" ֮ǰ���ɵ���ͼ�滻��λ������" + std::to_wstring(lastMatchNumber));
            std::wstring PartName = MatchFirstIndex_PartName_Map[MatchFirstIndex];
            if (findExistsFirstIndex) {
                LOG.Info(L"�������ڵ�һ��Drawʱʹ��ȫ�����ͼ���������ظ�ʶ����������");
                continue;
            }
            lastMatchNumber = (int)pixelShaderTextureAllFileNameList.size();


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
                TextureSlotReplace slotReplace = PartName_TextureSlotReplace_Map[MMTString::ToByteString(PartName)];
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



