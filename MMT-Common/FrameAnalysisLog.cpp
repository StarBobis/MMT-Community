#include "FrameAnalysisLog.h"

#include "MMTFileUtils.h"
#include "MMTLogUtils.h"
#include "MMTStringUtils.h"


FrameAnalysisLog::FrameAnalysisLog() {


}


FrameAnalysisLog::FrameAnalysisLog(std::wstring InputWorkFolder) {
	this->WorkFolder = InputWorkFolder;
	std::wstring LogTxtFilePath = InputWorkFolder + L"log.txt";
	if (!std::filesystem::exists(LogTxtFilePath)) {
		LOG.Error(L"�޷��ҵ�FrameAnalysis�ļ����е�log.txt: " + LogTxtFilePath);
	}
	std::vector<std::wstring> AllLineList = MMTFile::ReadAllLinesW(LogTxtFilePath);
	
	std::wstring CurrentIndex = L"";
	DrawCall CurrentDrawCall;
	for (std::wstring Line: AllLineList) {
		std::wstring LowerLine = boost::algorithm::to_lower_copy(Line);
		//���������0��ͷ�ģ�˵������Ҫ�ҵ���
		if (!LowerLine.starts_with(L"0")) {
			continue;
		}

		std::wstring Index = LowerLine.substr(0,6);
		if (Index != CurrentIndex) {
			//˵��������Index��������ǰDrawCall����Map������Ϊ�¶���
			if (CurrentIndex != L"") {
				this->Index_DrawCall_Map[CurrentIndex] = CurrentDrawCall;
			}
			CurrentDrawCall = DrawCall();
			CurrentIndex = Index;
		}

		//���substr 7������3dmigoto��ͷ��˵����dump�У����Էŵ�
		std::wstring FuncLine = LowerLine.substr(7);
		if (FuncLine.starts_with(L"3dmigoto")) {
			CurrentDrawCall.DumpCallStringList.push_back(Line);
		}
		else {
			CurrentDrawCall.APICallStringList.push_back(Line);
		}
	}

}


int FrameAnalysisLog::GetIndexDrawCallPsSetShaderResourcesTime(std::wstring Index) {
	DrawCall drawCall = Index_DrawCall_Map[Index];
	int time = 0;
	for (std::wstring ApiCall : drawCall.APICallStringList) {
		std::wstring ApiCallLower = boost::algorithm::to_lower_copy(ApiCall);
		if (ApiCallLower.find(L"pssetshaderresources") != std::wstring::npos) {
			LOG.Info(ApiCallLower);
			time++;
		}
	}
	return time;
}

uint32_t FrameAnalysisLog::GetIndexDrawCallPsSetConstantBuffersTime(std::wstring Index) {
	DrawCall drawCall = Index_DrawCall_Map[Index];
	uint32_t time = 0;
	for (std::wstring ApiCall : drawCall.APICallStringList) {
		std::wstring ApiCallLower = boost::algorithm::to_lower_copy(ApiCall);
		if (ApiCallLower.find(L"pssetconstantbuffers") != std::wstring::npos) {
			LOG.Info(ApiCallLower);
			time++;
		}
	}
	return time;
}



std::wstring FrameAnalysisLog::FindRealDedupedResourceFileNameFromDumpCallLog(std::wstring Index, std::wstring FakeFileName) {
	DrawCall indexDrawCall = Index_DrawCall_Map[Index];
	for (std::wstring DumpCallString : indexDrawCall.DumpCallStringList) {
		if (DumpCallString.find(FakeFileName) != std::wstring::npos) {
			std::vector<std::wstring> splits = MMTString::SplitString(DumpCallString, L"->");
			std::wstring NewTexturePath = boost::algorithm::trim_copy(splits[1]);
			return MMTString::GetFileNameFromFilePath(NewTexturePath);
		}
	}
	return L"";
}

uint32_t FrameAnalysisLog::FindRealDrawIndexedNumberFromLogByIndex(std::wstring Index) {
	//����Ҫ�ҵ���ǰDrawIB����ʵFirstIndex��IndexCount����log.txt�ļ�����
	std::vector<std::wstring> logFileLineList = MMTFile::ReadAllLinesW(this->WorkFolder + L"log.txt");
	uint32_t realIndexCount = 0;
	std::wstring target = Index + L" 3DMigoto";
	std::wstring target2 = L"DrawIndexed";
	LOG.Info(L"Target:" + target);
	LOG.Info(std::to_string(logFileLineList.size()));
	for (std::wstring logLine : logFileLineList) {
		size_t poss = logLine.find(target);
		size_t poss2 = logLine.find(target2);

		//WWMI V0.6.0�ķ�ʽ
		//TODO �����������ͨ�÷���
		if (poss != std::wstring::npos && poss2 != std::wstring::npos) {
			LOG.Info(logLine);
			//��ȡDrawIndexed��ߵ�����
			std::wstring endLine = logLine.substr(poss2 + target2.length());

			//ȥ�����ߵ�����
			std::wstring drawNumberStr = endLine.substr(1, endLine.length() - 1);
			std::vector<std::wstring> numList = MMTString::SplitString(drawNumberStr, L",");
			std::wstring DrawIndexCount = numList[0];
			LOG.Info(L"DrawIndexCount:" + DrawIndexCount);

			std::wstring DrawFirstIndex = numList[1];
			LOG.Info(L"DrawFirstIndex:" + DrawFirstIndex);

			boost::algorithm::trim(DrawIndexCount);
			boost::algorithm::trim(DrawFirstIndex);
			realIndexCount = std::stoi(DrawIndexCount);
			break;
		}

	}
	return realIndexCount;
}


uint32_t FrameAnalysisLog::FindRealDrawIndexedFirstIndexFromLogByIndex(std::wstring Index) {
	//����Ҫ�ҵ���ǰDrawIB����ʵFirstIndex��IndexCount����log.txt�ļ�����
	std::vector<std::wstring> logFileLineList = MMTFile::ReadAllLinesW(this->WorkFolder + L"log.txt");
	uint32_t realFirstIndex = 0;
	std::wstring target = Index + L" 3DMigoto";
	std::wstring target2 = L"DrawIndexed";
	LOG.Info(L"Target:" + target);
	LOG.Info(std::to_string(logFileLineList.size()));
	for (std::wstring logLine : logFileLineList) {
		size_t poss = logLine.find(target);
		size_t poss2 = logLine.find(target2);

		//WWMI V0.6.0�ķ�ʽ
		//TODO �����������ͨ�÷���
		if (poss != std::wstring::npos && poss2 != std::wstring::npos) {
			LOG.Info(logLine);
			//��ȡDrawIndexed��ߵ�����
			std::wstring endLine = logLine.substr(poss2 + target2.length());

			//ȥ�����ߵ�����
			std::wstring drawNumberStr = endLine.substr(1, endLine.length() - 1);
			std::vector<std::wstring> numList = MMTString::SplitString(drawNumberStr, L",");
			std::wstring DrawIndexCount = numList[0];
			LOG.Info(L"DrawIndexCount:" + DrawIndexCount);

			std::wstring DrawFirstIndex = numList[1];
			LOG.Info(L"DrawFirstIndex:" + DrawFirstIndex);

			boost::algorithm::trim(DrawIndexCount);
			boost::algorithm::trim(DrawFirstIndex);
			realFirstIndex = std::stoi(DrawFirstIndex);
			break;
		}

	}
	return realFirstIndex;
}