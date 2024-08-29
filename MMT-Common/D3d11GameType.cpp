#include "D3d11GameType.h"
#include "MMTStringUtils.h"

//����ElementList��������stride
uint32_t D3D11GameType::getElementListStride(std::vector<std::string> inputElementList) {
    uint32_t totalStride = 0;
	for (std::string elementName : inputElementList) {
		std::string elementNameUpper = boost::algorithm::to_upper_copy(elementName);
		D3D11Element elementObject = ElementNameD3D11ElementMap[elementNameUpper];
		totalStride = totalStride + elementObject.ByteWidth;
	}
	return totalStride;
}


D3D11GameType::D3D11GameType() {

}


//����ElementList��������stride
std::unordered_map<std::string, uint32_t>  D3D11GameType::getCategoryStrideMap(std::vector<std::string> inputElementList) {
    std::unordered_map<std::string, uint32_t> CategoryStrideMap;
    for (std::string elementName : inputElementList) {
        D3D11Element elementObject = this->ElementNameD3D11ElementMap[elementName];
        uint32_t byteWidth = elementObject.ByteWidth;
        std::wstring elementCategory = MMTString::ToWideString(elementObject.Category);
        uint32_t categoryStride = CategoryStrideMap[MMTString::ToByteString(elementCategory)];
        if (categoryStride == NULL) {
            categoryStride = byteWidth;
        }
        else {
            categoryStride = categoryStride + byteWidth;
        }
        CategoryStrideMap[MMTString::ToByteString(elementCategory)] = categoryStride;
    }
    return CategoryStrideMap;
}


std::vector<std::string>   D3D11GameType::getCategoryList(std::vector<std::string> inputElementList) {
    std::vector<std::string> CategoryList;
    for (std::string elementName : inputElementList) {
        D3D11Element elementObject = this->ElementNameD3D11ElementMap[elementName];
        int byteWidth = elementObject.ByteWidth;
        std::wstring elementCategory = MMTString::ToWideString(elementObject.Category);
        if (!boost::algorithm::any_of_equal(CategoryList, MMTString::ToByteString(elementCategory))) {
            CategoryList.push_back(MMTString::ToByteString(elementCategory));
        }

    }
    return CategoryList;
}


std::vector<std::string> D3D11GameType::getCategoryElementList(std::vector<std::string> inputElementList, std::string category) {
    std::vector <std::string> positionElementListTmp;
    for (std::string elementName : inputElementList) {
        D3D11Element d3d11ElementTmp = this->ElementNameD3D11ElementMap[elementName];
        if (d3d11ElementTmp.Category == category) {
            positionElementListTmp.push_back(elementName);
        }
    }
    return positionElementListTmp;
}


std::vector<std::string> D3D11GameType::getReorderedElementList(std::vector<std::string> elementList) {
    std::vector<std::string> orderedElementList;
    for (std::string elementName : this->OrderedFullElementList) {
        for (std::string element : elementList) {
            if (boost::algorithm::to_lower_copy(element) == boost::algorithm::to_lower_copy(elementName)) {
                orderedElementList.push_back(elementName);
                break;
            }
        }
    }
    return orderedElementList;
}

