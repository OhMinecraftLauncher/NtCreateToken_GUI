// priv.h
#pragma once

#include <windows.h>
#include <string>
#include <vector>

// 特权结构
struct Privilege {
    const wchar_t* name;
    bool enabled;
};

// 组结构
struct GroupEntry {
    const wchar_t* sid;
    DWORD attributes;
};

// 所有可用特权列表
const Privilege kAllPrivileges[] = {
    { L"SeTcbPrivilege", true },
    { L"SeTakeOwnershipPrivilege", true },
    { L"SeDebugPrivilege", true },
    { L"SeImpersonatePrivilege", true },
    { L"SeCreateTokenPrivilege", true },
    { L"SeAssignPrimaryTokenPrivilege", true },
    { L"SeLockMemoryPrivilege", true },
    { L"SeIncreaseQuotaPrivilege", true },
    { L"SeMachineAccountPrivilege", true },
    { L"SeSecurityPrivilege", true },
    { L"SeLoadDriverPrivilege", true },
    { L"SeSystemProfilePrivilege", true },
    { L"SeSystemtimePrivilege", true },
    { L"SeProfileSingleProcessPrivilege", true },
    { L"SeIncreaseBasePriorityPrivilege", true },
    { L"SeCreatePagefilePrivilege", true },
    { L"SeCreatePermanentPrivilege", true },
    { L"SeBackupPrivilege", true },
    { L"SeRestorePrivilege", true },
    { L"SeShutdownPrivilege", true },
    { L"SeAuditPrivilege", true },
    { L"SeSystemEnvironmentPrivilege", true },
    { L"SeChangeNotifyPrivilege", true },
    { L"SeRemoteShutdownPrivilege", true },
    { L"SeUndockPrivilege", true },
    { L"SeSyncAgentPrivilege", true },
    { L"SeEnableDelegationPrivilege", true },
    { L"SeManageVolumePrivilege", true },
    { L"SeCreateGlobalPrivilege", true },
    { L"SeTrustedCredManAccessPrivilege", true },
    { L"SeRelabelPrivilege", true },
    { L"SeIncreaseWorkingSetPrivilege", true },
    { L"SeTimeZonePrivilege", true },
    { L"SeCreateSymbolicLinkPrivilege", true },
    { L"SeDelegateSessionUserImpersonatePrivilege", true }
};

// 默认组列表
const GroupEntry kDefaultGroups[] = {
    // Administrators
    { L"S-1-5-32-544", SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_MANDATORY | SE_GROUP_OWNER },
    // Authenticated Users
    { L"S-1-5-11", SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_MANDATORY },
    // Everyone
    { L"S-1-1-0", SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_MANDATORY },
    // Medium Mandatory Level
    { L"S-1-16-16384", SE_GROUP_INTEGRITY | SE_GROUP_INTEGRITY_ENABLED },
    // 以下服务SID
    { L"S-1-5-80-956008885-3418522649-1831038044-1853292631-2271478464", SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_MANDATORY | SE_GROUP_OWNER },
    { L"S-1-5-80-30551196-2029750602-3680353947-2336859763-523537544", SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_OWNER },
    { L"S-1-5-80-3232712927-1625117661-2590453128-1738570065-3637376297", SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_OWNER },
    { L"S-1-5-80-1913148863-3492339771-4165695881-2087618961-4109116736", SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_OWNER },
    { L"S-1-5-80-259296475-4084429506-1152984619-38739575-565535606", SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_OWNER },
    { L"S-1-5-80-1523878533-411328482-2798077809-3098663872-2604013308", SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_OWNER },
    { L"S-1-5-80-3668810961-2468724468-4084584310-3029221373-430494444", SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_OWNER },
    { L"S-1-5-80-3088073201-1464728630-1879813800-1107566885-823218052", SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_OWNER },
    { L"S-1-5-80-1383147646-27650227-2710666058-1662982300-1023958487", SE_GROUP_ENABLED | SE_GROUP_ENABLED_BY_DEFAULT | SE_GROUP_OWNER },
};

// NtCreateToken 所需的结构和函数指针类型（自定义）
typedef struct _MY_OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PVOID ObjectName;          // 实际应为 PUNICODE_STRING
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} MY_OBJECT_ATTRIBUTES, * PMY_OBJECT_ATTRIBUTES;

typedef NTSTATUS(NTAPI* NtCreateTokenFunc)(
    PHANDLE TokenHandle,
    ACCESS_MASK DesiredAccess,
    PMY_OBJECT_ATTRIBUTES ObjectAttributes,
    TOKEN_TYPE TokenType,
    PLUID AuthenticationId,
    PLARGE_INTEGER ExpirationTime,
    PTOKEN_USER TokenUser,
    PTOKEN_GROUPS TokenGroups,
    PTOKEN_PRIVILEGES TokenPrivileges,
    PTOKEN_OWNER TokenOwner,
    PTOKEN_PRIMARY_GROUP TokenPrimaryGroup,
    PTOKEN_DEFAULT_DACL TokenDefaultDacl,
    PTOKEN_SOURCE TokenSource
);

typedef NTSTATUS(NTAPI* NtAllocateLocallyUniqueIdFunc)(PLUID LocallyUniqueId);
