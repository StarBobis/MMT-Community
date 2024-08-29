#include "Functions_WW1.1.h"

#include "MMTStringUtils.h"
#include "GlobalConfigs.h"
#include "FrameAnalysisData.h"
#include "FrameAnalysisLog.h"
#include "IndexBufferTxtFile.h"
#include <filesystem>
#include <set>
#include "MMTLogUtils.h"
#include "MMTFileUtils.h"


void Functions_WW11::AutoDetectTextureFiles(std::wstring OutputIB, std::unordered_map<std::wstring, std::wstring> MatchFirstIndex_PartName_Map, bool GPUPreSkinning) {
    LOG.Info(L"��ʼ�����Զ���ͼʶ��");
    FrameAnalysisLog FALog = G.GetFrameAnalysisLog();
    FrameAnalysisData FAData = G.GetFrameAnalysisData();

    //���������ȡ���еĲ�������ͼ��ȡ������͵��
    std::vector<std::wstring> trianglelistIndexList = FAData.ReadTrianglelistIndexList(OutputIB);

    //��������ʵ�ĵ�����ͼ��ֱ�ӵ���PSSetShaderResources�����ö�Ӧ�Ĳ�λ�����һ��DrawCall��û�������˵������������ͼʶ��
    std::vector<std::wstring> TrianglelistIndexList_Filter_PSSetShaderResources;
    for (std::wstring Index : trianglelistIndexList) {
        int Call_PSSetShaderResources_Time = FALog.GetIndexDrawCallPsSetShaderResourcesTime(Index);
        if (Call_PSSetShaderResources_Time != 0) {
            LOG.Info(L"Index With Call PSSetShaderResources: " + Index);
            TrianglelistIndexList_Filter_PSSetShaderResources.push_back(Index);
        }
    }
    LOG.NewLine();

    //����б�������ֹ�ظ�ƥ��
    std::vector<std::wstring> MatchedFirstIndexList;

    //��¼��һ��ƥ�䵽����Ч�ļ������������α���һ����Ч�ļ������������
    int lastMatchNumber = 0;
    
    //����ÿ��Index����Ҫ���д���
    for (std::wstring index : TrianglelistIndexList_Filter_PSSetShaderResources) {
        LOG.Info(L"��ǰʶ��Index: " + index);

        //��ȡMatchFirstIndex
        std::wstring MatchFirstIndex = FAData.GetIBMatchFirstIndexByIndex(G.WorkFolder, index);
        if (MatchFirstIndex == L"") {
            LOG.Info(L"��ǰIndexδ�ҵ�IB�ļ�����������ЧIndex���Զ�������");
            LOG.NewLine();
            continue;
        }

        //�жϵ�ǰMatchFirstIndex�Ƿ��Ѿ��������
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
            //����ÿ����ͼ���Ʋ���ps-t*����һ��Set��������Set����Ϊ����ͬ����dds��jpg���ͬһ����λ���ƥ������
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
                std::wstring AliasFileName = MMTString::ToWideString(PixelSlot_TextureType_Map[MMTString::ToByteString(PixelSlot)]);
                if (!PixelSlot_TextureType_Map.contains(MMTString::ToByteString(PixelSlot)) ) {
                    LOG.Info(L"Can't match for slot: " + PixelSlot + L"  " + AliasFileName);
                    allMatch = false;
                    break;
                }
                else {
                    LOG.Info(PixelSlot + L" matched! " + AliasFileName);
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


            //������DrawCall����PsSetShaderResources�Ĵ���������ڵ�������ʵ�����������dds��β�Ĳ�λ����
            uint32_t Call_PSSetShaderResources_Time = FALog.GetIndexDrawCallPsSetShaderResourcesTime(index);
            if (Call_PSSetShaderResources_Time < textureType.GetMeaningfulDdsFileCount()) {
                LOG.Info(L"��ǰDrawCall����PSSetShaderResources������С�ڴ���ͼ���͵���Ч��ͼ�������������");
                LOG.NewLine();
                continue;
            }

            //����ж�Diffuse��λ����ͼ�ĸ�ʽ��Deduped���ǲ��Ǹ���ͼ����ָ���ĸ�ʽ
            bool findDiffuseMap = false;
            for (std::wstring PixelShaderTextureFileName : pixelShaderTextureAllFileNameList) {
                if (PixelShaderTextureFileName.find(MMTString::ToWideString(textureType.GetDiffuseMapSlot())) != std::wstring::npos) {
                    std::wstring textureFileName = FAData.FindDedupedTextureName(G.WorkFolder, PixelShaderTextureFileName);
                    //����ÿ����Ϸ����׼ȷ�ĸ�ʽ�������ʽ�������޷�ƥ��
                    //����ʹ��find������ends_with���ӿ��ɣ��Ͼ��ر���ҪSRGB�Ļ�����ǿ��ָ��
                    //��������ر���Ҫ����ģ��һ��Ļ���ʹ��Ĭ�ϵ�BC7_UNORMҲ�ܴպϣ�����find��ͨ�ü����Ը��á�
                    if (textureFileName.find(L"BC7_UNORM_SRGB") != std::wstring::npos) {
                        int DiffuseSize = MMTFile::GetFileSize(G.WorkFolder + PixelShaderTextureFileName);
                        //������Ҫ�����ж�DiffuseMap�Ĵ�С��
                        if (DiffuseSize >= 4194452) {
                            findDiffuseMap = true;
                            LOG.Info(L"Diffuse Size: 4194452");
                            break;
                        }
                        else {
                            if (DiffuseSize == 262292 && PartNameCount >= 6) {
                                //262 292
                                findDiffuseMap = true;
                                LOG.Info(L"Diffuse Size: 262292  PartName >= 6");
                                break;
                                //����ᵼ����ͼ��λ����ShaderUsage���ȡwidth��heightΪ512��512Ҳ�޷�׼ȷʶ��
                                //ֻ����ʱ�жϵ�PartName >= 6ʱ������������ƥ�䣬��Ϊ�����������ǹ̶���
                            }
                            else {
                                LOG.Warning(L"�Ȳ����ڵ���4,194,452Ҳ������С�汾��ͼ");
                            }

                        }

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
                //GI���������������Drawһ��ֻ����DiffuseMap��LightMap��
                //�����Draw�ŵ��ô���NormalMap�ģ�����ʶ��ʱ��ʶ���λ�ٵĻᵼ�´���ʶ��

                //�ж�ʶ�𵽵���ͼ�����������֮ǰ���е���������������滻������ֱ��continue
                if (pixelShaderTextureAllFileNameList.size() < lastMatchNumber) {
                    LOG.Info(L"��ǰmatch_first_index��ʶ����ͼ���ͣ��ҵ�ǰʶ�����ͼ���Ͳ�λ��������֮ǰʶ�𵽵Ķ࣬��������������");
                    continue;
                }
                else {
                    LOG.Info(L"�����ǲ���");
                    PartName_TextureSlotReplace_Map[MMTString::ToByteString(PartName)] = {};
                }
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
                }

                //û���ֹ��������
                if (!findExists) {
                    slotReplace.In_SlotName_ResourceFileName_Map[PixelSlot] = outputTextureName;
                    slotReplace.In_SlotName_ResourceName_Map[PixelSlot] = L"Resource_" + outputTextureName;
                    slotReplace.SetSerializedList();
                    PartName_TextureSlotReplace_Map[MMTString::ToByteString(PartName)] = slotReplace;
                }

                LOG.Info(L"Try Copy Original File: " + G.WorkFolder + PixelShaderTextureFileName);
                if (!std::filesystem::exists(G.WorkFolder + PixelShaderTextureFileName)) {
                    LOG.Warning(L"Can't find original file: " + G.WorkFolder + PixelShaderTextureFileName);
                }
                std::filesystem::copy_file(G.WorkFolder + PixelShaderTextureFileName, G.OutputFolder + OutputIB + L"\\" + outputTextureName, std::filesystem::copy_options::overwrite_existing);
                LOG.Info(L"Copy To: " + outputTextureName);
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

    LOG.NewSeperator();
}

