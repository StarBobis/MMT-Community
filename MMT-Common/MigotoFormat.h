#pragma once
#pragma once
#include <vector>
#include "MMTStringUtils.h"
#include "MMTLogUtils.h"
#include <boost/algorithm/string.hpp>
#include "IndexBufferBufFile.h"

//-----------------------------------------------------------------------------------------------------------------------------------
// �����Ż����������ͣ�ָ���Ǵ�ini����з�������ȡ�ĵ�һ�������Ϣ������������еķ�������
// ���ڽ���3Dmigoto�Ļ�����������
//-----------------------------------------------------------------------------------------------------------------------------------
class IniLineObject {
public:
	std::wstring LeftStr;
	std::wstring LeftStrTrim;
	std::wstring RightStr;
	std::wstring RightStrTrim;
	bool valid = false;

	IniLineObject();
	IniLineObject(std::wstring readLine);
	//�����������ָ���ָ����������Ǹ������жϱ�����ص�ʱ��
	IniLineObject(std::wstring readLine, std::wstring delimiter);
};


class MigotoAttribute {
public:
	//������3Dmigoto��ini��ͨ�����ԣ�����NameSpace�ȵȡ�
	std::wstring NameSpace = L"";
};


class M_IniSection : public MigotoAttribute {
public:
	//�������ʱʹ��
	std::wstring SectionName;
	std::vector<std::wstring> SectionLineList;

	//���SectionTypeֻ��Mod���ɵ�ʱ��ʹ�á�
	std::wstring SectionType;

	M_IniSection();

	void Append(std::wstring InputLine) {
		this->SectionLineList.push_back(InputLine);
	}

};


//ͬʱ����constants��present
class M_Variable : public MigotoAttribute {
public:
	std::wstring VariableName;
	std::wstring InitializeValue = L"";
	std::wstring NamespacedVarName;
	std::wstring Type; //global local normal

	M_Variable();
	M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InType);
	M_Variable(std::wstring InNameSpace, std::wstring InVariableName, std::wstring InInitializeValue, std::wstring InType);
};


//Condition����һ��������һ��Condition�п����ж���Աȱ��ʽ
//���� if $var == 1 && $var2 == 2 ��ʱ$var,1����һ���Աȱ��ʽ�������Condition��Ч������֮һ
//Condition_VarName_VarValue_Map���洢�˴�Condition��Ч����������
//һ��ResourceReplace�����ж��M_Condition�����������ģ�������Ϊif���ж��Ƕ��
//����ÿ��Condition������һ��if��else if��ĶԱȱ��ʽ����ϣ���������Ƕ�����⣬Ƕ��Ӧ����CommandList����ʱ���п��ǡ�

//����������һ���������������ʽ����Ϊ����������֮�仹�й�ϵ����
class M_ConditionExpression {
public:
	std::wstring VarName;
	std::wstring VarValue;
};



class M_Condition : public MigotoAttribute {
public:
	//�����Ϊ�������ʽ�б��Լ�һ���߼����ӷ��б�
	std::vector<M_ConditionExpression> ConditionExpressionList;
	std::vector<std::wstring> LogicList;

	//��ʱ���ж�һ��Condition�Ƿ���Ч����Ҫ���ݴ���ı�����ֵ�������߼����ӷ��б����ж��Ƿ���Ч
	bool isActive(std::unordered_map<std::wstring, std::wstring> ActiveKeyValueMap);

	void show();

	M_Condition();

	M_Condition(std::wstring ConditionStr);
};


class M_Key : public MigotoAttribute {
public:

	M_Condition Condition;
	std::wstring KeyName;
	std::wstring BackName;
	std::wstring Type;

	//���� $variable5 = 0,1 �����������֮�����$variable5��ֵ��0,1֮��ѭ���л�����һ����������ѭ������
	std::unordered_map<std::wstring, std::vector<std::wstring>> CycleVariableName_PossibleValueList_Map;

	//����$creditinfo = 0 �����������֮������̼����$creditinfo��Ϊ0�������Ǽ������
	std::unordered_map<std::wstring, std::wstring> ActiveVariableName_ActiveValue_Map;
};


class M_DrawIndexed {
public:
	//����DrawIndexed = 6,12,0 ��ʾDrawStartIndexΪ0��DrawOffsetIndexΪ12��DrawNumberΪ6
	//��������0��ʼ���ƣ���Offset��ʼ��ȡ����ȡDrawNumber������������
	bool AutoDraw = false;
	std::wstring DrawNumber;
	std::wstring DrawOffsetIndex;
	std::wstring DrawStartIndex;

	std::vector<M_Condition> ActiveConditionList;

	M_DrawIndexed();
};


class M_ResourceReplace {
public:
	std::wstring ReplaceTarget;
	std::wstring ReplaceResource;
	std::vector<M_Condition> ActiveConditionList;

};


class M_TextureOverride : public MigotoAttribute {
public:

	std::wstring IndexBufferHash;
	std::vector<M_ResourceReplace> ResourceReplaceList;
	std::wstring MatchPriority;
	std::wstring Handling; //skip
	std::wstring MatchFirstIndex;
	std::vector<M_DrawIndexed> DrawIndexedList;

	//���ڴ���TextureOverrideIB����߼�
	std::wstring IBResourceName;
	std::wstring IBFileName;
	std::wstring IBFilePath;
	std::wstring IBFormat;
	IndexBufferBufFile IBBufFile;

	//���ڴ�����Key�л�ʱ�����ResourceReplaceList
	std::vector<M_ResourceReplace> ActiveResourceReplaceList;
};


class M_Resource : public MigotoAttribute {
public:

	std::wstring ResourceName = L"";
	std::wstring Type = L"";
	std::wstring Stride = L"";
	std::wstring Format = L"";
	std::wstring FileName = L"";

	//����ƴ�ӳ��������ԣ�����NameSpace��FileName
	std::wstring FilePath = L"";
};


class M_CommandList : public MigotoAttribute {
public:

	std::wstring CommandListName = L"";
	std::vector<M_ResourceReplace> ResourceReplaceList;
	std::vector<M_DrawIndexed> DrawIndexedList;

};