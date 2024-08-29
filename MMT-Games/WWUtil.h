#pragma once
#include <iostream>
#include <unordered_map>


class WuwaCSInfo {
public:
    //֮ǰ�Ѽ��㶥����ܺ�
    int Offset;
    //��ǰCS���������Ҳ���ǵ�ǰ�Ĳ����Ķ�����
    int CalculateTime;
    //ʹ���ĸ�Shader������Pre-Skinning
    std::string ComputeShaderHash;
};


class WuwaCSInfoJsonObject {
public:

    //���︺��WuwaCSInfoJsonObject��д���Ͷ�ȡ
    std::unordered_map<std::string, WuwaCSInfo> PartNameWuwaCSInfoMap;

    void saveToJsonFile(std::wstring outputFolder);

    WuwaCSInfoJsonObject();

    WuwaCSInfoJsonObject(std::wstring readFolderPath);
};