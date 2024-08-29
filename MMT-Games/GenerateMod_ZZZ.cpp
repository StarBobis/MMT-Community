#include "Functions_ZZZ.h"

#include "GlobalConfigs.h"
#include "D3d11GameType.h"
#include "VertexBufferBufFile.h"
#include "MMTConstants.h"

#include "BufferUtils.h"
#include "IniBuilder.h"


void Functions_ZZZ::GenerateMod() {

    //��һ��IniBuilder.AppendLine();
    LOG.Info("Executing: GenerateMod_ZZZ");
    
    IniBuilder ZZZIniBuilder;

    for (const auto& DrawIBPair : G.DrawIB_ExtractConfig_Map) {
        std::wstring drawIB = DrawIBPair.first;
        DrawIBConfig drawIBConfig = DrawIBPair.second;
        drawIBConfig.GameType = MMTString::ToWideString(drawIBConfig.WorkGameType);
        D3D11GameType d3d11GameType = GameTypeName_D3d11GameType_Map[drawIBConfig.WorkGameType];

        if (!drawIBConfig.Initialize(G.OutputFolder, d3d11GameType)) {
            continue;
        }
        LOG.Info("D3d11GameType: " + d3d11GameType.GameType);


        //��ȡ���ƵĶ�������
        drawIBConfig.DrawNumber = BufferUtils::GetSum_DrawNumber_FromVBFiles(drawIBConfig);

        //��ȡVB�ļ��е�����
        drawIBConfig.FinalVBCategoryDataMap = BufferUtils::Read_FinalVBCategoryDataMap(drawIBConfig);

        //��ֵ����
        if (drawIBConfig.TangentAlgorithm == L"average_normal") {
            BufferUtils::Unity_TANGENT_AverageNormal(drawIBConfig.FinalVBCategoryDataMap, d3d11GameType);
        }
        if (drawIBConfig.ColorAlgorithm == L"average_normal") {
            BufferUtils::Unity_COLOR_AverageNormal(drawIBConfig.FinalVBCategoryDataMap, drawIBConfig, d3d11GameType);
        }
        BufferUtils::Unity_Reverse_NORMAL_TANGENT(drawIBConfig.FinalVBCategoryDataMap, drawIBConfig, d3d11GameType);
        BufferUtils::Unity_Reset_COLOR(drawIBConfig.FinalVBCategoryDataMap, drawIBConfig, d3d11GameType);

        BufferUtils::Read_Convert_Output_IBBufferFiles(drawIBConfig);
        BufferUtils::OutputCategoryBufferFiles(drawIBConfig);

        if (G.ShareSourceModel) {
            BufferUtils::ShareSourceModel(drawIBConfig);
        }

        //��3����������Wheel��ʽ��ini�ļ�
        //------------------------------------------------------------------------------------------------------------------
        //ƴ��Ҫ�������ͼ����Դ��������ʹ��
        if (!drawIBConfig.ForbidAutoTexture) {
            this->PartName_TextureSlotReplace_Map.clear();
            for (const auto& pair : drawIBConfig.PartName_TextureSlotReplace_Map) {
                TextureSlotReplace slotReplace(pair.second);
                this->PartName_TextureSlotReplace_Map[pair.first] = slotReplace;
            }
        }

        LOG.Info(L"Start to output ini file.");

        //1.TextureOverrideVB���֣�ֻ��ʹ��GPU-PreSkinningʱ��ֱ���滻hash��Ӧ��λ
        if (d3d11GameType.GPUPreSkinning) {
            IniSection TextureOverrideVBSection(IniSectionType::TextureOverrideVB);
            LOG.Info(L"Start to output TextureOverrideVB section");
            TextureOverrideVBSection.Append(L"; " + drawIBConfig.DrawIB + L" -------------------------");
            for (std::string categoryName : d3d11GameType.OrderedCategoryNameList) {
                std::string categoryHash = drawIBConfig.CategoryHashMap[categoryName];
                std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
                LOG.Info(L"categoryName: " + MMTString::ToWideString(categoryName) + L" categorySlot: " + MMTString::ToWideString(categorySlot));
                TextureOverrideVBSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + MMTString::ToWideString(categoryName) + L"]");
                TextureOverrideVBSection.Append(L"hash = " + MMTString::ToWideString(categoryHash));

                //������ȡ�����ڵ�ǰ����hash�½����滻�ķ��࣬����Ӷ�Ӧ����Դ�滻
                for (const auto& pair : d3d11GameType.CategoryDrawCategoryMap) {
                    std::string originalCategoryName = pair.first;
                    std::string drawCategoryName = pair.second;
                    if (categoryName == drawCategoryName) {
                        std::string categoryOriginalSlot = d3d11GameType.CategorySlotMap[originalCategoryName];
                        TextureOverrideVBSection.Append(MMTString::ToWideString(categoryOriginalSlot) + L" = " + L"Resource" + drawIBConfig.DrawIB + MMTString::ToWideString(originalCategoryName));
                    }
                }

                //drawһ�㶼����Blend��λ�Ͻ��еģ�������������Ҫ�ж�ȷ����BlendҪ�滻��hash���ܽ���draw��
                if (categoryName == d3d11GameType.CategoryDrawCategoryMap["Blend"]) {
                    TextureOverrideVBSection.Append(L"handling = skip");
                    TextureOverrideVBSection.Append(L"draw = " + std::to_wstring(drawIBConfig.DrawNumber) + L", 0");
                }

                TextureOverrideVBSection.NewLine();
            }

            //2.VertexLimitRaise���֣�����ֻ��ʹ�õ�GPU-PreSkinning����ʱ����Ҫͻ�ƶ�����������
            //������drawNumber��ʵ�ֶ�̬����
            TextureOverrideVBSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + L"_" + std::to_wstring(d3d11GameType.CategoryStrideMap["Position"]) + L"_" + std::to_wstring(drawIBConfig.DrawNumber) + L"_VertexLimitRaise]");
            TextureOverrideVBSection.Append(L"hash = " + MMTString::ToWideString(drawIBConfig.VertexLimitVB));
            TextureOverrideVBSection.NewLine();

            ZZZIniBuilder.AppendSection(TextureOverrideVBSection);
        }

        IniSection IBSkipSection(IniSectionType::IBSkip);
        IBSkipSection.Append(L"[TextureOverride" + drawIBConfig.DrawIB + L"IB]");
        IBSkipSection.Append(L"hash = " + drawIBConfig.DrawIB);
        IBSkipSection.Append(L"handling = skip");
        IBSkipSection.NewLine();
        ZZZIniBuilder.AppendSection(IBSkipSection);

        //2.TextureOverrideIB����
        IniSection TextureOverrideIB(IniSectionType::TextureOverrideIB);
        for (int i = 0; i < drawIBConfig.PartNameList.size(); ++i) {
            std::string IBFirstIndex = drawIBConfig.MatchFirstIndexList[i];
            std::string partName = drawIBConfig.PartNameList[i];
            std::wstring IBResourceName = L"Resource" + drawIBConfig.DrawIB + G.GIMIPartNameAliasMap[MMTString::ToWideString(partName)];
            TextureOverrideIB.Append(L"[TextureOverride" + drawIBConfig.DrawIB + G.GIMIPartNameAliasMap[MMTString::ToWideString(partName)] + L"]");
            TextureOverrideIB.Append(L"hash = " + drawIBConfig.DrawIB);
            TextureOverrideIB.Append(L"match_first_index = " + MMTString::ToWideString(IBFirstIndex));

            //��ͼ��λcheck���õ�������check����
            TextureOverrideIB.Append(L";Add slot check here to compatible with ZZMI if you manually add more ps slot replace for this IB's match_firt_index.");
            std::vector<std::wstring> slotReplaceStrList = this->PartName_TextureSlotReplace_Map[partName].Out_SlotReplaceStrList;
            if (!drawIBConfig.ForbidAutoTexture ) {
                for (std::wstring slotReplaceStr : slotReplaceStrList) {
                    std::vector <std::wstring> splitList = MMTString::SplitString(slotReplaceStr, L"=");
                    TextureOverrideIB.Append(L"checktextureoverride = " + splitList[0]);
                }
            }

            //�滻IB��λ
            TextureOverrideIB.Append(L"ib = " + IBResourceName);

            //��ͼ��Դ�滻
            if (!drawIBConfig.ForbidAutoTexture && !drawIBConfig.UseHashTexture) {
                for (std::wstring slotReplaceStr : slotReplaceStrList) {
                    TextureOverrideIB.Append(slotReplaceStr);
                }
            }

            //�����ʹ��GPU-Skinning��ΪObject���ͣ���ʱ��Ҫ��ib�����滻��Ӧ��λ
            if (!d3d11GameType.GPUPreSkinning) {
                for (std::string categoryName : d3d11GameType.OrderedCategoryNameList) {
                    std::string categoryHash = drawIBConfig.CategoryHashMap[categoryName];
                    std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
                    LOG.Info(L"categoryName: " + MMTString::ToWideString(categoryName) + L" categorySlot: " + MMTString::ToWideString(categorySlot));

                    //������ȡ�����ڵ�ǰ����hash�½����滻�ķ��࣬����Ӷ�Ӧ����Դ�滻
                    for (const auto& pair : d3d11GameType.CategoryDrawCategoryMap) {
                        std::string originalCategoryName = pair.first;
                        std::string drawCategoryName = pair.second;
                        if (categoryName == drawCategoryName) {
                            std::string categoryOriginalSlot = d3d11GameType.CategorySlotMap[originalCategoryName];
                            TextureOverrideIB.Append(MMTString::ToWideString(categoryOriginalSlot) + L" = " + L"Resource" + drawIBConfig.DrawIB + MMTString::ToWideString(originalCategoryName));
                        }
                    }
                }
            }
            //Drawͼ��
            TextureOverrideIB.Append(L"drawindexed = auto");
            TextureOverrideIB.NewLine();
        }
        ZZZIniBuilder.AppendSection(TextureOverrideIB);


        //ResourceVB
        IniSection ResourceVBSection(IniSectionType::ResourceVB);
        for (std::string categoryName : d3d11GameType.OrderedCategoryNameList) {
            ResourceVBSection.Append(L"[Resource" + drawIBConfig.DrawIB + MMTString::ToWideString(categoryName) + L"]");
            ResourceVBSection.Append(L"type = Buffer");
            
            if (categoryName == "Blend" && d3d11GameType.PatchBLENDWEIGHTS) {
                int finalBlendStride = d3d11GameType.CategoryStrideMap[categoryName] - d3d11GameType.ElementNameD3D11ElementMap["BLENDWEIGHTS"].ByteWidth;
                ResourceVBSection.Append(L"stride = " + std::to_wstring(finalBlendStride));
            }
            else {
                ResourceVBSection.Append(L"stride = " + std::to_wstring(d3d11GameType.CategoryStrideMap[categoryName]));
            }
            //vb�ļ����ļ��� 
            ResourceVBSection.Append(L"filename = Buffer/" + drawIBConfig.DrawIB + MMTString::ToWideString(categoryName) + L".buf");
            ResourceVBSection.NewLine();
        }
        ZZZIniBuilder.AppendSection(ResourceVBSection);


        //ResourceIB
        IniSection ResourceIBSection(IniSectionType::ResourceIB);
        for (int i = 0; i < drawIBConfig.PartNameList.size(); ++i) {
            std::string partName = drawIBConfig.PartNameList[i];
            ResourceIBSection.Append(L"[Resource" + drawIBConfig.DrawIB + G.GIMIPartNameAliasMap[MMTString::ToWideString(partName)] + L"]");
            ResourceIBSection.Append(L"type = Buffer");
            ResourceIBSection.Append(L"format = DXGI_FORMAT_R32_UINT");
            ResourceIBSection.Append(L"filename = Buffer/" + drawIBConfig.DrawIB + G.GIMIPartNameAliasMap[MMTString::ToWideString(partName)] + L".ib");
            ResourceIBSection.NewLine();
        }
        ZZZIniBuilder.AppendSection(ResourceIBSection);

        //7.д����ͼresource����
        if (!drawIBConfig.ForbidAutoTexture && !drawIBConfig.UseHashTexture) {
            IniSection ResourceTextureSection(IniSectionType::ResourceTexture);
            for (const auto& pair : this->PartName_TextureSlotReplace_Map) {
                TextureSlotReplace slotReplace = pair.second;
                for (const auto& slotPair : slotReplace.Out_ResourceName_FileName_Map) {
                    std::wstring ResourceName = slotPair.first;
                    std::wstring ResourceFileName = slotPair.second;
                    ResourceTextureSection.Append(L"[" + ResourceName + L"]");
                    ResourceTextureSection.Append(L"filename = Texture/" + ResourceFileName);
                    ResourceTextureSection.NewLine();
                }
            }
            ZZZIniBuilder.AppendSection(ResourceTextureSection);
        }


        
        LOG.NewLine();

        //�ƶ���λ��DDS�ļ���Mod�ļ�����
        BufferUtils::CopySlotDDSTextureFromOutputFolder(drawIBConfig);

        LOG.Info(L"Generate mod completed!");
        LOG.NewLine();

    }
    LOG.NewLine();

    if (!std::filesystem::exists(G.TimeOutputFolder)) {
        LOG.Error(MMT_Tips::TIP_GenerateMod_NoAnyExportModelDetected);
    }
    else {
        LOG.Info(L"��ʼ���ɻ���Hashֵ��this = ������ͼ�滻,��δ��ѡ�򲻻�����");
        std::unordered_map<std::wstring, std::wstring> Hash_TextureFileNameMap;
        for (const auto& DrawIBPair : G.DrawIB_ExtractConfig_Map) {
            std::wstring drawIB = DrawIBPair.first;
            DrawIBConfig extractConfig = DrawIBPair.second;
            LOG.Info(L"DrawIB: " + drawIB);
            LOG.Info(L"UseHashTexture: " + std::to_wstring(extractConfig.UseHashTexture));
            LOG.Info(L"Allow TextureGenerate: " + std::to_wstring(!extractConfig.ForbidAutoTexture));
            LOG.NewLine();
            if (extractConfig.UseHashTexture && !extractConfig.ForbidAutoTexture) {
                for (const auto& texturePair : extractConfig.PartName_TextureSlotReplace_Map) {
                    std::string PartName = texturePair.first;
                    std::vector<std::string> TextureFileNameList = texturePair.second;
                    for (std::string TextureFileName : TextureFileNameList) {

                        std::vector<std::wstring> TextureFileNameSplitList = MMTString::SplitStringOnlyMatchFirst(MMTString::ToWideString(TextureFileName), L"=");
                        std::wstring RealTextureFileName = boost::algorithm::trim_copy(TextureFileNameSplitList[1]);
                        std::vector<std::wstring> StrSplitList = MMTString::SplitString(RealTextureFileName, L"-");
                        //std::wstring DrawIB = StrSplitList[0];
                        std::wstring TextureHash = StrSplitList[1];
                        Hash_TextureFileNameMap[TextureHash] = RealTextureFileName;
                    }
                }
            }
        }
        LOG.Info(L"Hash_TextureFileNameMap Size: " + std::to_wstring(Hash_TextureFileNameMap.size()));
        LOG.NewLine();


        if (Hash_TextureFileNameMap.size() != 0) {
            std::wstring TextureOutputFolder = G.TimeOutputFolder + L"Texture\\";
            std::filesystem::create_directories(TextureOutputFolder);

            for (const auto& DrawIBPair : G.DrawIB_ExtractConfig_Map) {
                std::wstring drawIB = DrawIBPair.first;
                DrawIBConfig extractConfig = DrawIBPair.second;
                std::wstring BufferReadFolder = G.OutputFolder + drawIB + L"/";
                LOG.Info(L"DrawIB: " + drawIB);

                for (const auto& TexturePair : Hash_TextureFileNameMap) {
                    std::wstring TextureHash = TexturePair.first;
                    std::wstring FileName = TexturePair.second;

                    std::wstring FilePath = BufferReadFolder + FileName;
                    LOG.Info(L"Current Processing: " + FilePath);

                    std::vector<std::wstring> StrSplitList = MMTString::SplitString(FileName, L"-");
                    std::wstring NewFileName = drawIB + L"_" + TextureHash + L"_" + StrSplitList[3];

                    if (std::filesystem::exists(FilePath)) {
                        //��ƴ��д��Resource

                        IniSection ResourceSection(IniSectionType::ResourceTexture);
                        ResourceSection.SectionLineList.push_back(L"[Resource_Texture_" + TextureHash + L"]");
                        ResourceSection.SectionLineList.push_back(L"filename = Texture/" + NewFileName + L"");
                        ResourceSection.NewLine();
                        ZZZIniBuilder.AppendSection(ResourceSection);

                        IniSection TextureOverrideSection(IniSectionType::TextureOverrideTexture);
                        TextureOverrideSection.SectionLineList.push_back(L"[TextureOverride_" + TextureHash + L"]");
                        TextureOverrideSection.SectionLineList.push_back(L"hash = " + TextureHash);
                        TextureOverrideSection.SectionLineList.push_back(L"this = Resource_Texture_" + TextureHash);
                        TextureOverrideSection.NewLine();
                        ZZZIniBuilder.AppendSection(TextureOverrideSection);

                        //��ԭ�����ļ����Ƶ���Ӧ��Hash�ļ�
                        std::filesystem::copy_file(FilePath, TextureOutputFolder + NewFileName, std::filesystem::copy_options::skip_existing);
                    }
                }
                LOG.NewLine();
            }

            LOG.Info(L"����Hash��ͼ�滻�ɹ�");
        }
        LOG.NewLine();
    }

    ZZZIniBuilder.AppendSection(BufferUtils::GetCreditInfoIniSection());

    ZZZIniBuilder.SaveToFile(G.TimeOutputFolder + L"Config.ini");

    BufferUtils::AddCreditREADME();





    LOG.NewLine();
}