#pragma once
#include <iostream>
#include <fstream>
#include <sstream>

#include "json.hpp"

namespace MMTJson {
	//���ڶ�ȡJson�ļ�������
	nlohmann::json ReadJsonFromFile(const std::wstring&);

	//���浽json�ļ�,�����json�ļ�·����json����
	void SaveToJsonFile(std::wstring, nlohmann::json);

}

