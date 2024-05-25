#include <napi.h>
#include <windows.h>
#include <lm.h>
#include <sddl.h>
#include <string>
#include <vector>
#include <iostream>  // Для логирования

#pragma comment(lib, "Netapi32.lib")  // Подключаем библиотеку Netapi32

// Функция для преобразования std::wstring в std::string
std::string WStringToString(const std::wstring& wstr) {
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}

std::string GetUserPrivilege(const std::string& username) {
    // Преобразуем username из std::string в std::wstring
    std::wstring wusername(username.begin(), username.end());
    
    LOCALGROUP_USERS_INFO_0 *groupInfo = nullptr;
    DWORD entriesRead = 0;
    DWORD totalEntries = 0;
    NET_API_STATUS status;

    status = NetUserGetLocalGroups(
        NULL,
        wusername.c_str(),
        0,
        LG_INCLUDE_INDIRECT,
        (LPBYTE*)&groupInfo,
        MAX_PREFERRED_LENGTH,
        &entriesRead,
        &totalEntries
    );

    if (status != NERR_Success) {
        std::wcout << L"NetUserGetLocalGroups error: " << status << std::endl;
        return "Пользователя " + username + " нет";
    }

    std::string privilege = "Пользователь не найден в известных группах";

    std::wcout << L"Группы пользователя " << wusername << L":" << std::endl;  // Логируем имя пользователя
    for (DWORD i = 0; i < entriesRead; i++) {
        if (groupInfo[i].lgrui0_name) {
            std::wstring groupName(groupInfo[i].lgrui0_name);
            std::string groupNameStr = WStringToString(groupName);  // Преобразуем в std::string для сравнения
            std::wcout << L"Group: " << groupName << std::endl;  // Логируем название группы
            if (groupNameStr == "Администраторы") {
                privilege = "Администратор";
                break;
            } else if (groupNameStr == "Пользователи") {
                privilege = "Пользователь";
            } else if (groupNameStr == "Гости") {
                privilege = "Гость";
            }
        } else {
            std::wcout << L"Group name is null" << std::endl;
        }
    }

    NetApiBufferFree(groupInfo);

    if (privilege == "Пользователь не найден в известных группах") {
        return "Пользователя " + username + " нет";
    }

    return "Пользователь " + username + " имеет привилегию: " + privilege;
}

Napi::String GetUserPrivilegeWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Строка ожидается").ThrowAsJavaScriptException();
    }
    std::string username = info[0].As<Napi::String>().Utf8Value();
    std::string result = GetUserPrivilege(username);
    return Napi::String::New(env, result);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("getUserPrivilege", Napi::Function::New(env, GetUserPrivilegeWrapped));
    return exports;
}

NODE_API_MODULE(addon, Init)
