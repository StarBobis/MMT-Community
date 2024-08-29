#include "BufferUtils.h"

#include "GlobalConfigs.h"
#include "MMTFormatUtils.h"

#include "MMTConstants.h"


namespace BufferUtils {

    void Unity_COLOR_AverageNormal(std::unordered_map<std::wstring, std::vector<std::byte>>& finalVBCategoryDataMap, DrawIBConfig basicConfig, D3D11GameType d3d11GameType) {

        LOG.Info(L"��ʼִ��COLOR�ؼ��㣬ʹ��average_normal�㷨��");
        //D3D11GameType d3d11GameType = GameTypeName_D3d11GameType_Map[basicConfig.WorkGameType];
        
        //��ȡPosition���������
        std::vector<std::byte> PositionCategoryValues = finalVBCategoryDataMap[L"Position"];

        //��ȡPOSITION��TANGENT
        std::vector<std::vector<double>> POSITION_VALUES;
        std::vector<std::vector<double>> NORMAL_VALUES;

        int positionStride = 0;
        //��ΪPOSITION���������TEXCOORD�����Ķ�̬��������������ֱ�ӻ�ȡOrderedElementList��ͳ�Ƴ�Position�ĳ���
        for (std::string elementName : d3d11GameType.OrderedFullElementList) {
            D3D11Element d3d11Element = d3d11GameType.ElementNameD3D11ElementMap[elementName];
            if (d3d11Element.Category == "Position") {
                int byteWidth = d3d11Element.ByteWidth;
                positionStride += byteWidth;
            }
        }
        LOG.Info(L"PositionStride: " + std::to_wstring(positionStride));
        for (std::size_t i = 0; i < PositionCategoryValues.size(); i = i + positionStride)
        {
            std::vector<std::byte> POSITION_X = MMTFormat::GetRange_Byte(PositionCategoryValues, i, i + 4);
            //LOG.LogOutput(L"POSITION_X: " + std::to_wstring(bytesToFloat(POSITION_X)));
            std::vector<std::byte> POSITION_Y = MMTFormat::GetRange_Byte(PositionCategoryValues, i + 4, i + 8);
            std::vector<std::byte> POSITION_Z = MMTFormat::GetRange_Byte(PositionCategoryValues, i + 8, i + 12);
            std::vector<double> POSITION_DOUBLE = { MMTFormat::ByteVectorToFloat(POSITION_X), MMTFormat::ByteVectorToFloat(POSITION_Y) ,MMTFormat::ByteVectorToFloat(POSITION_Z) };
            POSITION_VALUES.push_back(POSITION_DOUBLE);

            std::vector<std::byte> NORMAL_X = MMTFormat::GetRange_Byte(PositionCategoryValues, i + 12, i + 16);
            std::vector<std::byte> NORMAL_Y = MMTFormat::GetRange_Byte(PositionCategoryValues, i + 16, i + 20);
            std::vector<std::byte> NORMAL_Z = MMTFormat::GetRange_Byte(PositionCategoryValues, i + 20, i + 24);
            std::vector<double> NORMAL_DOUBLE = { MMTFormat::ByteVectorToFloat(NORMAL_X), MMTFormat::ByteVectorToFloat(NORMAL_Y) ,MMTFormat::ByteVectorToFloat(NORMAL_Z) };
            NORMAL_VALUES.push_back(NORMAL_DOUBLE);
        }
        LOG.Info(L"Start to CalculateAverageNormalsAndStoreTangent");
        //�Ѷ�����ͬ�ķ��� 3ά���� ���Ȼ���һ������Ϊ����ֵΪ�������ֵ����ͬ����
        //Ȼ���ÿ����Ӧ��TANGENT���������ֵ��Ϊǰ3λ��0��Ϊ���һλ

        //�����ƽ�ַ��ߵ�TANGENT
        std::vector<std::vector<double>> TANGENT_VALUES = MMTFormat::NormalNormalizeStoreTangent(POSITION_VALUES, NORMAL_VALUES);
        LOG.Info(L"TANGENT_VALUES size: " + std::to_wstring(TANGENT_VALUES.size()));
        LOG.Info(L"CalculateAverageNormalsAndStoreTangent success");


        //׼��һ���µ����ڷ���
        std::unordered_map<std::wstring, std::vector<std::byte>> newVBCategoryDataMap;

        //����Ҫȷ��Color���ĸ���λ��
        D3D11Element colorElement = d3d11GameType.ElementNameD3D11ElementMap["COLOR"];
        std::wstring colorCategory = MMTString::ToWideString(colorElement.Category);
        std::vector<std::byte> colorCategoryValues = finalVBCategoryDataMap[colorCategory];

        //ȷ��Color���ڲ�λ�Ĳ���
        int colorCategoryStride = d3d11GameType.CategoryStrideMap[MMTString::ToByteString(colorCategory)];

        //ȷ��Color���ڵĲ�λƫ��
        int colorOffset = 0;

        //TODO ȷ��Color��ByteWidth,�������ǹ̶�Ϊ4����Ϊֻ����һ���������ʵҲ�ò���������������������ʹ��
        //int colorByteWidth = 4;

        for (std::string elementName : basicConfig.TmpElementList) {
            D3D11Element d3d11Element = d3d11GameType.ElementNameD3D11ElementMap[elementName];

            //���ȱ�������color���ڵĲ�λ
            if (d3d11Element.Category == MMTString::ToByteString(colorCategory)) {
                if (elementName != "COLOR") {
                    colorOffset += d3d11Element.ByteWidth;
                }
                else {
                    break;
                }
            }
        }

        //��ʼ����
        std::vector<std::byte> newTexcoordCategoryValues;
        int count = 0;
        for (std::size_t i = 0; i < colorCategoryValues.size(); i = i + colorCategoryStride)
        {
            //�Ȼ�ȡCOLORǰ������ֵ
            std::vector<std::byte> beforeValues = MMTFormat::GetRange_Byte(colorCategoryValues, i, i + colorOffset);
            std::vector<std::byte> ColorValues = MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset, i + colorOffset + 4);
            std::vector<std::byte> afterValues = MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset + 4, i + colorCategoryStride);
            //��׼����COLORֵ
            std::vector<std::byte> newColorValues;
            std::vector<double> TANGENT_DOUBLES = TANGENT_VALUES[count];

            //R,G,B�ֱ�洢��һ��֮��Normal�ķ���
            newColorValues.push_back(MMTFormat::PackNumberToByte(static_cast<uint32_t>(round(TANGENT_DOUBLES[0] * 255))));
            newColorValues.push_back(MMTFormat::PackNumberToByte(static_cast<uint32_t>(round(TANGENT_DOUBLES[1] * 255))));
            newColorValues.push_back(MMTFormat::PackNumberToByte(static_cast<uint32_t>(round(TANGENT_DOUBLES[2] * 255))));

            //Alphaͨ���̶�Ϊ128
            newColorValues.push_back(ColorValues[3]);

            newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), beforeValues.begin(), beforeValues.end());
            newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), newColorValues.begin(), newColorValues.end());
            newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), afterValues.begin(), afterValues.end());

            count++;
        }

        finalVBCategoryDataMap[colorCategory] = newTexcoordCategoryValues;
        LOG.NewLine();
    }





    //����COLOR��Position�����е������һ���㷨ͬʱ����Position��Texcoord
    void Unity_Reset_COLOR(std::unordered_map<std::wstring, std::vector<std::byte>>& finalVBCategoryDataMap, DrawIBConfig basicConfig, D3D11GameType d3d11GameType) {
        LOG.Info(L"Start to reset COLOR Values:");

        //��ȡ����RGB������ֵ
        std::wstring rgb_r = MMTString::ToWideString(basicConfig.ColorMap["rgb_r"]);
        std::wstring rgb_g = MMTString::ToWideString(basicConfig.ColorMap["rgb_g"]);
        std::wstring rgb_b = MMTString::ToWideString(basicConfig.ColorMap["rgb_b"]);
        std::wstring rgb_a = MMTString::ToWideString(basicConfig.ColorMap["rgb_a"]);



        //����Ҫȷ��Color���ĸ���λ��
        //D3D11GameType d3d11GameType = GameTypeName_D3d11GameType_Map[basicConfig.WorkGameType];
        D3D11Element colorElement = d3d11GameType.ElementNameD3D11ElementMap["COLOR"];
        std::wstring colorCategory = MMTString::ToWideString(colorElement.Category);
        std::vector<std::byte> colorCategoryValues = finalVBCategoryDataMap[colorCategory];

        //ȷ��Color���ڲ�λ�Ĳ���
        int colorCategoryStride = d3d11GameType.CategoryStrideMap[MMTString::ToByteString(colorCategory)];
        LOG.Info(L"��ǰ����COLORֵ�ĸ�ʽ:"+MMTString::ToWideString(colorElement.Format));

        if (colorElement.Format == DXGI_FORMAT::R8G8B8A8_UNORM) {

            //׼�������������滻
            int rgb_r_num = -1;
            int rgb_g_num = -1;
            int rgb_b_num = -1;
            int rgb_a_num = -1;

            //���Խ��и�ʽת��
            if (L"default" != rgb_r) {
                rgb_r_num = std::stoi(rgb_r);
            }
            if (L"default" != rgb_g) {
                rgb_g_num = std::stoi(rgb_g);
            }
            if (L"default" != rgb_b) {
                rgb_b_num = std::stoi(rgb_b);
            }
            if (L"default" != rgb_a) {
                rgb_a_num = std::stoi(rgb_a);
            }

            //�������default�Ǹɴ�ɶҲ���ֱ�ӷ���
            if (rgb_a_num == -1 && rgb_g_num == -1 && rgb_b_num == -1 && rgb_a_num == -1) {
                LOG.Info(L"δ��⵽��Ҫ�޸ĵ�COLORֵ����");
                LOG.NewLine();
            }
            else {
                //׼������������д��
                std::byte patchRgb_R = MMTFormat::PackNumberToByte(rgb_r_num);
                std::byte patchRgb_G = MMTFormat::PackNumberToByte(rgb_g_num);
                std::byte patchRgb_B = MMTFormat::PackNumberToByte(rgb_b_num);
                std::byte patchRgb_A = MMTFormat::PackNumberToByte(rgb_a_num);

                LOG.Info(L"rgb_r_num: " + std::to_wstring(rgb_r_num));
                LOG.Info(L"rgb_g_num: " + std::to_wstring(rgb_g_num));
                LOG.Info(L"rgb_b_num: " + std::to_wstring(rgb_b_num));
                LOG.Info(L"rgb_a_num: " + std::to_wstring(rgb_a_num));

                //ȷ��Color���ڵĲ�λƫ��
                int colorOffset = 0;
                LOG.Info(L"colorOffset: " + std::to_wstring(colorOffset));

                for (std::string elementName : basicConfig.TmpElementList) {
                    D3D11Element d3d11Element = d3d11GameType.ElementNameD3D11ElementMap[elementName];

                    //���ȱ�������color���ڵĲ�λ
                    if (d3d11Element.Category == MMTString::ToByteString(colorCategory)) {
                        if (elementName != "COLOR") {
                            colorOffset += d3d11Element.ByteWidth;
                        }
                        else {
                            break;
                        }
                    }
                }

                //��ʼ����
                std::vector<std::byte> newTexcoordCategoryValues;
                for (std::size_t i = 0; i < colorCategoryValues.size(); i = i + colorCategoryStride)
                {
                    //�Ȼ�ȡCOLORǰ������ֵ
                    std::vector<std::byte> beforeValues = MMTFormat::GetRange_Byte(colorCategoryValues, i, i + colorOffset);
                    std::vector<std::byte> ColorValues = MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset, i + colorOffset + 4);
                    std::vector<std::byte> afterValues = MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset + 4, i + colorCategoryStride);
                    //��׼����COLORֵ
                    std::vector<std::byte> newColorValues;

                    if (rgb_r_num != -1) {
                        newColorValues.push_back(patchRgb_R);
                    }
                    else {
                        newColorValues.push_back(MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset, i + colorOffset + 1)[0]);
                    }

                    if (rgb_g_num != -1) {
                        newColorValues.push_back(patchRgb_G);
                    }
                    else {
                        newColorValues.push_back(MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset + 1, i + colorOffset + 2)[0]);
                    }

                    if (rgb_b_num != -1) {
                        newColorValues.push_back(patchRgb_B);
                    }
                    else {
                        newColorValues.push_back(MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset + 2, i + colorOffset + 3)[0]);
                    }

                    if (rgb_a_num != -1) {
                        newColorValues.push_back(patchRgb_A);
                    }
                    else {
                        newColorValues.push_back(MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset + 3, i + colorOffset + 4)[0]);
                    }

                    newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), beforeValues.begin(), beforeValues.end());
                    newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), newColorValues.begin(), newColorValues.end());
                    newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), afterValues.begin(), afterValues.end());
                }

                finalVBCategoryDataMap[colorCategory] = newTexcoordCategoryValues;
                LOG.NewLine();
            }

            
        }
        else if (colorElement.Format == DXGI_FORMAT::R32G32B32A32_FLOAT) {
            //׼�������������滻
            float rgb_r_num = -1;
            float rgb_g_num = -1;
            float rgb_b_num = -1;
            float rgb_a_num = -1;

            //���Խ��и�ʽת��
            if (L"default" != rgb_r) {
                rgb_r_num = std::stoi(rgb_r) / 255.0f;
            }
            if (L"default" != rgb_g) {
                rgb_g_num = std::stoi(rgb_g) / 255.0f;
            }
            if (L"default" != rgb_b) {
                rgb_b_num = std::stoi(rgb_b) / 255.0f;
            }
            if (L"default" != rgb_a) {
                rgb_a_num = std::stoi(rgb_a) / 255.0f;
            }

            LOG.Info(L"rgb_r_num: " + std::to_wstring(rgb_r_num));
            LOG.Info(L"rgb_g_num: " + std::to_wstring(rgb_g_num));
            LOG.Info(L"rgb_b_num: " + std::to_wstring(rgb_b_num));
            LOG.Info(L"rgb_a_num: " + std::to_wstring(rgb_a_num));

            //�������default�Ǹɴ�ɶҲ���ֱ�ӷ���
            if (rgb_a_num == -1 && rgb_g_num == -1 && rgb_b_num == -1 && rgb_a_num == -1) {
                LOG.Info(L"δ��⵽��Ҫ�޸ĵ�COLORֵ����");
                LOG.NewLine();
            }
            else {
                //TODO Ҫʵ������Ļ����ͱ�����ʵ��D3D11����Щ���ݸ�ʽ��byte��ʽ�����ת����
             //׼������������д��
                std::vector<std::byte> patchRgb_R = MMTFormat::PackNumberR32_FLOAT_littleIndian(rgb_r_num);
                std::vector<std::byte> patchRgb_G = MMTFormat::PackNumberR32_FLOAT_littleIndian(rgb_g_num);
                std::vector<std::byte> patchRgb_B = MMTFormat::PackNumberR32_FLOAT_littleIndian(rgb_b_num);
                std::vector<std::byte> patchRgb_A = MMTFormat::PackNumberR32_FLOAT_littleIndian(rgb_a_num);
                LOG.Info(L"patchRgb_R.size(): " + std::to_wstring(patchRgb_R.size()));
                LOG.Info(L"patchRgb_G.size(): " + std::to_wstring(patchRgb_G.size()));
                LOG.Info(L"patchRgb_B.size(): " + std::to_wstring(patchRgb_B.size()));
                LOG.Info(L"patchRgb_A.size(): " + std::to_wstring(patchRgb_A.size()));

                //ȷ��Color���ڵĲ�λƫ��
                int colorOffset = 0;
                for (std::string elementName : basicConfig.TmpElementList) {
                    D3D11Element d3d11Element = d3d11GameType.ElementNameD3D11ElementMap[elementName];

                    //���ȱ�������color���ڵĲ�λ
                    if (d3d11Element.Category == MMTString::ToByteString(colorCategory)) {
                        if (elementName != "COLOR") {
                            colorOffset += d3d11Element.ByteWidth;
                        }
                        else {
                            break;
                        }
                    }
                }
                LOG.Info(L"colorOffset: " + std::to_wstring(colorOffset));
                LOG.Info(L"colorCategoryStride: " + std::to_wstring(colorCategoryStride));
                LOG.Info(L"colorCategoryValues.size(): " + std::to_wstring(colorCategoryValues.size()));

                //��ʼ����
                std::vector<std::byte> newTexcoordCategoryValues;
                for (std::size_t i = 0; i < colorCategoryValues.size(); i = i + colorCategoryStride)
                {
                    //�Ȼ�ȡCOLORǰ������ֵ
                    std::vector<std::byte> beforeValues = MMTFormat::GetRange_Byte(colorCategoryValues, i, i + colorOffset);
                    std::vector<std::byte> ColorValues = MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset, i + colorOffset + 16);
                    std::vector<std::byte> afterValues = MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset + 16, i + colorCategoryStride);
                    //��׼����COLORֵ
                    std::vector<std::byte> newColorValues;

                    if (rgb_r_num != -1.0f) {
                        newColorValues.insert(newColorValues.end(), patchRgb_R.begin(), patchRgb_R.end());
                    }
                    else {
                        std::vector<std::byte> tmpValues = MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset, i + colorOffset + 4);
                        newColorValues.insert(newColorValues.end(), tmpValues.begin(), tmpValues.end());

                    }

                    if (rgb_g_num != -1.0f) {
                        newColorValues.insert(newColorValues.end(), patchRgb_G.begin(), patchRgb_G.end());

                    }
                    else {
                        std::vector<std::byte> tmpValues = MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset + 4, i + colorOffset + 8);
                        newColorValues.insert(newColorValues.end(), tmpValues.begin(), tmpValues.end());

                    }

                    if (rgb_b_num != -1.0f) {
                        newColorValues.insert(newColorValues.end(), patchRgb_B.begin(), patchRgb_B.end());
                    }
                    else {
                        std::vector<std::byte> tmpValues = MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset + 8, i + colorOffset + 12);
                        newColorValues.insert(newColorValues.end(), tmpValues.begin(), tmpValues.end());


                    }

                    if (rgb_a_num != -1.0f) {
                        newColorValues.insert(newColorValues.end(), patchRgb_A.begin(), patchRgb_A.end());
                    }
                    else {
                        std::vector<std::byte> tmpValues = MMTFormat::GetRange_Byte(colorCategoryValues, i + colorOffset + 12, i + colorOffset + 16);
                        newColorValues.insert(newColorValues.end(), tmpValues.begin(), tmpValues.end());

                    }

                    newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), beforeValues.begin(), beforeValues.end());
                    newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), newColorValues.begin(), newColorValues.end());
                    newTexcoordCategoryValues.insert(newTexcoordCategoryValues.end(), afterValues.begin(), afterValues.end());
                }

                finalVBCategoryDataMap[colorCategory] = newTexcoordCategoryValues;
                LOG.NewLine();
            }
        }
        else {
            LOG.Warning(L"��֧�ֵ�COLORֵ��ʽ: " + MMTString::ToWideString(colorElement.Format) + L" ����COLORֵ���ã���ʹ��VertexColorMaster�����Blender����ˢCOLORֵ��");
        }
        LOG.NewLine();
    };




    //TODO �����TANGENT���㲽������д�����������仯
    void Unity_TANGENT_AverageNormal(std::unordered_map<std::wstring, std::vector<std::byte>>& inputVBCategoryDataMap, D3D11GameType d3d11GameType) {
        LOG.Info(L"Start to recalculate tangent use averageNormal algorithm:");
        std::unordered_map<std::wstring, std::vector<std::byte>> newVBCategoryDataMap;
        //��ȡPosition���������
        std::vector<std::byte> PositionCategoryValues = inputVBCategoryDataMap[L"Position"];

        //��ȡPOSITION��TANGENT
        std::vector<std::vector<double>> POSITION_VALUES;
        std::vector<std::vector<double>> NORMAL_VALUES;

        int positionStride = 0;
        //��ΪPOSITION���������TEXCOORD�����Ķ�̬��������������ֱ�ӻ�ȡOrderedElementList��ͳ�Ƴ�Position�ĳ���
        for (std::string elementName : d3d11GameType.OrderedFullElementList) {
            D3D11Element d3d11Element = d3d11GameType.ElementNameD3D11ElementMap[elementName];
            if (d3d11Element.Category == "Position") {
                int byteWidth = d3d11Element.ByteWidth;
                positionStride += byteWidth;

            }
        }
        LOG.Info(L"PositionStride: " + std::to_wstring(positionStride));
        for (std::size_t i = 0; i < PositionCategoryValues.size(); i = i + positionStride)
        {
            std::vector<std::byte> POSITION_X = MMTFormat::GetRange_Byte(PositionCategoryValues, i, i + 4);
            //LOG.LogOutput(L"POSITION_X: " + std::to_wstring(bytesToFloat(POSITION_X)));
            std::vector<std::byte> POSITION_Y = MMTFormat::GetRange_Byte(PositionCategoryValues, i + 4, i + 8);
            std::vector<std::byte> POSITION_Z = MMTFormat::GetRange_Byte(PositionCategoryValues, i + 8, i + 12);
            std::vector<double> POSITION_DOUBLE = { MMTFormat::ByteVectorToFloat(POSITION_X), MMTFormat::ByteVectorToFloat(POSITION_Y) ,MMTFormat::ByteVectorToFloat(POSITION_Z) };
            POSITION_VALUES.push_back(POSITION_DOUBLE);

            std::vector<std::byte> NORMAL_X = MMTFormat::GetRange_Byte(PositionCategoryValues, i + 12, i + 16);
            std::vector<std::byte> NORMAL_Y = MMTFormat::GetRange_Byte(PositionCategoryValues, i + 16, i + 20);
            std::vector<std::byte> NORMAL_Z = MMTFormat::GetRange_Byte(PositionCategoryValues, i + 20, i + 24);
            std::vector<double> NORMAL_DOUBLE = { MMTFormat::ByteVectorToFloat(NORMAL_X), MMTFormat::ByteVectorToFloat(NORMAL_Y) ,MMTFormat::ByteVectorToFloat(NORMAL_Z) };
            NORMAL_VALUES.push_back(NORMAL_DOUBLE);
        }
        LOG.Info(L"Start to CalculateAverageNormalsAndStoreTangent");
        //�Ѷ�����ͬ�ķ��� 3ά���� ���Ȼ���һ������Ϊ����ֵΪ�������ֵ����ͬ����
        //Ȼ���ÿ����Ӧ��TANGENT���������ֵ��Ϊǰ3λ��0��Ϊ���һλ

        //�����ƽ�ַ��ߵ�TANGENT
        std::vector<std::vector<double>> TANGENT_VALUES = MMTFormat::CalculateAverageNormalsAndStoreTangent(POSITION_VALUES, NORMAL_VALUES);
        LOG.Info(L"TANGENT_VALUES size: " + std::to_wstring(TANGENT_VALUES.size()));
        LOG.Info(L"CalculateAverageNormalsAndStoreTangent success");

        //����һ���µ�vector����װ�޸ĺ��Position���������
        std::vector<std::byte> newPositionCategoryValues;
        //���TANGENT���µ�newPositionCategoryValues
        int count = 0;
        for (std::size_t i = 0; i < PositionCategoryValues.size(); i = i + positionStride)
        {
            //TODO �����positionStride - 16�õ��ľ��ǳ���TANGENT֮���������element���ܳ���
            //��Ϊ��HI3Body2�У�Position����POSITION��NORMAL��COLOR��TANGENT��ɵģ���������������POSITION��NORMAL��TANGENT
            //������Ȼ������ʱ�����ˣ����Ǽ����Բ����ã��Ժ������޸İɡ�
            std::vector<std::byte> POSITION_NORMAL_VALUES = MMTFormat::GetRange_Byte(PositionCategoryValues, i, i + (positionStride - 16));

            std::vector<double> TANGENT_DOUBLES = TANGENT_VALUES[count];

            std::vector<std::byte> TANGENT_X;
            std::vector<std::byte> TANGENT_Y;
            std::vector<std::byte> TANGENT_Z;
            std::vector<std::byte> TANGENT_W;

            double tangent_double_x = TANGENT_DOUBLES[0];

            TANGENT_X = MMTFormat::PackNumberR32_FLOAT_littleIndian((float)TANGENT_DOUBLES[0]);
            TANGENT_Y = MMTFormat::PackNumberR32_FLOAT_littleIndian((float)TANGENT_DOUBLES[1]);
            TANGENT_Z = MMTFormat::PackNumberR32_FLOAT_littleIndian((float)TANGENT_DOUBLES[2]);
            //TANGENT��W����Ҫô��-1Ҫô��1�������ɵ�ǰ��������Ծ����ģ���������ʹ��Blender����ʱ�Դ��ķ�����һ��
            //TANGENT_W = GetRange(PositionCategoryValues, i + 36, i + 40);
            TANGENT_W = MMTFormat::PackNumberR32_FLOAT_littleIndian((float)TANGENT_DOUBLES[3]);

            //LOG.LogOutput(L"Tangent: " + formatedFloat(TANGENT_DOUBLES[0],10) + L"," + formatedFloat(TANGENT_DOUBLES[1],10) + L"," + formatedFloat(TANGENT_DOUBLES[2], 10) + L"," + formatedFloat(TANGENT_DOUBLES[3], 10));

            newPositionCategoryValues.insert(newPositionCategoryValues.end(), POSITION_NORMAL_VALUES.begin(), POSITION_NORMAL_VALUES.end());

            newPositionCategoryValues.insert(newPositionCategoryValues.end(), TANGENT_X.begin(), TANGENT_X.end());
            newPositionCategoryValues.insert(newPositionCategoryValues.end(), TANGENT_Y.begin(), TANGENT_Y.end());
            newPositionCategoryValues.insert(newPositionCategoryValues.end(), TANGENT_Z.begin(), TANGENT_Z.end());
            newPositionCategoryValues.insert(newPositionCategoryValues.end(), TANGENT_W.begin(), TANGENT_W.end());



            count = count + 1;
            //LOG.LogOutput(L"Count: " + std::to_wstring(count));
        }

        LOG.NewLine();
        inputVBCategoryDataMap[L"Position"] = newPositionCategoryValues;
    };



    //ͨ����ȡ�Ĳ�������תNormal��X��Y��Z��������תTangent��X��Y��Z��W����
    void Unity_Reverse_NORMAL_TANGENT(
        std::unordered_map<std::wstring, std::vector<std::byte>>& finalVBCategoryDataMap, DrawIBConfig basicConfig, D3D11GameType d3d11GameType) {
        LOG.Info(L"Start to flip NORMAL and TANGENT values");
        std::unordered_map<std::wstring, std::vector<std::byte>> newVBCategoryDataMap;
        std::vector<std::byte> PositionCategoryValues = finalVBCategoryDataMap[L"Position"];

        //����Ҫ�õ�TmpElementList��Ȼ��ͳ�Ƴ�����Position��λ��Ԫ������Щ
        //D3D11GameType d3d11GameType = GameTypeName_D3d11GameType_Map[basicConfig.WorkGameType];
        std::vector<std::string> positionElementList;
        int positionStride = 0;
        for (std::string elementName : basicConfig.TmpElementList) {
            D3D11Element elementObject = d3d11GameType.ElementNameD3D11ElementMap[elementName];
            std::wstring category = MMTString::ToWideString(elementObject.Category);
            if (category == L"Position") {
                LOG.Info(L"Detect Position category element: " + MMTString::ToWideString(elementName) + L"  stride: " + std::to_wstring(elementObject.ByteWidth));
                positionElementList.push_back(elementName);
                positionStride += elementObject.ByteWidth;

            }
        }
        LOG.Info(L"PositionStride: " + std::to_wstring(positionStride));

        // Ȼ��̬�Ķ�ȡֵ��Ȼ��ֻ�޸�NORMAL��TANGENTֵ
        std::vector<std::byte> newPositionCategoryValues;
        for (std::size_t i = 0; i < PositionCategoryValues.size(); i = i + positionStride) {

            std::vector<std::byte> newPOSITIONValues;
            //����positionElementList������ֻ����NORMAL��TANGENT������ԭ�ⲻ���ŵ����б�
            int offset = 0;
            for (std::string elementName : positionElementList) {
                //LOG.LogOutput(L"Tmp  offset: " + std::to_wstring(offset));
                D3D11Element elementObject = d3d11GameType.ElementNameD3D11ElementMap[elementName];
                std::wstring format = MMTString::ToWideString(elementObject.Format);
                int elementStrie = elementObject.ByteWidth;

                //LOG.LogOutput(L"Tmp  format: " + format);
                //LOG.LogOutput(L"Tmp  elementStrie: " + std::to_wstring(elementStrie));


                std::vector<std::byte> TmpValues = MMTFormat::GetRange_Byte(PositionCategoryValues, i + offset, i + offset + elementStrie);
                if (elementName == "NORMAL") {
                    // TODO �������������������ת��Ӧ������Ŀǰ����NORMAL���������ͣ�һ��R32G32B32_FLOAT һ��R16G16B16A16_FLOAT����A16ȫ��0
                    if (format == L"R32G32B32_FLOAT") {
                        std::vector<std::byte> FlipedValues;

                        std::vector<std::byte> ValueR = MMTFormat::GetRange_Byte(TmpValues, 0, 4);
                        std::vector<std::byte> ValueG = MMTFormat::GetRange_Byte(TmpValues, 4, 8);
                        std::vector<std::byte> ValueB = MMTFormat::GetRange_Byte(TmpValues, 8, 12);

                        std::vector<std::byte> ValueR_Reversed = basicConfig.NormalReverseX ? MMTFormat::Reverse32BitFloatValue(ValueR) : ValueR;
                        std::vector<std::byte> ValueG_Reversed = basicConfig.NormalReverseY ? MMTFormat::Reverse32BitFloatValue(ValueG) : ValueG;
                        std::vector<std::byte> ValueB_Reversed = basicConfig.NormalReverseZ ? MMTFormat::Reverse32BitFloatValue(ValueB) : ValueB;

                        FlipedValues.insert(FlipedValues.end(), ValueR_Reversed.begin(), ValueR_Reversed.end());
                        FlipedValues.insert(FlipedValues.end(), ValueG_Reversed.begin(), ValueG_Reversed.end());
                        FlipedValues.insert(FlipedValues.end(), ValueB_Reversed.begin(), ValueB_Reversed.end());

                        newPOSITIONValues.insert(newPOSITIONValues.end(), FlipedValues.begin(), FlipedValues.end());
                    }
                    else if (format == L"R16G16B16A16_FLOAT") {
                        std::vector<std::byte> FlipedValues;

                        std::vector<std::byte> ValueR = MMTFormat::GetRange_Byte(TmpValues, 0, 2);
                        std::vector<std::byte> ValueG = MMTFormat::GetRange_Byte(TmpValues, 2, 4);
                        std::vector<std::byte> ValueB = MMTFormat::GetRange_Byte(TmpValues, 4, 6);
                        std::vector<std::byte> ValueA = MMTFormat::GetRange_Byte(TmpValues, 6, 8);

                        std::vector<std::byte> ValueR_Reversed = basicConfig.NormalReverseX ? MMTFormat::Reverse16BitShortValue(ValueR) : ValueR;
                        std::vector<std::byte> ValueG_Reversed = basicConfig.NormalReverseY ? MMTFormat::Reverse16BitShortValue(ValueG) : ValueG;
                        std::vector<std::byte> ValueB_Reversed = basicConfig.NormalReverseZ ? MMTFormat::Reverse16BitShortValue(ValueB) : ValueB;

                        FlipedValues.insert(FlipedValues.end(), ValueR_Reversed.begin(), ValueR_Reversed.end());
                        FlipedValues.insert(FlipedValues.end(), ValueG_Reversed.begin(), ValueG_Reversed.end());
                        FlipedValues.insert(FlipedValues.end(), ValueB_Reversed.begin(), ValueB_Reversed.end());
                        //NORMAL��A������Զ��0������Ҫ��ת
                        FlipedValues.insert(FlipedValues.end(), ValueA.begin(), ValueA.end());

                        newPOSITIONValues.insert(newPOSITIONValues.end(), FlipedValues.begin(), FlipedValues.end());
                    }
                    else {
                        newPOSITIONValues.insert(newPOSITIONValues.end(), TmpValues.begin(), TmpValues.end());
                        //LOG.Warning(L"Can't find any format for NORMAL vlaues,use original NORMAL values");
                    }

                }
                else if (elementName == "TANGENT") {

                    if (format == L"R32G32B32A32_FLOAT") {
                        std::vector<std::byte> FlipedValues;

                        std::vector<std::byte> ValueR = MMTFormat::GetRange_Byte(TmpValues, 0, 4);
                        std::vector<std::byte> ValueG = MMTFormat::GetRange_Byte(TmpValues, 4, 8);
                        std::vector<std::byte> ValueB = MMTFormat::GetRange_Byte(TmpValues, 8, 12);
                        std::vector<std::byte> ValueA = MMTFormat::GetRange_Byte(TmpValues, 12, 16);

                        std::vector<std::byte> ValueR_Reversed = basicConfig.TangentReverseX ? MMTFormat::Reverse32BitFloatValue(ValueR) : ValueR;
                        std::vector<std::byte> ValueG_Reversed = basicConfig.TangentReverseY ? MMTFormat::Reverse32BitFloatValue(ValueG) : ValueG;
                        std::vector<std::byte> ValueB_Reversed = basicConfig.TangentReverseZ ? MMTFormat::Reverse32BitFloatValue(ValueB) : ValueB;
                        std::vector<std::byte> ValueA_Reversed = basicConfig.TangentReverseW ? MMTFormat::Reverse32BitFloatValue(ValueA) : ValueA;

                        FlipedValues.insert(FlipedValues.end(), ValueR_Reversed.begin(), ValueR_Reversed.end());
                        FlipedValues.insert(FlipedValues.end(), ValueG_Reversed.begin(), ValueG_Reversed.end());
                        FlipedValues.insert(FlipedValues.end(), ValueB_Reversed.begin(), ValueB_Reversed.end());
                        FlipedValues.insert(FlipedValues.end(), ValueA_Reversed.begin(), ValueA_Reversed.end());

                        newPOSITIONValues.insert(newPOSITIONValues.end(), FlipedValues.begin(), FlipedValues.end());
                    }
                    else if (format == L"R16G16B16A16_FLOAT") {
                        std::vector<std::byte> FlipedValues;

                        std::vector<std::byte> ValueR = MMTFormat::GetRange_Byte(TmpValues, 0, 2);
                        std::vector<std::byte> ValueG = MMTFormat::GetRange_Byte(TmpValues, 2, 4);
                        std::vector<std::byte> ValueB = MMTFormat::GetRange_Byte(TmpValues, 4, 6);
                        std::vector<std::byte> ValueA = MMTFormat::GetRange_Byte(TmpValues, 6, 8);

                        std::vector<std::byte> ValueR_Reversed = basicConfig.TangentReverseX ? MMTFormat::Reverse16BitShortValue(ValueR) : ValueR;
                        std::vector<std::byte> ValueG_Reversed = basicConfig.TangentReverseY ? MMTFormat::Reverse16BitShortValue(ValueG) : ValueG;
                        std::vector<std::byte> ValueB_Reversed = basicConfig.TangentReverseZ ? MMTFormat::Reverse16BitShortValue(ValueB) : ValueB;

                        std::vector<std::byte> ValueA_Reversed = basicConfig.TangentReverseW ? MMTFormat::Reverse16BitShortValue(ValueA) : ValueA;
                        if (ValueA[0] == std::byte(0x00) && ValueA[1] == std::byte(0xBC)) {
                            ValueA_Reversed[0] = std::byte(0x00);
                            ValueA_Reversed[1] = std::byte(0x3C);
                        }
                        else if (ValueA[0] == std::byte(0x00) && ValueA[1] == std::byte(0x3C)) {
                            ValueA_Reversed[0] = std::byte(0x00);
                            ValueA_Reversed[1] = std::byte(0xBC);
                        }

                        FlipedValues.insert(FlipedValues.end(), ValueR_Reversed.begin(), ValueR_Reversed.end());
                        FlipedValues.insert(FlipedValues.end(), ValueG_Reversed.begin(), ValueG_Reversed.end());
                        FlipedValues.insert(FlipedValues.end(), ValueB_Reversed.begin(), ValueB_Reversed.end());
                        FlipedValues.insert(FlipedValues.end(), ValueA_Reversed.begin(), ValueA_Reversed.end());

                        newPOSITIONValues.insert(newPOSITIONValues.end(), FlipedValues.begin(), FlipedValues.end());
                    }
                    else {
                        newPOSITIONValues.insert(newPOSITIONValues.end(), TmpValues.begin(), TmpValues.end());
                        //LOG.Warning(L"Can't find any format for TANGENT vlaues,use default TANGENT values.");
                    }

                }
                else {
                    //����ת�Ļ�ֱ����ӽ�ȥ������
                    newPOSITIONValues.insert(newPOSITIONValues.end(), TmpValues.begin(), TmpValues.end());

                }
                offset += elementStrie;

            }

            //�������POSITION values��ӵ��ܵ�Position��λ��
            newPositionCategoryValues.insert(newPositionCategoryValues.end(), newPOSITIONValues.begin(), newPOSITIONValues.end());
        }

        LOG.Info(L"Size: newPositionCategoryValues : " + std::to_wstring(newPositionCategoryValues.size()));
        LOG.Info(L"Size: PositionCategoryValues : " + std::to_wstring(PositionCategoryValues.size()));

        finalVBCategoryDataMap[L"Position"] = newPositionCategoryValues;
        LOG.NewLine();
    };


}