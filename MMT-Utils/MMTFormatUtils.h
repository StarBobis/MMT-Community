#pragma once
#include <map>

namespace MMTFormat {
    //TODO �㵽һ���µ��������ͽ�Vector3D������
    struct Vector3D {
        double x;
        double y;
        double z;
    };
    double VectorLength(const Vector3D& vector);
    Vector3D NormalizeVector(const Vector3D vector);
    Vector3D AddVectors(const Vector3D vector1, const Vector3D vector2);

    //����ľ���Pythonһ�����ݷ�Χ��ȡ
    std::vector<std::byte> GetRange_Byte(const std::vector<std::byte>& vec, std::size_t startIndex, std::size_t endIndex);
    std::vector<uint32_t> GetRange_UINT32(const std::vector<uint32_t>& vec, std::size_t startIndex, std::size_t endIndex);
    std::uint32_t STDByteVector_To_UINT32T(std::vector<std::byte> STDByteVector);
    std::vector<std::byte> Int_To_ByteVector(int value);

    //��ȡ�ֽڲ�ת��Ϊ�������͡�
    std::uint32_t CharArray_To_UINT32(char* data);
    std::uint16_t CharArray_To_UINT16(char* data);
    std::vector<std::byte> Reverse16BitShortValue(std::vector<std::byte> shortValue); //TODO ����޷�׼ȷ��ת
    std::vector<std::byte> Reverse32BitFloatValue(std::vector<std::byte> floatValue);
    std::vector<std::byte> ReverseSNORMValue(std::vector<std::byte>& snormValue);
    std::byte ReverseSNORMValueSingle(std::byte snormValue);
    std::byte UnormToSNORMValueSingle(std::byte Value);

    //D3D11������������ز���
    std::wstring NearestFloat(std::wstring originalValue);
    std::wstring NearestFormatedFloat(std::wstring originalValue, int reserveCount);
    std::wstring FormatedFloat(double originalValue, int reserveCount);



    std::vector<std::vector<double>> CalculateAverageNormalsAndStoreTangent(const std::vector<std::vector<double>> positions, const std::vector<std::vector<double>> normals);
    std::vector<std::vector<double>> NormalNormalizeStoreTangent(const std::vector<std::vector<double>> positions, const std::vector<std::vector<double>> normals);
    int GetRandomNumber(int number);
    std::unordered_map<uint64_t, std::vector<std::byte>> PatchBlendBuf_BLENDWEIGHT_1000(std::unordered_map<uint64_t, std::vector<std::byte>> BlendBufMap);
    std::vector<std::byte> Remove_BLENDWEIGHTS(std::vector<std::byte> BlendCategoryData, uint32_t BLENDWEIGHTS_Width, uint32_t BLENDINDICES_Width);


    //��ʽת��
    std::byte PackNumberToByte(std::uint32_t number);
    std::vector<std::byte> PackNumberOneByte(int packNumber);

    std::vector<std::byte> PackNumberR32_FLOAT_littleIndian(float number);
    float ByteVectorToFloat(const std::vector<std::byte>& bytes);
}
