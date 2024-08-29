#include "IndexBufferBufFile.h"
#include "VertexBufferBufFile.h"
#include "MMTFormatUtils.h"
#include "GlobalConfigs.h"
#include "WWUtil.h"

#include "Functions_ToF.h"


void Functions_ToF::GenerateMod() {

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



        //(1) ���BUF�ļ�
        //��ȡvb�ļ���ÿ��vb�ļ�������category�ֿ�װ�ز�ͬcategory������
        int SplitStride = d3d11GameType.getElementListStride(extractConfig.TmpElementList);
        LOG.Info(L"SplitStride: " + std::to_wstring(SplitStride));
        int drawNumber = 0;
        std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::vector<std::byte>>> partName_VBCategoryDaytaMap;
        std::unordered_map<std::string, int> partNameOffsetMap;
        std::unordered_map<std::string, int> partNameVertexCountMap;
        for (std::string partName : extractConfig.PartNameList) {
            std::wstring VBFileName = MMTString::ToWideString(partName) + L".vb";
            uint64_t VBFileSize = MMTFile::GetFileSize(BufferReadFolder + VBFileName);
            uint64_t vbFileVertexNumber = VBFileSize / SplitStride;

            partNameVertexCountMap[partName] = (int)vbFileVertexNumber;
            LOG.Info(L"Processing VB file: " + VBFileName + L" size is: " + std::to_wstring(VBFileSize) + L" byte." + L" vertex number is: " + std::to_wstring(vbFileVertexNumber));
            VertexBufferBufFile vbBufFile(BufferReadFolder + VBFileName, d3d11GameType, extractConfig.TmpElementList);
            partName_VBCategoryDaytaMap[MMTString::ToWideString(partName)] = vbBufFile.CategoryVBDataMap;


            //����offset
            partNameOffsetMap[partName] = drawNumber;
            //��ӵ�drawNumber
            drawNumber = drawNumber + (int)vbFileVertexNumber;
        }



        //(2) ת�������ÿ��IB�ļ�,����ע�������IBҪ����Offset����Ϊ����Blender������IB���Ǵ�0��ʼ��
        //�����ö�ȡib�ļ���ʹ�õ�Format,��1.fmt�ļ����Զ���ȡ
        std::wstring IBReadDxgiFormat = MMTFile::FindMigotoIniAttributeInFile(BufferReadFolder + L"1.fmt", L"format");
        for (std::string partName : extractConfig.PartNameList) {
            std::wstring IBFileName = MMTString::ToWideString(partName) + L".ib";
            std::wstring readIBFileName = BufferReadFolder + IBFileName;
            std::wstring writeIBFileName = BufferOutputFolder + IBFileName;
            LOG.Info(L"Converting IB file: " + IBFileName);
            IndexBufferBufFile ibBufFile(readIBFileName, IBReadDxgiFormat);
            ibBufFile.SaveToFile_UINT32(writeIBFileName, partNameOffsetMap[partName]);
        }
        LOG.Info(L"Output ib file over");
        LOG.NewLine();


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

                        //2.�����۲�NORMAL��ֵΪTANGENTǰ��λֱ�ӷŹ��������һλ��Ϊ0x7F
                        tmpCategoryData[index + 0] = TangentValueX;
                        tmpCategoryData[index + 1] = TangentValueY;
                        tmpCategoryData[index + 2] = TangentValueZ;
                        tmpCategoryData[index + 3] = std::byte(0x7F);

                        //3.��תNORMAL��ǰ��λ���ŵ�TANGENT��ǰ��λ��NORMAL��W��Ϊ0x7F
                        tmpCategoryData[index + 4] = MMTFormat::ReverseSNORMValueSingle(NormalValueX);
                        tmpCategoryData[index + 5] = MMTFormat::ReverseSNORMValueSingle(NormalValueY);
                        tmpCategoryData[index + 6] = MMTFormat::ReverseSNORMValueSingle(NormalValueZ);
                    }
                }
                std::vector<std::byte>& finalCategoryData = finalVBCategoryDataMap[MMTString::ToWideString(category)];
                finalCategoryData.insert(finalCategoryData.end(), tmpCategoryData.begin(), tmpCategoryData.end());

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
            std::wstring outputDatFilePath = BufferOutputFolder + categoryGeneratedName + L".buf";
            // ������ļ� ��std::vecto������д���ļ�
            std::ofstream outputFile(MMTString::ToByteString(outputDatFilePath), std::ios::binary);
            outputFile.write(reinterpret_cast<const char*>(categoryData.data()), categoryData.size());
            outputFile.close();
            LOG.Info(L"Write " + categoryName + L" data into file: " + outputDatFilePath);
        }
        LOG.NewLine();


        //(3) ����ini�ļ�
        std::wstring outputIniFileName = BufferOutputFolder + extractConfig.DrawIB + L".ini";
        std::wofstream outputIniFile(outputIniFileName);

        //�����ʹ��CS���㣬��ͻ�ƶ�����������
        //TODO �п��о�һ�����������������Ӧ�����������Dispatch

        bool debug = false;
        if (d3d11GameType.GPUPreSkinning && debug) {
            outputIniFile << L"\n";
            outputIniFile << L"; -------------- Break Vertex Count Limit -----------------" << L"\n" << L"\n";
            outputIniFile << L"[TextureOverride_CSReplace_VertexLimitBreak]" << L"\n";
            //TODO ��Ҫ�ռ�cs-cb0��hashֵ
            outputIniFile << L"hash = " << MMTString::ToWideString(extractConfig.VertexLimitVB) << L"\n";

            //�ȶ�ȡcsinfo.json�������
            WuwaCSInfoJsonObject wwcsinfoObject(BufferReadFolder);

            //����ÿ��partName
            int currentOffset = 0;
            for (std::string partName : extractConfig.PartNameList) {
                int currentVertexCount = partNameVertexCountMap[partName];
                WuwaCSInfo wwcsInfo = wwcsinfoObject.PartNameWuwaCSInfoMap[partName];
                int originalVertexCount = wwcsInfo.CalculateTime;
                int originalOffset = wwcsInfo.Offset;
                LOG.Info("CurrentVertexCount: " + std::to_string(currentVertexCount) + " OriginalVertexCount: " + std::to_string(originalVertexCount));
                outputIniFile << L";------------" + MMTString::ToWideString(partName) + L"-----------" << L"\n";

                if (originalVertexCount == currentVertexCount) {
                    //��������ͬʱ������ı䶥�������������ƫ�Ʋ�ͬ������Ҫ��ƫ�Ƶġ�
                    if (currentOffset != originalOffset) {
                        if (wwcsInfo.ComputeShaderHash == "4d0760c2c7406824") {
                            //�޸�3��ƫ����
                            outputIniFile << L"csreplace = cs-cb0, 1, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << L"\n";
                            outputIniFile << L"csreplace = cs-cb0, 2, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << L"\n";
                            outputIniFile << L"csreplace = cs-cb0, 3, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << L"\n";
                        }
                        else if (wwcsInfo.ComputeShaderHash == "1ff924db9d4048d1") {
                            //�޸�2��ƫ����
                            outputIniFile << L"csreplace = cs-cb0, 1, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << L"\n";
                            outputIniFile << L"csreplace = cs-cb0, 2, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << L"\n";
                        }

                    }
                }
                else {
                    if (currentOffset != originalOffset) {
                        //��������ͬ��ƫ��Ҳ��ͬʱȫ����Ҫ�޸�
                        if (wwcsInfo.ComputeShaderHash == "4d0760c2c7406824") {
                            //�޸�3��ƫ����
                            outputIniFile << L"csreplace = cs-cb0, 1, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << L"\n";
                            outputIniFile << L"csreplace = cs-cb0, 2, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << L"\n";
                            outputIniFile << L"csreplace = cs-cb0, 3, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << L"\n" << L"\n";
                            //�޸�1��������
                            outputIniFile << L"csreplace = cs-cb0, 4, " + std::to_wstring(originalVertexCount) + L"," + std::to_wstring(currentVertexCount) << L"\n";

                        }
                        else if (wwcsInfo.ComputeShaderHash == "1ff924db9d4048d1") {
                            //�޸�2��ƫ����
                            outputIniFile << L"csreplace = cs-cb0, 1, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << L"\n";
                            outputIniFile << L"csreplace = cs-cb0, 2, " + std::to_wstring(originalOffset) + L"," + std::to_wstring(currentOffset) << L"\n" << L"\n";
                            //�޸�1��������
                            outputIniFile << L"csreplace = cs-cb0, 3, " + std::to_wstring(originalVertexCount) + L"," + std::to_wstring(currentVertexCount) << L"\n";

                        }
                    }
                    else {
                        //��������ͬ��ƫ����ͬʱ��ֻ��Ҫ�Ķ�����
                        if (wwcsInfo.ComputeShaderHash == "4d0760c2c7406824") {
                            //�޸�1��������
                            outputIniFile << L"csreplace = cs-cb0, 4, " + std::to_wstring(originalVertexCount) + L"," + std::to_wstring(currentVertexCount) << L"\n";

                        }
                        else if (wwcsInfo.ComputeShaderHash == "1ff924db9d4048d1") {
                            //�޸�1��������
                            outputIniFile << L"csreplace = cs-cb0, 3, " + std::to_wstring(originalVertexCount) + L"," + std::to_wstring(currentVertexCount) << L"\n";

                        }
                    }
                }

                currentOffset = currentOffset + currentVertexCount;
            }

        }

        outputIniFile << L"\n";
        outputIniFile << L"; -------------- TextureOverride VB -----------------" << L"\n" << L"\n";
        //1.д��VBResource����
        for (std::string categoryName : CategoryList) {
            std::wstring fileName = extractConfig.DrawIB + MMTString::ToWideString(categoryName) + L".buf";
            std::wstring filePath = BufferOutputFolder + fileName;
            int fileSize = MMTFile::GetFileSize(filePath);
            std::string categoryHash = extractConfig.CategoryHashMap[categoryName];
            std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];
            outputIniFile << L"[TextureOverride_" + MMTString::ToWideString(categoryName) + L"_Replace]" << L"\n";
            outputIniFile << L"hash = " << MMTString::ToWideString(categoryHash) << "" << L"\n";
            outputIniFile << "this = " << L"Resource_VB_" + MMTString::ToWideString(categoryName) + L"" << L"\n" << L"\n";
        }

        outputIniFile << L"\n";
        outputIniFile << L"; -------------- Resource VB -----------------" << L"\n" << L"\n";
        //ResourceVB
        for (std::string categoryName : CategoryList) {
            std::wstring fileName = extractConfig.DrawIB + MMTString::ToWideString(categoryName) + L".buf";
            std::wstring filePath = BufferOutputFolder + fileName;
            int fileSize = MMTFile::GetFileSize(filePath);
            std::string categoryHash = extractConfig.CategoryHashMap[categoryName];
            std::string categorySlot = d3d11GameType.CategorySlotMap[categoryName];


            outputIniFile << L"[Resource_VB_" + MMTString::ToWideString(categoryName) + L"]" << L"\n";
            outputIniFile << L"byte_width = " << std::to_wstring(fileSize) << L"\n";
            if (categoryName == "Texcoord") {
                outputIniFile << L"type = Buffer" << L"\n";
                outputIniFile << L"FORMAT = R16G16_FLOAT" << L"\n";
            }
            else if (categoryName == "Normal") {
                outputIniFile << L"type = Buffer" << L"\n";
                outputIniFile << L"FORMAT = R8G8B8A8_SNORM" << L"\n";
            }
            else if (categoryName == "Position" && categorySlot == "vb0") {
                outputIniFile << L"type = Buffer" << L"\n";
                outputIniFile << L"FORMAT = R32G32B32_FLOAT" << L"\n";
            }
            else {
                outputIniFile << L"type = ByteAddressBuffer" << L"\n";
                outputIniFile << "stride = " << CategoryStrideMap[categoryName] << L"\n";
            }
            outputIniFile << "filename = " << fileName << L"\n" << L"\n";


        }

        outputIniFile << L"\n";
        outputIniFile << L"; -------------- IB Skip -----------------" << L"\n" << L"\n";

        //IB SKIP����
        outputIniFile << L"[Resource_BakIB]" << L"\n";
        outputIniFile << L"[TextureOverride_" + extractConfig.DrawIB + L"_IB_SKIP]" << L"\n";
        outputIniFile << L"hash = " + extractConfig.DrawIB << L"\n";
        outputIniFile << "handling = skip" << L"\n";
        outputIniFile << L"\n";

        outputIniFile << L"\n";
        outputIniFile << L"; -------------- TextureOverride IB & Resource IB-----------------" << L"\n" << L"\n";

        //TextureOverride IB����
        for (int i = 0; i < extractConfig.PartNameList.size(); ++i) {
            std::string partName = extractConfig.PartNameList[i];
            LOG.Info(L"Start to output UE4 ini file.");

            //��������֧��
            bool generateSwitchKey = false;
            std::wstring activateFlagName = L"ActiveFlag_" + extractConfig.DrawIB;
            std::wstring switchVarName = L"SwitchVar_" + extractConfig.DrawIB;
            std::wstring replace_prefix = L"";

            LOG.Info(L"Generate Switch Key ini :" + std::to_wstring(generateSwitchKey));

            //4.IBOverride����
            std::string IBFirstIndex = extractConfig.MatchFirstIndexList[i];

            outputIniFile << L"[TextureOverride_IB_" + extractConfig.DrawIB + L"_" + MMTString::ToWideString(partName) + L"]" << L"\n";
            outputIniFile << L"hash = " + extractConfig.DrawIB << L"\n";
            outputIniFile << L"Resource_BakIB = ref ib" << L"\n";
            outputIniFile << L"match_first_index = " + MMTString::ToWideString(IBFirstIndex) << L"\n";
            if (generateSwitchKey) {
                outputIniFile << L"if $" + switchVarName + L" == 1" << L"\n";
            }
            outputIniFile << replace_prefix << L"ib = Resource_IB_" + extractConfig.DrawIB + L"_" + MMTString::ToWideString(partName) << L"\n";
            outputIniFile << replace_prefix << "drawindexed = auto" << L"\n";
            outputIniFile << L"ib = Resource_BakIB" << L"\n";

            if (generateSwitchKey) {
                outputIniFile << "endif" << L"\n";
            }
            outputIniFile << L"\n";
        }

        outputIniFile << L"\n";
        outputIniFile << L"; -------------- IB Resource -----------------" << L"\n" << L"\n";

        //2.д��IBResource����
        for (int i = 0; i < extractConfig.PartNameList.size(); ++i) {
            std::string partName = extractConfig.PartNameList[i];
            outputIniFile << L"[Resource_IB_" + extractConfig.DrawIB + L"_" + MMTString::ToWideString(partName) + L"]" << L"\n";
            outputIniFile << "type = Buffer" << L"\n";
            outputIniFile << "format = DXGI_FORMAT_R32_UINT" << L"\n";
            outputIniFile << "filename = " << MMTString::ToWideString(partName) + L".ib" << L"\n" << L"\n";
        }


        outputIniFile.close();

        LOG.NewLine();
        LOG.Info("Generate mod completed!");
        LOG.NewLine();


    }

}