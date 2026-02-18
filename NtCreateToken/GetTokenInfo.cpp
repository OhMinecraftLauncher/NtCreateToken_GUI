#include <psapi.h>

void PrintError(const char* msg) {
    DWORD eNum = GetLastError();
    printf("ERROR: %s (Error %d)\n", msg, eNum);
}

std::string SidToString(PSID sid) {
    if (sid == NULL) return "<NULL>";
    
    LPSTR szSid = NULL;
    if (ConvertSidToStringSidA(sid, &szSid)) {
        std::string result(szSid);
        LocalFree(szSid);
        return result;
    }
    return "<Cannot convert SID>";
}

std::string GetAccountNameFromSid(PSID sid) {
    if (sid == NULL) return "<NULL>";
    
    char szName[256] = {0};
    char szDomain[256] = {0};
    DWORD cchName = 256, cchDomain = 256;
    SID_NAME_USE sidType;
    
    if (LookupAccountSidA(NULL, sid, szName, &cchName, szDomain, &cchDomain, &sidType)) {
        return std::string(szDomain) + "\\" + std::string(szName);
    }
    return "<Unknown>";
}

void PrintPrivileges(PTOKEN_PRIVILEGES privileges) {
    if (!privileges) return;
    
    for (DWORD i = 0; i < privileges->PrivilegeCount; i++) {
        char szPrivilegeName[256] = {0};
        DWORD cchName = 256;
        
        if (LookupPrivilegeNameA(NULL, &privileges->Privileges[i].Luid, 
                                szPrivilegeName, &cchName)) {
            printf("    %s", szPrivilegeName);
            
            DWORD attrs = privileges->Privileges[i].Attributes;
            if (attrs & SE_PRIVILEGE_ENABLED) printf(" [Enabled]");
            if (attrs & SE_PRIVILEGE_ENABLED_BY_DEFAULT) printf(" [Default]");
            if (attrs & SE_PRIVILEGE_USED_FOR_ACCESS) printf(" [Used]");
            printf("\n");
        }
    }
}

void PrintGroups(PTOKEN_GROUPS groups, const char* groupType = "Group") {
    if (!groups) return;
    
    for (DWORD i = 0; i < groups->GroupCount; i++) {
        std::string sidStr = SidToString(groups->Groups[i].Sid);
        std::string accountName = GetAccountNameFromSid(groups->Groups[i].Sid);
        
        printf("    %s (SID: %s)", accountName.c_str(), sidStr.c_str());
        
        DWORD attrs = groups->Groups[i].Attributes;
        if (attrs & SE_GROUP_ENABLED) printf(" [Enabled]");
        if (attrs & SE_GROUP_ENABLED_BY_DEFAULT) printf(" [Default]");
        if (attrs & SE_GROUP_INTEGRITY) printf(" [Integrity]");
        if (attrs & SE_GROUP_INTEGRITY_ENABLED) printf(" [IntegrityEnabled]");
        if (attrs & SE_GROUP_LOGON_ID) printf(" [LogonID]");
        if (attrs & SE_GROUP_MANDATORY) printf(" [Mandatory]");
        if (attrs & SE_GROUP_OWNER) printf(" [Owner]");
        if (attrs & SE_GROUP_RESOURCE) printf(" [Resource]");
        if (attrs & SE_GROUP_USE_FOR_DENY_ONLY) printf(" [DenyOnly]");
        printf("\n");
    }
}

void PrintAcl(PACL acl) {
    if (acl == NULL) {
        printf("    <NULL>\n");
        return;
    }
    
    ACL_SIZE_INFORMATION aclSize;
    if (GetAclInformation(acl, &aclSize, sizeof(aclSize), AclSizeInformation)) {
        printf("    ACE Count: %d\n", aclSize.AceCount);
        
        for (DWORD i = 0; i < aclSize.AceCount; i++) {
            ACE_HEADER* aceHeader;
            if (GetAce(acl, i, (LPVOID*)&aceHeader)) {
                printf("    ACE[%d]: Type=0x%02X, Flags=0x%02X, Size=%d\n", 
                       i, aceHeader->AceType, aceHeader->AceFlags, aceHeader->AceSize);
            }
        }
    }
}

const char* GetTokenInfoClassName(TOKEN_INFORMATION_CLASS infoClass) {
    switch (infoClass) {
        case TokenUser: return "TokenUser";
        case TokenGroups: return "TokenGroups";
        case TokenPrivileges: return "TokenPrivileges";
        case TokenOwner: return "TokenOwner";
        case TokenPrimaryGroup: return "TokenPrimaryGroup";
        case TokenDefaultDacl: return "TokenDefaultDacl";
        case TokenSource: return "TokenSource";
        case TokenType: return "TokenType";
        case TokenImpersonationLevel: return "TokenImpersonationLevel";
        case TokenStatistics: return "TokenStatistics";
        case TokenRestrictedSids: return "TokenRestrictedSids";
        case TokenSessionId: return "TokenSessionId";
        case TokenGroupsAndPrivileges: return "TokenGroupsAndPrivileges";
        case TokenSessionReference: return "TokenSessionReference";
        case TokenSandBoxInert: return "TokenSandBoxInert";
        case TokenAuditPolicy: return "TokenAuditPolicy";
        case TokenOrigin: return "TokenOrigin";
        case TokenElevationType: return "TokenElevationType";
        case TokenLinkedToken: return "TokenLinkedToken";
        case TokenElevation: return "TokenElevation";
        case TokenHasRestrictions: return "TokenHasRestrictions";
        case TokenAccessInformation: return "TokenAccessInformation";
        case TokenVirtualizationAllowed: return "TokenVirtualizationAllowed";
        case TokenVirtualizationEnabled: return "TokenVirtualizationEnabled";
        case TokenIntegrityLevel: return "TokenIntegrityLevel";
        case TokenUIAccess: return "TokenUIAccess";
        case TokenMandatoryPolicy: return "TokenMandatoryPolicy";
        case TokenLogonSid: return "TokenLogonSid";
        case TokenIsAppContainer: return "TokenIsAppContainer";
        case TokenCapabilities: return "TokenCapabilities";
        case TokenAppContainerSid: return "TokenAppContainerSid";
        case TokenAppContainerNumber: return "TokenAppContainerNumber";
        case TokenUserClaimAttributes: return "TokenUserClaimAttributes";
        case TokenDeviceClaimAttributes: return "TokenDeviceClaimAttributes";
        case TokenRestrictedUserClaimAttributes: return "TokenRestrictedUserClaimAttributes";
        case TokenRestrictedDeviceClaimAttributes: return "TokenRestrictedDeviceClaimAttributes";
        case TokenDeviceGroups: return "TokenDeviceGroups";
        case TokenRestrictedDeviceGroups: return "TokenRestrictedDeviceGroups";
        case TokenSecurityAttributes: return "TokenSecurityAttributes";
        case TokenIsRestricted: return "TokenIsRestricted";
        default: return "Unknown";
    }
}

void PrintTokenInformation(HANDLE hToken) {
    printf("\n====================== TOKEN INFORMATION ======================\n");
    
    for (int infoClass = TokenUser; infoClass <= TokenIsRestricted; infoClass++) {
        DWORD dwLength = 0;
        TOKEN_INFORMATION_CLASS tokenInfoClass = static_cast<TOKEN_INFORMATION_CLASS>(infoClass);
        const char* className = GetTokenInfoClassName(tokenInfoClass);
        
        printf("\n[%s]:\n", className);
        
        if (!GetTokenInformation(hToken, tokenInfoClass, NULL, 0, &dwLength)) {
            DWORD err = GetLastError();
            
            if (err == ERROR_INSUFFICIENT_BUFFER) {
                std::vector<BYTE> buffer(dwLength);
                
                if (GetTokenInformation(hToken, tokenInfoClass, buffer.data(), dwLength, &dwLength)) {
                    switch (tokenInfoClass) {
                        case TokenUser: {
                            PTOKEN_USER ptu = (PTOKEN_USER)buffer.data();
                            std::string sidStr = SidToString(ptu->User.Sid);
                            std::string accountName = GetAccountNameFromSid(ptu->User.Sid);
                            printf("  User: %s\n", accountName.c_str());
                            printf("  SID: %s\n", sidStr.c_str());
                            break;
                        }
                        
                        case TokenGroups: {
                            PTOKEN_GROUPS ptg = (PTOKEN_GROUPS)buffer.data();
                            printf("  Group Count: %d\n", ptg->GroupCount);
                            PrintGroups(ptg);
                            break;
                        }
                        
                        case TokenPrivileges: {
                            PTOKEN_PRIVILEGES ptp = (PTOKEN_PRIVILEGES)buffer.data();
                            printf("  Privilege Count: %d\n", ptp->PrivilegeCount);
                            PrintPrivileges(ptp);
                            break;
                        }
                        
                        case TokenOwner: {
                            PTOKEN_OWNER pto = (PTOKEN_OWNER)buffer.data();
                            std::string sidStr = SidToString(pto->Owner);
                            std::string accountName = GetAccountNameFromSid(pto->Owner);
                            printf("  Owner: %s\n", accountName.c_str());
                            printf("  Owner SID: %s\n", sidStr.c_str());
                            break;
                        }
                        
                        case TokenPrimaryGroup: {
                            PTOKEN_PRIMARY_GROUP ptpg = (PTOKEN_PRIMARY_GROUP)buffer.data();
                            std::string sidStr = SidToString(ptpg->PrimaryGroup);
                            std::string accountName = GetAccountNameFromSid(ptpg->PrimaryGroup);
                            printf("  Primary Group: %s\n", accountName.c_str());
                            printf("  Primary Group SID: %s\n", sidStr.c_str());
                            break;
                        }
                        
                        case TokenDefaultDacl: {
                            PTOKEN_DEFAULT_DACL ptdd = (PTOKEN_DEFAULT_DACL)buffer.data();
                            printf("  Default DACL:\n");
                            PrintAcl(ptdd->DefaultDacl);
                            break;
                        }
                        
                        case TokenSource: {
                            PTOKEN_SOURCE pts = (PTOKEN_SOURCE)buffer.data();
                            printf("  Source Name: %.8s\n", pts->SourceName);
                            printf("  Source Identifier: 0x%08lX\n", pts->SourceIdentifier);
                            break;
                        }
                        
                        case TokenType: {
                            PTOKEN_TYPE ptt = (PTOKEN_TYPE)buffer.data();
                            printf("  Token Type: %s\n", 
                                  (*ptt == TokenPrimary) ? "Primary" : "Impersonation");
                            break;
                        }
                        
                        case TokenImpersonationLevel: {
                            PSECURITY_IMPERSONATION_LEVEL psil = (PSECURITY_IMPERSONATION_LEVEL)buffer.data();
                            printf("  Impersonation Level: ");
                            switch (*psil) {
                                case SecurityAnonymous: printf("Anonymous\n"); break;
                                case SecurityIdentification: printf("Identification\n"); break;
                                case SecurityImpersonation: printf("Impersonation\n"); break;
                                case SecurityDelegation: printf("Delegation\n"); break;
                                default: printf("Unknown (%d)\n", *psil); break;
                            }
                            break;
                        }
                        
                        case TokenStatistics: {
                            PTOKEN_STATISTICS pts = (PTOKEN_STATISTICS)buffer.data();
                            printf("  Token ID: 0x%08lX:0x%08lX\n", 
                                  pts->TokenId.HighPart, pts->TokenId.LowPart);
                            printf("  Authentication ID: 0x%08lX:0x%08lX\n", 
                                  pts->AuthenticationId.HighPart, pts->AuthenticationId.LowPart);
                            printf("  Expiration Time: 0x%08lX:0x%08lX\n", 
                                  pts->ExpirationTime.HighPart, pts->ExpirationTime.LowPart);
                            printf("  Token Type: %s\n", 
                                  (pts->TokenType == TokenPrimary) ? "Primary" : "Impersonation");
                            printf("  Impersonation Level: %d\n", pts->ImpersonationLevel);
                            printf("  Dynamic Charged: %lu\n", pts->DynamicCharged);
                            printf("  Dynamic Available: %lu\n", pts->DynamicAvailable);
                            printf("  Group Count: %lu\n", pts->GroupCount);
                            printf("  Privilege Count: %lu\n", pts->PrivilegeCount);
                            printf("  Modified ID: 0x%08lX:0x%08lX\n", 
                                  pts->ModifiedId.HighPart, pts->ModifiedId.LowPart);
                            break;
                        }
                        
                        case TokenRestrictedSids: {
                            PTOKEN_GROUPS ptg = (PTOKEN_GROUPS)buffer.data();
                            printf("  Restricted SID Count: %d\n", ptg->GroupCount);
                            PrintGroups(ptg, "Restricted SID");
                            break;
                        }
                        
                        case TokenSessionId: {
                            PDWORD psid = (PDWORD)buffer.data();
                            printf("  Session ID: %lu\n", *psid);
                            break;
                        }
                        
                        case TokenElevationType: {
                            PTOKEN_ELEVATION_TYPE pte = (PTOKEN_ELEVATION_TYPE)buffer.data();
                            printf("  Elevation Type: ");
                            switch (*pte) {
                                case TokenElevationTypeDefault: printf("Default\n"); break;
                                case TokenElevationTypeFull: printf("Full\n"); break;
                                case TokenElevationTypeLimited: printf("Limited\n"); break;
                                default: printf("Unknown (%d)\n", *pte); break;
                            }
                            break;
                        }
                        
                        case TokenElevation: {
                            PTOKEN_ELEVATION pte = (PTOKEN_ELEVATION)buffer.data();
                            printf("  Token Elevated: %s\n", pte->TokenIsElevated ? "Yes" : "No");
                            break;
                        }
                        
                        case TokenHasRestrictions: {
                            PBOOL phr = (PBOOL)buffer.data();
                            printf("  Has Restrictions: %s\n", *phr ? "Yes" : "No");
                            break;
                        }
                        
                        case TokenVirtualizationAllowed: {
                            PBOOL pva = (PBOOL)buffer.data();
                            printf("  Virtualization Allowed: %s\n", *pva ? "Yes" : "No");
                            break;
                        }
                        
                        case TokenVirtualizationEnabled: {
                            PBOOL pve = (PBOOL)buffer.data();
                            printf("  Virtualization Enabled: %s\n", *pve ? "Yes" : "No");
                            break;
                        }
                        
                        case TokenIntegrityLevel: {
                            PTOKEN_MANDATORY_LABEL ptl = (PTOKEN_MANDATORY_LABEL)buffer.data();
                            DWORD integrityLevel = *GetSidSubAuthority(ptl->Label.Sid, 
                                (DWORD)(UCHAR)(*GetSidSubAuthorityCount(ptl->Label.Sid)-1));
                            
                            printf("  Integrity Level: ");
                            if (integrityLevel == SECURITY_MANDATORY_LOW_RID) {
                                printf("Low (0x%08lX)\n", integrityLevel);
                            } else if (integrityLevel >= SECURITY_MANDATORY_MEDIUM_RID && 
                                      integrityLevel < SECURITY_MANDATORY_HIGH_RID) {
                                printf("Medium (0x%08lX)\n", integrityLevel);
                            } else if (integrityLevel >= SECURITY_MANDATORY_HIGH_RID && 
                                      integrityLevel < SECURITY_MANDATORY_SYSTEM_RID) {
                                printf("High (0x%08lX)\n", integrityLevel);
                            } else if (integrityLevel >= SECURITY_MANDATORY_SYSTEM_RID) {
                                printf("System (0x%08lX)\n", integrityLevel);
                            } else {
                                printf("Unknown (0x%08lX)\n", integrityLevel);
                            }
                            break;
                        }
                        
                        case TokenUIAccess: {
                            PBOOL pua = (PBOOL)buffer.data();
                            printf("  UI Access: %s\n", *pua ? "Yes" : "No");
                            break;
                        }
                        
                        case TokenMandatoryPolicy: {
					        DWORD policy = *(DWORD*)buffer.data();
					        printf("  Mandatory Policy: 0x%08lX\n", policy);
					        printf("    - No Write Up: %s\n", (policy & 0x1) ? "Yes" : "No");
					        printf("    - New Process Min: %s\n", (policy & 0x2) ? "Yes" : "No");
					        break;
					    }
                        
                        case TokenLogonSid: {
                            PTOKEN_GROUPS ptg = (PTOKEN_GROUPS)buffer.data();
                            printf("  Logon SID Count: %d\n", ptg->GroupCount);
                            for (DWORD i = 0; i < ptg->GroupCount; i++) {
                                if (ptg->Groups[i].Attributes & SE_GROUP_LOGON_ID) {
                                    std::string sidStr = SidToString(ptg->Groups[i].Sid);
                                    printf("    Logon SID: %s\n", sidStr.c_str());
                                }
                            }
                            break;
                        }
                        
                        case TokenIsAppContainer: {
                            PBOOL piac = (PBOOL)buffer.data();
                            printf("  Is AppContainer: %s\n", *piac ? "Yes" : "No");
                            break;
                        }
                        
                        case TokenAppContainerSid: {
                            PTOKEN_APPCONTAINER_INFORMATION ptai = (PTOKEN_APPCONTAINER_INFORMATION)buffer.data();
                            if (ptai->TokenAppContainer) {
                                std::string sidStr = SidToString(ptai->TokenAppContainer);
                                printf("  AppContainer SID: %s\n", sidStr.c_str());
                            } else {
                                printf("  AppContainer SID: <NULL>\n");
                            }
                            break;
                        }
                        
                        case TokenAppContainerNumber: {
                            PDWORD pacn = (PDWORD)buffer.data();
                            printf("  AppContainer Number: %lu\n", *pacn);
                            break;
                        }
                        
                        case TokenIsRestricted: {
                            PBOOL pir = (PBOOL)buffer.data();
                            printf("  Is Restricted: %s\n", *pir ? "Yes" : "No");
                            break;
                        }
                        
                        // 以下信息类结构较复杂，只显示基本信息
                        case TokenGroupsAndPrivileges:
                        case TokenAccessInformation:
                        case TokenUserClaimAttributes:
                        case TokenDeviceClaimAttributes:
                        case TokenRestrictedUserClaimAttributes:
                        case TokenRestrictedDeviceClaimAttributes:
                        case TokenDeviceGroups:
                        case TokenRestrictedDeviceGroups:
                        case TokenSecurityAttributes: {
                            printf("  Data Size: %lu bytes\n", dwLength);
                            printf("  [Complex structure - raw data available]\n");
                            break;
                        }
                        
                        // 较少使用的信息类
                        case TokenSessionReference:
                        case TokenSandBoxInert:
                        case TokenAuditPolicy:
                        case TokenOrigin:
                        case TokenLinkedToken:
                        case TokenCapabilities: {
                            printf("  Data Size: %lu bytes\n", dwLength);
                            // 这些结构较复杂或较少使用，只显示大小
                            break;
                        }
                        
                        default:
                            printf("  Data Size: %lu bytes\n", dwLength);
                            break;
                    }
                } else {
                    PrintError("GetTokenInformation failed after buffer allocation");
                }
            } else if (err == ERROR_INVALID_PARAMETER) {
                printf("  <Not supported on this system>\n");
            } else if (err == ERROR_ACCESS_DENIED) {
                printf("  <Access denied>\n");
            } else {
                printf("  <Failed with error %d>\n", err);
            }
        } else {
            printf("  <Unexpected success with zero buffer>\n");
        }
    }
}

BOOL ViewProcessToken(DWORD dwProcessId) {
    HANDLE hProcess = NULL;
    HANDLE hToken = NULL;
    BOOL bResult = FALSE;
    
    printf("=====================================================\n");
    printf("  TOKEN INFORMATION VIEWER\n");
    printf("  Process ID: %lu\n", dwProcessId);
    printf("=====================================================\n");
    
    hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_QUERY_INFORMATION, 
                          FALSE, dwProcessId);
    if (hProcess == NULL) {
        hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwProcessId);
    }
    
    if (hProcess == NULL) {
        PrintError("OpenProcess failed");
        return FALSE;
    }
    
    if (!OpenProcessToken(hProcess, TOKEN_QUERY | TOKEN_QUERY_SOURCE, &hToken)) {
        PrintError("OpenProcessToken failed");
        CloseHandle(hProcess);
        return FALSE;
    }
    
    char processName[MAX_PATH] = {0};
    if (GetModuleFileNameExA(hProcess, NULL, processName, MAX_PATH)) {
        printf("Process Name: %s\n", processName);
    }
    
    PrintTokenInformation(hToken);
    
    if (hToken != NULL) CloseHandle(hToken);
    if (hProcess != NULL) CloseHandle(hProcess);
    
    return TRUE;
}
