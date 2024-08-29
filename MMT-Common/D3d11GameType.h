#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <map>


class D3D11Element {
public:
    //�ɱ���
    std::string SemanticName = "";
    std::string SemanticIndex = "";
    std::string Format = "";
    int ByteWidth = 0;
    std::string ExtractSlot = "";
    std::string ExtractTechnique = "";
    std::string Category = "";

    //�̶���
    std::string InputSlot = "0";
    std::string InputSlotClass = "per-vertex";
    std::string InstanceDataStepRate = "0";

    //��Ҫ��̬����
    int ElementNumber = 0;
    int AlignedByteOffset = 0;

    std::string GetIndexedSemanticName() {
        if (SemanticIndex == "0") {
            return SemanticName;
        }
        else {
            return SemanticName + SemanticIndex;
        }
    }
};


class D3D11GameType {
public:
    std::string GameType;

    //�Ƿ���Texcoord��λPatch 0x00���ı��λ����������Mod�����Ѷ�
    int TexcoordPatchNull = 0;

    //�Ƿ�ʹ��GPU���������̬�任
    bool GPUPreSkinning = false;

    //�Ƿ���Ҫ����blendweights��Ĭ�ϲ���Ҫ
    bool PatchBLENDWEIGHTS = false;

    //UE4����0
    bool UE4PatchNullInBlend = false;

    //UnityCS����ֵ
    std::string RootComputeShaderHash = "";

    //ԭ���������ini�ļ����滻ʱд����draw��λ�����ÿһ�������ﶼ��Ҫ�ֶ�ָ��
    std::unordered_map <std::string, std::string> CategoryDrawCategoryMap;
    std::vector<std::string> OrderedFullElementList;
    std::vector<std::string> OrderedCategoryNameList;

    //��Ҫ��������ó�������
    std::unordered_map<std::string, D3D11Element> ElementNameD3D11ElementMap;
    std::unordered_map <std::string, std::string> CategorySlotMap;
    std::unordered_map <std::string, std::string> CategoryTopologyMap;
    std::unordered_map<std::string, uint32_t> CategoryStrideMap;

    D3D11GameType();

    //�����ṩ��ElementList��ȡ�ܵ�Stride
    uint32_t getElementListStride(std::vector<std::string>);

    //��ȡCategory Stride Map
    std::unordered_map<std::string, uint32_t> getCategoryStrideMap(std::vector<std::string> inputElementList);

    //��ȡCategory List
    std::vector<std::string> getCategoryList(std::vector<std::string> inputElementList);

    //��ȡCategory��ElementList
    std::vector<std::string> getCategoryElementList(std::vector<std::string> inputElementList,std::string category);

    //����OrderedElementList�����������ElementList
    std::vector<std::string> getReorderedElementList(std::vector<std::string> elementList);
};