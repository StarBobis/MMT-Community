#include "Functions_KBY.h"


#include "IndexBufferBufFile.h"
#include "VertexBufferBufFile.h"
#include "MMTFormatUtils.h"
#include "GlobalConfigs.h"


void Functions_KBY::GenerateMod() {

    for (const auto& pair : G.DrawIB_ExtractConfig_Map) {
        std::wstring DrawIB = pair.first;
        DrawIBConfig extractConfig = pair.second;

        D3D11GameType d3d11GameType = GameTypeName_D3d11GameType_Map[extractConfig.WorkGameType];

        std::wstring timeStr = MMTString::GetFormattedDateTimeForFilename().substr(0, 10);
        std::wstring BufferReadFolder = G.OutputFolder + DrawIB + L"\\";
        std::wstring BufferOutputFolder = G.OutputFolder + timeStr + L"\\" + DrawIB + L"\\";
        std::filesystem::create_directories(BufferOutputFolder);

        bool findValidFile = false;
        for (std::string partName : extractConfig.PartNameList) {
            std::wstring VBFileName = MMTString::ToWideString(partName) + L".vb";
            if (std::filesystem::exists(BufferReadFolder + VBFileName)) {
                findValidFile = true;
                break;
            }
        }

        if (!findValidFile) {
            LOG.Info(L"Detect didn't export vb file for DrawIB: " + DrawIB + L" , so skip this drawIB generate.");
            continue;
        }


        std::unordered_map<std::string, uint32_t> CategoryStrideMap = d3d11GameType.getCategoryStrideMap(extractConfig.TmpElementList);
        //����������Ҫ��Blend�ĳ���+8���16

        if (CategoryStrideMap["Blend"] == 8 && d3d11GameType.UE4PatchNullInBlend) {
            CategoryStrideMap["Blend"] = 16;
        }

        std::vector<std::string> CategoryList = d3d11GameType.getCategoryList(extractConfig.TmpElementList);
        //����鿴ÿ��Category�Ĳ���
        for (const auto& pair : CategoryStrideMap) {
            const std::string& key = pair.first;
            int value = pair.second;
            LOG.Info("Category: " + key + ", CategoryStride: " + std::to_string(value));
        }
        LOG.NewLine();

        //�鿴CategoryList
        LOG.Info(L"CategoryList:");
        for (std::string categoryName : CategoryList) {
            LOG.Info("Ordered CategoryName: " + categoryName);
        }
        LOG.NewLine();


        //------------------------------------------------------------------------------------------------------------------
        //ƴ��Ҫ�������ͼ����Դ��������ʹ�� TODO ���Ҫ�Ż���������
        //std::unordered_map<std::wstring, std::vector<std::wstring>> PartName_SlotReplaceStrList_Map;
        //std::unordered_map<std::wstring, std::wstring> ResourceName_FileName_Map;
        //for (const auto& pair : extractConfig.PartNameTextureResourceReplaceList) {
        //    std::string partName = pair.first;
        //    std::vector<std::string> textureResourceReplaceList = pair.second;
        //    std::vector<std::wstring> slotReplaceStrList;
        //    for (std::string textureResourceReplace : textureResourceReplaceList) {
        //        std::vector<std::wstring> splitsStrList = MMTString_SplitString(MMTString_ToWideString(textureResourceReplace), L"=");
        //        std::wstring ReplaceSlot = boost::algorithm::trim_copy(splitsStrList[0]);
        //        std::wstring ReplaceFileName = boost::algorithm::trim_copy(splitsStrList[1]);
        //        std::wstring ResourceName = L"Resource_" + MMTString_GetFileNameWithOutSuffix(ReplaceFileName);
        //        slotReplaceStrList.push_back(ReplaceSlot + L" = " + ResourceName);
        //        ResourceName_FileName_Map[ResourceName] = ReplaceFileName;

        //    }
        //    PartName_SlotReplaceStrList_Map[MMTString_ToWideString(partName)] = slotReplaceStrList;
        //}




        //UE4��ͨʹ��VB��λ�����ͣ�ÿһ��IB����һ��������Mod
        int SplitStride = d3d11GameType.getElementListStride(extractConfig.TmpElementList);
        LOG.Info(L"SplitStride: " + std::to_wstring(SplitStride));
        std::unordered_map<std::string, int> partNameOffsetMap;
        int drawNumber = 0;

        for (int i = 0; i < extractConfig.PartNameList.size(); i++) {
            std::string partName = extractConfig.PartNameList[i];
            //��ʼ�����·��
            std::wstring splitPartNameOutputFolder = BufferOutputFolder + MMTString::ToWideString(partName) + L"\\";
            std::filesystem::create_directories(splitPartNameOutputFolder);

            //��ȡ���е�dds�ļ��������Ƶ���Ӧ�ļ��У�����ֻ�����ã�������д
            /*std::vector<std::wstring> ddsFileList = MMTFile_FindFileNameListWithCondition(splitReadFolder, L"", L".dds");
            for (std::wstring ddsFileName : ddsFileList) {
                if (ddsFileName.find(L"-" + MMTString_ToWideString(partName) + L"-") != std::wstring::npos) {
                    std::filesystem::copy_file(splitReadFolder + ddsFileName, splitPartNameOutputFolder + ddsFileName, std::filesystem::copy_options::skip_existing);
                }
            }*/

            //(1) ת�������ÿ��IB�ļ�,����Blender������IB���Ǵ�0��ʼ��
            //�����ö�ȡib�ļ���ʹ�õ�Format,��1.fmt�ļ����Զ���ȡ
            std::wstring IBReadDxgiFormat = MMTFile::FindMigotoIniAttributeInFile(BufferReadFolder + L"1.fmt", L"format");
            std::wstring IBFileName = MMTString::ToWideString(partName) + L".ib";
            std::wstring readIBFileName = BufferReadFolder + IBFileName;
            std::wstring writeIBFileName = splitPartNameOutputFolder + IBFileName;
            LOG.Info(L"Converting IB file: " + IBFileName);
            IndexBufferBufFile ibBufFile(readIBFileName, IBReadDxgiFormat);

            std::wstring OutputIBDxgiFormat = MMTString::ToWideString(ibBufFile.GetSelfMinFormat());
            ibBufFile.SaveToFile_MinSize(writeIBFileName, ibBufFile.MinNumber * -1);
            LOG.Info(L"Output ib file over");
            LOG.NewLine();


            //(2) ���BUF�ļ�
            //��ȡvb�ļ���ÿ��vb�ļ�������category�ֿ�װ�ز�ͬcategory������
            std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::vector<std::byte>>> partName_VBCategoryDaytaMap;
            std::wstring VBFileName = MMTString::ToWideString(partName) + L".vb";
            uint64_t VBFileSize = MMTFile::GetFileSize(BufferReadFolder + VBFileName);
            uint64_t vbFileVertexNumber = VBFileSize / SplitStride;

            LOG.Info(L"Processing VB file: " + VBFileName + L" size is: " + std::to_wstring(VBFileSize) + L" byte." + L" vertex number is: " + std::to_wstring(vbFileVertexNumber));
            VertexBufferBufFile vbBufFile(BufferReadFolder + VBFileName, d3d11GameType, extractConfig.TmpElementList);
            partName_VBCategoryDaytaMap[MMTString::ToWideString(partName)] = vbBufFile.CategoryVBDataMap;

            //����offset
            partNameOffsetMap[partName] = drawNumber;
            //��ӵ�drawNumber
            drawNumber = drawNumber + (int)vbFileVertexNumber;

            //ֱ�ӷָ���
            LOG.Info(L"Combine and put partName_VBCategoryDaytaMap's content back to finalVBCategoryDataMap");
            //��partName_VBCategoryDaytaMap������ݣ�����finalVBCategoryDataMap����ϳ�һ����������ʹ��
            std::unordered_map<std::wstring, std::vector<std::byte>> finalVBCategoryDataMap;
            for (std::string partName : extractConfig.PartNameList) {
                std::unordered_map<std::wstring, std::vector<std::byte>> tmpVBCategoryDataMap = partName_VBCategoryDaytaMap[MMTString::ToWideString(partName)];
                for (size_t i = 0; i < CategoryList.size(); ++i) {
                    const std::string& category = CategoryList[i];
                    std::vector<std::byte> tmpCategoryData = tmpVBCategoryDataMap[MMTString::ToWideString(category)];
                    if (category == "Normal") {
                        for (int index = 0; index < tmpCategoryData.size(); index = index + 8) {
                            //1.��ȡNORMAL��TANGENTֵ
                            std::byte NormalValueX = tmpCategoryData[index + 0];
                            std::byte NormalValueY = tmpCategoryData[index + 1];
                            std::byte NormalValueZ = tmpCategoryData[index + 2];
                            std::byte TangentValueX = tmpCategoryData[index + 4];
                            std::byte TangentValueY = tmpCategoryData[index + 5];
                            std::byte TangentValueZ = tmpCategoryData[index + 6];

                            //2.�����۲�NORMAL��ֵΪTANGENTǰ��λֱ�ӷŹ�����NORMAL��W��Ϊ0x7F
                            tmpCategoryData[index + 0] = TangentValueX;
                            tmpCategoryData[index + 1] = TangentValueY;
                            tmpCategoryData[index + 2] = TangentValueZ;
                            tmpCategoryData[index + 3] = std::byte(0x7F);

                            //3.��תNORMAL��ǰ��λ���ŵ�TANGENT��ǰ��λ
                            tmpCategoryData[index + 4] = MMTFormat::ReverseSNORMValueSingle(NormalValueX);
                            tmpCategoryData[index + 5] = MMTFormat::ReverseSNORMValueSingle(NormalValueY);
                            tmpCategoryData[index + 6] = MMTFormat::ReverseSNORMValueSingle(NormalValueZ);
                        }
                        std::vector<std::byte>& finalCategoryData = finalVBCategoryDataMap[MMTString::ToWideString(category)];
                        finalCategoryData.insert(finalCategoryData.end(), tmpCategoryData.begin(), tmpCategoryData.end());
                    }
                    //��ͨUE4��Ҫ����Blend�е�00
                    else if (category == "Blend" && d3d11GameType.UE4PatchNullInBlend) {
                        std::vector<std::byte> tmpCategoryDataNew;
                        for (int index = 0; index < tmpCategoryData.size(); index = index + 8) {
                            //1.��ȡNORMAL��TANGENTֵ
                            std::byte BlendIndicesValueX = tmpCategoryData[index + 0];
                            std::byte BlendIndicesValueY = tmpCategoryData[index + 1];
                            std::byte BlendIndicesValueZ = tmpCategoryData[index + 2];
                            std::byte BlendIndicesValueW = tmpCategoryData[index + 3];
                            std::byte BlendWeightValueX = tmpCategoryData[index + 4];
                            std::byte BlendWeightValueY = tmpCategoryData[index + 5];
                            std::byte BlendWeightValueZ = tmpCategoryData[index + 6];
                            std::byte BlendWeightValueW = tmpCategoryData[index + 7];
                            tmpCategoryDataNew.push_back(BlendIndicesValueX);
                            tmpCategoryDataNew.push_back(BlendIndicesValueY);
                            tmpCategoryDataNew.push_back(BlendIndicesValueZ);
                            tmpCategoryDataNew.push_back(BlendIndicesValueW);

                            tmpCategoryDataNew.push_back(std::byte(0x00));
                            tmpCategoryDataNew.push_back(std::byte(0x00));
                            tmpCategoryDataNew.push_back(std::byte(0x00));
                            tmpCategoryDataNew.push_back(std::byte(0x00));


                            tmpCategoryDataNew.push_back(BlendWeightValueX);
                            tmpCategoryDataNew.push_back(BlendWeightValueY);
                            tmpCategoryDataNew.push_back(BlendWeightValueZ);
                            tmpCategoryDataNew.push_back(BlendWeightValueW);


                            tmpCategoryDataNew.push_back(std::byte(0x00));
                            tmpCategoryDataNew.push_back(std::byte(0x00));
                            tmpCategoryDataNew.push_back(std::byte(0x00));
                            tmpCategoryDataNew.push_back(std::byte(0x00));

                        }

                        std::vector<std::byte>& finalCategoryData = finalVBCategoryDataMap[MMTString::ToWideString(category)];
                        finalCategoryData.insert(finalCategoryData.end(), tmpCategoryDataNew.begin(), tmpCategoryDataNew.end());
                    }
                    else {
                        std::vector<std::byte>& finalCategoryData = finalVBCategoryDataMap[MMTString::ToWideString(category)];
                        finalCategoryData.insert(finalCategoryData.end(), tmpCategoryData.begin(), tmpCategoryData.end());
                    }
                }
            }
            LOG.NewLine();

            //ֱ�����
            for (const auto& pair : finalVBCategoryDataMap) {
                const std::wstring& categoryName = pair.first;
                const std::vector<std::byte>& categoryData = pair.second;
                LOG.Info(L"Output buf file, current category: " + categoryName + L" Length:" + std::to_wstring(categoryData.size() / drawNumber));
                //���û���ǾͲ����
                if (categoryData.size() == 0) {
                    LOG.Info(L"Current category's size is 0, can't output, skip this.");
                    continue;
                }
                std::wstring categoryGeneratedName = DrawIB + categoryName;
                // ��������ļ�·��
                std::wstring outputDatFilePath = splitPartNameOutputFolder + categoryGeneratedName + L".buf";
                // ������ļ� ��std::vecto������д���ļ�
                std::ofstream outputFile(MMTString::ToByteString(outputDatFilePath), std::ios::binary);
                outputFile.write(reinterpret_cast<const char*>(categoryData.data()), categoryData.size());
                outputFile.close();
                LOG.Info(L"Write " + categoryName + L" data into file: " + outputDatFilePath);
            }
            LOG.NewLine();


            //(3) ����ini�ļ�
            std::wstring outputIniFileName = splitPartNameOutputFolder + extractConfig.DrawIB + L".ini";
            std::wofstream outputIniFile(outputIniFileName);



            outputIniFile << L"; -------------- Resource VB -----------------" << L"\n" << L"\n";
            //ResourceVB
            for (std::string categoryName : CategoryList) {
                std::wstring fileName = extractConfig.DrawIB + MMTString::ToWideString(categoryName) + L".buf";
                std::wstring filePath = splitPartNameOutputFolder + fileName;
                int fileSize = MMTFile::GetFileSize(filePath);
                std::string categoryHash = extractConfig.CategoryHashMap[categoryName];
                std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];


                outputIniFile << L"[Resource_VB_" + MMTString::ToWideString(categoryName) + L"]" << L"\n";
                outputIniFile << L"byte_width = " << std::to_wstring(fileSize) << L"\n";
                outputIniFile << L"type = Buffer" << L"\n";
                outputIniFile << "stride = " << CategoryStrideMap[categoryName] << L"\n";
                outputIniFile << "filename = " << fileName << L"\n" << L"\n";


            }



            outputIniFile << L"\n";
            outputIniFile << L"; -------------- TextureOverride IB & Resource IB-----------------" << L"\n" << L"\n";

            //TextureOverride IB����

            //��������֧��
            bool generateSwitchKey = false;
            std::wstring activateFlagName = L"ActiveFlag_" + extractConfig.DrawIB;
            std::wstring switchVarName = L"SwitchVar_" + extractConfig.DrawIB;
            std::wstring replace_prefix = L"";


            //4.IBOverride����
            std::string IBFirstIndex = extractConfig.MatchFirstIndexList[i];

            outputIniFile << L"[TextureOverride_" + extractConfig.DrawIB + L"_IB_SKIP_" + MMTString::ToWideString(IBFirstIndex) + L"]" << L"\n";
            outputIniFile << L"hash = " + extractConfig.DrawIB << L"\n";
            outputIniFile << L"match_first_index = " + MMTString::ToWideString(IBFirstIndex) << L"\n";
            outputIniFile << "handling = skip" << L"\n" << L"\n";


            outputIniFile << L"[Resource_BakIB" + MMTString::ToWideString(partName) + L"]" << L"\n";

            //��λ�����б�
            for (std::string categoryName : CategoryList) {
                std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
                outputIniFile << L"[Resource_Bak_" + MMTString::ToWideString(categorySlot) + L"]" << L"\n";
            }

            outputIniFile << L"[TextureOverride_IB_" + extractConfig.DrawIB + L"_" + MMTString::ToWideString(partName) + L"]" << L"\n";
            outputIniFile << L"hash = " + extractConfig.DrawIB << L"\n";
            outputIniFile << L"Resource_BakIB" + MMTString::ToWideString(partName) + L" = ref ib" << L"\n";

            for (std::string categoryName : CategoryList) {
                std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
                outputIniFile << L"Resource_Bak_" + MMTString::ToWideString(categorySlot) + L" = ref " + MMTString::ToWideString(categorySlot) << L"\n";
            }

            outputIniFile << L"match_first_index = " + MMTString::ToWideString(IBFirstIndex) << L"\n";
            if (generateSwitchKey) {
                outputIniFile << L"if $" + switchVarName + L" == 1" << L"\n";
            }

            outputIniFile << replace_prefix << L"ib = Resource_IB_" + extractConfig.DrawIB + L"_" + MMTString::ToWideString(partName) << L"\n";

            ////��ͼ��Դ�滻
            //std::vector<std::wstring> slotReplaceStrList = PartName_SlotReplaceStrList_Map[MMTString_ToWideString(partName)];
            //for (std::wstring slotReplaceStr : slotReplaceStrList) {
            //    outputIniFile << slotReplaceStr << L"\n";
            //}

            //��λ�滻�б�
            for (std::string categoryName : CategoryList) {
                std::wstring fileName = extractConfig.DrawIB + MMTString::ToWideString(categoryName) + L".buf";
                std::wstring filePath = splitPartNameOutputFolder + fileName;
                //int fileSize = MMTFile_GetFileSize(filePath);
                std::string categoryHash = extractConfig.CategoryHashMap[categoryName];
                std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
                outputIniFile << MMTString::ToWideString(categorySlot) << " = " << L"ref Resource_VB_" + MMTString::ToWideString(categoryName) + L"" << L"\n";
            }


            outputIniFile << replace_prefix << "drawindexed = auto" << L"\n";

            outputIniFile << L"ib = Resource_BakIB" + MMTString::ToWideString(partName) << L"\n";

            for (std::string categoryName : CategoryList) {
                std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
                outputIniFile << MMTString::ToWideString(categorySlot) + L" = " + L"Resource_Bak_" + MMTString::ToWideString(categorySlot) << L"\n";
            }

            if (generateSwitchKey) {
                outputIniFile << "endif" << L"\n";
            }
            outputIniFile << L"\n";

            outputIniFile << L"\n";
            outputIniFile << L"; -------------- IB Resource -----------------" << L"\n" << L"\n";

            //2.д��IBResource����
            outputIniFile << L"[Resource_IB_" + extractConfig.DrawIB + L"_" + MMTString::ToWideString(partName) + L"]" << L"\n";
            outputIniFile << "type = Buffer" << L"\n";
            outputIniFile << L"format = " + OutputIBDxgiFormat << L"\n";
            outputIniFile << "filename = " << MMTString::ToWideString(partName) + L".ib" << L"\n" << L"\n";



            ////7.д����ͼresource����
            //for (const auto& pair : ResourceName_FileName_Map) {
            //    std::wstring ResourceName = pair.first;
            //    std::wstring ResourceFileName = pair.second;
            //    outputIniFile << L"[" + ResourceName + L"]" << L"\n";
            //    outputIniFile << L"filename = " + ResourceFileName << L"\n" << L"\n";
            //}



            outputIniFile.close();

            LOG.NewLine();
            LOG.Info(L"Generate mod completed!");
            LOG.NewLine();
        }
    }

}