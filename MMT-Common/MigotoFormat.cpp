#include "MigotoFormat.h"
#include <iostream>


//--------------------------------------------------------------------------------------------------------------------
M_Condition::M_Condition() {

}


bool M_Condition::isActive(std::unordered_map<std::wstring, std::wstring> ActiveKeyValueMap) {
	//����key��ϣ��ж�����ÿһ��Expression�Ƿ���Ч
	//����ּ�������ֱ�����ж�
	//LOG.NewLine();
	//LOG.Info("Start to calculate if condition is active:");
	if (this->LogicList.size() == 0) {
		//LOG.Info("LogicList is empty, only one single expression detected:");
		//û���߼�����˵��ֻ��һ������
		M_ConditionExpression condition_expression = this->ConditionExpressionList[0];

		if (condition_expression.VarValue == ActiveKeyValueMap[condition_expression.VarName]) {
			//LOG.Info(L"Expression VarName: " + condition_expression.VarName);
			//LOG.Info(L"Expression VarValue: " + condition_expression.VarValue);
			//LOG.Info("Find match key value! expression active!");
			return true;
		}
		else {
			//LOG.Info(L"Match Key Value: " + ActiveKeyValueMap[condition_expression.VarName]);
			//LOG.Info("Can't find match key value in ActiveKeyValueMap, expression not active!");
			return false;
		}
	}
	else {
		//LOG.Info("LogicList is not empty, multiple expression detected:");
		//���߼�����˵���ж��������������Ҫ�ֶ���������д���һ����ֻ��&&�������һ����&& �� || ��������
		//TODO ��������Ŀǰ������Mod�������ֻʹ�õ��˼򵥵�&&�߼���������ʱ����֧��||

		//�����Ƿ������||
		bool findOrLogic = false;
		for (std::wstring LogicStr : this->LogicList) {
			if (LogicStr == L"||") {
				findOrLogic = true;
			}
		}
		if (!findOrLogic) {
			//���û�ҵ� || ˵�����е�Expression��������Ч����
			bool allLogicMatch = true;

			//TODO 
			for (M_ConditionExpression condition_expression : this->ConditionExpressionList) {
				//LOG.Info(L"Expression VarName: " + condition_expression.VarName);
				//LOG.Info(L"Expression VarValue: " + condition_expression.VarValue);
				if (condition_expression.VarValue != ActiveKeyValueMap[condition_expression.VarName]) {
					//LOG.Info(L"Match Key Value: " + ActiveKeyValueMap[condition_expression.VarName]);
					allLogicMatch = false;
					break;
				}
			}

			if (allLogicMatch) {
				//LOG.Info("All key value match, condition active.");
				return true;
			}
			else {
				//LOG.Info("Not all key value match in all && logic, condition active fail.");
				return false;
			}


		}
		else {
			//�ں���||ʱ��������ô�СΪLogicList��С+1��˵��ȫ��||����ʱ�����ֱ������һ����Ч��������Ч
			if (this->ConditionExpressionList.size() == this->LogicList.size() + 1) {
				for (M_ConditionExpression condition_expression : this->ConditionExpressionList) {
					LOG.Info(L"Expression VarName: " + condition_expression.VarName);
					LOG.Info(L"Expression VarValue: " + condition_expression.VarValue);

					if (condition_expression.VarValue == ActiveKeyValueMap[condition_expression.VarName]) {
						return true;
					}
				}
			}
			else {
				LOG.Error("Currently Can't Parse && and || in logic expression in the same condition, please contact NicoMico to fix this or waiting for later version update.");
			}

		}

	}
	LOG.NewLine();
	return false;
}

//���ڼ�Ⲣ�ָ��ַ����а���&& �� || 
std::vector<std::wstring> split_logic_get_expression(const std::wstring& expression) {
	std::vector<std::wstring> result;
	std::wstring current;

	for (size_t i = 0; i < expression.length(); i++) {
		if (expression[i] == '&' && expression[i + 1] == '&') {
			if (!current.empty()) {
				boost::algorithm::trim(current);
				result.push_back(current);
				current.clear();
			}
			i++;
		}
		else if (expression[i] == '|' && expression[i + 1] == '|') {
			if (!current.empty()) {
				boost::algorithm::trim(current);
				result.push_back(current);
				current.clear();
			}
			i++;
		}
		else {
			if (expression[i] != '|' && expression[i] != '&') {
				current += expression[i];
			}
		}
	}

	if (!current.empty()) {
		result.push_back(current);
	}

	return result;
}


std::vector<std::wstring> split_logic_get_logic(const std::wstring& expression) {
	std::vector<std::wstring> result;
	std::wstring current;
	for (size_t i = 0; i < expression.length(); i++) {
		if (expression[i] == '&' && expression[i + 1] == '&') {
			result.push_back(L"&&");
			i++;
		}
		else if (expression[i] == '|' && expression[i + 1] == '|') {
			result.push_back(L"||");
			i++;
		}
	}
	return result;
}


//����if����else if����������жϲ����ַ������������������漰�ı������߼�
M_Condition::M_Condition(std::wstring ConditionStr) {

	LOG.NewLine();
	LOG.Info(L"Start to parse condition expression for: " + ConditionStr);
	//���Ȳ����Ƿ���&& ���� || �������ͷ��ţ�����ҵ����ǾͰ�������д������򰴵������д���
	bool singleExpression = true;
	if (ConditionStr.find(L"&&") != std::wstring::npos) {
		singleExpression = false;
	}
	else if (ConditionStr.find(L"||") != std::wstring::npos) {
		singleExpression = false;
	}

	if (singleExpression) {
		LOG.Info("Can't find && or || in ConditionStr, take it as a single expression condition.");
		//����͵����ʹ��= �� ==�ָ����ָ������ConditionVarValue��û���ٳ���== !=���߱���$����Ķ�����˵����������
		IniLineObject conditionLine(ConditionStr);
		M_ConditionExpression conditionExpression;
		conditionExpression.VarName = conditionLine.LeftStrTrim;
		conditionExpression.VarValue = conditionLine.RightStrTrim;
		this->ConditionExpressionList.push_back(conditionExpression);
	}
	else {
		LOG.Info("find && or || in ConditionStr, take it as a multiple expression condition.");
		//����Ҫ�������&& ���� ||�����зָ�õ���һ���֣��ټ����ָ�õ���һ���֣�ֱ���Ҳ���&& �� ||Ϊֹ
		std::vector<std::wstring> result = split_logic_get_expression(ConditionStr);
		for (std::wstring condition_str : result) {
			LOG.Info(L"ConditionStr: " + condition_str);
			//����Ҫ��������ǣ����ֻ�ǵ�����&&����ô��ȫ���Էŵ�Condition�б���
			//�����������||��ô�죿��ʱ��������һ����Ч���ᵼ������ConditionMap����Ч��
			IniLineObject conditionLine(condition_str);
			M_ConditionExpression conditionExpression;
			conditionExpression.VarName = conditionLine.LeftStrTrim;
			conditionExpression.VarValue = conditionLine.RightStrTrim;
			this->ConditionExpressionList.push_back(conditionExpression);
		}
		this->LogicList = split_logic_get_logic(ConditionStr);
	}

	//���ConditionVarValue���Ƿ��б��������߼����ŵȣ����и����ӵĽ����жϡ�
	LOG.Info(L"Parse condition expression over.");
	LOG.NewLine();
}

void M_Condition::show() {

}


//--------------------------------------------------------------------------------------------------------------------
IniLineObject::IniLineObject() {

}

IniLineObject::IniLineObject(std::wstring readLine) {
	int firstDoubleEqualIndex = (int)readLine.find(L"==");
	int firstEqualIndex = (int)readLine.find(L"=");
	//LOG.Info(L"firstDoubleEqualIndex: " + std::to_wstring(firstDoubleEqualIndex));
	//LOG.Info(L"firstEqualIndex: " + std::to_wstring(firstEqualIndex));
	//Ĭ��ʹ��==�����==�Ҳ������Ǿͻ���=
	std::wstring delimiter = L"==";
	if (firstDoubleEqualIndex == std::wstring::npos) {
		delimiter = L"=";
	}

	//�ҵ���==�����ҵ���=�����Խ���
	if (firstEqualIndex != std::wstring::npos || firstDoubleEqualIndex != std::wstring::npos) {
		std::vector<std::wstring> lowerReadLineSplitList = MMTString::SplitStringOnlyMatchFirst(readLine, delimiter);
		if (lowerReadLineSplitList.size() < 2) {
			LOG.Error(L"lowerReadLineSplitList size is " + std::to_wstring(lowerReadLineSplitList.size()) + L",please check!");
		}
		std::wstring leftStr = lowerReadLineSplitList[0];
		std::wstring rightStr = lowerReadLineSplitList[1];
		//LOG.Info(L"leftStr:" + leftStr);
		//LOG.Info(L"rightStr:" + rightStr);
		LeftStr = leftStr;
		RightStr = rightStr;
		boost::algorithm::trim(leftStr);
		boost::algorithm::trim(rightStr);
		LeftStrTrim = leftStr;
		RightStrTrim = rightStr;
		valid = true;
	}
	else {
		LeftStr = L"";
		RightStr = L"";
		LeftStrTrim = L"";
		RightStrTrim = L"";
		valid = false;
	}
}

//�����������ָ���ָ����������Ǹ������жϱ�����ص�ʱ��
IniLineObject::IniLineObject(std::wstring readLine, std::wstring delimiter) {
	int firstEqualIndex = (int)readLine.find_first_of(delimiter);
	if (firstEqualIndex != std::wstring::npos) {
		std::vector<std::wstring> lowerReadLineSplitList = MMTString::SplitStringOnlyMatchFirst(readLine, delimiter);
		std::wstring leftStr = lowerReadLineSplitList[0];
		std::wstring rightStr = lowerReadLineSplitList[1];
		LeftStr = leftStr;
		RightStr = rightStr;
		boost::algorithm::trim(leftStr);
		boost::algorithm::trim(rightStr);
		LeftStrTrim = leftStr;
		RightStrTrim = rightStr;
		valid = true;
	}
	else {
		LeftStr = L"";
		RightStr = L"";
		LeftStrTrim = L"";
		RightStrTrim = L"";
		valid = false;
	}
}


//--------------------------------------------------------------------------------------------------------------------
M_IniSection::M_IniSection() {

}


//--------------------------------------------------------------------------------------------------------------------
M_Variable::M_Variable() {

}


M_Variable::M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InType) {
	this->NameSpace = InNameSpace;
	this->VariableName = InVariableName;
	this->NamespacedVarName = this->NameSpace + L"\\" + this->VariableName;
	this->Type = InType;
}


M_Variable::M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InInitializeValue, std::wstring InType) {
	this->NameSpace = InNameSpace;
	this->VariableName = InVariableName;
	this->NamespacedVarName = this->NameSpace + L"\\" + this->VariableName;
	this->InitializeValue = InInitializeValue;
	this->Type = InType;
}


//--------------------------------------------------------------------------------------------------------------------
M_DrawIndexed::M_DrawIndexed() {

}

//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------