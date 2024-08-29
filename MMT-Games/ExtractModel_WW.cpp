#include "GlobalConfigs.h"
#include "IndexBufferTxtFile.h"
#include "ConstantBufferBufFile.h"
#include "FrameAnalysisData.h"
#include "Functions_WW.h"
#include "IndexBufferBufFile.h"

#include "GlobalConfigs.h"
#include "VertexBufferTxtFileDetect.h"
#include "IndexBufferTxtFile.h"
#include "FmtFile.h"
#include "ConstantBufferBufFile.h"
#include <set>
#include "FrameAnalysisData.h"
#include "IndexBufferBufFile.h"
#include "VertexBufferBufFile.h"
#include "Functions_WW.h"
#include "GlobalConfigs.h"
#include "VertexBufferTxtFileDetect.h"
#include "IndexBufferTxtFile.h"
#include "FmtFile.h"
#include "ConstantBufferBufFile.h"
#include <set>
#include "FrameAnalysisData.h"
#include "WWUtil.h"
#include "IndexBufferBufFile.h"
#include "VertexBufferBufFile.h"
#include "Functions_WW.h"

void Functions_WW::Extract_GPU_PreSkinning_WW(std::wstring DrawIB, std::wstring GameType) {
    LOG.Info("Start to extract from CS Buffer");

    DrawIBConfig extractConfig = G.DrawIB_ExtractConfig_Map[DrawIB];
    D3D11GameType d3d11GameType = GameTypeName_D3d11GameType_Map[MMTString::ToByteString(GameType)];
    LOG.Info("GameType: " + d3d11GameType.GameType);
    std::wstring OutputDrawIBFolder = G.OutputFolder + DrawIB + L"\\";
    std::filesystem::create_directories(OutputDrawIBFolder);

    FrameAnalysisData FAData = G.GetFrameAnalysisData();
    std::vector<std::wstring> FrameAnalyseFileNameList = FAData.FrameAnalysisFileNameList;
    std::map<int, std::wstring> matchFirstIndexIBFileNameMap;
    std::wstring VSExtractIndex;
    // ��vs-t0�ж�ȡ����������
    // Ȼ��������Ĺ��������ƶ�������ƥ�䵽���ǵ�DrawNumber�����ö�Ӧ�������Ŷԣ�����������������
    int MatchNumber = 0;
    for (std::wstring filename : FrameAnalyseFileNameList) {
        if (!filename.ends_with(L".txt")) {
            continue;
        }
        if (filename.find(L"-ib=" + DrawIB) == std::wstring::npos) {
            continue;
        }
        IndexBufferTxtFile ibFileData(G.WorkFolder + filename, true);
        std::wstring IBBufFilePath = G.WorkFolder + MMTString::GetFileNameWithOutSuffix(filename) + L".buf";
        IndexBufferBufFile ibBufFile(IBBufFilePath, ibFileData.Format);
        VSExtractIndex = ibFileData.Index;
        MatchNumber = ibBufFile.UniqueVertexCount;
        LOG.Info("Match DrawNumber: " + std::to_string(MatchNumber));
        LOG.Info(filename);
        if (ibFileData.FirstIndex == L"") {
            LOG.Info("Can't find FirstIndex attribute in this file, so skip this.");
            continue;
        }
        int matchFirstIndex = std::stoi(ibFileData.FirstIndex);

        matchFirstIndexIBFileNameMap[matchFirstIndex] = filename;
    }
    for (const auto& pair : matchFirstIndexIBFileNameMap) {
        LOG.Info("Match First Index: " + std::to_string(pair.first));
    }
    LOG.NewLine();


    LOG.NewLine();
    int MatchedDrawNumber = 0;
    std::wstring MatchedDrawNumberCSIndex = L"";
    std::string DrawComputeShader = "";
    //CalculateTime , (Offset,CalculateTime,CS)
    std::unordered_map<int, WuwaCSInfo> vertexCountWuwaCSInfoMap;
    for (std::wstring filename : FrameAnalyseFileNameList) {
        if (!filename.ends_with(L".buf")) {
            continue;
        }
        if (filename.find(L"-cs=") == std::wstring::npos) {
            continue;
        }
        //��ʱֻ��Ҫcs-cb0�͹���
        if (filename.find(L"-cs-cb0=") == std::wstring::npos) {
            continue;
        }
        //��Ҫ���˵���������ComputeShader���ļ���
        if (filename.find(L"1ff924db9d4048d1") == std::wstring::npos && filename.find(L"4d0760c2c7406824") == std::wstring::npos) {
            continue;
        }

        LOG.Info(filename);
        std::wstring filepath = G.WorkFolder + filename;
        //LOG.Info(filepath);

        ConstantBufferBufFile cbFileData(filepath);
        LOG.Info("CB[0].W CS Calculate Time: " + std::to_string(cbFileData.lineCBValueMap[0].W));
        LOG.Info("CB[0].Y Offset 1: " + std::to_string(cbFileData.lineCBValueMap[0].Y));
        LOG.Info("CB[0].Z Offset 2: " + std::to_string(cbFileData.lineCBValueMap[0].Z));
        LOG.Info("CB[1].X Calculate Time: " + std::to_string(cbFileData.lineCBValueMap[1].X));

        //�������cb0.w�� cb0.y��ͬ����Ϊcb0.w����cb1.x
        //�����ͬ��Ϊcb0.w + cb0.x
        WuwaCSInfo wwcsinfo;

        int drawNumber = 0;
        if (cbFileData.lineCBValueMap[0].W == cbFileData.lineCBValueMap[0].Y) {
            drawNumber = cbFileData.lineCBValueMap[0].W + cbFileData.lineCBValueMap[1].X;
            wwcsinfo.CalculateTime = cbFileData.lineCBValueMap[1].X;
            wwcsinfo.Offset = cbFileData.lineCBValueMap[0].W;
            LOG.Info("CB[0].W == CB[0].Y   DrawNumber = CB[0].W + CB[1].X = " + std::to_string(drawNumber));
        }
        else {
            drawNumber = cbFileData.lineCBValueMap[0].W + cbFileData.lineCBValueMap[0].Y;
            wwcsinfo.CalculateTime = cbFileData.lineCBValueMap[0].W;
            wwcsinfo.Offset = cbFileData.lineCBValueMap[0].Y;
            LOG.Info("CB[0].W != CB[0].Y   DrawNumber = CB[0].W + CB[0].Y = " + std::to_string(drawNumber));
        }
        LOG.NewLine();
        //LOG.Info("Draw Number: " + std::to_string(drawNumber));

        //����ֻ��drawNumber�������Ǵ�vs-t0���ҵ���DrawNumberʱ���Ž����滻
        if (MatchNumber == drawNumber) {
            //��Ⲣ����DrawComputeShader�������жϾ���Ҫ���ĸ���λ��ȡ
            if (filename.find(L"1ff924db9d4048d1") != std::wstring::npos) {
                DrawComputeShader = "1ff924db9d4048d1";

            }
            else if (filename.find(L"4d0760c2c7406824") != std::wstring::npos) {
                DrawComputeShader = "4d0760c2c7406824";
            }
            LOG.Info("Find Match Number File! Set draw compute shader to : " + DrawComputeShader);
            MatchedDrawNumberCSIndex = filename.substr(0, 6);
        }


        if (filename.find(L"1ff924db9d4048d1") != std::wstring::npos) {
            wwcsinfo.ComputeShaderHash = "1ff924db9d4048d1";
        }
        else if (filename.find(L"4d0760c2c7406824") != std::wstring::npos) {
            wwcsinfo.ComputeShaderHash = "4d0760c2c7406824";

        }
        vertexCountWuwaCSInfoMap[wwcsinfo.CalculateTime] = wwcsinfo;
    }
    LOG.Info(L"MatchedDrawNumber: " + std::to_wstring(MatchedDrawNumber));
    LOG.Info(L"MatchedDrawNumberCSIndex: " + MatchedDrawNumberCSIndex);
    LOG.NewLine();
    for (const auto& pair : vertexCountWuwaCSInfoMap) {
        WuwaCSInfo csInfo = pair.second;
        LOG.Info("VertexCount: " + std::to_string(csInfo.CalculateTime) + "  Offset:" + std::to_string(csInfo.Offset) + "  ComputeShaderHash:" + csInfo.ComputeShaderHash);
    }
    LOG.NewLine();

    /*
        1.��DrawComputeShaderΪ1ff924db9d4048d1ʱCS����λ�������£�
            cs-t3 stride=8    33145   ֻ���ǳ��ȸ�Ϊ4��BLENDWEIGHT��BLENDINDICES
            cs-t4 stride=8    33145   NORMAL,COLOR
            cs-t5 stride=12   33145   POSITION
        2.��DrawComputeShaderΪ4d0760c2c7406824ʱCS����λ�������£�
            cs-t4 stride=8    33145   ֻ���ǳ��ȸ�Ϊ4��BLENDWEIGHT��BLENDINDICES
            cs-t5 stride=8    33145   NORMAL,COLOR
            cs-t6 stride=12   33145   POSITION
    */
    std::wstring PositionExtractSlot = L"-cs-t5=";
    std::wstring TexcoordExtractSlot = L"-" + MMTString::ToWideString(d3d11GameType.CategorySlotMap["Texcoord"]) + L"=";
    std::wstring NormalExtractSlot = L"-cs-t4=";
    std::wstring BlendExtractSlot = L"-cs-t3=";
    if (DrawComputeShader == "1ff924db9d4048d1") {
        PositionExtractSlot = L"-cs-t5=";
        NormalExtractSlot = L"-cs-t4=";
        BlendExtractSlot = L"-cs-t3=";
    }
    else if (DrawComputeShader == "4d0760c2c7406824") {
        PositionExtractSlot = L"-cs-t6=";
        NormalExtractSlot = L"-cs-t5=";
        BlendExtractSlot = L"-cs-t4=";
    }
    else {
        LOG.Info("Can't find DrawComputeShader, use default setting.");
    }
    LOG.Info(L"Position Extract Slot: " + PositionExtractSlot);
    LOG.Info(L"Texcoord Extract Slot: " + TexcoordExtractSlot);
    LOG.Info(L"Normal Extract Slot: " + NormalExtractSlot);
    LOG.Info(L"Blend Extract Slot: " + BlendExtractSlot);
    LOG.NewLine();
    //�ռ�������λ�����ݣ�����ϳ�VB0������
    std::wstring PositionExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(MatchedDrawNumberCSIndex + PositionExtractSlot, L".buf")[0];
    std::wstring TexcoordExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + TexcoordExtractSlot, L".buf")[0];
    std::wstring NormalColorExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(MatchedDrawNumberCSIndex + NormalExtractSlot, L".buf")[0];
    std::wstring BlendExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(MatchedDrawNumberCSIndex + BlendExtractSlot, L".buf")[0];
    std::wstring CS_CB0_FileName = FAData.FindFrameAnalysisFileNameListWithCondition(MatchedDrawNumberCSIndex + L"-cs-cb0=", L".buf")[0];

    //����ı������ŵ������ʼ����
    LOG.Info(L"PositionExtractFileName: " + PositionExtractFileName);
    LOG.Info(L"NormalExtractFileName: " + NormalColorExtractFileName);
    LOG.Info(L"BlendExtractFileName: " + BlendExtractFileName);
    LOG.Info(L"TexcoordExtractFileName: " + TexcoordExtractFileName);
    LOG.Info(L"CS_CB0_FileName: " + CS_CB0_FileName);

    std::unordered_map<uint64_t, std::vector<std::byte>> PositionBufMap = MMTFile::ReadBufMapFromFile(G.WorkFolder + PositionExtractFileName, MatchNumber);
    std::unordered_map<uint64_t, std::vector<std::byte>> NormalTangentBufMap = MMTFile::ReadBufMapFromFile(G.WorkFolder + NormalColorExtractFileName, MatchNumber);
    std::unordered_map<uint64_t, std::vector<std::byte>> BlendBufMap = MMTFile::ReadBufMapFromFile(G.WorkFolder + BlendExtractFileName, MatchNumber);
    std::unordered_map<uint64_t, std::vector<std::byte>> TexcoordBufMap = MMTFile::ReadBufMapFromFile(G.WorkFolder + TexcoordExtractFileName, MatchNumber);

    std::wstring CategoryHash_Position = PositionExtractFileName.substr(13, 8);
    std::wstring CategoryHash_NormalColor = NormalColorExtractFileName.substr(13, 8);
    std::wstring CategoryHash_Blend = BlendExtractFileName.substr(13, 8);
    std::wstring CategoryHash_Texcoord = TexcoordExtractFileName.substr(13, 8);
    std::wstring CS_CB0_Hash = CS_CB0_FileName.substr(14, 8);

    extractConfig.CategoryHashMap["Position"] = MMTString::ToByteString(CategoryHash_Position);
    extractConfig.CategoryHashMap["Normal"] = MMTString::ToByteString(CategoryHash_NormalColor);
    extractConfig.CategoryHashMap["Blend"] = MMTString::ToByteString(CategoryHash_Blend);
    extractConfig.CategoryHashMap["Texcoord"] = MMTString::ToByteString(CategoryHash_Texcoord);
    extractConfig.VertexLimitVB = MMTString::ToByteString(CS_CB0_Hash);

    std::vector<std::byte> finalVB0Buf;
    for (int i = 0; i < MatchNumber; i++) {
        finalVB0Buf.insert(finalVB0Buf.end(), PositionBufMap[i].begin(), PositionBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), NormalTangentBufMap[i].begin(), NormalTangentBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), BlendBufMap[i].begin(), BlendBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), TexcoordBufMap[i].begin(), TexcoordBufMap[i].end());
    }

    FmtFile fmtFileData;
    fmtFileData.ElementNameList = d3d11GameType.OrderedFullElementList;
    fmtFileData.d3d11GameType = d3d11GameType;
    fmtFileData.Format = L"DXGI_FORMAT_R32_UINT";
    fmtFileData.Stride = d3d11GameType.getElementListStride(fmtFileData.ElementNameList);
    LOG.NewLine();

    LOG.Info("Start to go through every IB file:");
    //����ÿһ��IB�����
    int outputCount = 1;
    std::vector<std::string> MatchFirstIndexList;
    std::vector<std::string> PartNameList;

    WuwaCSInfoJsonObject wuwaCSInfoJsonObject;
    for (const auto& pair : matchFirstIndexIBFileNameMap) {
        std::wstring IBFileName = pair.second;
        std::wstring Index = IBFileName.substr(0, 6);

        std::wstring IBReadBufferFileName = IBFileName.substr(0, IBFileName.length() - 4) + L".buf";
        std::wstring IBReadBufferFilePath = G.WorkFolder + IBReadBufferFileName;

        std::wstring IBReadTxtFilePath = G.WorkFolder + IBFileName;
        IndexBufferTxtFile ibFileData(IBReadTxtFilePath, true);

        MatchFirstIndexList.push_back(MMTString::ToByteString(ibFileData.FirstIndex));
        PartNameList.push_back(std::to_string(outputCount));

        LOG.Info(IBFileName);
        LOG.Info(L"MatcheFirstIndex: " + ibFileData.FirstIndex + L"  PartName:" + std::to_wstring(outputCount));
        LOG.Info("MinNumber: " + std::to_string(ibFileData.MinNumber) + "\t\tMaxNumber:" + std::to_string(ibFileData.MaxNumber));

        //�ֱ����fmt,ib,vb
        std::wstring OutputIBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".ib";
        std::wstring OutputVBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".vb";
        std::wstring OutputFmtFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".fmt";

        //���FMT�ļ�������ǿ����ΪR32_UINT
        fmtFileData.OutputFmtFile(OutputFmtFilePath);

        //���IB�ļ�
        IndexBufferBufFile ibBufFile(IBReadBufferFilePath, ibFileData.Format);
        ibBufFile.SelfDivide(std::stoi(ibFileData.FirstIndex), std::stoi(ibFileData.IndexCount));
        //��Ϊ����VB�ļ���ȡ�����ˣ����������������0��ʼ���㣬����Ҫÿ��������ȥ�����е���Сֵ
        ibBufFile.SaveToFile_UINT32(OutputIBBufFilePath, -1 * ibBufFile.MinNumber);
        LOG.Info("Subdivided Unique Vertex Count: " + std::to_string(ibBufFile.UniqueVertexCount));
        wuwaCSInfoJsonObject.PartNameWuwaCSInfoMap[std::to_string(outputCount)] = vertexCountWuwaCSInfoMap[ibBufFile.UniqueVertexCount];

        //���VB�ļ�
        VertexBufferBufFile vbBufFile;
        vbBufFile.FinalVB0Buf = finalVB0Buf;
        LOG.Info("VBFile SelfDivide: minNumber:" + std::to_string(ibBufFile.MinNumber) + "  maxNumber:" + std::to_string(ibBufFile.MaxNumber) + " stride: " + std::to_string(fmtFileData.Stride));
        LOG.Info("Size: " + std::to_string(vbBufFile.FinalVB0Buf.size()));
        vbBufFile.SelfDivide(ibBufFile.MinNumber, ibBufFile.MaxNumber, fmtFileData.Stride);
        vbBufFile.SaveToFile(OutputVBBufFilePath);

        outputCount++;

    }

    //д�������Ķ�������
    wuwaCSInfoJsonObject.saveToJsonFile(OutputDrawIBFolder);

    //���Tmp.json
    extractConfig.MatchFirstIndexList = MatchFirstIndexList;
    extractConfig.PartNameList = PartNameList;
    extractConfig.TmpElementList = d3d11GameType.OrderedFullElementList;
    extractConfig.WorkGameType = MMTString::ToByteString(GameType);

    extractConfig.SaveTmpJsonConfigs(OutputDrawIBFolder);

    MoveAllUsedTexturesToOutputFolder(DrawIB, OutputDrawIBFolder);
}

void Functions_WW::Extract_Object_WW(std::wstring DrawIB, std::wstring GameType) {
    DrawIBConfig extractConfig = G.DrawIB_ExtractConfig_Map[DrawIB];
    D3D11GameType d3d11GameType = GameTypeName_D3d11GameType_Map[MMTString::ToByteString(GameType)];
    LOG.Info("GameType: " + d3d11GameType.GameType);
    std::wstring OutputDrawIBFolder = G.OutputFolder + DrawIB + L"\\";
    std::filesystem::create_directories(OutputDrawIBFolder);
    FrameAnalysisData FAData = G.GetFrameAnalysisData();
    std::vector<std::wstring> FrameAnalyseFileNameList = FAData.FrameAnalysisFileNameList;

    // ����object����Ӧ�ô�vb0�ж�ȡ��������
    // ���ƶ�������ƥ�䵽���ǵ�DrawNumber�����ö�Ӧ������
    std::map<int, std::wstring> matchFirstIndexIBFileNameMap;
    std::wstring VSExtractIndex;
    int MatchNumber = 0;
    for (std::wstring filename : FrameAnalyseFileNameList) {
        if (!filename.ends_with(L".txt")) {
            continue;
        }
        if (filename.find(L"-ib=" + DrawIB) == std::wstring::npos) {
            continue;
        }

        IndexBufferTxtFile ibFileData(G.WorkFolder + filename, true);
        //ע��:�����г����˲����������Ͷ��Drawʱ����ЩDraw��ʹ����ͼ��λ�Ҷ�������Ҳ�޷����ϣ�HashֵҲ��ͬ�����
        //����������ȡ����Mod����ʱ������Ҫȷ��ps-t0��λ����ͼȷʵ����
        //�����������˵������������Ⱦ��ͼ���Ǹ���λ��
        std::vector<std::wstring> Pst0_TextureDDSFileList = FAData.FindFrameAnalysisFileNameListWithCondition(ibFileData.Index + L"-ps-t1=", L".dds");
        std::vector<std::wstring> Pst0_TextureJPGFileList = FAData.FindFrameAnalysisFileNameListWithCondition(ibFileData.Index + L"-ps-t1=", L".jpg");
        if (Pst0_TextureDDSFileList.size() == 0 && Pst0_TextureJPGFileList.size() == 0) {
            LOG.Warning(L"Can't find ps-t1 or jpg texture for index:" + ibFileData.Index + L" it will not be a valid object type, so skip this.");
            continue;
        }

        VSExtractIndex = ibFileData.Index;

        MatchNumber = ibFileData.UniqueVertexCount;
        LOG.Info("Match DrawNumber: " + std::to_string(MatchNumber));

        LOG.Info(filename);
        if (ibFileData.FirstIndex == L"") {
            LOG.Info("Can't find FirstIndex attribute in this file, so skip this.");
            continue;
        }
        int matchFirstIndex = std::stoi(ibFileData.FirstIndex);

        matchFirstIndexIBFileNameMap[matchFirstIndex] = filename;
    }
    for (const auto& pair : matchFirstIndexIBFileNameMap) {
        LOG.Info("Match First Index: " + std::to_string(pair.first));
    }
    LOG.NewLine();



    std::wstring TexcoordExtractSlot = L"-" + MMTString::ToWideString(d3d11GameType.CategorySlotMap["Texcoord"]) + L"=";
    std::wstring NormalExtractSlot = L"-vs-t1=";
    LOG.Info(L"Texcoord Extract Slot: " + TexcoordExtractSlot);
    LOG.Info(L"Normal Extract Slot: " + NormalExtractSlot);
    LOG.NewLine();
    //�ռ�������λ�����ݣ�����ϳ�VB0������
    std::wstring PositionExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + L"-vb0=", L".buf")[0];
    std::wstring TexcoordExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + TexcoordExtractSlot, L".buf")[0];
    std::wstring NormalExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + NormalExtractSlot, L".buf")[0];

    //����ı������ŵ������ʼ����
    LOG.Info(L"PositionExtractFileName: " + PositionExtractFileName);
    LOG.Info(L"TexcoordExtractFileName: " + TexcoordExtractFileName);
    LOG.Info(L"NormalExtractFileName: " + NormalExtractFileName);

    std::unordered_map<uint64_t, std::vector<std::byte>> PositionBufMap = MMTFile::ReadBufMapFromFile(G.WorkFolder + PositionExtractFileName, MatchNumber);
    std::unordered_map<uint64_t, std::vector<std::byte>> NormalBufMap = MMTFile::ReadBufMapFromFile(G.WorkFolder + NormalExtractFileName, MatchNumber);
    std::unordered_map<uint64_t, std::vector<std::byte>> TexcoordBufMap = MMTFile::ReadBufMapFromFile(G.WorkFolder + TexcoordExtractFileName, MatchNumber);

    std::wstring CategoryHash_Position = PositionExtractFileName.substr(11, 8);
    std::wstring CategoryHash_Normal = NormalExtractFileName.substr(13, 8);
    std::wstring CategoryHash_Texcoord = TexcoordExtractFileName.substr(13, 8);

    extractConfig.CategoryHashMap["Position"] = MMTString::ToByteString(CategoryHash_Position);
    extractConfig.CategoryHashMap["Normal"] = MMTString::ToByteString(CategoryHash_Normal);
    extractConfig.CategoryHashMap["Texcoord"] = MMTString::ToByteString(CategoryHash_Texcoord);


    std::vector<std::byte> finalVB0Buf;
    for (int i = 0; i < MatchNumber; i++) {
        finalVB0Buf.insert(finalVB0Buf.end(), PositionBufMap[i].begin(), PositionBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), NormalBufMap[i].begin(), NormalBufMap[i].end());
        finalVB0Buf.insert(finalVB0Buf.end(), TexcoordBufMap[i].begin(), TexcoordBufMap[i].end());
    }

    FmtFile fmtFileData;
    fmtFileData.ElementNameList = d3d11GameType.OrderedFullElementList;
    fmtFileData.d3d11GameType = d3d11GameType;
    fmtFileData.Format = L"DXGI_FORMAT_R32_UINT";
    fmtFileData.Stride = d3d11GameType.getElementListStride(fmtFileData.ElementNameList);

    LOG.NewLine();

    LOG.Info("Start to go through every IB file:");
    //����ÿһ��IB�����
    int outputCount = 1;
    std::vector<std::string> MatchFirstIndexList;
    std::vector<std::string> PartNameList;

    for (const auto& pair : matchFirstIndexIBFileNameMap) {
        std::wstring IBFileName = pair.second;
        std::wstring Index = IBFileName.substr(0, 6);

        std::wstring IBReadBufferFileName = IBFileName.substr(0, IBFileName.length() - 4) + L".buf";
        std::wstring IBReadBufferFilePath = G.WorkFolder + IBReadBufferFileName;

        std::wstring IBReadFilePath = G.WorkFolder + IBFileName;
        IndexBufferTxtFile ibFileData(IBReadFilePath, true);

        MatchFirstIndexList.push_back(MMTString::ToByteString(ibFileData.FirstIndex));
        PartNameList.push_back(std::to_string(outputCount));

        LOG.Info(IBFileName);
        LOG.Info(L"MatcheFirstIndex: " + ibFileData.FirstIndex + L"  PartName:" + std::to_wstring(outputCount));
        LOG.Info("MinNumber: " + std::to_string(ibFileData.MinNumber) + "\t\tMaxNumber:" + std::to_string(ibFileData.MaxNumber));

        //�ֱ����fmt,ib,vb
        std::wstring OutputIBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".ib";
        std::wstring OutputVBBufFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".vb";
        std::wstring OutputFmtFilePath = OutputDrawIBFolder + DrawIB + L"-" + std::to_wstring(outputCount) + L".fmt";

        //���FMT�ļ�
        fmtFileData.OutputFmtFile(OutputFmtFilePath);

        //���IB�ļ�
        IndexBufferBufFile ibBufFile(IBReadBufferFilePath, ibFileData.Format);
        ibBufFile.SelfDivide(std::stoi(ibFileData.FirstIndex), std::stoi(ibFileData.IndexCount));
        ibBufFile.SaveToFile_UINT32(OutputIBBufFilePath, -1 * ibBufFile.MinNumber);

        //���VB�ļ�
        VertexBufferBufFile vbBufFile;
        vbBufFile.FinalVB0Buf = finalVB0Buf;
        vbBufFile.SelfDivide(ibBufFile.MinNumber, ibBufFile.MaxNumber, fmtFileData.Stride);
        vbBufFile.SaveToFile(OutputVBBufFilePath);

        outputCount++;
    }


    //���Tmp.json
    extractConfig.MatchFirstIndexList = MatchFirstIndexList;
    extractConfig.PartNameList = PartNameList;
    extractConfig.TmpElementList = d3d11GameType.OrderedFullElementList;
    extractConfig.WorkGameType = MMTString::ToByteString(GameType);

    extractConfig.SaveTmpJsonConfigs(OutputDrawIBFolder);

    MoveAllUsedTexturesToOutputFolder(DrawIB, OutputDrawIBFolder);
}

void Functions_WW::ExtractModel() {
    for (const auto& pair : G.DrawIB_ExtractConfig_Map) {
        std::wstring DrawIB = pair.first;
        DrawIBConfig extractConfig = pair.second;
        LOG.NewLine();
        LOG.Info("I suggest this test Setting in your d3dx.ini for dump:\n analyse_options = dump_rt dump_tex dump_cb dump_vb dump_ib buf txt");
        LOG.NewLine();
        LOG.Info("Extract Buffer from UE4 CS:");

        std::wstring MatchedGameType = extractConfig.GameType;
        if (extractConfig.GameType == L"Auto") {
            LOG.Info(L"Try auto detect game type:" );
            std::vector<std::wstring> MatchGameTypeList;
            for (const auto& pair: GameTypeName_D3d11GameType_Map) {
                D3D11GameType d3d11GameType = pair.second;
                FrameAnalysisData FAData = G.GetFrameAnalysisData();
                std::vector<std::wstring> FrameAnalyseFileNameList = FAData.FrameAnalysisFileNameList;
                std::wstring VSExtractIndex;
                // ��vs-t0�ж�ȡ����������
                // Ȼ��������Ĺ��������ƶ�������ƥ�䵽���ǵ�DrawNumber�����ö�Ӧ�������Ŷԣ�����������������
                int MatchNumber = 0;
                LOG.Info(L"Start to check for every file related with your DrawIB: " + DrawIB);
                for (std::wstring filename : FrameAnalyseFileNameList) {
                    if (!filename.ends_with(L".txt")) {
                        continue;
                    }
                    if (filename.find(L"-ib=" + DrawIB) == std::wstring::npos) {
                        continue;
                    }
                    IndexBufferTxtFile ibFileData(G.WorkFolder + filename, true);
                    std::wstring IBBufFilePath = G.WorkFolder + MMTString::GetFileNameWithOutSuffix(filename) + L".buf";
                    IndexBufferBufFile ibBufFile(IBBufFilePath, ibFileData.Format);
                    VSExtractIndex = ibFileData.Index;
                    MatchNumber = ibBufFile.UniqueVertexCount;
                    LOG.Info("Match DrawNumber: " + std::to_string(MatchNumber));

                    LOG.Info(filename);
                    if (ibFileData.FirstIndex == L"") {
                        LOG.Info("Can't find FirstIndex attribute in this file, so skip this.");
                        continue;
                    }
                    int matchFirstIndex = std::stoi(ibFileData.FirstIndex);

                }
                if (MatchNumber == 0) {
                    LOG.Error(L"Can't find any related ib file with your DrawIB : " + DrawIB + L" Please check:\n1.Did you dump a FrameAnalysis folder for extract this IB hash.\n2.Did you choose the correct IB value,there may be multiple IB control render,only one of them is real.");
                }
                LOG.NewLine();


                LOG.NewLine();
                int MatchedDrawNumber = 0;
                std::wstring MatchedDrawNumberCSIndex = L"";
                std::string DrawComputeShader = "";
                for (std::wstring filename : FrameAnalyseFileNameList) {
                    if (!filename.ends_with(L".buf")) {
                        continue;
                    }
                    if (filename.find(L"-cs=") == std::wstring::npos) {
                        continue;
                    }
                    //��ʱֻ��Ҫcs-cb0�͹���
                    if (filename.find(L"-cs-cb0=") == std::wstring::npos) {
                        continue;
                    }
                    //��Ҫ���˵���������ComputeShader���ļ���
                    if (filename.find(L"1ff924db9d4048d1") == std::wstring::npos && filename.find(L"4d0760c2c7406824") == std::wstring::npos) {
                        continue;
                    }

                    LOG.Info(filename);
                    std::wstring filepath = G.WorkFolder + filename;
                    //LOG.Info(filepath);

                    ConstantBufferBufFile cbFileData(filepath);
                    LOG.Info("CB[0].W CS Calculate Time: " + std::to_string(cbFileData.lineCBValueMap[0].W));
                    LOG.Info("CB[0].Y Draw Number1: " + std::to_string(cbFileData.lineCBValueMap[0].Y));
                    LOG.Info("CB[0].Z Draw Number2: " + std::to_string(cbFileData.lineCBValueMap[0].Z));
                    LOG.Info("CB[1].X Draw Number3: " + std::to_string(cbFileData.lineCBValueMap[1].X));

                    //�������cb0.w�� cb0.y��ͬ����Ϊcb0.w����cb1.x
                    //�����ͬ��Ϊcb0.w + cb0.x
                    int drawNumber = 0;
                    if (cbFileData.lineCBValueMap[0].W == cbFileData.lineCBValueMap[0].Y) {
                        drawNumber = cbFileData.lineCBValueMap[0].W + cbFileData.lineCBValueMap[1].X;
                        LOG.Info("CB[0].W == CB[0].Y   DrawNumber = CB[0].W + CB[1].X = " + std::to_string(drawNumber));
                    }
                    else {
                        drawNumber = cbFileData.lineCBValueMap[0].W + cbFileData.lineCBValueMap[0].Y;
                        LOG.Info("CB[0].W != CB[0].Y   DrawNumber = CB[0].W + CB[0].Y = " + std::to_string(drawNumber));
                    }
                    LOG.NewLine();

                    //LOG.Info("Draw Number: " + std::to_string(drawNumber));

                    //����ֻ��drawNumber�������Ǵ�vs-t0���ҵ���DrawNumberʱ���Ž����滻
                    
                        if (MatchNumber == drawNumber) {
                            //��Ⲣ����DrawComputeShader�������жϾ���Ҫ���ĸ���λ��ȡ
                            if (filename.find(L"1ff924db9d4048d1") != std::wstring::npos) {
                                DrawComputeShader = "1ff924db9d4048d1";

                            }
                            else if (filename.find(L"4d0760c2c7406824") != std::wstring::npos) {
                                DrawComputeShader = "4d0760c2c7406824";
                            }
                            LOG.Info("Find Match Number File! Set draw compute shader to : " + DrawComputeShader);
                            MatchedDrawNumberCSIndex = filename.substr(0, 6);
                        }

                }
                LOG.Info(L"MatchedDrawNumber: " + std::to_wstring(MatchedDrawNumber));
                LOG.Info(L"MatchedDrawNumberCSIndex: " + MatchedDrawNumberCSIndex);

                //������ҵ�MaxDrawNumberIndex����ֱ�ӵ�ǰ���ͼ��������
                //TODO ����Ҳ���ԣ����ͬʱ���ݶ����Ϸ��������ͻ������⣬����������ʱֻ����һ��
                if (MatchedDrawNumberCSIndex != L"") {
                    if (d3d11GameType.GPUPreSkinning) {
                        LOG.Info("Find MatchedDrawNumberCSIndex��Must be A GPU-PreSkinning type.");
                        MatchedGameType = MMTString::ToWideString(d3d11GameType.GameType);
                        LOG.Info(L"GameType Matched: " + MatchedGameType);
                        MatchGameTypeList.push_back(MMTString::ToWideString(d3d11GameType.GameType));
                        break;
                    }
                    else {
                        LOG.Info("Not GPU PRESKINNING??");
                    }
                    
                }
                else {
                    LOG.Info("MatchedDrawNumberCSIndex IS EMPTY??");
                }

                //����Ҳ�����Ӧ����ֵ��˵����ʹ��CS���м��㣬˵��ΪObject���ͣ�����������object����ƥ��
                //��ÿ����λ�Ŀ�Ƚ��м�飬��ͨ����continue��ȫ��ͨ��������б�
                
                LOG.Info("Start to match object types:");
                // ����object����Ӧ�ô�vb0�ж�ȡ��������
                // ���ƶ�������ƥ�䵽���ǵ�DrawNumber�����ö�Ӧ������
                MatchNumber = 0;
                std::unordered_map<std::string, uint32_t> CategoryStrideMap = d3d11GameType.getCategoryStrideMap(d3d11GameType.OrderedFullElementList);
                int PositionStride = CategoryStrideMap["Position"];
                for (std::wstring filename : FrameAnalyseFileNameList) {
                    if (!filename.ends_with(L".txt")) {
                        continue;
                    }
                    if (filename.find(L"-ib=" + DrawIB) == std::wstring::npos) {
                        continue;
                    }

                    IndexBufferTxtFile ibFileData(G.WorkFolder + filename, false);

                    VSExtractIndex = ibFileData.Index;
                    MatchNumber = ibFileData.UniqueVertexCount;
                    LOG.Info("Match DrawNumber: " + std::to_string(MatchNumber));

                    LOG.Info(filename);
                    if (ibFileData.FirstIndex == L"") {
                        LOG.Info("Can't find FirstIndex attribute in this file, so skip this.");
                        continue;
                    }

                    //ע��:�����г����˲����������Ͷ��Drawʱ����ЩDraw��ʹ����ͼ��λ�Ҷ�������Ҳ�޷����ϣ�HashֵҲ��ͬ�����
                    //����������ȡ����Mod����ʱ������Ҫȷ��ps-t0��λ����ͼȷʵ����
                    //�����������˵������������Ⱦ��ͼ���Ǹ���λ��
                    std::vector<std::wstring> Pst0_TextureDDSFileList = FAData.FindFrameAnalysisFileNameListWithCondition(ibFileData.Index + L"-ps-t1=", L".dds");
                    std::vector<std::wstring> Pst0_TextureJPGFileList = FAData.FindFrameAnalysisFileNameListWithCondition(ibFileData.Index + L"-ps-t1=", L".jpg");
                    if (Pst0_TextureDDSFileList.size() == 0 && Pst0_TextureJPGFileList.size() == 0) {
                        LOG.Warning(L"Can't find ps-t1 or jpg texture for index:" + ibFileData.Index + L" it will not be a valid object type, so skip this.");
                        continue;
                    }

                    //TODO ���ﲻ������ps-t0����ps-t1���޷�����ͨ�ü��ݣ����������Ž⣬�����пտ����Ż�һ��
                }
               
                LOG.NewLine();



                std::wstring TexcoordExtractSlot = L"-" + MMTString::ToWideString(d3d11GameType.CategorySlotMap["Texcoord"]) + L"=";
                std::wstring NormalExtractSlot = L"-" + MMTString::ToWideString(d3d11GameType.CategorySlotMap["Normal"]) + L"=";
                LOG.Info(L"Texcoord Extract Slot: " + TexcoordExtractSlot);
                LOG.Info(L"Normal Extract Slot: " + NormalExtractSlot);
                LOG.NewLine();
                //�ռ�������λ�����ݣ�����ϳ�VB0������
                std::wstring PositionExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + L"-vb0=", L".buf")[0];
                std::wstring TexcoordExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + TexcoordExtractSlot, L".buf")[0];
                std::wstring NormalExtractFileName = FAData.FindFrameAnalysisFileNameListWithCondition(VSExtractIndex + NormalExtractSlot, L".buf")[0];

                //����ı������ŵ������ʼ����
                LOG.Info(L"PositionExtractFileName: " + PositionExtractFileName);
                LOG.Info(L"TexcoordExtractFileName: " + TexcoordExtractFileName);
                LOG.Info(L"NormalExtractFileName: " + NormalExtractFileName);

                int NormalStride = MMTFile::GetFileSize(G.WorkFolder + NormalExtractFileName) / MatchNumber;
                int TexcoordStride = MMTFile::GetFileSize(G.WorkFolder + TexcoordExtractFileName) / MatchNumber;

                if (NormalStride != CategoryStrideMap["Normal"]) {
                    LOG.Info("NormalStride: " + std::to_string(TexcoordStride) + "  CategoryStride: " + std::to_string(CategoryStrideMap["Normal"]));
                    LOG.Info("GameType:" + d3d11GameType.GameType + " Can't match stride with Normal");
                    continue;
                }
                if (TexcoordStride != CategoryStrideMap["Texcoord"]) {
                    LOG.Info("TexcoordStride: " + std::to_string(TexcoordStride) + "  CategoryStride: " + std::to_string(CategoryStrideMap["Texcoord"]));
                    LOG.Info("GameType:" + d3d11GameType.GameType + " Can't match stride with Texcoord");
                    continue;
                }

                //�����������������ܶ��ϣ�������ֱ�Ӽ��뼴��
                MatchedGameType = MMTString::ToWideString(pair.first);
                MatchGameTypeList.push_back(MMTString::ToWideString(pair.first));

            }

            D3D11GameType matchGameTypeObject = GameTypeName_D3d11GameType_Map[MMTString::ToByteString(MatchedGameType)];

            //Ȼ����ݴ�С������Ӧѡ��
            if (MatchGameTypeList.size() == 1) {
                LOG.NewLine();
                LOG.Info(L"GameType Matched: " + MatchedGameType);
                LOG.NewLine();

                if (matchGameTypeObject.GPUPreSkinning ) {
                    Extract_GPU_PreSkinning_WW(DrawIB, MatchedGameType);
                }
                else {
                    Extract_Object_WW(DrawIB, MatchedGameType);
                }
            }
            else if (MatchGameTypeList.size() > 1) {
                std::wstring GameTypeListStr = L"";
                for (std::wstring GameType: MatchGameTypeList) {
                    GameTypeListStr = GameTypeListStr + GameType + L" ";
                }
                //�г����п��ܵ�����
                LOG.Error(L"More than one GameType Matched: " + GameTypeListStr);
            }
            else {
                LOG.Error("Unknown GameType! Please contanct NicoMico to add support for this type!");
            }

        }
        else {
            D3D11GameType matchGameTypeObject = GameTypeName_D3d11GameType_Map[MMTString::ToByteString(MatchedGameType)];
            if (matchGameTypeObject.GPUPreSkinning) {
                Extract_GPU_PreSkinning_WW(DrawIB, MatchedGameType);
            }
            else {
                Extract_Object_WW(DrawIB, MatchedGameType);
            }
        }



    }
}