#include "FmtFile.h"
#include <iostream>
#include "MMTStringUtils.h"
#include "MMTFileUtils.h"
#include "MMTLogUtils.h"


FmtFile::FmtFile() {

}


bool FmtFile::IsD3d11ElementListMatch(std::vector<std::string> TargetElementList) {
	//��֤GameType��ElementList������Ԫ���Ƿ�����һ��ElementList�г��ֹ�
	bool AllFind = true;
	for (std::string ElementName : TargetElementList) {
		std::string ElementNameLower = boost::algorithm::to_lower_copy(ElementName);
		//LOG.Info(L"��ǰ���ҵ�GameType��ElementName: " + MMTString::ToWideString(ElementNameLower));

		bool findElementName = false;
		for (D3D11Element d3d11Element : this->d3d11ElementList) {
			//LOG.Info(L"��ǰ�Աȣ�" + MMTString::ToWideString(d3d11Element.GetIndexedSemanticName()));
			std::string fmtElementNameLower = boost::algorithm::to_lower_copy(d3d11Element.GetIndexedSemanticName());
			//LOG.Info(L"��ǰ�ж�FMT�ļ��е�ElementName: " + MMTString::ToWideString(fmtElementNameLower));
			if (ElementNameLower == fmtElementNameLower) {
				findElementName = true;
				//LOG.Info(L"���ҵ�");
				break;
			}
		}
		if (!findElementName) {
			//LOG.Info(L"δ�ҵ�");
			AllFind = false;
			break;
		}
	}

	return AllFind;
}


void FmtFile::OutputFmtFile(std::wstring OutputFmtPath) {
	//���һ�ݹ̶���D3D11Element��fmt�ļ�
    std::wofstream outputFmtFile(OutputFmtPath);
    outputFmtFile << "stride: " << std::to_wstring(d3d11GameType.getElementListStride(this->ElementNameList)) << L"\n";
    outputFmtFile << "topology: trianglelist" << L"\n";
    outputFmtFile << "format: "<< this->Format << L"\n";

    int elementNumber = 0;
    int alignedByteOffset = 0;
    for (std::string elementName : this->ElementNameList) {
		std::string elementNameUpper = boost::algorithm::to_upper_copy(elementName);
        D3D11Element elementObject = this->d3d11GameType.ElementNameD3D11ElementMap[elementNameUpper];
        outputFmtFile << "element[" << elementNumber << "]:" << L"\n";
        outputFmtFile << "  SemanticName: " << elementObject.SemanticName.c_str() << L"\n";
        outputFmtFile << "  SemanticIndex: " << elementObject.SemanticIndex.c_str() << L"\n";
        outputFmtFile << "  Format: " << elementObject.Format.c_str() << L"\n";
        outputFmtFile << "  InputSlot: " << elementObject.InputSlot.c_str() << L"\n";
        outputFmtFile << "  AlignedByteOffset: " << alignedByteOffset << L"\n";
        alignedByteOffset = alignedByteOffset + elementObject.ByteWidth;
        outputFmtFile << "  InputSlotClass: " << elementObject.InputSlotClass.c_str() << L"\n";
        outputFmtFile << "  InstanceDataStepRate: " << elementObject.InstanceDataStepRate.c_str() << L"\n";
        elementNumber = elementNumber + 1;
    }

    outputFmtFile.close();
}

//TODO �������ʵ�Ǵ���ģ���������Ҳ���ã��Ͼ�����ƥ��fmt�ļ������㹻����
//����ע�ⲻ������ʶ��dump�ļ���
FmtFile::FmtFile(std::wstring fmtFilePath) {
	std::vector<std::wstring> fmtLines = MMTFile::ReadAllLinesW(fmtFilePath);

	D3D11Element d3d11Element;
	bool meetElementLine = false;
	int elementNumber = 0;
	for (std::wstring line : fmtLines) {

		boost::algorithm::to_lower(line);
		boost::algorithm::trim(line);

		//LOG.Info(L"Current Parsing Line: " + line);
		//û������Element֮ǰ���Ƚ�������
		if (!meetElementLine) {
			if (boost::algorithm::starts_with(line, "stride")) {
				std::wstring str;
				std::size_t colonPos = line.find(':');
				if (colonPos != std::wstring::npos) {
					// ��ȡð�ź���������ַ���+1 ��Ϊ������ð�ű���
					str = line.substr(colonPos + 1);
					boost::algorithm::trim(str);
				}
				this->Stride = std::stoi(str);
				continue;
			}
			else if (boost::algorithm::starts_with(line, "topology")) {
				
				std::wstring str;
				std::size_t colonPos = line.find(':');
				if (colonPos != std::wstring::npos) {
					// ��ȡð�ź���������ַ���+1 ��Ϊ������ð�ű���
					str = line.substr(colonPos + 1);
					boost::algorithm::trim(str);
				}
				this->Topology = str;
				continue;
			}
			else if (boost::algorithm::starts_with(line, "format")) {
				std::wstring str;
				std::size_t colonPos = line.find(':');
				if (colonPos != std::wstring::npos) {
					// ��ȡð�ź���������ַ���+1 ��Ϊ������ð�ű���
					str = line.substr(colonPos + 1);
					boost::algorithm::trim(str);
				}
				this->Format = str;
				continue;
			}
		}
		
		if (boost::algorithm::starts_with(line, "element")) {
			meetElementLine = true;
			//LOG.Info(L"Parse Element[]");
			d3d11Element.ElementNumber = elementNumber;
			elementNumber++;
			continue;
		}

		std::size_t colonPos = line.find(':');
		if (colonPos != std::wstring::npos) {
			std::wstring key = line.substr(0, colonPos);
			boost::algorithm::trim(key);
			
			std::wstring value = line.substr(colonPos + 1);
			boost::algorithm::trim(value);

			//LOG.Info(L"Current key: " + key + L" value: " + value);

			if (boost::algorithm::starts_with(key, "semanticname")) {
				d3d11Element.SemanticName = MMTString::ToByteString(value);
			}
			else if (boost::algorithm::starts_with(key, "semanticindex")) {
				d3d11Element.SemanticIndex = MMTString::ToByteString(value);
			}
			else if (boost::algorithm::starts_with(key, "format")) {
				d3d11Element.Format = MMTString::ToByteString(value);
			}
			else if (boost::algorithm::starts_with(key, "inputslot")) {
				d3d11Element.InputSlot = MMTString::ToByteString(value);
			}
			else if (boost::algorithm::starts_with(key, "alignedbyteoffset")) {
				d3d11Element.AlignedByteOffset = std::stoi(value);
			}
			else if (boost::algorithm::starts_with(key, "inputslotclass")) {
				d3d11Element.InputSlotClass = MMTString::ToByteString(value);
			}
			else if (boost::algorithm::starts_with(key, "instancedatasteprate")) {
				d3d11Element.InstanceDataStepRate = MMTString::ToByteString(value);
				//���������Ӧ�������һ��

				//LOG.Info("Add into list: " + d3d11Element.SemanticName);
				this->d3d11ElementList.push_back(d3d11Element);

			}
		}


	}
	//LOG.Info(L"Parse each line over.");
	//LOG.NewLine();

	//TODO ����Ҫ����AlignedByteOffset��ԭ��ÿ��d3d11Element��ByteWidth
	int lastAlignedByteOffset = 0;
	std::string lastSemanticName = "";
	int lastByteWidth = 0;
	std::unordered_map<std::string, int> semanticNameByteWidthMap;

	for (size_t i = 0; i < this->d3d11ElementList.size(); i++)
	{
		D3D11Element d3d11Element = this->d3d11ElementList[i];
		//LOG.Info("Processing: " + d3d11Element.SemanticName);
		if (i == 0) {
			lastSemanticName = d3d11Element.SemanticName;
			lastAlignedByteOffset = d3d11Element.AlignedByteOffset;
			continue;
		}
		else {
			lastByteWidth = d3d11Element.AlignedByteOffset - lastAlignedByteOffset;
			semanticNameByteWidthMap[lastSemanticName] = lastByteWidth;

			lastSemanticName = d3d11Element.SemanticName;
			lastAlignedByteOffset = d3d11Element.AlignedByteOffset;
		}
	}
	//�������һ��
	D3D11Element lastD3D11Element = this->d3d11ElementList[this->d3d11ElementList.size() - 1];
	lastByteWidth = this->Stride - lastD3D11Element.AlignedByteOffset;
	semanticNameByteWidthMap[lastD3D11Element.SemanticName] = lastByteWidth;
	//LOG.NewLine();


	//for (const auto& pair: semanticNameByteWidthMap) {
	//	std::string semanticName = pair.first;
	//	int byteWidth = pair.second;
	//	LOG.Info("SemanticName: " + semanticName + " ByteWidth: " + std::to_string(byteWidth));
	//}
	//LOG.NewLine();


	//TODO ����SemanticName������Ĭ�Ϸ���,˳�㸳ֵbyteWidth
	//�����������������Ǵ���ģ���fmt�ļ��ж�ȡ�������޷�����Category��Ϣ�ġ�
	//TODO ���������Ϊ��ѡ�����ֱ��ȥ������ʱ���ܡ�
	std::vector<D3D11Element> newD3D11ElementList;
	for (D3D11Element d3d11Element: this->d3d11ElementList) {
		d3d11Element.ByteWidth = semanticNameByteWidthMap[d3d11Element.SemanticName];
		if (d3d11Element.SemanticName == "position" 
			|| d3d11Element.SemanticName == "normal"
			|| d3d11Element.SemanticName == "tangent") {
			d3d11Element.Category = "Position";
		}

		if (d3d11Element.SemanticName == "color"
			|| d3d11Element.SemanticName == "texcoord"
			|| d3d11Element.SemanticName == "texcoord1"
			|| d3d11Element.SemanticName == "texcoord2") {

			d3d11Element.Category = "Texcoord";
		}

		if (d3d11Element.SemanticName == "blendweight"
			|| d3d11Element.SemanticName == "blendweights"
			|| d3d11Element.SemanticName == "blendindices") {
			d3d11Element.Category = "Blend";
		}
		newD3D11ElementList.push_back(d3d11Element);

	}
	this->d3d11ElementList = newD3D11ElementList;

}