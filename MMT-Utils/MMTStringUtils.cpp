#include "MMTStringUtils.h"
#include <filesystem>
// uuid
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
// time
#include <boost/date_time/posix_time/posix_time.hpp>

namespace MMTString {
    //TODO ���������ȡ����һ������
    std::wstring GetCurrentWorkingDirectoryPath()
    {
        std::wstring workingDir;
        wchar_t buffer[MAX_PATH];

        DWORD ret = GetCurrentDirectory(MAX_PATH, (LPSTR)buffer);
        if (ret == 0)
        {
            // ��ȡ·��ʧ�ܣ��������򷵻ؿյ�wstring
            return std::wstring();
        }
        // ���ذ�����ǰ����Ŀ¼·����wstring
        return std::wstring(buffer);
    }


    std::wstring GetParentFolderPathFromFolderPath(std::wstring FolderPath) {
        // �����ļ�ϵͳ·������
        std::filesystem::path path(FolderPath);

        // ��ȡ��һ��Ŀ¼·��
        std::filesystem::path parent_path = path.parent_path();

        return parent_path.wstring();
    }



    std::wstring GetFolderPathFromFilePath(std::wstring filePath) {
        std::size_t lastSlash = filePath.find_last_of(L"\\/");
        std::wstring path = filePath.substr(0, lastSlash);
        return path;
    }

    std::wstring GetFileNameFromFilePath(std::wstring filePath) {
        std::filesystem::path pathObj(filePath);
        return pathObj.filename();
    }

    std::wstring GetFileNameWithOutSuffix(std::wstring filePath) {
        // �������һ��'.'��λ��
        size_t dotPos = filePath.find_last_of(L'.');

        // ����ҵ���'.'
        if (dotPos != std::wstring::npos) {
            // �����ļ�������
            return filePath.substr(0, dotPos);
        }
        else {
            // ���û���ҵ�'.'��ֱ�ӷ��������ļ�·��
            return filePath;
        }
    }


    std::wstring ToWideString(std::string input) {
        if (input.empty()) return L"";

        int size_needed = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, NULL, 0);
        if (size_needed == 0) {
            // Handle error appropriately
            throw std::runtime_error("Failed in MultiByteToWideChar conversion.");
        }

        std::wstring wstrTo(size_needed, L'\0');
        int chars_converted = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, &wstrTo[0], size_needed);
        if (chars_converted == 0) {
            // Handle error appropriately
            throw std::runtime_error("Failed in MultiByteToWideChar conversion.");
        }

        // Remove the null terminator as it is implicitly handled in std::wstring
        wstrTo.pop_back();

        return wstrTo;
    }


    std::string ToByteString(std::wstring input) {
        if (input.empty()) return "";

        int size_needed = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, NULL, 0, NULL, NULL);
        if (size_needed == 0) {
            // Handle error appropriately
            throw std::runtime_error("Failed in WideCharToMultiByte conversion.");
        }

        std::string strTo(size_needed, '\0');
        int bytes_converted = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, &strTo[0], size_needed, NULL, NULL);
        if (bytes_converted == 0) {
            // Handle error appropriately
            throw std::runtime_error("Failed in WideCharToMultiByte conversion.");
        }

        // Remove the null terminator as it is implicitly handled in std::string
        strTo.pop_back();

        return strTo;
    }


    std::wstring GenerateUUIDW() {
        boost::uuids::random_generator generator;
        boost::uuids::uuid uuid = generator();
        return ToWideString(boost::uuids::to_string(uuid));
    }


    std::wstring GetFormattedDateTimeForFilename()
    {
        // ��ȡ�������ں�ʱ��
        boost::posix_time::ptime localTime =
            boost::posix_time::second_clock::local_time();

        // ��ʽ�����ں�ʱ��Ϊ�ַ���
        std::wstring dateTimeStr =
            boost::posix_time::to_iso_extended_wstring(localTime);

        // ���»����滻���ں�ʱ���еķ���ĸ�����ַ�
        boost::replace_all(dateTimeStr, "-", "_");
        boost::replace_all(dateTimeStr, ":", "_");

        return dateTimeStr;
    }


    //ԭʼ�ַ��������ڷָ���ַ���
    //ע������Ҳ����ָ����᷵��һ������Ϊ1���б�װ��ԭʼ���ַ����������жϵ�ʱ��Ҫ��1������0
    std::vector <std::wstring> SplitString(std::wstring originalStr, std::wstring delimiter) {
        std::vector<std::wstring> result;
        std::wstring str = originalStr;
        size_t pos = 0;
        while ((pos = str.find(delimiter)) != std::wstring::npos) {
            std::wstring token = str.substr(0, pos);
            result.push_back(token);
            str.erase(0, pos + delimiter.length());
        }
        result.push_back(str);
        return result;
    }


    std::vector <std::wstring> SplitStringOnlyMatchFirst(std::wstring originalStr, std::wstring delimiter) {
        std::vector<std::wstring> result;
        std::wstring str = originalStr;
        size_t pos = str.find_first_of(delimiter);
        result.push_back(str.substr(0, pos));
        result.push_back(str.substr(pos + delimiter.length()));
        return result;
    }


    std::wstring ToLowerCase(std::wstring str)
    {
        return boost::algorithm::to_lower_copy(str);
    }
    std::string ToLowerCase(std::string str)
    {
        return boost::algorithm::to_lower_copy(str);
    }
    std::wstring ToUpperCase(std::wstring str) {
        return boost::algorithm::to_upper_copy(str);
    }
    std::string ToUpperCase(std::string str) {
        return boost::algorithm::to_upper_copy(str);
    }


    std::wstring GetPSHashFromFileName(const std::wstring& input) {
        std::wstring result;
        size_t pos = input.find(L"-ps=");
        if (pos != std::wstring::npos && pos + 4 + 16 <= input.length()) {
            result = input.substr(pos + 4, 16);
        }
        return result;
    }


    std::wstring GetVSHashFromFileName(const std::wstring& input) {
        std::wstring result;
        size_t pos = input.find(L"-vs=");
        if (pos != std::wstring::npos && pos + 4 + 16 <= input.length()) {
            result = input.substr(pos + 4, 16);
        }
        return result;
    }


    //��ȷ��������ַ�����[]��������
    std::wstring RemoveSquareBrackets(std::wstring inputStr) {
        if (inputStr.size() >= 2 && inputStr.front() == L'[' && inputStr.back() == L']') {
            inputStr = inputStr.substr(1, inputStr.size() - 2);
        }
        return inputStr;
    }

    std::wstring Remove$Prefix(std::wstring VariableName) {
        std::wstring VariableNameRemove$ = VariableName.substr(1);
        return VariableNameRemove$;
    }


    std::wstring GetPixelSlotFromTextureFileName(std::wstring TextureFileName) {
        size_t firstPos = TextureFileName.find_first_of(L"-");
        size_t secondsPos = TextureFileName.find_first_of(L"=");
        std::wstring PixelSlot = TextureFileName.substr(firstPos + 1, secondsPos - firstPos - 1);
        return PixelSlot;
    }


    LPSTR WCHAR_TO_LPSTR(WCHAR wStr[]) {

        // �����Ҫ�Ķ��ֽ��ַ����ĳ���
        int length = WideCharToMultiByte(CP_ACP, 0, wStr, -1, nullptr, 0, nullptr, nullptr);

        // �����㹻�Ŀռ����洢���ֽ��ַ���
        LPSTR str = new char[length];

        // ��WCHAR�ַ���ת��Ϊ���ֽ��ַ���
        WideCharToMultiByte(CP_ACP, 0, wStr, -1, str, length, nullptr, nullptr);

        return str;
    }



    std::wstring GetFileHashFromFileName(std::wstring InputMigotoFileName) {
        std::wstring result;

        if (InputMigotoFileName.find(L"!S!") == std::wstring::npos) {
            size_t pos = InputMigotoFileName.find_first_of(L"=");
            if (pos != std::wstring::npos && pos + 1 + 8 <= InputMigotoFileName.length()) {
                result = InputMigotoFileName.substr(pos + 1, 8);
            }
        }
        else {
            //���磺
            //000061-ps-t7=!S!=ab2cbb0c-vs=479e531b67d3e9f3-ps=92139b61ff840c7b.dds
            //�����ȡ��posλ��Ϊ!S!= ǰ��λ������Ҫ+4
            //����Ϊʲô��������ټ�1�أ����շ���!S!=��Ȼ��5���ַ�
            //�������ʹ���ַ�����.size()������ȡ���ֲ����ˣ��ֱ�֮ǰ��һ����������Size��Ӱ�������=
            //��������ֻ�ܵ����������������
            size_t pos = InputMigotoFileName.find_first_of(L"!S!=");
            if (pos != std::wstring::npos && pos + 4 + 1 + 8 <= InputMigotoFileName.length()) {
                result = InputMigotoFileName.substr(pos + 4 + 1, 8);
            }
        }
        
        return result;
    }

}
