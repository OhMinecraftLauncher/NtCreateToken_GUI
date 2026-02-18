#include <windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <sddl.h>
#include <NTSecAPI.h>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include "priv.h"

#include "GetTokenInfo.cpp"

// 是否显示令牌信息（由 /n 控制）
static bool g_showTokenInfo = true;

// 将特权名称转换为 LUID
LUID LookupPrivilegeLuid(const std::wstring& privName) {
    LUID luid = { 0 };
    LookupPrivilegeValueW(nullptr, privName.c_str(), &luid);
    return luid;
}

// 根据进程名查找 PID
DWORD FindProcessId(const std::wstring& processName) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return 0;

    PROCESSENTRY32W pe = { sizeof(pe) };
    DWORD pid = 0;
    if (Process32FirstW(snapshot, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, processName.c_str()) == 0) {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &pe));
    }
    CloseHandle(snapshot);
    return pid;
}

// 从指定进程获取模拟令牌
HANDLE GetTokenFromProcess(DWORD pid) {
    if (pid == 0)
        return nullptr;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProcess)
        return nullptr;

    HANDLE hToken = nullptr;
    HANDLE hDupToken = nullptr;
    if (OpenProcessToken(hProcess, MAXIMUM_ALLOWED, &hToken)) {
        DuplicateTokenEx(hToken, TOKEN_ALL_ACCESS, nullptr, SecurityImpersonation, TokenImpersonation, &hDupToken);
        CloseHandle(hToken);
    }
    CloseHandle(hProcess);
    return hDupToken;
}

// 获取当前进程的 Logon SID
bool GetCurrentLogonSid(std::wstring& outLogonSid) {
    HANDLE hToken = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
        return false;

    DWORD size = 0;
    GetTokenInformation(hToken, TokenGroups, nullptr, 0, &size);
    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
        CloseHandle(hToken);
        return false;
    }

    std::unique_ptr<BYTE[]> buffer(new BYTE[size]);
    PTOKEN_GROUPS pGroups = reinterpret_cast<PTOKEN_GROUPS>(buffer.get());
    if (!GetTokenInformation(hToken, TokenGroups, pGroups, size, &size)) {
        CloseHandle(hToken);
        return false;
    }

    bool found = false;
    for (DWORD i = 0; i < pGroups->GroupCount; ++i) {
        if (pGroups->Groups[i].Attributes & SE_GROUP_LOGON_ID) {
            LPWSTR sidStr = nullptr;
            if (ConvertSidToStringSidW(pGroups->Groups[i].Sid, &sidStr)) {
                outLogonSid = sidStr;
                LocalFree(sidStr);
                found = true;
                break;
            }
        }
    }

    CloseHandle(hToken);
    return found;
}

// 核心函数：使用 NtCreateToken 创建令牌并启动进程
bool CreateTokenWithNtCreateToken(
    const std::wstring& cmdLine,
    const std::vector<Privilege>& privileges,
    const std::vector<GroupEntry>& groups,
    const std::wstring& userSid)
{
    HMODULE ntdll = LoadLibraryW(L"ntdll.dll");
    if (!ntdll) {
        std::wcerr << L"Failed to load ntdll.dll" << std::endl;
        return false;
    }

    auto NtCreateToken = reinterpret_cast<NtCreateTokenFunc>(
        GetProcAddress(ntdll, "NtCreateToken"));
    auto NtAllocateLocallyUniqueId = reinterpret_cast<NtAllocateLocallyUniqueIdFunc>(
        GetProcAddress(ntdll, "NtAllocateLocallyUniqueId"));

    if (!NtCreateToken || !NtAllocateLocallyUniqueId) {
        std::wcerr << L"Failed to get NtCreateToken or NtAllocateLocallyUniqueId" << std::endl;
        FreeLibrary(ntdll);
        return false;
    }

    // 转换用户 SID
    PSID userSidPtr = nullptr;
    if (!ConvertStringSidToSidW(userSid.c_str(), &userSidPtr)) {
        std::wcerr << L"Failed to convert user SID: " << userSid << std::endl;
        FreeLibrary(ntdll);
        return false;
    }
    std::unique_ptr<void, decltype(&LocalFree)> userSidGuard(userSidPtr, LocalFree);

    // 转换所有者 SID (Administrators)
    const wchar_t* ownerSidStr = L"S-1-5-32-544";
    PSID ownerSidPtr = nullptr;
    if (!ConvertStringSidToSidW(ownerSidStr, &ownerSidPtr)) {
        std::wcerr << L"Failed to convert owner SID: " << ownerSidStr << std::endl;
        FreeLibrary(ntdll);
        return false;
    }
    std::unique_ptr<void, decltype(&LocalFree)> ownerSidGuard(ownerSidPtr, LocalFree);

    // 获取当前 Logon SID
    std::wstring logonSidStr;
    if (!GetCurrentLogonSid(logonSidStr)) {
        std::wcerr << L"Failed to get current logon SID" << std::endl;
        FreeLibrary(ntdll);
        return false;
    }

    // 构建完整的组列表（用户组 + Logon SID）
    std::vector<GroupEntry> allGroups = groups;
    allGroups.push_back({ logonSidStr.c_str(),
        SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_MANDATORY | SE_GROUP_LOGON_ID });

    // 转换所有组 SID
    std::vector<PSID> groupSids;
    std::vector<std::unique_ptr<void, decltype(&LocalFree)>> groupSidGuards;
    for (const auto& g : allGroups) {
        PSID sid = nullptr;
        if (!ConvertStringSidToSidW(g.sid, &sid)) {
            std::wcerr << L"Failed to convert group SID: " << g.sid << std::endl;
            FreeLibrary(ntdll);
            return false;
        }
        groupSids.push_back(sid);
        groupSidGuards.emplace_back(sid, LocalFree);
    }

    // 分配 LUID
    LUID luid;
    NtAllocateLocallyUniqueId(&luid);

    // 准备 TOKEN_USER
    TOKEN_USER tokenUser = {};
    tokenUser.User.Sid = userSidPtr;
    tokenUser.User.Attributes = 0;

    // 准备 TOKEN_GROUPS
    DWORD groupCount = static_cast<DWORD>(groupSids.size());
    size_t groupStructSize = sizeof(TOKEN_GROUPS) + groupCount * sizeof(SID_AND_ATTRIBUTES);
    std::unique_ptr<BYTE[]> groupBuffer(new BYTE[groupStructSize]);
    PTOKEN_GROUPS tokenGroups = reinterpret_cast<PTOKEN_GROUPS>(groupBuffer.get());
    tokenGroups->GroupCount = groupCount;
    for (DWORD i = 0; i < groupCount; ++i) {
        tokenGroups->Groups[i].Sid = groupSids[i];
        tokenGroups->Groups[i].Attributes = allGroups[i].attributes;
    }

    // 准备 TOKEN_PRIVILEGES
    DWORD privCount = static_cast<DWORD>(privileges.size());
    size_t privStructSize = sizeof(TOKEN_PRIVILEGES) + privCount * sizeof(LUID_AND_ATTRIBUTES);
    std::unique_ptr<BYTE[]> privBuffer(new BYTE[privStructSize]);
    PTOKEN_PRIVILEGES tokenPrivs = reinterpret_cast<PTOKEN_PRIVILEGES>(privBuffer.get());
    tokenPrivs->PrivilegeCount = privCount;
    for (DWORD i = 0; i < privCount; ++i) {
        tokenPrivs->Privileges[i].Luid = LookupPrivilegeLuid(privileges[i].name);
        tokenPrivs->Privileges[i].Attributes = privileges[i].enabled ?
            (SE_PRIVILEGE_ENABLED | SE_PRIVILEGE_ENABLED_BY_DEFAULT) :
            SE_PRIVILEGE_ENABLED_BY_DEFAULT;
    }

    // 准备 TOKEN_OWNER 和 TOKEN_PRIMARY_GROUP
    TOKEN_OWNER tokenOwner = {};
    tokenOwner.Owner = ownerSidPtr;
    TOKEN_PRIMARY_GROUP tokenPrimaryGroup = {};
    tokenPrimaryGroup.PrimaryGroup = ownerSidPtr;

    // 准备 TOKEN_DEFAULT_DACL（空）
    TOKEN_DEFAULT_DACL tokenDefaultDacl = {};

    // 准备 TOKEN_SOURCE
    TOKEN_SOURCE tokenSource = {};
    tokenSource.SourceIdentifier = luid;
    memcpy(tokenSource.SourceName, "User32\0", 8);

    // 准备 OBJECT_ATTRIBUTES
    SECURITY_QUALITY_OF_SERVICE sqos = { sizeof(sqos), SecurityAnonymous, SECURITY_STATIC_TRACKING, FALSE };
    MY_OBJECT_ATTRIBUTES oa = { sizeof(oa), nullptr, nullptr, 0, nullptr, &sqos };

    // 调用 NtCreateToken
    HANDLE hToken = nullptr;
    LARGE_INTEGER expiration = {};
    expiration.QuadPart = -1;  // 永不超时
    LUID systemLuid = SYSTEM_LUID;

    NTSTATUS status = NtCreateToken(
        &hToken,
        TOKEN_ALL_ACCESS,
        &oa,
        TokenPrimary,
        &systemLuid,
        &expiration,
        &tokenUser,
        tokenGroups,
        tokenPrivs,
        &tokenOwner,
        &tokenPrimaryGroup,
        &tokenDefaultDacl,
        &tokenSource);

    if (status != 0) {
        std::wprintf(L"Failed to create token, status: 0x%08X\n", status);
        FreeLibrary(ntdll);
        return false;
    }

    // 可选：显示令牌信息
    if (g_showTokenInfo)
        PrintTokenInformation(hToken);

    // 使用令牌创建进程
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = {};
    if (!CreateProcessWithTokenW(hToken, LOGON_NETCREDENTIALS_ONLY, nullptr,
                                  const_cast<LPWSTR>(cmdLine.c_str()), CREATE_NEW_CONSOLE,
                                  nullptr, nullptr, &si, &pi)) {
        DWORD err = GetLastError();
        std::wprintf(L"CreateProcessWithTokenW failed: %u\n", err);
        CloseHandle(hToken);
        FreeLibrary(ntdll);
        return false;
    }

    std::wcout << L"Process created successfully." << std::endl;
    AllowSetForegroundWindow(pi.dwProcessId);
    SetProcessWorkingSetSize(pi.hProcess, -1, -1);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hToken);
    FreeLibrary(ntdll);
    return true;
}

// 检查字符串是否以指定后缀结尾
bool EndsWith(const std::wstring& str, const std::wstring& suffix) {
    if (suffix.size() > str.size())
        return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

// 解析特权字符串（格式：SeDebugPrivilege;SeTcbPrivilege:N;...）
std::vector<Privilege> ParsePrivileges(const std::wstring& privStr) {
    std::vector<Privilege> result;
    std::wstringstream ss(privStr);
    std::wstring token;
    while (std::getline(ss, token, L';')) {
        if (token.empty())
            continue;
        if (EndsWith(token, L":N")) {
            token.resize(token.size() - 2);
            result.push_back({ _wcsdup(token.c_str()), false });
        } else {
            result.push_back({ _wcsdup(token.c_str()), true });
        }
    }
    return result;
}

// 解析组字符串（格式：SID,attr1*attr2;SID,attr3;...）
std::vector<GroupEntry> ParseGroups(const std::wstring& groupStr) {
    std::vector<GroupEntry> result;
    std::wstringstream ss(groupStr);
    std::wstring token;
    while (std::getline(ss, token, L';')) {
        if (token.empty())
            continue;
        size_t comma = token.find(L',');
        if (comma == std::wstring::npos)
            continue;
        std::wstring sid = token.substr(0, comma);
        std::wstring attrStr = token.substr(comma + 1);

        DWORD attributes = 0;
        std::wstringstream attrStream(attrStr);
        std::wstring attrToken;
        while (std::getline(attrStream, attrToken, L'*')) {
            if (attrToken.empty())
                continue;
            if (_wcsicmp(attrToken.c_str(), L"SE_GROUP_ENABLED") == 0)
                attributes |= SE_GROUP_ENABLED;
            else if (_wcsicmp(attrToken.c_str(), L"SE_GROUP_ENABLED_BY_DEFAULT") == 0)
                attributes |= SE_GROUP_ENABLED_BY_DEFAULT;
            else if (_wcsicmp(attrToken.c_str(), L"SE_GROUP_MANDATORY") == 0)
                attributes |= SE_GROUP_MANDATORY;
            else if (_wcsicmp(attrToken.c_str(), L"SE_GROUP_OWNER") == 0)
                attributes |= SE_GROUP_OWNER;
            else if (_wcsicmp(attrToken.c_str(), L"SE_GROUP_LOGON_ID") == 0)
                attributes |= SE_GROUP_LOGON_ID;
            else if (_wcsicmp(attrToken.c_str(), L"SE_GROUP_INTEGRITY") == 0)
                attributes |= SE_GROUP_INTEGRITY;
            else if (_wcsicmp(attrToken.c_str(), L"SE_GROUP_INTEGRITY_ENABLED") == 0)
                attributes |= SE_GROUP_INTEGRITY_ENABLED;
            else if (_wcsicmp(attrToken.c_str(), L"SE_GROUP_USE_FOR_DENY_ONLY") == 0)
                attributes |= SE_GROUP_USE_FOR_DENY_ONLY;
            else if (_wcsicmp(attrToken.c_str(), L"SE_GROUP_RESOURCE") == 0)
                attributes |= SE_GROUP_RESOURCE;
            else {
                // 尝试解析为十六进制数
                wchar_t* end = nullptr;
                DWORD val = wcstoul(attrToken.c_str(), &end, 16);
                if (end != attrToken.c_str())
                    attributes |= val;
            }
        }
        result.push_back({ _wcsdup(sid.c_str()), attributes });
    }
    return result;
}

// 显示帮助信息
void ShowUsage() {
    wprintf(L"Usage: NtCreateToken.exe [options]\n");
    wprintf(L"Options:\n");
    wprintf(L"  /c <command>  Command line to execute (default: cmd.exe)\n");
    wprintf(L"  /s <SID>      User SID for token (default: S-1-5-18 = SYSTEM)\n");
    wprintf(L"  /p <privs>    Privileges separated by ';' (default: all privileges)\n");
    wprintf(L"                You can add \":N\" after a privilege to disable it.\n");
    wprintf(L"  /g <groups>   Groups separated by ';', group format: SID,attr1*attr2\n");
    wprintf(L"  /n            Don't show the token's information before running process\n");
    wprintf(L"  /t <pid>      Show a process's token's information\n");
    wprintf(L"  /? or /h      Show this help\n");
    wprintf(L"\nExamples:\n");
    wprintf(L"  NtCreateToken.exe\n");
    wprintf(L"  NtCreateToken.exe /c \"powershell.exe\" /s S-1-5-21-...\n");
    wprintf(L"  NtCreateToken.exe /p \"SeDebugPrivilege;SeImpersonatePrivilege\"\n");
    wprintf(L"  NtCreateToken.exe /g \"S-1-5-32-544,SE_GROUP_ENABLED*SE_GROUP_OWNER;S-1-1-0,SE_GROUP_ENABLED\"\n");
    wprintf(L"  NtCreateToken.exe /t 1234\n");
}

// 将字符串转换为 DWORD（用于解析 PID）
DWORD ParseDword(const std::wstring& str) {
    if (str.empty())
        return 0;
    wchar_t* end = nullptr;
    unsigned long val = wcstoul(str.c_str(), &end, 0);
    if (end == str.c_str() || *end != L'\0')
        return 0;
    return static_cast<DWORD>(val);
}

// 主函数
int wmain(int argc, wchar_t* argv[])
{
    std::wstring cmdLine = L"C:\\Windows\\System32\\cmd.exe";
    std::wstring userSid = L"S-1-5-18";  // SYSTEM
    std::vector<Privilege> privileges;
    std::vector<GroupEntry> groups;

    // 初始化默认特权
    for (const auto& p : kAllPrivileges) {
        privileges.push_back(p);
    }

    // 初始化默认组
    for (const auto& g : kDefaultGroups) {
        groups.push_back(g);
    }

    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        if (_wcsicmp(argv[i], L"/c") == 0 && i + 1 < argc) {
            cmdLine = argv[i + 1];
            ++i;
        } else if (_wcsicmp(argv[i], L"/s") == 0 && i + 1 < argc) {
            userSid = argv[i + 1];
            ++i;
        } else if (_wcsicmp(argv[i], L"/p") == 0 && i + 1 < argc) {
            privileges = ParsePrivileges(argv[i + 1]);
            ++i;
        } else if (_wcsicmp(argv[i], L"/g") == 0 && i + 1 < argc) {
            groups = ParseGroups(argv[i + 1]);
            ++i;
        } else if (_wcsicmp(argv[i], L"/t") == 0) {
            DWORD pid = GetCurrentProcessId();
            if (i + 1 < argc) {
                pid = ParseDword(argv[i + 1]);
                if (pid == 0 && _wcsicmp(argv[i + 1], L"0") != 0) {
                    std::wcout << L"Warning: Invalid PID, using current process." << std::endl;
                    pid = GetCurrentProcessId();
                }
                ++i;
            }
            // 尝试模拟 lsass 令牌以查看受保护进程的令牌（可能失败）
            DWORD lsassPid = FindProcessId(L"lsass.exe");
            if (lsassPid != 0) {
                HANDLE hToken = GetTokenFromProcess(lsassPid);
                if (hToken) {
                    if (!ImpersonateLoggedOnUser(hToken)) {
                        std::wcout << L"Warning: Failed to impersonate lsass token." << std::endl;
                    }
                    CloseHandle(hToken);
                } else {
                    std::wcout << L"Warning: Failed to get lsass token." << std::endl;
                }
            }
            if (!ViewProcessToken(pid)) {
                std::wcout << L"Failed to view token for process " << pid << std::endl;
            }
            return 0;
        } else if (_wcsicmp(argv[i], L"/n") == 0) {
            g_showTokenInfo = false;
        } else if (_wcsicmp(argv[i], L"/?") == 0 || _wcsicmp(argv[i], L"/h") == 0) {
            ShowUsage();
            return 0;
        }
    }

    // 获取 lsass 进程的令牌
    DWORD lsassPid = FindProcessId(L"lsass.exe");
    if (lsassPid == 0) {
        std::wcout << L"Error: Could not find lsass.exe process." << std::endl;
        return 1;
    }

    HANDLE hToken = GetTokenFromProcess(lsassPid);
    if (!hToken) {
        std::wcout << L"Error: Failed to get token from lsass.exe." << std::endl;
        return 1;
    }

    // 模拟 lsass 令牌
    if (!ImpersonateLoggedOnUser(hToken)) {
        std::wcout << L"Error: Failed to impersonate lsass token: " << GetLastError() << std::endl;
        CloseHandle(hToken);
        return 1;
    }

    // 创建新令牌并启动进程
    bool success = CreateTokenWithNtCreateToken(cmdLine, privileges, groups, userSid);

    RevertToSelf();
    CloseHandle(hToken);

    return success ? 0 : 1;
}
