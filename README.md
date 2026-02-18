# NtCreateToken_GUI

A small tool which can use Windows' `NtCreateToken` API to create a process with any groups and privileges.  
一个使用 Windows `NtCreateToken` API 创建进程的小工具，可任意指定进程令牌的组和权限。

---

## Introduction / 介绍

**English**  
This tool demonstrates the use of the low-level `NtCreateToken` system call to construct a custom token and launch a process with it.  
You can specify the user SID, enable/disable privileges, and add arbitrary groups with their attributes.  
Both command-line (`NtCreateToken.exe`) and GUI (`NtCreateToken_GUI.exe`) versions are provided.

**中文**  
本工具演示了如何使用底层系统调用 `NtCreateToken` 构造自定义令牌并以此启动进程。  
您可以指定用户 SID、启用/禁用权限，以及添加任意组及其属性。  
同时提供了命令行版本（`NtCreateToken.exe`）和图形界面版本（`NtCreateToken_GUI.exe`）。

---

## Features / 功能特点

**English**  
- Create a process with a fully customized token  
- Specify user SID (default: SYSTEM S-1-5-18)  
- Enable or disable specific privileges (e.g., `SeDebugPrivilege`, `SeImpersonatePrivilege`)  
- Add groups with attributes like `SE_GROUP_ENABLED`, `SE_GROUP_OWNER`  
- `[Command-line Version]` Display token information of an existing process (`/t <pid>`)  
- GUI version for easy privilege/group selection  

**中文**  
- 使用完全自定义的令牌创建进程  
- 指定用户 SID（默认：SYSTEM S-1-5-18）  
- 启用或禁用特定权限（例如 `SeDebugPrivilege`、`SeImpersonatePrivilege`）  
- 添加组并指定属性，如 `SE_GROUP_ENABLED`、`SE_GROUP_OWNER`  
- `[在命令行版本下]` 查看现有进程的令牌信息（`/t <pid>`）  
- 提供图形界面版本，方便选择权限和组  

---

## Usage / 使用方法

### Command Line / 命令行

```
Usage: NtCreateToken.exe [options]
Options:
  /c <command>  Command line to execute (default: cmd.exe)
  /s <SID>      User SID for token (default: S-1-5-18 = SYSTEM)
  /p <privs>    Privileges separated by ';' (default: all privileges)
                You can add ":N" after a privilege to disable it.
  /g <groups>   Groups separated by ';', group format: SID,attr1*attr2
  /n            Don't show the token's information before running process
  /t <pid>      Show a process's token's information
  /? or /h      Show this help

Examples:
  NtCreateToken.exe
  NtCreateToken.exe /c "powershell.exe" /s S-1-5-21-...
  NtCreateToken.exe /p "SeDebugPrivilege;SeImpersonatePrivilege"
  NtCreateToken.exe /g "S-1-5-32-544,SE_GROUP_ENABLED*SE_GROUP_OWNER;S-1-1-0,SE_GROUP_ENABLED"
  NtCreateToken.exe /t 1234
```

**Notes / 说明**  
- Privilege names are Windows privilege constants (e.g., `SeDebugPrivilege`). Append `:N` to disable a privilege.  
- Group attributes can be combined with `*` (e.g., `SE_GROUP_ENABLED*SE_GROUP_OWNER`). Available attributes: `SE_GROUP_MANDATORY`, `SE_GROUP_ENABLED_BY_DEFAULT`, `SE_GROUP_ENABLED`, `SE_GROUP_OWNER`, `SE_GROUP_USE_FOR_DENY_ONLY`, `SE_GROUP_INTEGRITY`, `SE_GROUP_INTEGRITY_ENABLED`, `SE_GROUP_LOGON_ID`, `SE_GROUP_RESOURCE`.  
- The tool requires **SeCreateTokenPrivilege** . Run as Administrator to get it.  

- 权限名称使用 Windows 权限常量（例如 `SeDebugPrivilege`）。在权限后添加 `:N` 可禁用该权限。  
- 组属性可以使用 `*` 组合（例如 `SE_GROUP_ENABLED*SE_GROUP_OWNER`）。可用属性：`SE_GROUP_MANDATORY`, `SE_GROUP_ENABLED_BY_DEFAULT`, `SE_GROUP_ENABLED`, `SE_GROUP_OWNER`, `SE_GROUP_USE_FOR_DENY_ONLY`, `SE_GROUP_INTEGRITY`, `SE_GROUP_INTEGRITY_ENABLED`, `SE_GROUP_LOGON_ID`, `SE_GROUP_RESOURCE`。  
- 该工具需要 **SeCreateTokenPrivilege** 。请以管理员身份运行以获取该特权。

---

### GUI / 图形界面

Simply run `NtCreateToken_GUI.exe`.  
The interface allows you to:  
- Enter the command line to execute  
- Choose user SID from a list or input custom SID  
- Select privileges and mark them as enabled/disabled  
- Add groups with SID and attribute flags

The GUI version requires at least [.NET 6.0](https://dotnet.microsoft.com/zh-cn/download/dotnet/6.0)

直接运行 `NtCreateToken_GUI.exe` 即可。  
界面功能包括：  
- 输入要执行的命令行  
- 从列表中选择用户 SID 或输入自定义 SID  
- 选择权限并标记为启用/禁用  
- 添加组（指定 SID 和属性标志）

GUI 版本要求至少 [.NET 6.0](https://dotnet.microsoft.com/zh-cn/download/dotnet/6.0)

---

## Screenshots / 截图

![GUI Screenshot](https://github.com/user-attachments/assets/2d086d4b-4019-4fb4-9c51-71a582fc288e)  
*Main window of NtCreateToken_GUI*  
*NtCreateToken_GUI 主窗口*

![Process Info](https://github.com/user-attachments/assets/937b0e1f-f3ac-4066-8bfb-933c8b5b2e8f)
*Information about the created process*  
*创建出的进程的信息*

---

## License / 许可证

[MIT License](LICENSE)

---

**Note / 注意**  
This tool uses low-level Windows APIs. Use it responsibly and only on systems you own or have permission to test.  
本工具使用了底层 Windows API。请负责任地使用，并仅在你拥有或有权测试的系统上运行。
