#if WITH_MAIN_PATCHER
#include "hooking.h"
#endif
#include <initguid.h>
#include <stdio.h>
#include <stdbool.h>
#include <Windows.h>
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib") // required by funchook
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")
#include <windowsx.h>
#include <Uxtheme.h>
#pragma comment(lib, "UxTheme.lib")
#include <Shlobj_core.h>
#include <propvarutil.h>
#pragma comment(lib, "Propsys.lib")
#include <commctrl.h>
#pragma comment(lib, "Comctl32.lib")
#include <dwmapi.h>
#pragma comment(lib, "Dwmapi.lib")
#include <roapi.h>
#include <ShellScalingApi.h>
#include <DbgHelp.h>
#pragma comment(lib, "Dbghelp.lib")
#include <tlhelp32.h>
#include <UIAutomationClient.h>
#include <math.h>
#include "lvt.h"
#if WITH_MAIN_PATCHER
#include <valinet/pdb/pdb.h>
#endif
#if defined(DEBUG) | defined(_DEBUG)
#define _LIBVALINET_DEBUG_HOOKING_IATPATCH
#endif
#include <valinet/hooking/iatpatch.h>
#include <valinet/utility/memmem.h>
#include "../ep_weather_host/ep_weather.h"
#if WITH_MAIN_PATCHER
#include "../ep_weather_host/ep_weather_host_h.h"
IEPWeather* epw = NULL;
CRITICAL_SECTION lock_epw;
int prev_total_h = 0;
HWND PeopleButton_LastHWND = NULL;
#endif
#include "osutility.h"
HANDLE hServiceWindowThread = NULL;
//#pragma comment(lib, "Winmm.lib")
#if !WITH_MAIN_PATCHER
RTL_OSVERSIONINFOW global_rovi;
DWORD32 global_ubr;
#endif
#include <featurestagingapi.h>
#ifndef WITH_SMA_PATCH_REPORT
#define WITH_SMA_PATCH_REPORT 1
#endif
#if WITH_SMA_PATCH_REPORT
#include <userenv.h>
#pragma comment(lib, "Userenv.lib")
#endif

#define CHECKFOREGROUNDELAPSED_TIMEOUT 300
#define POPUPMENU_SAFETOREMOVE_TIMEOUT 300
#define POPUPMENU_BLUETOOTH_TIMEOUT 700
#define POPUPMENU_PNIDUI_TIMEOUT 300
#define POPUPMENU_SNDVOLSSO_TIMEOUT 300
#define POPUPMENU_INPUTSWITCH_TIMEOUT 700
#define POPUPMENU_WINX_TIMEOUT 700
#define POPUPMENU_EX_ELAPSED 300

// Only use this for developing fixes for 22621.2134+ using 22621.1413-1992.
#define USE_MOMENT_3_FIXES_ON_MOMENT_2 0

BOOL bIsExplorerProcess = FALSE;
BOOL bInstanced = FALSE;
HWND archivehWnd;
DWORD bOldTaskbar = TRUE;
DWORD bWasOldTaskbarSet = FALSE;
DWORD bAllocConsole = FALSE;
DWORD bHideExplorerSearchBar = FALSE;
DWORD bShrinkExplorerAddressBar = FALSE;
DWORD bMicaEffectOnTitlebar = FALSE;
DWORD bHideIconAndTitleInExplorer = FALSE;
DWORD bHideControlCenterButton = FALSE;
DWORD bFlyoutMenus = TRUE;
DWORD bCenterMenus = TRUE;
DWORD bSkinMenus = TRUE;
DWORD bSkinIcons = TRUE;
DWORD bReplaceNetwork = FALSE;
DWORD bEnableArchivePlugin = FALSE;
DWORD bMonitorOverride = TRUE;
DWORD bOpenAtLogon = FALSE;
DWORD bClockFlyoutOnWinC = FALSE;
DWORD bUseClassicDriveGrouping = FALSE;
DWORD dwFileExplorerCommandUI = 9999;
DWORD bLegacyFileTransferDialog = FALSE;
DWORD bDisableImmersiveContextMenu = FALSE;
DWORD bClassicThemeMitigations = FALSE;
DWORD bWasClassicThemeMitigationsSet = FALSE;
DWORD bHookStartMenu = TRUE;
DWORD bPropertiesInWinX = FALSE;
DWORD bNoMenuAccelerator = FALSE;
DWORD dwIMEStyle = 0;
DWORD dwTaskbarAl = 1;
DWORD bShowUpdateToast = FALSE;
DWORD bToolbarSeparators = FALSE;
DWORD bTaskbarAutohideOnDoubleClick = FALSE;
DWORD dwOrbStyle = 0;
DWORD bEnableSymbolDownload = TRUE;
DWORD dwAltTabSettings = 0;
DWORD bDisableAeroSnapQuadrants = FALSE;
DWORD dwSnapAssistSettings = 0;
DWORD dwStartShowClassicMode = 0;
BOOL bDoNotRedirectSystemToSettingsApp = FALSE;
BOOL bDoNotRedirectProgramsAndFeaturesToSettingsApp = FALSE;
BOOL bDoNotRedirectDateAndTimeToSettingsApp = FALSE;
BOOL bDoNotRedirectNotificationIconsToSettingsApp = FALSE;
BOOL bDisableOfficeHotkeys = FALSE;
BOOL bDisableWinFHotkey = FALSE;
DWORD bNoPropertiesInContextMenu = FALSE;
HMODULE hModule = NULL;
HANDLE hShell32 = NULL;
// HANDLE hDelayedInjectionThread = NULL;
HANDLE hSwsSettingsChanged = NULL;
HANDLE hSwsOpacityMaybeChanged = NULL;
HANDLE hWin11AltTabInitialized = NULL;
BYTE* lpShouldDisplayCCButton = NULL;
HANDLE hCanStartSws = NULL;
DWORD dwWeatherViewMode = EP_WEATHER_VIEW_ICONTEXT;
DWORD dwWeatherTemperatureUnit = EP_WEATHER_TUNIT_CELSIUS;
DWORD dwWeatherUpdateSchedule = EP_WEATHER_UPDATE_NORMAL;
DWORD bWeatherFixedSize = FALSE;
DWORD dwWeatherTheme = 0;
DWORD dwWeatherGeolocationMode = 0;
DWORD dwWeatherWindowCornerPreference = DWMWCP_ROUND;
DWORD dwWeatherDevMode = FALSE;
DWORD dwWeatherIconPack = EP_WEATHER_ICONPACK_MICROSOFT;
DWORD dwWeatherToLeft = 0;
DWORD dwWeatherContentsMode = 0;
DWORD dwWeatherZoomFactor = 0;
WCHAR* wszWeatherTerm = NULL;
WCHAR* wszWeatherLanguage = NULL;
WCHAR* wszEPWeatherKillswitch = NULL;
HANDLE hEPWeatherKillswitch = NULL;
DWORD bWasPinnedItemsActAsQuickLaunch = FALSE;
DWORD bPinnedItemsActAsQuickLaunch = FALSE;
DWORD bWasRemoveExtraGapAroundPinnedItems = FALSE;
DWORD bRemoveExtraGapAroundPinnedItems = FALSE;
DWORD dwUndeadStartCorner = FALSE;
DWORD dwOldTaskbarAl = 0b110;
DWORD dwMMOldTaskbarAl = 0b110;
DWORD dwTaskbarSmallIcons = FALSE;
DWORD dwShowTaskViewButton = FALSE;
DWORD dwSearchboxTaskbarMode = FALSE;
DWORD dwTaskbarDa = FALSE;
DWORD bDisableSpotlightIcon = FALSE;
DWORD dwSpotlightDesktopMenuMask = 0;
DWORD dwSpotlightUpdateSchedule = 0;
int Code = 0;
HRESULT InjectStartFromExplorer();
BOOL InvokeClockFlyout();
void WINAPI Explorer_RefreshUI(int unused);
int (*SHWindowsPolicy)(REFIID);

#define ORB_STYLE_WINDOWS10 0
#define ORB_STYLE_WINDOWS11 1
#define ORB_STYLE_TRANSPARENT 2
typedef struct _OrbInfo
{
    HTHEME hTheme;
    UINT dpi;
} OrbInfo;

void* P_Icon_Light_Search = NULL;
DWORD S_Icon_Light_Search = 0;

void* P_Icon_Light_TaskView = NULL;
DWORD S_Icon_Light_TaskView = 0;

void* P_Icon_Light_Widgets = NULL;
DWORD S_Icon_Light_Widgets = 0;

void* P_Icon_Dark_Search = NULL;
DWORD S_Icon_Dark_Search = 0;

void* P_Icon_Dark_TaskView = NULL;
DWORD S_Icon_Dark_TaskView = 0;

void* P_Icon_Dark_Widgets = NULL;
DWORD S_Icon_Dark_Widgets = 0;

BOOL g_bIsDesktopRaised = FALSE;

#include "utility.h"
#include "Localization.h"
#include "resource.h"
#include "../ep_gui/resources/EPSharedResources.h"
#ifdef USE_PRIVATE_INTERFACES
#include "ep_private.h"
#endif
#if WITH_MAIN_PATCHER
#include "symbols.h"
#include "dxgi_imp.h"
#include "ArchiveMenu.h"
#include "InputSwitch.h"
#include "StartupSound.h"
#include "StartMenu.h"
#include "TaskbarCenter.h"
#include "../libs/sws/SimpleWindowSwitcher/sws_WindowSwitcher.h"
#endif
#include "SettingsMonitor.h"
#include "HideExplorerSearchBar.h"
#include "ImmersiveFlyouts.h"
#include "updates.h"
DWORD dwUpdatePolicy = UPDATE_POLICY_DEFAULT;
wchar_t* EP_TASKBAR_LENGTH_PROP_NAME = L"EPTBLEN";

#if WITH_MAIN_PATCHER
#define MAX_NUM_MONITORS 30
MonitorListEntry hMonitorList[MAX_NUM_MONITORS];
DWORD dwMonitorCount = 0;
#endif

HRESULT WINAPI _DllRegisterServer();
HRESULT WINAPI _DllUnregisterServer();
HRESULT WINAPI _DllCanUnloadNow();
HRESULT WINAPI _DllGetClassObject(
    REFCLSID rclsid,
    REFIID   riid,
    LPVOID* ppv
);

// {09717D01-5D10-4FB5-BD05-46380B5165AA}
#define CLSID_EPStart10_TEXT "{A6EA9C2D-4982-4827-9204-0AC532959F6D}"
#define EPStart10_AnimationsPatched "EPStart10_AnimationsPatched_" CLSID_EPStart10_TEXT
DEFINE_GUID(CLSID_EPStart10,
    0x9717d01, 0x5d10, 0x4fb5, 0xbd, 0x5, 0x46, 0x38, 0xb, 0x51, 0x65, 0xaa);

#pragma region "Generics"
#if WITH_MAIN_PATCHER
HWND GetMonitorInfoFromPointForTaskbarFlyoutActivation(POINT ptCursor, DWORD dwFlags, LPMONITORINFO lpMi)
{
    HMONITOR hMonitor = MonitorFromPoint(ptCursor, dwFlags);
    HWND hWnd = NULL;
    do
    {
        hWnd = FindWindowEx(
            NULL,
            hWnd,
            L"Shell_SecondaryTrayWnd",
            NULL
        );
        if (MonitorFromWindow(hWnd, dwFlags) == hMonitor)
        {
            if (lpMi)
            {
                GetMonitorInfo(
                    MonitorFromPoint(
                        ptCursor,
                        dwFlags
                    ),
                    lpMi
                );
            }
            break;
        }
    } while (hWnd);
    if (!hWnd)
    {
        hWnd = FindWindowEx(
            NULL,
            NULL,
            L"Shell_TrayWnd",
            NULL
        );
        //ptCursor.x = 0;
        //ptCursor.y = 0;
        if (lpMi)
        {
            GetMonitorInfo(
                MonitorFromWindow(
                    hWnd,
                    dwFlags
                ),
                lpMi
            );
        }
    }
    return hWnd;
}

POINT GetDefaultWinXPosition(BOOL bUseRcWork, BOOL* lpBottom, BOOL* lpRight, BOOL bAdjust, BOOL bToRight)
{
    if (lpBottom) *lpBottom = FALSE;
    if (lpRight) *lpRight = FALSE;
    POINT point;
    point.x = 0;
    point.y = 0;
    POINT ptCursor;
    GetCursorPos(&ptCursor);
    MONITORINFO mi;
    mi.cbSize = sizeof(MONITORINFO);
    HWND hWnd = GetMonitorInfoFromPointForTaskbarFlyoutActivation(
        ptCursor,
        MONITOR_DEFAULTTOPRIMARY,
        &mi
    );
    if (hWnd)
    {
        RECT rc;
        GetWindowRect(hWnd, &rc);
        if (rc.left - mi.rcMonitor.left <= 0)
        {
            if (bUseRcWork)
            {
                point.x = mi.rcWork.left;
            }
            else
            {
                point.x = mi.rcMonitor.left;
            }
            if (bToRight)
            {
                point.x = mi.rcMonitor.right;
            }
            if (bAdjust)
            {
                point.x++;
            }
            if (rc.top - mi.rcMonitor.top <= 0)
            {
                if (bUseRcWork)
                {
                    point.y = mi.rcWork.top;
                }
                else
                {
                    point.y = mi.rcMonitor.top;
                }
                if (bAdjust)
                {
                    point.y++;
                }
            }
            else
            {
                if (lpBottom) *lpBottom = TRUE;
                if (bUseRcWork)
                {
                    point.y = mi.rcWork.bottom;
                }
                else
                {
                    point.y = mi.rcMonitor.bottom;
                }
                if (bAdjust)
                {
                    point.y--;
                }
            }
        }
        else
        {
            if (lpRight) *lpRight = TRUE;
            if (bUseRcWork)
            {
                point.x = mi.rcWork.right;
            }
            else
            {
                point.x = mi.rcMonitor.right;
            }
            if (bAdjust)
            {
                point.x--;
            }
            if (rc.top - mi.rcMonitor.top <= 0)
            {
                if (bUseRcWork)
                {
                    point.y = mi.rcWork.top;
                }
                else
                {
                    point.y = mi.rcMonitor.top;
                }
                if (bAdjust)
                {
                    point.y++;
                }
            }
            else
            {
                if (lpBottom) *lpBottom = TRUE;
                if (bUseRcWork)
                {
                    point.y = mi.rcWork.bottom;
                }
                else
                {
                    point.y = mi.rcMonitor.bottom;
                }
                if (bAdjust)
                {
                    point.y--;
                }
            }
        }
    }
    return point;
}

BOOL TerminateShellExperienceHost()
{
    BOOL bRet = FALSE;
    WCHAR wszKnownPath[MAX_PATH];
    GetWindowsDirectoryW(wszKnownPath, MAX_PATH);
    wcscat_s(wszKnownPath, MAX_PATH, L"\\SystemApps\\ShellExperienceHost_cw5n1h2txyewy\\ShellExperienceHost.exe");
    HANDLE hSnapshot = NULL;
    PROCESSENTRY32 pe32;
    ZeroMemory(&pe32, sizeof(PROCESSENTRY32));
    pe32.dwSize = sizeof(PROCESSENTRY32);
    hSnapshot = CreateToolhelp32Snapshot(
        TH32CS_SNAPPROCESS,
        0
    );
    if (Process32First(hSnapshot, &pe32) == TRUE)
    {
        do
        {
            if (!wcscmp(pe32.szExeFile, TEXT("ShellExperienceHost.exe")))
            {
                HANDLE hProcess = OpenProcess(
                    PROCESS_QUERY_LIMITED_INFORMATION |
                    PROCESS_TERMINATE,
                    FALSE,
                    pe32.th32ProcessID
                );
                if (hProcess)
                {
                    TCHAR wszProcessPath[MAX_PATH];
                    DWORD dwLength = MAX_PATH;
                    QueryFullProcessImageNameW(
                        hProcess,
                        0,
                        wszProcessPath,
                        &dwLength
                    );
                    if (!_wcsicmp(wszProcessPath, wszKnownPath))
                    {
                        TerminateProcess(hProcess, 0);
                        bRet = TRUE;
                    }
                    CloseHandle(hProcess);
                    hProcess = NULL;
                }
            }
        } while (Process32Next(hSnapshot, &pe32) == TRUE);
        if (hSnapshot)
        {
            CloseHandle(hSnapshot);
        }
    }
    return bRet;
}

long long elapsedCheckForeground = 0;
HANDLE hCheckForegroundThread = NULL;
DWORD CheckForegroundThread(DWORD dwMode)
{
    printf("Started \"Check foreground window\" thread.\n");
    UINT i = 0;
    while (TRUE)
    {
        wchar_t text[200];
        ZeroMemory(text, 200);
        GetClassNameW(GetForegroundWindow(), text, 200);
        if (!wcscmp(text, L"Windows.UI.Core.CoreWindow"))
        {
            break;
        }
        i++;
        if (i >= 15) break;
        Sleep(100);
    }
    while (TRUE)
    {
        wchar_t text[200];
        ZeroMemory(text, 200);
        GetClassNameW(GetForegroundWindow(), text, 200);
        if (wcscmp(text, L"Windows.UI.Core.CoreWindow"))
        {
            break;
        }
        Sleep(100);
    }
    elapsedCheckForeground = milliseconds_now();
    if (!dwMode)
    {
        RegDeleteTreeW(HKEY_CURRENT_USER, _T(SEH_REGPATH));
        TerminateShellExperienceHost();
        Sleep(100);
    }
    printf("Ended \"Check foreground window\" thread.\n");
    return 0;
}

void LaunchNetworkTargets(DWORD dwTarget)
{
    // very helpful: https://www.tenforums.com/tutorials/3123-clsid-key-guid-shortcuts-list-windows-10-a.html
    if (!dwTarget)
    {
        InvokeFlyout(INVOKE_FLYOUT_SHOW, INVOKE_FLYOUT_NETWORK);
    }
    else if (dwTarget == 5)
    {
        ShellExecuteW(
            NULL,
            L"open",
            L"ms-availablenetworks:",
            NULL,
            NULL,
            SW_SHOWNORMAL
        );
    }
    else if (dwTarget == 6)
    {
        InvokeActionCenter();
        // ShellExecuteW(
        //     NULL,
        //     L"open",
        //     L"ms-actioncenter:controlcenter/&showFooter=true",
        //     NULL,
        //     NULL,
        //     SW_SHOWNORMAL
        // );
    }
    else if (dwTarget == 1)
    {
        ShellExecuteW(
            NULL,
            L"open",
            L"ms-settings:network",
            NULL,
            NULL,
            SW_SHOWNORMAL
        );
    }
    else if (dwTarget == 2)
    {
        HMODULE hVan = LoadLibraryW(L"van.dll");
        if (hVan)
        {
            long(*ShowVAN)(BOOL, BOOL, void*) = GetProcAddress(hVan, "ShowVAN");
            if (ShowVAN)
            {
                ShowVAN(0, 0, 0);
            }
            FreeLibrary(hVan);
        }
    }
    else if (dwTarget == 3)
    {
        ShellExecuteW(
            NULL,
            L"open",
            L"shell:::{8E908FC9-BECC-40f6-915B-F4CA0E70D03D}",
            NULL,
            NULL,
            SW_SHOWNORMAL
        );
    }
    else if (dwTarget == 4)
    {
        ShellExecuteW(
            NULL,
            L"open",
            L"shell:::{7007ACC7-3202-11D1-AAD2-00805FC1270E}",
            NULL,
            NULL,
            SW_SHOWNORMAL
        );
    }
}
#endif
#pragma endregion


#pragma region "Service Window"
#if WITH_MAIN_PATCHER
HWND hWndServiceWindow = NULL;

void FixUpCenteredTaskbar()
{
    HWND hwndPrimaryTray = FindWindowW(L"Shell_TrayWnd", NULL);
    PostMessageW(hwndPrimaryTray, WM_DWMCOMPOSITIONCHANGED, 0, 0); // uMsg = 0x31E in explorer!TrayUI::WndProc
    if (!TaskbarCenter_ShouldStartBeCentered(dwOldTaskbarAl) && hwndPrimaryTray)
    {
        HWND hwndStart = FindWindowExW(hwndPrimaryTray, NULL, L"Start", NULL);
        SetWindowPos(hwndStart, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
        InvalidateRect(hwndStart, NULL, TRUE);
    }
    if (!TaskbarCenter_ShouldStartBeCentered(dwMMOldTaskbarAl))
    {
        HWND hWnd = NULL;
        do
        {
            hWnd = FindWindowEx(
                NULL,
                hWnd,
                L"Shell_SecondaryTrayWnd",
                NULL
            );
            if (hWnd)
            {
                HWND hwndStart = FindWindowExW(hWnd, NULL, L"Start", NULL);
                SetWindowPos(hwndStart, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
                InvalidateRect(hwndStart, NULL, TRUE);
            }
        } while (hWnd);
    }
}

#define EP_SERVICE_WINDOW_CLASS_NAME L"EP_Service_Window_" _T(EP_CLSID)
LRESULT CALLBACK EP_Service_Window_WndProc(
    HWND hWnd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    static UINT s_uTaskbarRestart = 0;
    if (uMsg == WM_CREATE)
    {
        s_uTaskbarRestart = RegisterWindowMessageW(L"TaskbarCreated");
    }
    else if (uMsg == WM_HOTKEY && (wParam == 1 || wParam == 2))
    {
        InvokeClockFlyout();
        return 0;
    }
    else if (uMsg == s_uTaskbarRestart && bOldTaskbar && (dwOldTaskbarAl || dwMMOldTaskbarAl))
    {
        SetTimer(hWnd, 1, 1000, NULL);
    }
    else if (uMsg == WM_TIMER && wParam < 3)
    {
        FixUpCenteredTaskbar();
        if (wParam != 3 - 1) SetTimer(hWnd, wParam + 1, 1000, NULL);
        KillTimer(hWnd, wParam);
        return 0;
    }
    else if (uMsg == WM_TIMER && wParam == 10)
    {
        if (GetClassWord(GetForegroundWindow(), GCW_ATOM) != RegisterWindowMessageW(L"Windows.UI.Core.CoreWindow"))
        {
            DWORD dwVal = 1;
            RegSetKeyValueW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"TaskbarAl", REG_DWORD, &dwVal, sizeof(DWORD));
            KillTimer(hWnd, 10);
        }
        return 0;
    }
    else if (uMsg == WM_TIMER && wParam == 100)
    {
        if (IsSpotlightEnabled()) SpotlightHelper(SPOP_CLICKMENU_NEXTPIC, hWnd, NULL, NULL);
        printf("Refreshed Spotlight\n");
    }
    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}
DWORD EP_ServiceWindowThread(DWORD unused)
{
    WNDCLASS wc = { 0 };
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = EP_Service_Window_WndProc;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = EP_SERVICE_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    RegisterClassW(&wc);

    hWndServiceWindow = CreateWindowExW(
        0,
        EP_SERVICE_WINDOW_CLASS_NAME,
        0,
        WS_POPUP,
        0,
        0,
        0,
        0,
        0,
        0,
        GetModuleHandle(NULL),
        NULL
    );
    if (hWndServiceWindow)
    {
        if (IsSpotlightEnabled() && dwSpotlightUpdateSchedule) SetTimer(hWndServiceWindow, 100, dwSpotlightUpdateSchedule * 1000, NULL);
        if (bClockFlyoutOnWinC)
        {
            RegisterHotKey(hWndServiceWindow, 1, MOD_WIN | MOD_NOREPEAT, 'C');
        }
        RegisterHotKey(hWndServiceWindow, 2, MOD_WIN | MOD_ALT, 'D');
        MSG msg;
        BOOL bRet;
        while ((bRet = GetMessageW(&msg, NULL, 0, 0)) != 0)
        {
            if (bRet == -1)
            {
                break;
            }
            else if (!msg.hwnd)
            {
                if (msg.message == WM_USER + 1)
                {
                    EnterCriticalSection(&lock_epw);
                    if (epw)
                    {
                        epw->lpVtbl->Release(epw);
                        epw = NULL;
                        prev_total_h = 0;
                        if (PeopleButton_LastHWND) InvalidateRect(PeopleButton_LastHWND, NULL, TRUE);
                        //PlaySoundA((LPCTSTR)SND_ALIAS_SYSTEMASTERISK, NULL, SND_ALIAS_ID);
                    }
                    LeaveCriticalSection(&lock_epw);
                }
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        DestroyWindow(hWndServiceWindow);
    }
    SetEvent(hCanStartSws);
}
#endif
#pragma endregion


#pragma region "Toggle shell features"
// More details in explorer.exe!CTray::_HandleGlobalHotkey

BOOL CALLBACK ToggleImmersiveCallback(HWND hWnd, LPARAM lParam)
{
    WORD ClassWord;

    ClassWord = GetClassWord(hWnd, GCW_ATOM);
    if (ClassWord == RegisterWindowMessageW(L"WorkerW"))
    {
        PostMessageW(hWnd, WM_HOTKEY, lParam, 0);
    }

    return TRUE;
}

BOOL ToggleHelp()
{
    return PostMessageW(FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL), WM_HOTKEY, 505, 0);
}

BOOL ToggleRunDialog()
{
    return PostMessageW(FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL), WM_HOTKEY, 502, MAKELPARAM(MOD_WIN, 0x52));
}

BOOL ToggleSystemProperties()
{
    return PostMessageW(FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL), WM_HOTKEY, 512, 0);
}

BOOL FocusSystray()
{
    return PostMessageW(FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL), WM_HOTKEY, 514, 0);
}

BOOL TriggerAeroShake()
{
    return PostMessageW(FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL), WM_HOTKEY, 515, 0);
}

BOOL PeekDesktop()
{
    return PostMessageW(FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL), WM_HOTKEY, 516, 0);
}

BOOL ToggleEmojiPanel()
{
    return PostMessageW(FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL), WM_HOTKEY, 579, 0);
}

BOOL ShowDictationPanel()
{
    return PostMessageW(FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL), WM_HOTKEY, 577, 0);
}

BOOL ToggleClipboardViewer()
{
    return PostMessageW(FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL), WM_HOTKEY, 578, 0);
}

BOOL ToggleSearch()
{
    return PostMessageW(FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL), WM_HOTKEY, 507, MAKELPARAM(MOD_WIN, 0x53));
}

BOOL ToggleTaskView()
{
    return EnumThreadWindows(GetWindowThreadProcessId(FindWindowExW(NULL, NULL, L"ApplicationManager_ImmersiveShellWindow", NULL), NULL), ToggleImmersiveCallback, 11);
}

BOOL ToggleWidgetsPanel()
{
    return EnumThreadWindows(GetWindowThreadProcessId(FindWindowExW(NULL, NULL, L"ApplicationManager_ImmersiveShellWindow", NULL), NULL), ToggleImmersiveCallback, 0x66);
}

BOOL ToggleMainClockFlyout()
{
    return EnumThreadWindows(GetWindowThreadProcessId(FindWindowExW(NULL, NULL, L"ApplicationManager_ImmersiveShellWindow", NULL), NULL), ToggleImmersiveCallback, 0x6B);
}

BOOL ToggleNotificationsFlyout()
{
    return PostMessageW(FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL), WM_HOTKEY, 591, 0);
}

BOOL ToggleActionCenter()
{
    return PostMessageW(FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL), WM_HOTKEY, 500, MAKELPARAM(MOD_WIN, 0x41));
}
#pragma endregion


#pragma region "windowsudk.shellcommon Hooks"

static HRESULT(WINAPI *SLGetWindowsInformationDWORDFunc)(PCWSTR pwszValueName, DWORD* pdwValue) = NULL;

HRESULT WINAPI windowsudkshellcommon_SLGetWindowsInformationDWORDHook(PCWSTR pwszValueName, DWORD* pdwValue)
{
    HRESULT hr = SLGetWindowsInformationDWORDFunc(pwszValueName, pdwValue);

    if (bDisableAeroSnapQuadrants && !wcsncmp(pwszValueName, L"Shell-Windowing-LimitSnappedWindows", 35))
        *pdwValue = 1;

    return hr;
}

#pragma endregion


#pragma region "twinui.pcshell.dll hooks"
#if WITH_MAIN_PATCHER
typedef HRESULT(*ImmersiveContextMenuHelper_ApplyOwnerDrawToMenu_t)(HMENU hmenu, HWND hWnd, POINT* pptOrigin, unsigned int icmoFlags, void* srgRenderingData);
ImmersiveContextMenuHelper_ApplyOwnerDrawToMenu_t ImmersiveContextMenuHelper_ApplyOwnerDrawToMenuFunc;
typedef void(*ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenu_t)(HMENU hmenu, HWND hwnd);
ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenu_t ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc;
typedef LRESULT(*CImmersiveContextMenuOwnerDrawHelper_s_ContextMenuWndProcFunc_t)(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
CImmersiveContextMenuOwnerDrawHelper_s_ContextMenuWndProcFunc_t CImmersiveContextMenuOwnerDrawHelper_s_ContextMenuWndProcFunc;

extern void ToggleLauncherTipContextMenu();
extern LRESULT CALLBACK CLauncherTipContextMenu_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern void RunTwinUIPCShellPatches(symbols_addr* symbols_PTRS);

BOOL VnPatchIAT_NonInline(HMODULE hMod, const char* libName, const char* funcName, uintptr_t hookAddr)
{
    return VnPatchIAT(hMod, (PSTR)libName, (PSTR)funcName, hookAddr);
}

void ReportSuccessfulAnimationPatching()
{
#if WITH_SMA_PATCH_REPORT
    PSID pMainSid = NULL;
    GetLogonSid(&pMainSid);
    PSID pSecondaySid = NULL;
    DeriveAppContainerSidFromAppContainerName(L"Microsoft.Windows.StartMenuExperienceHost_cw5n1h2txyewy", &pSecondaySid);
    PSECURITY_DESCRIPTOR pSecurityDescriptor = NULL;
    if (PrepareSecurityDescriptor(pMainSid, STANDARD_RIGHTS_ALL | MUTEX_ALL_ACCESS, pSecondaySid, SYNCHRONIZE, &pSecurityDescriptor))
    {
        SECURITY_ATTRIBUTES SecurityAttributes;
        ZeroMemory(&SecurityAttributes, sizeof(SecurityAttributes));
        SecurityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
        SecurityAttributes.bInheritHandle = FALSE;
        SecurityAttributes.lpSecurityDescriptor = pSecurityDescriptor;
        if (CreateMutexW(&SecurityAttributes, FALSE, _T(EPStart10_AnimationsPatched)))
        {
            printf("[SMA] Advertising successful animations patching.\n");
        }
        free(pSecurityDescriptor);
    }
    if (pMainSid) free(pMainSid);
    if (pSecondaySid) FreeSid(pSecondaySid);
#endif
}
#endif
#pragma endregion


#pragma region "Windows 10 Taskbar Hooks"
#if WITH_MAIN_PATCHER
// credits: https://github.com/m417z/7-Taskbar-Tweaker

DEFINE_GUID(IID_ITaskGroup,
    0x3af85589, 0x678f, 0x4fb5, 0x89, 0x25, 0x5a, 0x13, 0x4e, 0xbf, 0x57, 0x2c);

typedef interface ITaskGroup ITaskGroup;

typedef struct ITaskGroupVtbl
{
    BEGIN_INTERFACE

    HRESULT(STDMETHODCALLTYPE* QueryInterface)(
        ITaskGroup* This,
        /* [in] */ REFIID riid,
        /* [annotation][iid_is][out] */
        _COM_Outptr_  void** ppvObject);

    ULONG(STDMETHODCALLTYPE* AddRef)(
        ITaskGroup* This);

    ULONG(STDMETHODCALLTYPE* Release)(
        ITaskGroup* This);

    HRESULT(STDMETHODCALLTYPE* Initialize)(
        ITaskGroup* This);

    HRESULT(STDMETHODCALLTYPE* AddTaskItem)(
        ITaskGroup* This);

    HRESULT(STDMETHODCALLTYPE* RemoveTaskItem)(
        ITaskGroup* This);

    HRESULT(STDMETHODCALLTYPE* EnumTaskItems)(
        ITaskGroup* This);

    HRESULT(STDMETHODCALLTYPE* DoesWindowMatch)(
        ITaskGroup* This,
        HWND hCompareWnd,
        ITEMIDLIST* pCompareItemIdList,
        WCHAR* pCompareAppId,
        int* pnMatch,
        LONG** p_task_item);
    // ...

    END_INTERFACE
} ITaskGroupVtbl;

interface ITaskGroup
{
    CONST_VTBL struct ITaskGroupVtbl* lpVtbl;
};

HRESULT(*CTaskGroup_DoesWindowMatchFunc)(LONG_PTR* task_group, HWND hCompareWnd, ITEMIDLIST* pCompareItemIdList,
    WCHAR* pCompareAppId, int* pnMatch, LONG_PTR** p_task_item) = NULL;
HRESULT __stdcall CTaskGroup_DoesWindowMatchHook(LONG_PTR* task_group, HWND hCompareWnd, ITEMIDLIST* pCompareItemIdList,
    WCHAR* pCompareAppId, int* pnMatch, LONG_PTR** p_task_item)
{
    HRESULT hr = CTaskGroup_DoesWindowMatchFunc(task_group, hCompareWnd, pCompareItemIdList, pCompareAppId, pnMatch, p_task_item);
    BOOL bDontGroup = FALSE;
    BOOL bPinned = FALSE;
    if (bPinnedItemsActAsQuickLaunch && SUCCEEDED(hr) && *pnMatch >= 1 && *pnMatch <= 3) // itemlist or appid match
    {
        bDontGroup = FALSE;
        bPinned = (!task_group[4] || (int)((LONG_PTR*)task_group[4])[0] == 0);
        if (bPinned)
        {
            bDontGroup = TRUE;
        }
        if (bDontGroup)
        {
            hr = E_FAIL;
        }
    }
    return hr;
}

DEFINE_GUID(IID_ITaskBtnGroup,
    0x2e52265d, 0x1a3b, 0x4e46, 0x94, 0x17, 0x51, 0xa5, 0x9c, 0x47, 0xd6, 0x0b);

typedef interface ITaskBtnGroup ITaskBtnGroup;

typedef struct ITaskBtnGroupVtbl
{
    BEGIN_INTERFACE

    HRESULT(STDMETHODCALLTYPE* QueryInterface)(
        ITaskBtnGroup* This,
        /* [in] */ REFIID riid,
        /* [annotation][iid_is][out] */
        _COM_Outptr_  void** ppvObject);

    ULONG(STDMETHODCALLTYPE* AddRef)(
        ITaskBtnGroup* This);

    ULONG(STDMETHODCALLTYPE* Release)(
        ITaskBtnGroup* This);

    HRESULT(STDMETHODCALLTYPE* Shutdown)(
        ITaskBtnGroup* This);

    HRESULT(STDMETHODCALLTYPE* GetGroupType)(
        ITaskBtnGroup* This);

    HRESULT(STDMETHODCALLTYPE* UpdateGroupType)(
        ITaskBtnGroup* This);

    HRESULT(STDMETHODCALLTYPE* GetGroup)(
        ITaskBtnGroup* This);

    HRESULT(STDMETHODCALLTYPE* AddTaskItem)(
        ITaskBtnGroup* This);

    HRESULT(STDMETHODCALLTYPE* IndexOfTaskItem)(
        ITaskBtnGroup* This);

    HRESULT(STDMETHODCALLTYPE* RemoveTaskItem)(
        ITaskBtnGroup* This);

    HRESULT(STDMETHODCALLTYPE* RealityCheck)(
        ITaskBtnGroup* This);

    HRESULT(STDMETHODCALLTYPE* IsItemBeingRemoved)(
        ITaskBtnGroup* This);

    HRESULT(STDMETHODCALLTYPE* CancelRemoveItem)(
        ITaskBtnGroup* This);

    LONG_PTR(STDMETHODCALLTYPE* GetIdealSpan)(
        ITaskBtnGroup* This,
        LONG_PTR var2, 
        LONG_PTR var3,
        LONG_PTR var4, 
        LONG_PTR var5, 
        LONG_PTR var6);
    // ...

    END_INTERFACE
} ITaskBtnGroupVtbl;

interface ITaskBtnGroup
{
    CONST_VTBL struct ITaskBtnGroupVtbl* lpVtbl;
};

LONG_PTR (*CTaskBtnGroup_GetIdealSpanFunc)(ITaskBtnGroup* _this, LONG_PTR var2, LONG_PTR var3,
    LONG_PTR var4, LONG_PTR var5, LONG_PTR var6) = NULL;
LONG_PTR __stdcall CTaskBtnGroup_GetIdealSpanHook(ITaskBtnGroup* _this, LONG_PTR var2, LONG_PTR var3,
    LONG_PTR var4, LONG_PTR var5, LONG_PTR var6)
{
    LONG_PTR ret = NULL;
    BOOL bTypeModified = FALSE;
    int button_group_type = *(unsigned int*)((INT64)_this + 64);
    if (bRemoveExtraGapAroundPinnedItems && button_group_type == 2)
    {
        *(unsigned int*)((INT64)_this + 64) = 4;
        bTypeModified = TRUE;
    }
    ret = CTaskBtnGroup_GetIdealSpanFunc(_this, var2, var3, var4, var5, var6);
    if (bRemoveExtraGapAroundPinnedItems && bTypeModified)
    {
        *(unsigned int*)((INT64)_this + 64) = button_group_type;
    }
    return ret;
}

HRESULT explorer_QISearch(void* that, LPCQITAB pqit, REFIID riid, void** ppv)
{
    HRESULT hr = QISearch(that, pqit, riid, ppv);
    if (SUCCEEDED(hr) && IsEqualGUID(pqit[0].piid, &IID_ITaskGroup) && bPinnedItemsActAsQuickLaunch)
    {
        ITaskGroup* pTaskGroup = (char*)that + pqit[0].dwOffset;
        DWORD flOldProtect = 0;
        if (VirtualProtect(pTaskGroup->lpVtbl, sizeof(ITaskGroupVtbl), PAGE_EXECUTE_READWRITE, &flOldProtect))
        {
            if (!CTaskGroup_DoesWindowMatchFunc)
            {
                CTaskGroup_DoesWindowMatchFunc = pTaskGroup->lpVtbl->DoesWindowMatch;
            }
            pTaskGroup->lpVtbl->DoesWindowMatch = CTaskGroup_DoesWindowMatchHook;
            VirtualProtect(pTaskGroup->lpVtbl, sizeof(ITaskGroupVtbl), flOldProtect, &flOldProtect);
        }
    }
    else if (SUCCEEDED(hr) && IsEqualGUID(pqit[0].piid, &IID_ITaskBtnGroup) && bRemoveExtraGapAroundPinnedItems)
    {
        ITaskBtnGroup* pTaskBtnGroup = (char*)that + pqit[0].dwOffset;
        DWORD flOldProtect = 0;
        if (VirtualProtect(pTaskBtnGroup->lpVtbl, sizeof(ITaskBtnGroupVtbl), PAGE_EXECUTE_READWRITE, &flOldProtect))
        {
            if (!CTaskBtnGroup_GetIdealSpanFunc)
            {
                CTaskBtnGroup_GetIdealSpanFunc = pTaskBtnGroup->lpVtbl->GetIdealSpan;
            }
            pTaskBtnGroup->lpVtbl->GetIdealSpan = CTaskBtnGroup_GetIdealSpanHook;
            VirtualProtect(pTaskBtnGroup->lpVtbl, sizeof(ITaskBtnGroupVtbl), flOldProtect, &flOldProtect);
        }
    }
    return hr;
}
#endif
#pragma endregion


#pragma region "Show Start in correct location according to TaskbarAl"
#if WITH_MAIN_PATCHER
void UpdateStartMenuPositioning(LPARAM loIsShouldInitializeArray_hiIsShouldRoInitialize)
{
    BOOL bShouldInitialize = LOWORD(loIsShouldInitializeArray_hiIsShouldRoInitialize);
    BOOL bShouldRoInitialize = HIWORD(loIsShouldInitializeArray_hiIsShouldRoInitialize);

    if (!bOldTaskbar)
    {
        return;
    }

    DWORD dwPosCurrent = GetStartMenuPosition(SHRegGetValueFromHKCUHKLMFunc);
    if (bShouldInitialize || InterlockedAdd(&dwTaskbarAl, 0) != dwPosCurrent)
    {
        HRESULT hr = S_OK;
        if (bShouldRoInitialize)
        {
            hr = RoInitialize(RO_INIT_MULTITHREADED);
        }
        if (SUCCEEDED(hr))
        {
            InterlockedExchange(&dwTaskbarAl, dwPosCurrent);
            StartMenuPositioningData spd;
            spd.pMonitorCount = &dwMonitorCount;
            spd.pMonitorList = hMonitorList;
            spd.location = dwPosCurrent;
            spd.i = (DWORD)-1;
            if (bShouldInitialize)
            {
                spd.operation = STARTMENU_POSITIONING_OPERATION_REMOVE;
                unsigned int k = InterlockedAdd(&dwMonitorCount, 0);
                for (unsigned int i = 0; i < k; ++i)
                {
                    spd.i = i;
                    NeedsRo_PositionStartMenuForMonitor(hMonitorList[i].hMonitor, NULL, NULL, &spd);
                }
                InterlockedExchange(&dwMonitorCount, 0);
                spd.i = (DWORD)-1;
                spd.operation = STARTMENU_POSITIONING_OPERATION_ADD;
            }
            else
            {
                spd.operation = STARTMENU_POSITIONING_OPERATION_CHANGE;
            }
            EnumDisplayMonitors(NULL, NULL, NeedsRo_PositionStartMenuForMonitor, &spd);
            if (bShouldRoInitialize)
            {
                RoUninitialize();
            }
        }
    }
}

__declspec(dllexport) unsigned __int64 FindTaskbarLayoutTokenByHMONITOR(HMONITOR hMonitor)
{
    for (DWORD i = 0; i < dwMonitorCount; i++)
    {
        if (hMonitorList[i].hMonitor == hMonitor)
        {
            return hMonitorList[i].token;
        }
    }
    return 0;
}
#else
void UpdateStartMenuPositioning(LPARAM loIsShouldInitializeArray_hiIsShouldRoInitialize) {}
#endif
#pragma endregion


#pragma region "Fix Windows 11 taskbar not showing tray when auto hide is on"
#if WITH_MAIN_PATCHER
#define FIXTASKBARAUTOHIDE_CLASS_NAME L"FixTaskbarAutohide_" _T(EP_CLSID)
LRESULT CALLBACK FixTaskbarAutohide_WndProc(
    HWND hWnd,
    UINT uMessage,
    WPARAM wParam,
    LPARAM lParam)
{
    static UINT s_uTaskbarRestart;

    switch (uMessage)
    {
    case WM_CREATE:
        s_uTaskbarRestart = RegisterWindowMessageW(TEXT("TaskbarCreated"));
        break;

    default:
        if (uMessage == s_uTaskbarRestart)
            PostQuitMessage(0);
        break;
    }

    return DefWindowProcW(hWnd, uMessage, wParam, lParam);
}
DWORD FixTaskbarAutohide(DWORD unused)
{
    WNDCLASS wc = { 0 };
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = FixTaskbarAutohide_WndProc;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = FIXTASKBARAUTOHIDE_CLASS_NAME;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    RegisterClassW(&wc);

    HWND hWnd = CreateWindowExW(
        0,
        FIXTASKBARAUTOHIDE_CLASS_NAME,
        0,
        WS_POPUP,
        0,
        0,
        0,
        0,
        0,
        0,
        GetModuleHandle(NULL),
        NULL
    );
    if (hWnd)
    {
        MSG msg;
        BOOL bRet;
        while ((bRet = GetMessageW(&msg, NULL, 0, 0)) != 0)
        {
            if (bRet == -1)
            {
                break;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
        DestroyWindow(hWnd);

        APPBARDATA abd;
        abd.cbSize = sizeof(APPBARDATA);
        if (SHAppBarMessage(ABM_GETSTATE, &abd) == ABS_AUTOHIDE)
        {
            abd.lParam = 0;
            SHAppBarMessage(ABM_SETSTATE, &abd);
            Sleep(1000);
            abd.lParam = ABS_AUTOHIDE;
            SHAppBarMessage(ABM_SETSTATE, &abd);
        }
    }
    SetEvent(hCanStartSws);

    return 0;
}
#endif
#pragma endregion


#pragma region "Allow enabling XAML sounds"
#if WITH_MAIN_PATCHER
void ForceEnableXamlSounds(HMODULE hWindowsUIXaml)
{
    MODULEINFO mi;
    if (!hWindowsUIXaml || !GetModuleInformation(GetCurrentProcess(), hWindowsUIXaml, &mi, sizeof(MODULEINFO)))
        return;

    // Patch DirectUI::ElementSoundPlayerService::ShouldPlaySound() to disregard XboxUtility::IsOnXbox() check

#if defined(_M_X64)
    // 74 ?? 39 59 ?? 75 ?? E8 ?? ?? ?? ?? 84 C0 75
    //                                           ^^ change jnz to jmp
    PBYTE match = FindPattern(
        mi.lpBaseOfDll,
        mi.SizeOfImage,
        "\x74\x00\x39\x59\x00\x75\x00\xE8\x00\x00\x00\x00\x84\xC0\x75",
        "x?xx?x?x????xxx"
    );
    if (match)
    {
        PBYTE jnz = match + 14;
        DWORD flOldProtect = 0;
        if (VirtualProtect(jnz, 1, PAGE_EXECUTE_READWRITE, &flOldProtect))
        {
            *jnz = 0xEB;
            VirtualProtect(jnz, 1, flOldProtect, &flOldProtect);
        }
    }
#elif defined(_M_ARM64)
    // 1F 09 00 71 ?? ?? ?? 54 ?? 00 00 35 ?? ?? ?? ?? 08 1C 00 53 ?? ?? ?? ??
    //                                                             ^^^^^^^^^^^ CBNZ -> B, CBZ -> NOP
    PBYTE match = FindPattern(
        mi.lpBaseOfDll,
        mi.SizeOfImage,
        "\x1F\x09\x00\x71\x00\x00\x00\x54\x00\x00\x00\x35\x00\x00\x00\x00\x08\x1C\x00\x53",
        "xxxx???x?xxx????xxxx"
    );
    if (match)
    {
        match += 20;
        DWORD currentInsn = *(DWORD*)match;
        DWORD newInsn = ARM64_CBNZWToB(currentInsn);
        if (!newInsn && ARM64_IsCBZW(currentInsn))
        {
            newInsn = 0xD503201F; // NOP
        }
        if (newInsn)
        {
            DWORD flOldProtect = 0;
            if (VirtualProtect(match, 4, PAGE_EXECUTE_READWRITE, &flOldProtect))
            {
                *(DWORD*)match = newInsn;
                VirtualProtect(match, 4, flOldProtect, &flOldProtect);
            }
        }
    }
#endif
}

BOOL IsXamlSoundsEnabled()
{
    DWORD dwRes = 0, dwSize = sizeof(DWORD);
    RegGetValueW(HKEY_CURRENT_USER, TEXT(REGPATH_OLD), L"XamlSounds", RRF_RT_DWORD, NULL, &dwRes, &dwSize);
    return dwRes != 0;
}
#endif
#pragma endregion


#pragma region "EnsureXAML on OS builds 22621+"
#if WITH_MAIN_PATCHER
DEFINE_GUID(uuidof_Windows_Internal_Shell_XamlExplorerHost_IXamlApplicationStatics,
    0xECC13292,
    0x27EF, 0x547A, 0xAC, 0x8B,
    0x76, 0xCD, 0x17, 0x32, 0x21, 0x86
);

// 22621.2134+. Still named IXamlApplicationStatics.
DEFINE_GUID(uuidof_Windows_Internal_Shell_XamlExplorerHost_IXamlApplicationStatics2,
    0x5148D7B1,
    0x800E, 0x5C86, 0x8F, 0x69,
    0x55, 0x81, 0x97, 0x48, 0x31, 0x23
);

DEFINE_GUID(uuidof_Windows_UI_Core_ICoreWindow5,
    0x28258A12,
    0x7D82, 0x505B, 0xB2, 0x10,
    0x71, 0x2B, 0x04, 0xA5, 0x88, 0x82
);

BOOL bIsXAMLEnsured = FALSE;
void EnsureXAML()
{
    if (bIsXAMLEnsured)
        return;
    bIsXAMLEnsured = TRUE;
    ULONGLONG initTime = GetTickCount64();
    HRESULT hr;

    HSTRING_HEADER hstringheaderXamlApplication;
    HSTRING hstringXamlApplication = NULL;
    hr = WindowsCreateStringReference(L"Windows.Internal.Shell.XamlExplorerHost.XamlApplication", 55, &hstringheaderXamlApplication, &hstringXamlApplication);
    if (FAILED(hr))
    {
        printf("[EnsureXAML] WindowsCreateStringReference(XamlApplication) failed. 0x%lX\n", hr);
        goto cleanup;
    }

    IInspectable* pXamlApplicationStatics = NULL;
    hr = RoGetActivationFactory(hstringXamlApplication, &uuidof_Windows_Internal_Shell_XamlExplorerHost_IXamlApplicationStatics, &pXamlApplicationStatics);
    if (FAILED(hr))
    {
        hr = RoGetActivationFactory(hstringXamlApplication, &uuidof_Windows_Internal_Shell_XamlExplorerHost_IXamlApplicationStatics2, &pXamlApplicationStatics);
        if (FAILED(hr))
        {
            printf("[EnsureXAML] RoGetActivationFactory(IXamlApplicationStatics) failed. 0x%lX\n", hr);
            goto cleanup0;
        }
    }

    IUnknown* pXamlApplication = NULL;
    HRESULT (*IXamlApplicationStatics_get_Current)(IInspectable*, void**) = ((void**)pXamlApplicationStatics->lpVtbl)[6];
    hr = IXamlApplicationStatics_get_Current(pXamlApplicationStatics, &pXamlApplication);
    if (FAILED(hr))
    {
        printf("[EnsureXAML] IXamlApplicationStatics::get_Current() failed. 0x%lX\n", hr);
        goto cleanup1;
    }
    pXamlApplication->lpVtbl->Release(pXamlApplication);

    HSTRING_HEADER hstringheaderWindowsXamlManager;
    HSTRING hstringWindowsXamlManager = NULL;
    hr = WindowsCreateStringReference(L"Windows.UI.Xaml.Hosting.WindowsXamlManager", 42, &hstringheaderWindowsXamlManager, &hstringWindowsXamlManager);
    if (FAILED(hr))
    {
        printf("[EnsureXAML] WindowsCreateStringReference(WindowsXamlManager) failed. 0x%lX\n", hr);
        goto cleanup1;
    }

    __x_ABI_CWindows_CUI_CCore_CICoreWindow5* pCoreWindow5 = NULL;
    hr = RoGetActivationFactory(hstringWindowsXamlManager, &uuidof_Windows_UI_Core_ICoreWindow5, &pCoreWindow5);
    if (FAILED(hr))
    {
        printf("[EnsureXAML] RoGetActivationFactory(ICoreWindow5) failed. 0x%lX\n", hr);
        goto cleanup2;
    }

    if (pCoreWindow5)
    {
        __x_ABI_CWindows_CSystem_CIDispatcherQueue* pDispatcherQueue = NULL;
        hr = pCoreWindow5->lpVtbl->get_DispatcherQueue(pCoreWindow5, &pDispatcherQueue);
        if (FAILED(hr))
        {
            printf("[EnsureXAML] ICoreWindow5::get_DispatcherQueue() failed. 0x%lX\n", hr);
            goto cleanup3;
        }
        // Keep pDispatcherQueue referenced in memory
    }

    ULONGLONG finalTime = GetTickCount64();
    printf("[EnsureXAML] %lld ms.\n", finalTime - initTime);

cleanup3:
    if (pCoreWindow5) pCoreWindow5->lpVtbl->Release(pCoreWindow5);
cleanup2:
    if (hstringWindowsXamlManager) WindowsDeleteString(hstringWindowsXamlManager);
cleanup1:
    if (pXamlApplicationStatics) pXamlApplicationStatics->lpVtbl->Release(pXamlApplicationStatics);
cleanup0:
    if (hstringXamlApplication) WindowsDeleteString(hstringXamlApplication);
cleanup:
    ;
}

HRESULT(*ICoreWindow5_get_DispatcherQueueFunc)(INT64, INT64);
HRESULT WINAPI ICoreWindow5_get_DispatcherQueueHook(void* _this, void** ppValue)
{
    SendMessageTimeoutW(FindWindowW(L"Shell_TrayWnd", NULL), WM_SETTINGCHANGE, 0, L"EnsureXAML", SMTO_NOTIMEOUTIFNOTHUNG, 5000, NULL);
    return ICoreWindow5_get_DispatcherQueueFunc(_this, ppValue);
}

HMODULE __fastcall Windows11v22H2_combase_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE hModule = LoadLibraryExW(lpLibFileName, hFile, dwFlags);
    if (hModule && hModule == GetModuleHandleW(L"Windows.UI.Xaml.dll"))
    {
        if (IsXamlSoundsEnabled())
        {
            ForceEnableXamlSounds(hModule);
        }
        DWORD flOldProtect = 0;
        IActivationFactory* pWindowsXamlManagerFactory = NULL;
        HSTRING_HEADER hstringHeaderWindowsXamlManager;
        HSTRING hstringWindowsXamlManager = NULL;
        FARPROC DllGetActivationFactory = GetProcAddress(hModule, "DllGetActivationFactory");
        if (!DllGetActivationFactory)
        {
            printf("Error in Windows11v22H2_combase_LoadLibraryExW on DllGetActivationFactory\n");
            return hModule;
        }
        if (FAILED(WindowsCreateStringReference(L"Windows.UI.Xaml.Hosting.WindowsXamlManager", 0x2Au, &hstringHeaderWindowsXamlManager, &hstringWindowsXamlManager)))
        {
            printf("Error in Windows11v22H2_combase_LoadLibraryExW on WindowsCreateStringReference\n");
            return hModule;
        }
        ((void(__fastcall*)(HSTRING, __int64*))DllGetActivationFactory)(hstringWindowsXamlManager, &pWindowsXamlManagerFactory);
        if (pWindowsXamlManagerFactory)
        {
            IInspectable* pCoreWindow5 = NULL;
            pWindowsXamlManagerFactory->lpVtbl->QueryInterface(pWindowsXamlManagerFactory, &uuidof_Windows_UI_Core_ICoreWindow5, &pCoreWindow5);
            if (pCoreWindow5)
            {
                INT64* pCoreWindow5Vtbl = pCoreWindow5->lpVtbl;
                if (VirtualProtect(pCoreWindow5->lpVtbl, sizeof(IInspectableVtbl) + sizeof(INT64), PAGE_EXECUTE_READWRITE, &flOldProtect))
                {
                    ICoreWindow5_get_DispatcherQueueFunc = pCoreWindow5Vtbl[6];
                    pCoreWindow5Vtbl[6] = ICoreWindow5_get_DispatcherQueueHook;
                    VirtualProtect(pCoreWindow5->lpVtbl, sizeof(IInspectableVtbl) + sizeof(INT64), flOldProtect, &flOldProtect);
                }
                pCoreWindow5->lpVtbl->Release(pCoreWindow5);
            }
            pWindowsXamlManagerFactory->lpVtbl->Release(pWindowsXamlManagerFactory);
        }
        WindowsDeleteString(hstringWindowsXamlManager);
    }
    return hModule;
}

HMODULE __fastcall combase_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags)
{
    HMODULE hModule = LoadLibraryExW(lpLibFileName, hFile, dwFlags);
    if (hModule && hModule == GetModuleHandleW(L"Windows.UI.Xaml.dll"))
    {
        ForceEnableXamlSounds(hModule);
    }
    return hModule;
}
#endif
#pragma endregion


#pragma region "Shell_TrayWnd subclass"
#if WITH_MAIN_PATCHER
int HandleTaskbarCornerInteraction(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    POINT pt; pt.x = 0; pt.y = 0;
    if (uMsg == WM_RBUTTONUP || uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_LBUTTONDOWN)
    {
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
        ClientToScreen(hWnd, &pt);
    }
    else if (uMsg == WM_NCLBUTTONUP || uMsg == WM_NCRBUTTONUP || uMsg == WM_NCLBUTTONDOWN || uMsg == WM_NCRBUTTONDOWN)
    {
        DWORD dwPos = GetMessagePos();
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);
    }
    HMONITOR hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi;
    ZeroMemory(&mi, sizeof(MONITORINFO));
    mi.cbSize = sizeof(MONITORINFO);
    GetMonitorInfoW(hMonitor, &mi);
    int t = 2;
    BOOL bOk = FALSE;
    if (pt.x < mi.rcMonitor.left + t && pt.y > mi.rcMonitor.bottom - t)
    {
        //printf("bottom left\n");
        bOk = TRUE;
    }
    else if (pt.x < mi.rcMonitor.left + t && pt.y < mi.rcMonitor.top + t)
    {
        //printf("top left\n");
        bOk = TRUE;
    }
    else if (pt.x > mi.rcMonitor.right - t && pt.y < mi.rcMonitor.top + t)
    {
        //printf("top right\n");
        bOk = TRUE;
    }
    if (bOk)
    {
        if (uMsg == WM_RBUTTONUP || uMsg == WM_NCRBUTTONUP || uMsg == WM_RBUTTONDOWN || uMsg == WM_NCRBUTTONDOWN)
        {
            ToggleLauncherTipContextMenu();
            return 1;
        }
        else if (uMsg == WM_LBUTTONUP || uMsg == WM_NCLBUTTONUP || uMsg == WM_LBUTTONDOWN || uMsg == WM_NCLBUTTONDOWN)
        {
            if (!dwUndeadStartCorner)
            {
                return 1;
            }
            if (dwUndeadStartCorner != 2)
            {
                OpenStartOnMonitor(hMonitor);
                return 1;
            }
            DWORD dwVal = 1, dwSize = sizeof(DWORD);
            RegGetValueW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"TaskbarAl", RRF_RT_DWORD, NULL, &dwVal, &dwSize);
            if (dwVal)
            {
                dwVal = 0;
                RegSetKeyValueW(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"TaskbarAl", REG_DWORD, &dwVal, sizeof(DWORD));
                if (hWndServiceWindow) SetTimer(hWndServiceWindow, 10, 1000, NULL);
            }
            OpenStartOnMonitor(hMonitor);
            return 1;
        }
    }
    return 0;
}

INT64 ReBarWindow32SubclassProc(_In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwUnused)
{
    if (uMsg == WM_NCDESTROY)
    {
        RemoveWindowSubclass(hWnd, ReBarWindow32SubclassProc, ReBarWindow32SubclassProc);
    }
    if (TaskbarCenter_ShouldCenter(dwOldTaskbarAl) && TaskbarCenter_ShouldStartBeCentered(dwOldTaskbarAl) && uMsg == WM_WINDOWPOSCHANGING)
    {
        LPWINDOWPOS lpWP = lParam;
        lpWP->cx += lpWP->x;
        lpWP->x = 0;
        lpWP->cy += lpWP->y;
        lpWP->y = 0;
    }
    else if (uMsg == RB_INSERTBANDW)
    {
        REBARBANDINFOW* lpRbi = lParam;
    }
    else if (uMsg == RB_SETBANDINFOW)
    {
        REBARBANDINFOW* lpRbi = lParam;
        if (GetClassWord(lpRbi->hwndChild, GCW_ATOM) == RegisterWindowMessageW(L"PeopleBand"))
        {
            lpRbi->fMask |= RBBIM_STYLE;
            lpRbi->fStyle &= ~RBBS_FIXEDSIZE;
            //lpRbi->fStyle &= ~RBBS_NOGRIPPER;
        }
    }
    else if (TaskbarCenter_ShouldCenter(dwOldTaskbarAl) && TaskbarCenter_ShouldStartBeCentered(dwOldTaskbarAl) && (uMsg == WM_LBUTTONUP || uMsg == WM_RBUTTONUP) && HandleTaskbarCornerInteraction(hWnd, uMsg, wParam, lParam))
    {
        return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

HMODULE g_hMyTaskbar;

HMENU explorer_LoadMenuW(HINSTANCE hInstance, LPCWSTR lpMenuName)
{
    HMENU hMenu = LoadMenuW(hInstance, lpMenuName);
    if ((hInstance == GetModuleHandle(NULL) || (g_hMyTaskbar && hInstance == g_hMyTaskbar)) && lpMenuName == MAKEINTRESOURCEW(205))
    {
        HMENU hSubMenu = GetSubMenu(hMenu, 0);
        if (hSubMenu)
        {
            TCHAR buffer[260];
            LoadStringW(GetModuleHandleW(L"ExplorerFrame.dll"), 50222, buffer + (bNoMenuAccelerator ? 0 : 1), 260);
            if (!bNoMenuAccelerator)
            {
                buffer[0] = L'&';
            }
            wchar_t* p = wcschr(buffer, L'(');
            if (p)
            {
                p--;
                if (*p == L' ')
                {
                    *p = 0;
                }
                else
                {
                    p++;
                    *p = 0;
                }
            }
            MENUITEMINFOW menuInfo;
            ZeroMemory(&menuInfo, sizeof(MENUITEMINFOW));
            menuInfo.cbSize = sizeof(MENUITEMINFOW);
            menuInfo.fMask = MIIM_ID | MIIM_STRING | MIIM_DATA;
            menuInfo.wID = 12100;
            menuInfo.dwItemData = CheckForUpdatesThread;
            menuInfo.fType = MFT_STRING;
            menuInfo.dwTypeData = buffer;
            menuInfo.cch = wcslen(buffer);
            if (!bNoPropertiesInContextMenu)
            {
                InsertMenuItemW(
                    hSubMenu,
                    GetMenuItemCount(hSubMenu) - 4,
                    TRUE,
                    &menuInfo
                );
            }
        }
    }
    return hMenu;
}

HHOOK Shell_TrayWndMouseHook = NULL;

BOOL IsPointOnEmptyAreaOfNewTaskbar(POINT pt)
{
    HRESULT hr = S_OK;
    IUIAutomation2* pIUIAutomation2 = NULL;
    IUIAutomationElement* pIUIAutomationElement = NULL;
    HWND hWnd = NULL;
    BOOL bRet = FALSE;
    BSTR elemName = NULL;
    BSTR elemType = NULL;
    BOOL bIsWindows11Version22H2OrHigher = IsWindows11Version22H2OrHigher();
    
    if (SUCCEEDED(hr))
    {
        hr = CoCreateInstance(&CLSID_CUIAutomation8, NULL, CLSCTX_INPROC_SERVER, &IID_IUIAutomation2, &pIUIAutomation2);
    }
    if (SUCCEEDED(hr))
    {
        hr = pIUIAutomation2->lpVtbl->ElementFromPoint(pIUIAutomation2, pt, &pIUIAutomationElement);
    }
    if (SUCCEEDED(hr) && bIsWindows11Version22H2OrHigher)
    {
        hr = pIUIAutomationElement->lpVtbl->get_CurrentName(pIUIAutomationElement, &elemName);
    }
    if (SUCCEEDED(hr) && bIsWindows11Version22H2OrHigher)
    {
        hr = pIUIAutomationElement->lpVtbl->get_CurrentClassName(pIUIAutomationElement, &elemType);
    }
    if (SUCCEEDED(hr) && bIsWindows11Version22H2OrHigher)
    {
        bRet = elemName && elemType && (!wcscmp(elemName, L"") && !wcscmp(elemType, L"Taskbar.TaskbarFrameAutomationPeer"));
    }
    if (SUCCEEDED(hr) && !bIsWindows11Version22H2OrHigher)
    {
        hr = pIUIAutomationElement->lpVtbl->get_CurrentNativeWindowHandle(pIUIAutomationElement, &hWnd);
    }
    if (SUCCEEDED(hr) && !bIsWindows11Version22H2OrHigher)
    {
        WCHAR wszClassName[200];
        GetClassNameW(hWnd, wszClassName, 200);
        if (IsWindow(hWnd))
        {
            if (!wcscmp(wszClassName, L"Windows.UI.Input.InputSite.WindowClass"))
            {
                HWND hAncestor = GetAncestor(hWnd, GA_ROOT);
                HWND hWindow = FindWindowExW(hAncestor, NULL, L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
                if (IsWindow(hWindow))
                {
                    hWindow = FindWindowExW(hWindow, NULL, L"Windows.UI.Input.InputSite.WindowClass", NULL);
                    if (IsWindow(hWindow))
                    {
                        if (hWindow == hWnd)
                        {
                            bRet = TRUE;
                        }
                    }
                }
            }
            else if (!wcscmp(wszClassName, L"MSTaskListWClass"))
            {
                IUIAutomationTreeWalker* pControlWalker = NULL;
                IUIAutomationElement* pTaskbarButton = NULL;
                IUIAutomationElement* pNextTaskbarButton = NULL;
                RECT rc;
                if (SUCCEEDED(hr))
                {
                    hr = pIUIAutomation2->lpVtbl->get_RawViewWalker(pIUIAutomation2, &pControlWalker);
                }
                if (SUCCEEDED(hr) && pControlWalker)
                {
                    hr = pControlWalker->lpVtbl->GetFirstChildElement(pControlWalker, pIUIAutomationElement, &pTaskbarButton);
                }
                BOOL bValid = TRUE, bFirst = TRUE;
                while (SUCCEEDED(hr) && pTaskbarButton)
                {
                    pControlWalker->lpVtbl->GetNextSiblingElement(pControlWalker, pTaskbarButton, &pNextTaskbarButton);
                    SetRect(&rc, 0, 0, 0, 0);
                    pTaskbarButton->lpVtbl->get_CurrentBoundingRectangle(pTaskbarButton, &rc);
                    if (bFirst)
                    {
                        // Account for Start button as well
                        rc.left -= (rc.right - rc.left);
                        bFirst = FALSE;
                    }
                    //printf("PT %d %d RECT %d %d %d %d\n", pt.x, pt.y, rc.left, rc.top, rc.right, rc.bottom);
                    if (pNextTaskbarButton && PtInRect(&rc, pt))
                    {
                        bValid = FALSE;
                    }
                    pTaskbarButton->lpVtbl->Release(pTaskbarButton);
                    pTaskbarButton = pNextTaskbarButton;
                }
                //printf("IS VALID %d\n", bValid);
                //printf("\n");
                if (pControlWalker)
                {
                    pControlWalker->lpVtbl->Release(pControlWalker);
                }
                if (bValid)
                {
                    HWND hAncestor = GetAncestor(hWnd, GA_ROOT);
                    HWND hWindow = FindWindowExW(hAncestor, NULL, L"WorkerW", NULL);
                    if (IsWindow(hWindow))
                    {
                        hWindow = FindWindowExW(hWindow, NULL, L"MSTaskListWClass", NULL);
                        if (IsWindow(hWindow))
                        {
                            if (hWindow == hWnd)
                            {
                                bRet = TRUE;
                            }
                        }
                    }
                }
            }
        }
    }
    if (elemName)
    {
        SysFreeString(elemName);
    }
    if (elemType)
    {
        SysFreeString(elemType);
    }
    if (pIUIAutomationElement)
    {
        pIUIAutomationElement->lpVtbl->Release(pIUIAutomationElement);
    }
    if (pIUIAutomation2)
    {
        pIUIAutomation2->lpVtbl->Release(pIUIAutomation2);
    }
    return bRet;
}

long long TaskbarLeftClickTime = 0;
BOOL bTaskbarLeftClickEven = FALSE;
LRESULT CALLBACK Shell_TrayWndMouseProc(
    _In_ int    nCode,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
)
{
    if (!bOldTaskbar &&
        !bNoPropertiesInContextMenu &&
        nCode == HC_ACTION && 
        wParam == WM_RBUTTONUP && 
        IsPointOnEmptyAreaOfNewTaskbar(((MOUSEHOOKSTRUCT*)lParam)->pt)
        )
    {
        PostMessageW(
            FindWindowW(L"Shell_TrayWnd", NULL),
            RegisterWindowMessageW(L"Windows11ContextMenu_" _T(EP_CLSID)),
            0,
            MAKELPARAM(((MOUSEHOOKSTRUCT*)lParam)->pt.x, ((MOUSEHOOKSTRUCT*)lParam)->pt.y)
        );
        return 1;
    }
    if (!bOldTaskbar &&
        bTaskbarAutohideOnDoubleClick && 
        nCode == HC_ACTION && 
        wParam == WM_LBUTTONUP &&
        IsPointOnEmptyAreaOfNewTaskbar(((MOUSEHOOKSTRUCT*)lParam)->pt)
        )
    {
        /*BOOL bShouldCheck = FALSE;
        if (bOldTaskbar)
        {
            WCHAR cn[200];
            GetClassNameW(((MOUSEHOOKSTRUCT*)lParam)->hwnd, cn, 200);
            wprintf(L"%s\n", cn);
            bShouldCheck = !wcscmp(cn, L"Shell_SecondaryTrayWnd"); // !wcscmp(cn, L"Shell_TrayWnd")
        }
        else
        {
            bShouldCheck = IsPointOnEmptyAreaOfNewTaskbar(((MOUSEHOOKSTRUCT*)lParam)->pt);
        }
        if (bShouldCheck)
        {*/
            if (bTaskbarLeftClickEven)
            {
                if (TaskbarLeftClickTime != 0)
                {
                    TaskbarLeftClickTime = milliseconds_now() - TaskbarLeftClickTime;
                }
                if (TaskbarLeftClickTime != 0 && TaskbarLeftClickTime < GetDoubleClickTime())
                {
                    TaskbarLeftClickTime = 0;
                    ToggleTaskbarAutohide();
                }
                else
                {
                    TaskbarLeftClickTime = milliseconds_now();
                }
            }
            bTaskbarLeftClickEven = !bTaskbarLeftClickEven;
        //}
    }
    return CallNextHookEx(Shell_TrayWndMouseHook, nCode, wParam, lParam);
}

ITrayUIHost* g_pTrayUIHost;

INT64 Shell_TrayWndSubclassProc(
    _In_ HWND   hWnd,
    _In_ UINT   uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    UINT_PTR    uIdSubclass,
    DWORD_PTR   bIsPrimaryTaskbar
)
{
    switch (uMsg)
    {
        case WM_NCDESTROY:
        {
            if (bIsPrimaryTaskbar)
            {
                UnhookWindowsHookEx(Shell_TrayWndMouseHook);
            }
            RemoveWindowSubclass(hWnd, Shell_TrayWndSubclassProc, Shell_TrayWndSubclassProc);
            break;
        }
        case WM_NCLBUTTONDOWN:
        case WM_NCRBUTTONUP:
        {
            if (bOldTaskbar && !bIsPrimaryTaskbar && TaskbarCenter_ShouldCenter(dwMMOldTaskbarAl) && TaskbarCenter_ShouldStartBeCentered(dwMMOldTaskbarAl) && HandleTaskbarCornerInteraction(hWnd, uMsg, wParam, lParam))
                return 0;
            break;
        }
        case WM_CONTEXTMENU:
        {
            if (!bIsPrimaryTaskbar)
            {
                // Received some times when right clicking a secondary taskbar button, and it would
                // show the classic taskbar context menu but containing only "Show desktop" instead
                // of ours or a button's jump list, so we cancel it and that seems to properly invoke
                // the right menu
                return 0;
            }
            break;
        }
        case WM_SETCURSOR:
        {
            if (!bOldTaskbar && !bIsPrimaryTaskbar)
            {
                // Received when mouse is over taskbar edge and autohide is on
                PostMessageW(hWnd, WM_ACTIVATE, WA_ACTIVE, NULL);
            }
            break;
        }
        case WM_LBUTTONDBLCLK:
        {
            if (bOldTaskbar && bTaskbarAutohideOnDoubleClick)
            {
                ToggleTaskbarAutohide();
                return 0;
            }
            break;
        }
        case WM_HOTKEY:
        {
            if (wParam == 500 && lParam == MAKELPARAM(MOD_WIN, 'A') && (bOldTaskbar && bHideControlCenterButton || global_rovi.dwBuildNumber >= 25921 && bOldTaskbar == 1))
            {
                InvokeActionCenter();
                return 0;
            }
            break;
        }
        case WM_DISPLAYCHANGE:
        {
            if (bIsPrimaryTaskbar)
            {
                UpdateStartMenuPositioning(MAKELPARAM(TRUE, FALSE));
            }
            break;
        }
        /*case WM_PARENTNOTIFY:
        {
            if (!bOldTaskbar && wParam == WM_RBUTTONDOWN && !Shell_TrayWndMouseHook) // && !IsUndockingDisabled
            {
                DWORD dwThreadId = GetCurrentThreadId();
                Shell_TrayWndMouseHook = SetWindowsHookExW(WH_MOUSE, Shell_TrayWndMouseProc, NULL, dwThreadId);
            }
            break;
        }*/
        case WM_SETTINGCHANGE:
        {
            if (IsWindows11Version22H2OrHigher() && (*((WORD*)&(lParam)+1)) && !wcscmp(lParam, L"EnsureXAML"))
            {
                EnsureXAML();
                return 0;
            }
            break;
        }
        case 0x558:
        {
            g_bIsDesktopRaised = (lParam & 1) == 0;
            break;
        }
        case WM_QUIT:
        {
            if (AreLogonLogoffShutdownSoundsEnabled())
            {
                TermSoundWindow();
            }
            break;
        }
        case 0x581:
        {
            if (AreLogonLogoffShutdownSoundsEnabled())
            {
                BOOL bLogoff = wParam != 0;
                SHPlaySound(bLogoff ? L"WindowsLogoff" : L"WindowsLogon", 1);
            }
            break;
        }
        case 0x590:
        {
            if (AreLogonLogoffShutdownSoundsEnabled())
            {
                InitSoundWindow();
            }
            break;
        }
        case 0x5B4:
        {
            if (AreLogonLogoffShutdownSoundsEnabled())
            {
                TermSoundWindow();
            }
            break;
        }
    }

    if (uMsg >= 0xC000 && uMsg <= 0xFFFF && uMsg == RegisterWindowMessageW(L"Windows11ContextMenu_" _T(EP_CLSID)))
    {
        POINT pt;
        pt.x = GET_X_LPARAM(lParam);
        pt.y = GET_Y_LPARAM(lParam);

        HMENU hMenu = LoadMenuW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(205));
        if (!hMenu && g_hMyTaskbar)
        {
            hMenu = LoadMenuW(g_hMyTaskbar, MAKEINTRESOURCEW(205));
        }
        if (hMenu)
        {
            HMENU hSubMenu = GetSubMenu(hMenu, 0);
            if (hSubMenu)
            {
                if (GetAsyncKeyState(VK_SHIFT) >= 0 || GetAsyncKeyState(VK_CONTROL) >= 0)
                {
                    DeleteMenu(hSubMenu, 518, MF_BYCOMMAND); // Exit Explorer
                }
                DeleteMenu(hSubMenu, 424, MF_BYCOMMAND); // Lock the taskbar
                DeleteMenu(hSubMenu, 425, MF_BYCOMMAND); // Lock all taskbars
                if (g_pTrayUIHost)
                {
                    void** pTrayUIHostVtbl = *(void***)g_pTrayUIHost;
                    BOOL(*ShouldDeleteContextMenuUndo)(ITrayUIHost*) = pTrayUIHostVtbl[13];
                    UINT(*GetContextMenuUndoResourceId)(ITrayUIHost*) = pTrayUIHostVtbl[14];

                    if (ShouldDeleteContextMenuUndo(g_pTrayUIHost))
                    {
                        DeleteMenu(hSubMenu, 416, MF_BYCOMMAND);
                    }
                    else
                    {
                        WCHAR wszTemplate[64];
                        WCHAR wszCommand[96];
                        WCHAR wszMenu[160];
                        LoadStringW(GetModuleHandleW(NULL), 534, wszTemplate, 64);
                        LoadStringW(GetModuleHandleW(NULL), GetContextMenuUndoResourceId(g_pTrayUIHost), wszCommand, 96);
                        swprintf_s(wszMenu, 160, wszTemplate, wszCommand);
                        ModifyMenuW(hSubMenu, 416, MF_BYCOMMAND, 416, wszMenu);
                    }
                }
                else
                {
                    DeleteMenu(hSubMenu, 416, MF_BYCOMMAND); // Undo
                }
                DeleteMenu(hSubMenu, 437, MF_BYCOMMAND); // Show Pen button
                DeleteMenu(hSubMenu, 438, MF_BYCOMMAND); // Show touchpad button
                DeleteMenu(hSubMenu, 435, MF_BYCOMMAND); // Show People on the taskbar
                DeleteMenu(hSubMenu, 430, MF_BYCOMMAND); // Show Task View button
                DeleteMenu(hSubMenu, 449, MF_BYCOMMAND); // Show Cortana button
                DeleteMenu(hSubMenu, 621, MF_BYCOMMAND); // News and interests
                DeleteMenu(hSubMenu, 445, MF_BYCOMMAND); // Cortana
                DeleteMenu(hSubMenu, 431, MF_BYCOMMAND); // Search
                DeleteMenu(hSubMenu, 421, MF_BYCOMMAND); // Customize notification icons
                DeleteMenu(hSubMenu, 408, MF_BYCOMMAND); // Adjust date/time
                DeleteMenu(hSubMenu, 436, MF_BYCOMMAND); // Show touch keyboard button
                DeleteMenu(hSubMenu, 0, MF_BYPOSITION); // Separator
                DeleteMenu(hSubMenu, 0, MF_BYPOSITION); // Separator

                TCHAR buffer[260];
                LoadStringW(GetModuleHandleW(L"ExplorerFrame.dll"), 50222, buffer + (bNoMenuAccelerator ? 0 : 1), 260);
                if (!bNoMenuAccelerator)
                {
                    buffer[0] = L'&';
                }
                wchar_t* p = wcschr(buffer, L'(');
                if (p)
                {
                    p--;
                    if (*p == L' ')
                    {
                        *p = 0;
                    }
                    else
                    {
                        p++;
                        *p = 0;
                    }
                }
                MENUITEMINFOW menuInfo;
                ZeroMemory(&menuInfo, sizeof(MENUITEMINFOW));
                menuInfo.cbSize = sizeof(MENUITEMINFOW);
                menuInfo.fMask = MIIM_ID | MIIM_STRING | MIIM_DATA | MIIM_STATE;
                menuInfo.wID = 3999;
                menuInfo.dwItemData = 0;
                menuInfo.fType = MFT_STRING;
                menuInfo.dwTypeData = buffer;
                menuInfo.cch = wcslen(buffer);
                if (!bNoPropertiesInContextMenu)
                {
                    InsertMenuItemW(
                        hSubMenu,
                        GetMenuItemCount(hSubMenu) - 1,
                        TRUE,
                        &menuInfo
                    );
                }

                INT64* unknown_array = NULL;
                if (bSkinMenus)
                {
                    unknown_array = calloc(4, sizeof(INT64));
                    if (ImmersiveContextMenuHelper_ApplyOwnerDrawToMenuFunc)
                    {
                        ImmersiveContextMenuHelper_ApplyOwnerDrawToMenuFunc(
                            hSubMenu,
                            hWnd,
                            &pt,
                            0xc,
                            unknown_array
                        );
                    }
                }

                BOOL res = TrackPopupMenu(
                    hSubMenu,
                    TPM_RETURNCMD | TPM_RIGHTBUTTON,
                    pt.x,
                    pt.y,
                    0,
                    hWnd,
                    0
                );
                if (res == 3999)
                {
                    LaunchPropertiesGUI(hModule);
                }
                else if (res == 420)
                {
                    // Restore Task Manager context menu item action on 24H2+
                    SHELLEXECUTEINFOW sei = { sizeof(sei) };
                    sei.fMask = SEE_MASK_DOENVSUBST;
                    sei.lpFile = L"%SystemRoot%\\system32\\taskmgr.exe";
                    sei.lpParameters = L"/4";
                    sei.nShow = SW_SHOWNORMAL;
                    ShellExecuteExW(&sei);
                }
                else
                {
                    PostMessageW(hWnd, WM_COMMAND, res, 0);
                }

                if (bSkinMenus)
                {
                    if (ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc)
                    {
                        ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc(
                            hSubMenu,
                            hWnd
                        );
                    }
                    free(unknown_array);
                }

                DestroyMenu(hSubMenu);
            }
            DestroyMenu(hMenu);
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
#endif
#pragma endregion


#pragma region "Allow legacy volume applet"
#if WITH_MAIN_PATCHER
LSTATUS sndvolsso_RegGetValueW(
    HKEY    hkey,
    LPCWSTR lpSubKey,
    LPCWSTR lpValue,
    DWORD   dwFlags,
    LPDWORD pdwType,
    PVOID   pvData,
    LPDWORD pcbData
)
{
    if (SHRegGetValueFromHKCUHKLMFunc &&
        hkey == HKEY_LOCAL_MACHINE &&
        !_wcsicmp(lpSubKey, L"Software\\Microsoft\\Windows NT\\CurrentVersion\\MTCUVC") &&
        !_wcsicmp(lpValue, L"EnableMTCUVC"))
    {
        return SHRegGetValueFromHKCUHKLMFunc(
            lpSubKey,
            lpValue,
            SRRF_RT_REG_DWORD,
            pdwType,
            pvData,
            pcbData
        );
    }
    return RegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}
#endif
#pragma endregion


#pragma region "Allow legacy date and time"
#if WITH_MAIN_PATCHER
DEFINE_GUID(GUID_Win32Clock,
    0x0A323554A,
    0x0FE1, 0x4E49, 0xae, 0xe1,
    0x67, 0x22, 0x46, 0x5d, 0x79, 0x9f
);
DEFINE_GUID(IID_Win32Clock,
    0x7A5FCA8A,
    0x76B1, 0x44C8, 0xa9, 0x7c,
    0xe7, 0x17, 0x3c, 0xca, 0x5f, 0x4f
);
typedef interface Win32Clock Win32Clock;

typedef struct Win32ClockVtbl
{
    BEGIN_INTERFACE

        HRESULT(STDMETHODCALLTYPE* QueryInterface)(
            Win32Clock* This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */
            _COM_Outptr_  void** ppvObject);

    ULONG(STDMETHODCALLTYPE* AddRef)(
        Win32Clock* This);

    ULONG(STDMETHODCALLTYPE* Release)(
        Win32Clock* This);

    HRESULT(STDMETHODCALLTYPE* ShowWin32Clock)(
        Win32Clock* This,
        /* [in] */ HWND hWnd,
        /* [in] */ LPRECT lpRect);

    END_INTERFACE
} Win32ClockVtbl;

interface Win32Clock
{
    CONST_VTBL struct Win32ClockVtbl* lpVtbl;
};
DWORD ShouldShowLegacyClockExperience()
{
    DWORD dwVal = 0, dwSize = sizeof(DWORD);
    if (SHRegGetValueFromHKCUHKLMFunc && SHRegGetValueFromHKCUHKLMFunc(
        TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\ImmersiveShell"),
        TEXT("UseWin32TrayClockExperience"),
        SRRF_RT_REG_DWORD,
        NULL,
        &dwVal,
        (LPDWORD)(&dwSize)
    ) == ERROR_SUCCESS)
    {
        return dwVal;
    }
    return 0;
}
BOOL ShowLegacyClockExperience(HWND hWnd)
{
    if (!hWnd)
    {
        return FALSE;
    }
    HRESULT hr = S_OK;
    Win32Clock* pWin32Clock = NULL;
    hr = CoCreateInstance(
        &GUID_Win32Clock,
        NULL,
        CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
        &IID_Win32Clock,
        &pWin32Clock
    );
    if (SUCCEEDED(hr))
    {
        RECT rc;
        GetWindowRect(hWnd, &rc);
        pWin32Clock->lpVtbl->ShowWin32Clock(pWin32Clock, hWnd, &rc);
        pWin32Clock->lpVtbl->Release(pWin32Clock);
    }
    return TRUE;
}

INT64 ClockButtonSubclassProc(
    _In_ HWND   hWnd,
    _In_ UINT   uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    UINT_PTR    uIdSubclass,
    DWORD_PTR   dwRefData
)
{
    if (uMsg == WM_NCDESTROY)
    {
        RemoveWindowSubclass(hWnd, ClockButtonSubclassProc, ClockButtonSubclassProc);
    }
    else if (uMsg == WM_LBUTTONDOWN || (uMsg == WM_KEYDOWN && wParam == VK_RETURN))
    {
        if (ShouldShowLegacyClockExperience() == 1)
        {
            if (!FindWindowW(L"ClockFlyoutWindow", NULL))
            {
                return ShowLegacyClockExperience(hWnd);
            }
            else
            {
                return 1;
            }
        }
        else if (ShouldShowLegacyClockExperience() == 2)
        {
            if (FindWindowW(L"Windows.UI.Core.CoreWindow", NULL))
            {
                if (IsWindows11())
                {
                    ToggleNotificationsFlyout();
                }
                else
                {
                    ToggleActionCenter();
                }
            }
            return 1;
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
#endif
#pragma endregion


#pragma region "Popup menu hooks"
BOOL IsImmersiveMenu = FALSE;
BOOL CheckIfImmersiveContextMenu(
    HWND unnamedParam1,
    LPCSTR unnamedParam2,
    HANDLE unnamedParam3
)
{
    if ((*((WORD*)&(unnamedParam2)+1)))
    {
        if (!strncmp(unnamedParam2, "ImmersiveContextMenuArray", 25))
        {
            IsImmersiveMenu = TRUE;
            return FALSE;
        }
    }
    return TRUE;
}
void RemoveOwnerDrawFromMenu(int level, HMENU hMenu)
{
    if (hMenu)
    {
        int k = GetMenuItemCount(hMenu);
        for (int i = 0; i < k; ++i)
        {
            MENUITEMINFO mii;
            mii.cbSize = sizeof(MENUITEMINFO);
            mii.fMask = MIIM_FTYPE | MIIM_SUBMENU;
            if (GetMenuItemInfoW(hMenu, i, TRUE, &mii) && (mii.fType & MFT_OWNERDRAW))
            {
                mii.fType &= ~MFT_OWNERDRAW;
                printf("[ROD]: Level %d Position %d/%d Status %d\n", level, i, k, SetMenuItemInfoW(hMenu, i, TRUE, &mii));
                RemoveOwnerDrawFromMenu(level + 1, mii.hSubMenu);
            }
        }
    }
}
BOOL CheckIfMenuContainsOwnPropertiesItem(HMENU hMenu)
{
#if WITH_MAIN_PATCHER
    if (hMenu)
    {
        int k = GetMenuItemCount(hMenu);
        for (int i = k - 1; i >= 0; i--)
        {
            MENUITEMINFO mii;
            mii.cbSize = sizeof(MENUITEMINFO);
            mii.fMask = MIIM_DATA | MIIM_ID;
            BOOL b = GetMenuItemInfoW(hMenu, i, TRUE, &mii);
            if (b && (mii.wID >= 12000 && mii.wID <= 12200) && mii.dwItemData == (ULONG_PTR)CheckForUpdatesThread)
            {
                return TRUE;
            }
        }
    }
#endif
    return FALSE;
}

#if WITH_MAIN_PATCHER
#define DEFINE_IMMERSIVE_MENU_HOOK(name) \
    static ImmersiveContextMenuHelper_ApplyOwnerDrawToMenu_t name##_ApplyOwnerDrawToMenuFunc = NULL; \
    static HRESULT name##_ApplyOwnerDrawToMenuHook(HMENU hMenu, HWND hWnd, POINT* pPt, unsigned int options, void* data) \
    { \
        wchar_t wszClassName[200]; \
        ZeroMemory(wszClassName, 200); \
        GetClassNameW(hWnd, wszClassName, 200); \
        \
        BOOL bDisableSkinning = (!wcscmp(wszClassName, L"Shell_TrayWnd") || !wcscmp(wszClassName, L"Shell_SecondaryTrayWnd")) ? !bSkinMenus : bDisableImmersiveContextMenu; \
        if (bDisableSkinning) \
        { \
            return S_OK; \
        } \
        return name##_ApplyOwnerDrawToMenuFunc(hMenu, hWnd, pPt, options, data); \
    }

#define DEFINE_IMMERSIVE_MENU_HOOK_TB(name) \
    static ImmersiveContextMenuHelper_ApplyOwnerDrawToMenu_t name##_ApplyOwnerDrawToMenuFunc = NULL; \
    static HRESULT name##_ApplyOwnerDrawToMenuHook(HMENU hMenu, HWND hWnd, POINT* pPt, unsigned int options, void* data) \
    { \
        BOOL bDisableSkinning = !bSkinMenus; \
        if (bDisableSkinning) \
        { \
            return S_OK; \
        } \
        return name##_ApplyOwnerDrawToMenuFunc(hMenu, hWnd, pPt, options, data); \
    }

DEFINE_IMMERSIVE_MENU_HOOK_TB(Sndvolsso);
DEFINE_IMMERSIVE_MENU_HOOK(Shell32);
DEFINE_IMMERSIVE_MENU_HOOK(ExplorerFrame);
DEFINE_IMMERSIVE_MENU_HOOK(Explorer);
DEFINE_IMMERSIVE_MENU_HOOK_TB(Pnidui);
DEFINE_IMMERSIVE_MENU_HOOK_TB(InputSwitch);

static void HookImmersiveMenuFunctions(
    funchook_t* funchook,
    HMODULE module,
    ImmersiveContextMenuHelper_ApplyOwnerDrawToMenu_t* applyFunc,
    ImmersiveContextMenuHelper_ApplyOwnerDrawToMenu_t applyHook)
{
    MODULEINFO mi;
    GetModuleInformation(GetCurrentProcess(), module, &mi, sizeof(MODULEINFO));

#if defined(_M_X64)
    // 40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 4C 8B B5 ? ? ? ? 41 8B C1
    PBYTE match = (PBYTE)FindPattern(
        mi.lpBaseOfDll, mi.SizeOfImage,
        "\x40\x55\x53\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x8D\xAC\x24\x00\x00\x00\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x85\x00\x00\x00\x00\x4C\x8B\xB5\x00\x00\x00\x00\x41\x8B\xC1",
        "xxxxxxxxxxxxxxxxx????xxx????xxx????xxxxxx????xxx????xxx"
    );
#elif defined(_M_ARM64)
    // 40 F9 43 03 1C 32 E4 03 15 AA ?? ?? FF 97
    //                               ^^^^^^^^^^^
    // Ref: ImmersiveContextMenuHelper::ApplyOwnerDrawToMenu()
    PBYTE match = (PBYTE)FindPattern(
        mi.lpBaseOfDll, mi.SizeOfImage,
        "\x40\xF9\x43\x03\x1C\x32\xE4\x03\x15\xAA\x00\x00\xFF\x97",
        "xxxxxxxxxx??xx"
    );
    if (match)
    {
        match += 10;
        match = (PBYTE)ARM64_FollowBL((DWORD*)match);
    }
#endif
    if (match)
    {
        *applyFunc = match;
        funchook_prepare(
            funchook,
            (void**)applyFunc,
            applyHook
        );
    }
}

#define HOOK_IMMERSIVE_MENUS(name) \
    HookImmersiveMenuFunctions( \
        funchook, \
        h##name, \
        &name##_ApplyOwnerDrawToMenuFunc, \
        name##_ApplyOwnerDrawToMenuHook \
    )
#else
#define HOOK_IMMERSIVE_MENUS(name)
#endif

BOOL TrackPopupMenuHookEx(
    HMENU       hMenu,
    UINT        uFlags,
    int         x,
    int         y,
    HWND        hWnd,
    LPTPMPARAMS lptpm
)
{
    IsImmersiveMenu = FALSE;

    wchar_t wszClassName[200];
    ZeroMemory(wszClassName, 200);
    GetClassNameW(hWnd, wszClassName, 200);

    BOOL bIsTaskbar = (!wcscmp(wszClassName, L"Shell_TrayWnd") || !wcscmp(wszClassName, L"Shell_SecondaryTrayWnd")) ? !bSkinMenus : bDisableImmersiveContextMenu;
    //wprintf(L">> %s %d %d\n", wszClassName, bIsTaskbar, bIsExplorerProcess);

    BOOL bContainsOwn = FALSE;
    if (bIsExplorerProcess && (!wcscmp(wszClassName, L"Shell_TrayWnd") || !wcscmp(wszClassName, L"Shell_SecondaryTrayWnd")))
    {
        bContainsOwn = CheckIfMenuContainsOwnPropertiesItem(hMenu);
    }

    if (bIsTaskbar && (bIsExplorerProcess ? 1 : (!wcscmp(wszClassName, L"SHELLDLL_DefView") || !wcscmp(wszClassName, L"SysTreeView32"))))
    {
        EnumPropsA(hWnd, CheckIfImmersiveContextMenu);
        if (IsImmersiveMenu)
        {
            IsImmersiveMenu = FALSE;
#if !WITH_MAIN_PATCHER
            if (bIsExplorerProcess)
            {
#else
            if (bIsExplorerProcess && ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc)
            {
                POINT pt;
                pt.x = x;
                pt.y = y;
                ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc(
                    hMenu,
                    hWnd
                );
#endif
            }
            else
            {
                RemoveOwnerDrawFromMenu(0, hMenu);
            }

            BOOL bRet = TrackPopupMenuEx(
                hMenu,
                uFlags,
                x,
                y,
                hWnd,
                lptpm
            );
#if WITH_MAIN_PATCHER
            if (bContainsOwn && (bRet >= 12000 && bRet <= 12200))
            {
                LaunchPropertiesGUI(hModule);
                return FALSE;
            }
#endif
            return bRet;
        }
        IsImmersiveMenu = FALSE;
    }
    BOOL b = TrackPopupMenuEx(
        hMenu,
        uFlags,
        x,
        y,
        hWnd,
        lptpm
    );
#if WITH_MAIN_PATCHER
    if (bContainsOwn && (b >= 12000 && b <= 12200))
    {
        LaunchPropertiesGUI(hModule);
        return FALSE;
    }
#endif
    return b;
}
BOOL TrackPopupMenuHook(
    HMENU       hMenu,
    UINT        uFlags,
    int         x,
    int         y,
    int         nReserved,
    HWND        hWnd,
    const RECT* prcRect
)
{
    IsImmersiveMenu = FALSE;

    wchar_t wszClassName[200];
    ZeroMemory(wszClassName, 200);
    GetClassNameW(hWnd, wszClassName, 200);

    BOOL bIsTaskbar = (!wcscmp(wszClassName, L"Shell_TrayWnd") || !wcscmp(wszClassName, L"Shell_SecondaryTrayWnd")) ? !bSkinMenus : bDisableImmersiveContextMenu;
    //wprintf(L">> %s %d %d\n", wszClassName, bIsTaskbar, bIsExplorerProcess);

    BOOL bContainsOwn = FALSE;
    if (bIsExplorerProcess && (!wcscmp(wszClassName, L"Shell_TrayWnd") || !wcscmp(wszClassName, L"Shell_SecondaryTrayWnd")))
    {
        bContainsOwn = CheckIfMenuContainsOwnPropertiesItem(hMenu);
    }

    if (bIsTaskbar && (bIsExplorerProcess ? 1 : (!wcscmp(wszClassName, L"SHELLDLL_DefView") || !wcscmp(wszClassName, L"SysTreeView32"))))
    {
        EnumPropsA(hWnd, CheckIfImmersiveContextMenu);
        if (IsImmersiveMenu)
        {
            IsImmersiveMenu = FALSE;

#if !WITH_MAIN_PATCHER
            if (bIsExplorerProcess)
            {
#else
            if (bIsExplorerProcess && ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc)
            {
                POINT pt;
                pt.x = x;
                pt.y = y;
                ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc(
                    hMenu,
                    hWnd
                );
#endif
            }
            else
            {
                RemoveOwnerDrawFromMenu(0, hMenu);
            }

            BOOL bRet = TrackPopupMenu(
                hMenu,
                uFlags,
                x,
                y,
                0,
                hWnd,
                prcRect
            );
#if WITH_MAIN_PATCHER
            if (bContainsOwn && (bRet >= 12000 && bRet <= 12200))
            {
                LaunchPropertiesGUI(hModule);
                return FALSE;
            }
#endif
            return bRet;
        }
        IsImmersiveMenu = FALSE;
    }
    BOOL b = TrackPopupMenu(
        hMenu,
        uFlags,
        x,
        y,
        0,
        hWnd,
        prcRect
    );
#if WITH_MAIN_PATCHER
    if (bContainsOwn && (b >= 12000 && b <= 12200))
    {
        LaunchPropertiesGUI(hModule);
        return FALSE;
    }
#endif
    return b;
}
#if WITH_MAIN_PATCHER
#define TB_POS_NOWHERE 0
#define TB_POS_BOTTOM 1
#define TB_POS_TOP 2
#define TB_POS_LEFT 3
#define TB_POS_RIGHT 4
UINT GetTaskbarLocationAndSize(POINT ptCursor, RECT* rc)
{
    MONITORINFO mi;
    mi.cbSize = sizeof(MONITORINFO);
    HWND hWnd = GetMonitorInfoFromPointForTaskbarFlyoutActivation(
        ptCursor,
        MONITOR_DEFAULTTOPRIMARY,
        &mi
    );
    if (hWnd)
    {
        GetWindowRect(hWnd, rc);
        RECT rcC = *rc;
        rcC.left -= mi.rcMonitor.left;
        rcC.right -= mi.rcMonitor.left;
        rcC.top -= mi.rcMonitor.top;
        rcC.bottom -= mi.rcMonitor.top;
        if (rcC.left < 5 && rcC.top > 5)
        {
            return TB_POS_BOTTOM;
        }
        else if (rcC.left < 5 && rcC.top < 5 && rcC.right > rcC.bottom)
        {
            return TB_POS_TOP;
        }
        else if (rcC.left < 5 && rcC.top < 5 && rcC.right < rcC.bottom)
        {
            return TB_POS_LEFT;
        }
        else if (rcC.left > 5 && rcC.top < 5)
        {
            return TB_POS_RIGHT;
        }
    }
    return TB_POS_NOWHERE;
}
void PopupMenuAdjustCoordinatesAndFlags(int* x, int* y, UINT* uFlags)
{
    POINT pt;
    GetCursorPos(&pt);
    RECT rc;
    UINT tbPos = GetTaskbarLocationAndSize(pt, &rc);
    if (tbPos == TB_POS_BOTTOM)
    {
        *y = MIN(*y, rc.top);
        *uFlags |= TPM_CENTERALIGN | TPM_BOTTOMALIGN;
    }
    else if (tbPos == TB_POS_TOP)
    {
        *y = MAX(*y, rc.bottom);
        *uFlags |= TPM_CENTERALIGN | TPM_TOPALIGN;
    }
    else if (tbPos == TB_POS_LEFT)
    {
        *x = MAX(*x, rc.right);
        *uFlags |= TPM_VCENTERALIGN | TPM_LEFTALIGN;
    }
    if (tbPos == TB_POS_RIGHT)
    {
        *x = MIN(*x, rc.left);
        *uFlags |= TPM_VCENTERALIGN | TPM_RIGHTALIGN;
    }
}
INT64 OwnerDrawSubclassProc(
    _In_ HWND   hWnd,
    _In_ UINT   uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    UINT_PTR    uIdSubclass,
    DWORD_PTR   dwRefData
)
{
    if ((uMsg == WM_DRAWITEM || uMsg == WM_MEASUREITEM) &&
        CImmersiveContextMenuOwnerDrawHelper_s_ContextMenuWndProcFunc &&
        CImmersiveContextMenuOwnerDrawHelper_s_ContextMenuWndProcFunc(hWnd, uMsg, wParam, lParam))
    {
        return 0;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
long long explorer_TrackPopupMenuExElapsed = 0;
BOOL explorer_TrackPopupMenuExHook(
    HMENU       hMenu,
    UINT        uFlags,
    int         x,
    int         y,
    HWND        hWnd,
    LPTPMPARAMS lptpm
)
{
    long long elapsed = milliseconds_now() - explorer_TrackPopupMenuExElapsed;
    BOOL b = FALSE;

    wchar_t wszClassName[200];
    ZeroMemory(wszClassName, 200);
    GetClassNameW(hWnd, wszClassName, 200);
    BOOL bContainsOwn = FALSE;
    if (bIsExplorerProcess && (!wcscmp(wszClassName, L"Shell_TrayWnd") || !wcscmp(wszClassName, L"Shell_SecondaryTrayWnd")))
    {
        bContainsOwn = CheckIfMenuContainsOwnPropertiesItem(hMenu);
    }
    
    wchar_t wszClassNameOfWindowUnderCursor[200];
    ZeroMemory(wszClassNameOfWindowUnderCursor, 200);
    POINT p; p.x = x; p.y = y;
    GetClassNameW(WindowFromPoint(p), wszClassNameOfWindowUnderCursor, 200);
    BOOL bIsSecondaryTaskbar = (!wcscmp(wszClassName, L"Shell_SecondaryTrayWnd") && !wcscmp(wszClassNameOfWindowUnderCursor, L"Shell_SecondaryTrayWnd"));

    if (elapsed > POPUPMENU_EX_ELAPSED || !bFlyoutMenus || bIsSecondaryTaskbar)
    {
        if (bCenterMenus && !bIsSecondaryTaskbar)
        {
            PopupMenuAdjustCoordinatesAndFlags(&x, &y, &uFlags);
        }
        IsImmersiveMenu = FALSE;
        if (!bSkinMenus)
        {
            EnumPropsA(hWnd, CheckIfImmersiveContextMenu);
            if (IsImmersiveMenu)
            {
                if (ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc)
                {
                    POINT pt;
                    pt.x = x;
                    pt.y = y;
                    ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc(
                        hMenu,
                        hWnd
                    );
                }
                else
                {
                    RemoveOwnerDrawFromMenu(0, hMenu);
                }
            }
            IsImmersiveMenu = FALSE;
        }
        b = TrackPopupMenuEx(
            hMenu,
            uFlags,
            x,
            y,
            hWnd,
            lptpm
        );
        if (bContainsOwn && (b >= 12000 && b <= 12200))
        {
            LaunchPropertiesGUI(hModule);
            return FALSE;
        }
        if (!bIsSecondaryTaskbar)
        {
            explorer_TrackPopupMenuExElapsed = milliseconds_now();
        }
    }
    return b;
}
long long pnidui_TrackPopupMenuElapsed = 0;
BOOL pnidui_TrackPopupMenuHook(
    HMENU       hMenu,
    UINT        uFlags,
    int         x,
    int         y,
    int         nReserved,
    HWND        hWnd,
    const RECT* prcRect
)
{
    long long elapsed = milliseconds_now() - pnidui_TrackPopupMenuElapsed;
    BOOL b = FALSE;
    if (elapsed > POPUPMENU_PNIDUI_TIMEOUT || !bFlyoutMenus)
    {
        if (bCenterMenus)
        {
            PopupMenuAdjustCoordinatesAndFlags(&x, &y, &uFlags);
        }
        IsImmersiveMenu = FALSE;
        if (!bSkinMenus)
        {
            EnumPropsA(hWnd, CheckIfImmersiveContextMenu);
            if (IsImmersiveMenu)
            {
                if (ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc)
                {
                    POINT pt;
                    pt.x = x;
                    pt.y = y;
                    ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc(
                        hMenu,
                        hWnd
                    );
                }
                else
                {
                    RemoveOwnerDrawFromMenu(0, hMenu);
                }
            }
            IsImmersiveMenu = FALSE;
        }
        b = TrackPopupMenu(
            hMenu,
            uFlags | TPM_RIGHTBUTTON,
            x,
            y,
            0,
            hWnd,
            prcRect
        );
        if (bReplaceNetwork && b == 3109)
        {
            LaunchNetworkTargets(bReplaceNetwork + 2);
            b = 0;
        }
        pnidui_TrackPopupMenuElapsed = milliseconds_now();
    }
    return b;
}
long long sndvolsso_TrackPopupMenuExElapsed = 0;
BOOL sndvolsso_TrackPopupMenuExHook(
    HMENU       hMenu,
    UINT        uFlags,
    int         x,
    int         y,
    HWND        hWnd,
    LPTPMPARAMS lptpm
)
{
    long long elapsed = milliseconds_now() - sndvolsso_TrackPopupMenuExElapsed;
    BOOL b = FALSE;
    if (elapsed > POPUPMENU_SNDVOLSSO_TIMEOUT || !bFlyoutMenus)
    {
        if (bCenterMenus)
        {
            PopupMenuAdjustCoordinatesAndFlags(&x, &y, &uFlags);
        }
        IsImmersiveMenu = FALSE;
        if (!bSkinMenus)
        {
            EnumPropsA(hWnd, CheckIfImmersiveContextMenu);
            if (IsImmersiveMenu)
            {
                if (ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc)
                {
                    POINT pt;
                    pt.x = x;
                    pt.y = y;
                    ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc(
                        hMenu,
                        hWnd
                    );
                }
                else
                {
                    RemoveOwnerDrawFromMenu(0, hMenu);
                }
            }
            IsImmersiveMenu = FALSE;
        }

        /*MENUITEMINFOW menuInfo;
        ZeroMemory(&menuInfo, sizeof(MENUITEMINFOW));
        menuInfo.cbSize = sizeof(MENUITEMINFOW);
        menuInfo.fMask = MIIM_ID | MIIM_STRING;
        printf("GetMenuItemInfoW %d\n", GetMenuItemInfoW(
            hMenu,
            GetMenuItemCount(hMenu) - 1,
            TRUE,
            &menuInfo
        ));
        menuInfo.dwTypeData = malloc(menuInfo.cch + sizeof(wchar_t));
        menuInfo.cch++;
        printf("GetMenuItemInfoW %d\n", GetMenuItemInfoW(
            hMenu,
            GetMenuItemCount(hMenu) - 1,
            TRUE,
            &menuInfo
        ));
        wcscpy_s(menuInfo.dwTypeData, menuInfo.cch, L"test");
        menuInfo.fMask = MIIM_STRING;
        wprintf(L"SetMenuItemInfoW %s %d\n", menuInfo.dwTypeData, SetMenuItemInfoW(
            hMenu,
            GetMenuItemCount(hMenu) - 1,
            TRUE,
            &menuInfo
        ));
        wcscpy_s(menuInfo.dwTypeData, menuInfo.cch, L"");
        printf("GetMenuItemInfoW %d\n", GetMenuItemInfoW(
            hMenu,
            GetMenuItemCount(hMenu) - 1,
            TRUE,
            &menuInfo
        ));
        wprintf(L"%s\n", menuInfo.dwTypeData);
        free(menuInfo.dwTypeData);*/

        b = TrackPopupMenuEx(
            hMenu,
            uFlags | TPM_RIGHTBUTTON,
            x,
            y,
            hWnd,
            lptpm
        );
        sndvolsso_TrackPopupMenuExElapsed = milliseconds_now();
    }
    return b;
}
void PatchSndvolsso()
{
    HANDLE hSndvolsso = LoadLibraryW(L"sndvolsso.dll");
    VnPatchIAT(hSndvolsso, "user32.dll", "TrackPopupMenuEx", sndvolsso_TrackPopupMenuExHook);

    // Create a local funchook because we can get called after the global one is installed
    funchook_t* funchook = funchook_create();
    HOOK_IMMERSIVE_MENUS(Sndvolsso);
    funchook_install(funchook, 0);
    funchook_destroy(funchook);
    funchook = NULL;

    VnPatchIAT(hSndvolsso, "api-ms-win-core-registry-l1-1-0.dll", "RegGetValueW", sndvolsso_RegGetValueW);
#ifdef USE_PRIVATE_INTERFACES
    if (bSkinIcons)
    {
        VnPatchIAT(hSndvolsso, "user32.dll", "LoadImageW", SystemTray_LoadImageWHook);
    }
#endif
    printf("Setup sndvolsso functions done\n");
}
long long stobject_TrackPopupMenuExElapsed = 0;
BOOL stobject_TrackPopupMenuExHook(
    HMENU       hMenu,
    UINT        uFlags,
    int         x,
    int         y,
    HWND        hWnd,
    LPTPMPARAMS lptpm
)
{
    long long elapsed = milliseconds_now() - stobject_TrackPopupMenuExElapsed;
    BOOL b = FALSE;
    if (elapsed > POPUPMENU_SAFETOREMOVE_TIMEOUT || !bFlyoutMenus)
    {
        if (bCenterMenus)
        {
            PopupMenuAdjustCoordinatesAndFlags(&x, &y, &uFlags);
        }
        INT64* unknown_array = NULL;
        POINT pt;
        if (bSkinMenus)
        {
            unknown_array = calloc(4, sizeof(INT64));
            pt.x = x;
            pt.y = y;
            if (ImmersiveContextMenuHelper_ApplyOwnerDrawToMenuFunc)
            {
                ImmersiveContextMenuHelper_ApplyOwnerDrawToMenuFunc(
                    hMenu,
                    hWnd,
                    &(pt),
                    0xc,
                    unknown_array
                );
            }
            SetWindowSubclass(hWnd, OwnerDrawSubclassProc, OwnerDrawSubclassProc, 0);
        }
        b = TrackPopupMenuEx(
            hMenu,
            uFlags | TPM_RIGHTBUTTON,
            x,
            y,
            hWnd,
            lptpm
        );
        stobject_TrackPopupMenuExElapsed = milliseconds_now();
        if (bSkinMenus)
        {
            RemoveWindowSubclass(hWnd, OwnerDrawSubclassProc, OwnerDrawSubclassProc);
            if (ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc)
            {
                ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc(
                    hMenu,
                    hWnd
                );
            }
            free(unknown_array);
        }
    }
    return b;
}
long long stobject_TrackPopupMenuElapsed = 0;
BOOL stobject_TrackPopupMenuHook(
    HMENU       hMenu,
    UINT        uFlags,
    int         x,
    int         y,
    int         nReserved,
    HWND        hWnd,
    const RECT* prcRect
)
{
    long long elapsed = milliseconds_now() - stobject_TrackPopupMenuElapsed;
    BOOL b = FALSE;
    if (elapsed > POPUPMENU_SAFETOREMOVE_TIMEOUT || !bFlyoutMenus)
    {
        if (bCenterMenus)
        {
            PopupMenuAdjustCoordinatesAndFlags(&x, &y, &uFlags);
        }
        INT64* unknown_array = NULL;
        POINT pt;
        if (bSkinMenus)
        {
            unknown_array = calloc(4, sizeof(INT64));
            pt.x = x;
            pt.y = y;
            if (ImmersiveContextMenuHelper_ApplyOwnerDrawToMenuFunc)
            {
                ImmersiveContextMenuHelper_ApplyOwnerDrawToMenuFunc(
                    hMenu,
                    hWnd,
                    &(pt),
                    0xc,
                    unknown_array
                );
            }
            SetWindowSubclass(hWnd, OwnerDrawSubclassProc, OwnerDrawSubclassProc, 0);
        }
        b = TrackPopupMenu(
            hMenu,
            uFlags | TPM_RIGHTBUTTON,
            x,
            y,
            0,
            hWnd,
            prcRect
        );
        stobject_TrackPopupMenuElapsed = milliseconds_now();
        if (bSkinMenus)
        {
            RemoveWindowSubclass(hWnd, OwnerDrawSubclassProc, OwnerDrawSubclassProc);
            if (ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc)
            {
                ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc(
                    hMenu,
                    hWnd
                );
            }
            free(unknown_array);
        }
    }
    return b;
}
long long bthprops_TrackPopupMenuExElapsed = 0;
BOOL bthprops_TrackPopupMenuExHook(
    HMENU       hMenu,
    UINT        uFlags,
    int         x,
    int         y,
    HWND        hWnd,
    LPTPMPARAMS lptpm
)
{
    long long elapsed = milliseconds_now() - bthprops_TrackPopupMenuExElapsed;
    BOOL b = FALSE;
    if (elapsed > POPUPMENU_BLUETOOTH_TIMEOUT || !bFlyoutMenus)
    {
        if (bCenterMenus)
        {
            PopupMenuAdjustCoordinatesAndFlags(&x, &y, &uFlags);
        }
        INT64* unknown_array = NULL;
        POINT pt;
        if (bSkinMenus)
        {
            unknown_array = calloc(4, sizeof(INT64));
            pt.x = x;
            pt.y = y;
            if (ImmersiveContextMenuHelper_ApplyOwnerDrawToMenuFunc)
            {
                ImmersiveContextMenuHelper_ApplyOwnerDrawToMenuFunc(
                    hMenu,
                    hWnd,
                    &(pt),
                    0xc,
                    unknown_array
                );
            }
            SetWindowSubclass(hWnd, OwnerDrawSubclassProc, OwnerDrawSubclassProc, 0);
        }
        b = TrackPopupMenuEx(
            hMenu,
            uFlags | TPM_RIGHTBUTTON,
            x,
            y,
            hWnd,
            lptpm
        );
        bthprops_TrackPopupMenuExElapsed = milliseconds_now();
        if (bSkinMenus)
        {
            RemoveWindowSubclass(hWnd, OwnerDrawSubclassProc, OwnerDrawSubclassProc);
            if (ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc)
            {
                ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc(
                    hMenu,
                    hWnd
                );
            }
            free(unknown_array);
        }
    }
    return b;
}
long long inputswitch_TrackPopupMenuExElapsed = 0;
BOOL inputswitch_TrackPopupMenuExHook(
    HMENU       hMenu,
    UINT        uFlags,
    int         x,
    int         y,
    HWND        hWnd,
    LPTPMPARAMS lptpm
)
{
    long long elapsed = milliseconds_now() - inputswitch_TrackPopupMenuExElapsed;
    BOOL b = FALSE;
    if (elapsed > POPUPMENU_INPUTSWITCH_TIMEOUT || !bFlyoutMenus)
    {
        if (bCenterMenus)
        {
            PopupMenuAdjustCoordinatesAndFlags(&x, &y, &uFlags);
        }
        IsImmersiveMenu = FALSE;
        if (!bSkinMenus)
        {
            EnumPropsA(hWnd, CheckIfImmersiveContextMenu);
            if (IsImmersiveMenu)
            {
                if (ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc)
                {
                    POINT pt;
                    pt.x = x;
                    pt.y = y;
                    ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc(
                        hMenu,
                        hWnd
                    );
                }
                else
                {
                    RemoveOwnerDrawFromMenu(0, hMenu);
                }
            }
            IsImmersiveMenu = FALSE;
        }
        b = TrackPopupMenuEx(
            hMenu,
            uFlags | TPM_RIGHTBUTTON,
            x,
            y,
            hWnd,
            lptpm
        );
        inputswitch_TrackPopupMenuExElapsed = milliseconds_now();
    }
    return b;
}
long long twinui_TrackPopupMenuElapsed = 0;
BOOL twinui_TrackPopupMenuHook(
    HMENU       hMenu,
    UINT        uFlags,
    int         x,
    int         y,
    int         nReserved,
    HWND        hWnd,
    const RECT* prcRect
)
{
    //long long elapsed = milliseconds_now() - twinui_TrackPopupMenuElapsed;
    BOOL b = FALSE;
    if (1 /*elapsed > POPUPMENU_WINX_TIMEOUT || !bFlyoutMenus*/)
    {
        if (bCenterMenus)
        {
            //PopupMenuAdjustCoordinatesAndFlags(&x, &y, &uFlags);
        }
        IsImmersiveMenu = FALSE;
        if (!bSkinMenus)
        {
            EnumPropsA(hWnd, CheckIfImmersiveContextMenu);
            if (IsImmersiveMenu)
            {
                if (ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc)
                {
                    POINT pt;
                    pt.x = x;
                    pt.y = y;
                    ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc(
                        hMenu,
                        hWnd
                    );
                }
                else
                {
                    RemoveOwnerDrawFromMenu(0, hMenu);
                }
            }
            IsImmersiveMenu = FALSE;
        }
        b = TrackPopupMenu(
            hMenu,
            uFlags | TPM_RIGHTBUTTON,
            x,
            y,
            0,
            hWnd,
            prcRect
        );
        //twinui_TrackPopupMenuElapsed = milliseconds_now();
    }
    return b;
}
#endif
#pragma endregion


#pragma region "Disable immersive menus"
BOOL WINAPI DisableImmersiveMenus_SystemParametersInfoW(
    UINT  uiAction,
    UINT  uiParam,
    PVOID pvParam,
    UINT  fWinIni
)
{
    if (bDisableImmersiveContextMenu && uiAction == SPI_GETSCREENREADER)
    {
        *(BOOL*)pvParam = TRUE;
        return TRUE;
    }
    return SystemParametersInfoW(uiAction, uiParam, pvParam, fWinIni);
}
#pragma endregion


#pragma region "Explorer: Hide search bar, hide icon and/or title, Mica effect, hide navigation bar"
inline BOOL IsRibbonEnabled(HWND hWnd)
{
    return GetPropW(hWnd, (LPCWSTR)0xA91C);
}

inline BOOL ShouldApplyMica(HWND hWnd)
{
    if (!IsRibbonEnabled(hWnd)) return TRUE;
    return FindWindowExW(hWnd, NULL, L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
}

HRESULT ApplyMicaToExplorerTitlebar(HWND hWnd, DWORD_PTR bMicaEffectOnTitleBarOrig)
{
    RECT Rect;
    GetWindowRect(hWnd, &Rect);
    HWND hWndRoot = GetAncestor(hWnd, GA_ROOT);
    MapWindowPoints(NULL, hWndRoot, (LPPOINT)&Rect, 2);
    MARGINS pMarInset;
    ZeroMemory(&pMarInset, sizeof(MARGINS));
    pMarInset.cyTopHeight = Rect.bottom;
    wchar_t wszParentText[100];
    GetWindowTextW(GetParent(hWnd), wszParentText, 100);
    if (!_wcsicmp(wszParentText, L"FloatingWindow")) pMarInset.cyTopHeight = 0;
    BOOL bShouldApplyMica;
    if (bMicaEffectOnTitleBarOrig == 2) bShouldApplyMica = FALSE;
    else bShouldApplyMica = ShouldApplyMica(GetAncestor(hWnd, GA_ROOT));
    if (bShouldApplyMica)
    {
        DwmExtendFrameIntoClientArea(hWndRoot, &pMarInset);
        SetPropW(hWndRoot, L"EP_METB", TRUE);
    }
    else
    {
        RemovePropW(hWndRoot, L"EP_METB");
    }
    return SetMicaMaterialForThisWindow(hWndRoot, bShouldApplyMica);
}

LRESULT RebarWindow32MicaTitlebarSubclassproc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    if (uMsg == RB_SETWINDOWTHEME && !wcsncmp(lParam, L"DarkMode", 8) && dwRefData != 2 && ShouldApplyMica(GetAncestor(hWnd, GA_ROOT)))
    {
        lParam = wcsstr(lParam, L"NavbarComposited");
    }
    else if (uMsg == WM_DESTROY)
    {
        RemoveWindowSubclass(hWnd, RebarWindow32MicaTitlebarSubclassproc, RebarWindow32MicaTitlebarSubclassproc);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT ExplorerMicaTitlebarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    if (uMsg == WM_DESTROY)
    {
        RemoveWindowSubclass(hWnd, ExplorerMicaTitlebarSubclassProc, ExplorerMicaTitlebarSubclassProc);
    }
    if (uMsg == WM_ERASEBKGND)
    {
        wchar_t wszParentText[100];
        GetWindowTextW(GetParent(hWnd), wszParentText, 100);
        if (_wcsicmp(wszParentText, L"FloatingWindow") && dwRefData != 2 && ShouldApplyMica(GetAncestor(hWnd, GA_ROOT))) return TRUE;
    }
    else if (uMsg == WM_WINDOWPOSCHANGED)
    {
        WINDOWPOS* lpWp = (WINDOWPOS*)lParam;
        if (lpWp->flags & SWP_NOMOVE)
        {
            ApplyMicaToExplorerTitlebar(hWnd, dwRefData);
        }
        else
        {
            PostMessageW(hWnd, WM_APP, 0, 0);
        }
    }
    else if (uMsg == WM_APP)
    {
        ApplyMicaToExplorerTitlebar(hWnd, dwRefData);
    }
    else if (uMsg == WM_PARENTNOTIFY)
    {
        if (LOWORD(wParam) == WM_CREATE)
        {
            ATOM atom = GetClassWord(lParam, GCW_ATOM);
            if (atom == RegisterWindowMessageW(L"ReBarWindow32"))
            {
                SetWindowSubclass(lParam, RebarWindow32MicaTitlebarSubclassproc, RebarWindow32MicaTitlebarSubclassproc, dwRefData);
            }
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK HideIconAndTitleInExplorerSubClass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    if (uMsg == WM_DESTROY)
    {
        RemoveWindowSubclass(hWnd, HideIconAndTitleInExplorerSubClass, HideIconAndTitleInExplorerSubClass);
    }
    else if (uMsg == WM_PARENTNOTIFY)
    {
        if (LOWORD(wParam) == WM_CREATE)
        {
            WTA_OPTIONS ops;
            ops.dwFlags = bHideIconAndTitleInExplorer;
            ops.dwMask = WTNCA_NODRAWCAPTION | WTNCA_NODRAWICON;
            SetWindowThemeAttribute(hWnd, WTA_NONCLIENT, &ops, sizeof(WTA_OPTIONS));
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

HRESULT uxtheme_DwmExtendFrameIntoClientAreaHook(HWND hWnd, MARGINS* m)
{
    if (GetPropW(hWnd, L"EP_METB"))
    {
        return S_OK;
    }
    return DwmExtendFrameIntoClientArea(hWnd, m);
}

HWND(__stdcall *explorerframe_SHCreateWorkerWindowFunc)(
    WNDPROC  	wndProc,
    HWND  	hWndParent,
    DWORD  	dwExStyle,
    DWORD  	dwStyle,
    HMENU  	hMenu,
    LONG_PTR  	wnd_extra
    );

HWND WINAPI explorerframe_SHCreateWorkerWindowHook(
    WNDPROC  	wndProc,
    HWND  	hWndParent,
    DWORD  	dwExStyle,
    DWORD  	dwStyle,
    HMENU  	hMenu,
    LONG_PTR  	wnd_extra
)
{
    HWND result;
    LSTATUS lRes = ERROR_FILE_NOT_FOUND;
    DWORD dwSize = 0;
    
    printf("%x %x\n", dwExStyle, dwStyle);

    if (SHRegGetValueFromHKCUHKLMWithOpt(
        TEXT("SOFTWARE\\Classes\\CLSID\\{056440FD-8568-48e7-A632-72157243B55B}\\InProcServer32"),
        TEXT(""),
        KEY_READ | KEY_WOW64_64KEY,
        NULL,
        (LPDWORD)(&dwSize)
    ) == ERROR_SUCCESS && (dwSize < 4) && dwExStyle == 0x10000 && dwStyle == 0x46000000)
    {
        result = 0;
    }
    else
    {
        result = explorerframe_SHCreateWorkerWindowFunc(
            wndProc,
            hWndParent,
            dwExStyle,
            dwStyle,
            hMenu,
            wnd_extra
        );
    }
    if (dwExStyle == 0x10000 && dwStyle == 0x46000000 && result)
    {
        if (bHideIconAndTitleInExplorer)
        {
            SetWindowSubclass(hWndParent, HideIconAndTitleInExplorerSubClass, HideIconAndTitleInExplorerSubClass, 0);
        }
        if ((!bIsExplorerProcess || dwFileExplorerCommandUI == 2) && bMicaEffectOnTitlebar)
        {
            SetWindowSubclass(result, ExplorerMicaTitlebarSubclassProc, ExplorerMicaTitlebarSubclassProc, bMicaEffectOnTitlebar);
        }
        if (bHideExplorerSearchBar)
        {
            SetWindowSubclass(hWndParent, HideExplorerSearchBarSubClass, HideExplorerSearchBarSubClass, 0);
        }
        if (bIsExplorerProcess && (dwFileExplorerCommandUI == 1 || dwFileExplorerCommandUI == 2 || dwFileExplorerCommandUI == 3) && IsWindows11Version22H2OrHigher())
        {
            // Fix initial title bar style after disabling TIFE
            // If we don't do this, it will only fix itself once the user changes the system color scheme or toggling transparency effects
            if (!ShouldAppsUseDarkMode)
            {
                HANDLE hUxtheme = LoadLibraryW(L"uxtheme.dll");
                ShouldAppsUseDarkMode = GetProcAddress(hUxtheme, (LPCSTR)0x84);
            }
            if (ShouldAppsUseDarkMode)
            {
                BOOL bDarkMode = ShouldAppsUseDarkMode() && !IsHighContrast();
                DwmSetWindowAttribute(hWndParent, DWMWA_USE_IMMERSIVE_DARK_MODE, &bDarkMode, sizeof(BOOL));
            }
        }
    }
    return result;
}
#pragma endregion


#pragma region "Fix battery flyout"
#if WITH_MAIN_PATCHER
LSTATUS stobject_RegGetValueW(
    HKEY    hkey,
    LPCWSTR lpSubKey,
    LPCWSTR lpValue,
    DWORD   dwFlags,
    LPDWORD pdwType,
    PVOID   pvData,
    LPDWORD pcbData
)
{
    if (!lstrcmpW(lpValue, L"UseWin32BatteryFlyout"))
    {
        if (SHRegGetValueFromHKCUHKLMFunc)
        {
            return SHRegGetValueFromHKCUHKLMFunc(
                lpSubKey,
                lpValue,
                SRRF_RT_REG_DWORD,
                pdwType,
                pvData,
                pcbData
            );
        }
    }
    return RegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}

DEFINE_GUID(CLSID_NetworkTraySSO, 0xC2796011, 0x81BA, 0x4148, 0x8F, 0xCA, 0xC6, 0x64, 0x32, 0x45, 0x11, 0x3F);
DEFINE_GUID(CLSID_WindowsToGoSSO, 0x4DC9C264, 0x730E, 0x4CF6, 0x83, 0x74, 0x70, 0xF0, 0x79, 0xE4, 0xF8, 0x2B);

typedef HRESULT(WINAPI* DllGetClassObject_t)(REFCLSID rclsid, REFIID riid, LPVOID* ppv);

void PatchPnidui(HMODULE hPnidui);

HRESULT stobject_CoCreateInstanceHook(
    REFCLSID  rclsid,
    LPUNKNOWN pUnkOuter,
    DWORD     dwClsContext,
    REFIID    riid,
    LPVOID* ppv
)
{
    if (global_rovi.dwBuildNumber >= 25236 && IsEqualGUID(rclsid, &CLSID_NetworkTraySSO) && bOldTaskbar)
    {
        wchar_t szPath[MAX_PATH];
        ZeroMemory(szPath, sizeof(szPath));
        SHGetFolderPathW(NULL, SPECIAL_FOLDER, NULL, SHGFP_TYPE_CURRENT, szPath);
        wcscat_s(szPath, MAX_PATH, _T(APP_RELATIVE_PATH) L"\\pnidui.dll");
        HMODULE hPnidui = LoadLibraryW(szPath);
        DllGetClassObject_t pfnDllGetClassObject = hPnidui ? (DllGetClassObject_t)GetProcAddress(hPnidui, "DllGetClassObject") : NULL;
        if (!pfnDllGetClassObject)
        {
            return REGDB_E_CLASSNOTREG;
        }
        PatchPnidui(hPnidui);
        IClassFactory* pClassFactory = NULL;
        HRESULT hr = pfnDllGetClassObject(rclsid, &IID_IClassFactory, (LPVOID*)&pClassFactory);
        if (SUCCEEDED(hr))
        {
            hr = pClassFactory->lpVtbl->CreateInstance(pClassFactory, pUnkOuter, riid, ppv);
            pClassFactory->lpVtbl->Release(pClassFactory);
        }
        return hr;
    }

    DWORD dwVal = 0, dwSize = sizeof(DWORD);
    if (IsEqualGUID(rclsid, &CLSID_ImmersiveShell) &&
        IsEqualGUID(riid, &IID_IServiceProvider) &&
        SHRegGetValueFromHKCUHKLMFunc)
    {
        SHRegGetValueFromHKCUHKLMFunc(
            TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\ImmersiveShell"),
            TEXT("UseWin32BatteryFlyout"),
            SRRF_RT_REG_DWORD,
            NULL,
            &dwVal,
            (LPDWORD)(&dwSize)
        );
        if (!dwVal && IsWindows11() && !IsWindows11Version22H2Build2134OrHigher())
        {
            if (hCheckForegroundThread)
            {
                if (WaitForSingleObject(hCheckForegroundThread, 0) == WAIT_TIMEOUT)
                {
                    return E_NOINTERFACE;
                }
                WaitForSingleObject(hCheckForegroundThread, INFINITE);
                CloseHandle(hCheckForegroundThread);
                hCheckForegroundThread = NULL;
            }
            HKEY hKey = NULL;
            if (RegCreateKeyExW(
                HKEY_CURRENT_USER,
                _T(SEH_REGPATH),
                0,
                NULL,
                REG_OPTION_VOLATILE,
                KEY_READ,
                NULL,
                &hKey,
                NULL
            ) == ERROR_SUCCESS)
            {
                RegCloseKey(hKey);
            }
            TerminateShellExperienceHost();
            InvokeFlyout(0, INVOKE_FLYOUT_BATTERY);
            Sleep(100);
            hCheckForegroundThread = CreateThread(
                0,
                0,
                CheckForegroundThread,
                dwVal,
                0,
                0
            );
        }
    }
    return CoCreateInstance(
        rclsid,
        pUnkOuter,
        dwClsContext,
        riid,
        ppv
    );
}
#endif
#pragma endregion


#pragma region "Show WiFi networks on network icon click"
#if WITH_MAIN_PATCHER
HRESULT pnidui_CoCreateInstanceHook(
    REFCLSID  rclsid,
    LPUNKNOWN pUnkOuter,
    DWORD     dwClsContext,
    REFIID    riid,
    LPVOID* ppv
)
{
    DWORD dwVal = 0, dwSize = sizeof(DWORD);
    if (IsEqualGUID(rclsid, &CLSID_ImmersiveShell) && 
        IsEqualGUID(riid, &IID_IServiceProvider) &&
        SHRegGetValueFromHKCUHKLMFunc)
    {
        SHRegGetValueFromHKCUHKLMFunc(
            TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Control Panel\\Settings\\Network"),
            TEXT("ReplaceVan"),
            SRRF_RT_REG_DWORD,
            NULL,
            &dwVal,
            (LPDWORD)(&dwSize)
        );
        if (dwVal)
        {
            if ((dwVal == 5 || dwVal == 6) && IsWindows11() && !IsWindows11Version22H2Build1413OrHigher())
            {
                if (hCheckForegroundThread)
                {
                    WaitForSingleObject(hCheckForegroundThread, INFINITE);
                    CloseHandle(hCheckForegroundThread);
                    hCheckForegroundThread = NULL;
                }
                if (milliseconds_now() - elapsedCheckForeground > CHECKFOREGROUNDELAPSED_TIMEOUT)
                {
                    LaunchNetworkTargets(dwVal);
                    hCheckForegroundThread = CreateThread(
                        0,
                        0,
                        CheckForegroundThread,
                        dwVal,
                        0,
                        0
                    );
                }
            }
            else
            {
                LaunchNetworkTargets(dwVal);
            }
            return E_NOINTERFACE;
        }
        else if (IsWindows11() && !IsWindows11Version22H2Build1413OrHigher())
        {
            if (hCheckForegroundThread)
            {
                if (WaitForSingleObject(hCheckForegroundThread, 0) == WAIT_TIMEOUT)
                {
                    return E_NOINTERFACE;
                }
                WaitForSingleObject(hCheckForegroundThread, INFINITE);
                CloseHandle(hCheckForegroundThread);
                hCheckForegroundThread = NULL;
            }
            HKEY hKey = NULL;
            if (RegCreateKeyExW(
                HKEY_CURRENT_USER,
                _T(SEH_REGPATH),
                0,
                NULL,
                REG_OPTION_NON_VOLATILE,
                KEY_READ,
                NULL,
                &hKey,
                NULL
            ) == ERROR_SUCCESS)
            {
                RegCloseKey(hKey);
            }
            TerminateShellExperienceHost();
            InvokeFlyout(0, INVOKE_FLYOUT_NETWORK);
            Sleep(100);
            hCheckForegroundThread = CreateThread(
                0,
                0,
                CheckForegroundThread,
                dwVal,
                0,
                0
            );
        }
    }
    return CoCreateInstance(
        rclsid,
        pUnkOuter,
        dwClsContext,
        riid,
        ppv
    );
}
#endif
#pragma endregion


#pragma region "Clock flyout helper"
#if WITH_MAIN_PATCHER
typedef struct _ClockButton_ToggleFlyoutCallback_Params
{
    void* TrayUIInstance;
    unsigned int CLOCKBUTTON_OFFSET_IN_TRAYUI;
    void* oldClockButtonInstance;
} ClockButton_ToggleFlyoutCallback_Params;
void ClockButton_ToggleFlyoutCallback(
    HWND hWnd,
    UINT uMsg,
    ClockButton_ToggleFlyoutCallback_Params* params,
    LRESULT lRes
)
{
    *((INT64*)params->TrayUIInstance + params->CLOCKBUTTON_OFFSET_IN_TRAYUI) = params->oldClockButtonInstance;
    free(params);
}
BOOL InvokeClockFlyout()
{
    POINT ptCursor;
    GetCursorPos(&ptCursor);
    HWND hWnd = GetMonitorInfoFromPointForTaskbarFlyoutActivation(
        ptCursor,
        MONITOR_DEFAULTTOPRIMARY,
        NULL
    );
    HWND prev_hWnd = hWnd;
    HWND hShellTray_Wnd = FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL);
    const unsigned int WM_TOGGLE_CLOCK_FLYOUT = 1486;
    if (hWnd == hShellTray_Wnd)
    {
        if (ShouldShowLegacyClockExperience() == 1)
        {
            if (!FindWindowW(L"ClockFlyoutWindow", NULL))
            {
                if (bOldTaskbar)
                {
                    return ShowLegacyClockExperience(FindWindowExW(FindWindowExW(hShellTray_Wnd, NULL, L"TrayNotifyWnd", NULL), NULL, L"TrayClockWClass", NULL));
                }
                else
                {
                    POINT pt;
                    pt.x = 0;
                    pt.y = 0;
                    GetCursorPos(&pt);
                    BOOL bBottom, bRight;
                    POINT dPt = GetDefaultWinXPosition(FALSE, NULL, NULL, FALSE, TRUE);
                    SetCursorPos(dPt.x - 1, dPt.y);
                    BOOL bRet = ShowLegacyClockExperience(hShellTray_Wnd);
                    SetCursorPos(pt.x, pt.y);
                    return bRet;
                }
            }
            else
            {
                return PostMessageW(FindWindowW(L"ClockFlyoutWindow", NULL), WM_CLOSE, 0, 0);
            }
        }
        else if (ShouldShowLegacyClockExperience() == 2)
        {
            return ToggleNotificationsFlyout();
        }
        // On the main monitor, the TrayUI component of CTray handles this
        // message and basically does a `ClockButton::ToggleFlyout`; that's
        // the only place in code where that is used, otherwise, clicking and
        // dismissing the clock flyout probably involves 2 separate methods
        return PostMessageW(hShellTray_Wnd, WM_TOGGLE_CLOCK_FLYOUT, 0, 0);
    }
    else
    {
        // Of course, on secondary monitors, the situation is much more
        // complicated; there is no simple way to do this, afaik; the way I do it
        // is to obtain a pointer to TrayUI from CTray (pointers to the classes
        // that created the windows are always available at location 0 in the hWnd)
        // and from there issue a "show clock flyout" message manually, taking care to temporarly
        // change the internal clock button pointer of the class to point
        // to the clock button on the secondary monitor.
        if (bOldTaskbar)
        {
            hWnd = FindWindowExW(hWnd, NULL, L"ClockButton", NULL);
        }
        if (hWnd)
        {
            if (ShouldShowLegacyClockExperience() == 1)
            {
                if (!FindWindowW(L"ClockFlyoutWindow", NULL))
                {
                    if (bOldTaskbar)
                    {
                        return ShowLegacyClockExperience(hWnd);
                    }
                    else
                    {
                        POINT pt;
                        pt.x = 0;
                        pt.y = 0;
                        GetCursorPos(&pt);
                        BOOL bBottom, bRight;
                        POINT dPt = GetDefaultWinXPosition(FALSE, NULL, NULL, FALSE, TRUE);
                        SetCursorPos(dPt.x, dPt.y);
                        BOOL bRet = ShowLegacyClockExperience(hWnd);
                        SetCursorPos(pt.x, pt.y);
                        return bRet;
                    }
                }
                else
                {
                    return PostMessageW(FindWindowW(L"ClockFlyoutWindow", NULL), WM_CLOSE, 0, 0);
                }
            }
            else if (ShouldShowLegacyClockExperience() == 2)
            {
                return ToggleNotificationsFlyout();
            }
            if (bOldTaskbar)
            {
                INT64* CTrayInstance = (BYTE*)(GetWindowLongPtrW(hShellTray_Wnd, 0)); // -> CTray
                void* ClockButtonInstance = (BYTE*)(GetWindowLongPtrW(hWnd, 0)); // -> ClockButton

                // inspect CTray::v_WndProc, look for mentions of
                // CTray::_HandlePowerStatus or patterns like **((_QWORD **)this + 110) + 184i64
                const unsigned int TRAYUI_OFFSET_IN_CTRAY = 110;
                // simply inspect vtable of TrayUI
                const unsigned int TRAYUI_WNDPROC_POSITION_IN_VTABLE = 4;
                // inspect TrayUI::WndProc, specifically this section
                /*
                    {
                      if ( (_DWORD)a3 == 1486 )
                      {
                        v80 = (ClockButton *)*((_QWORD *)this + 100);
                        if ( v80 )
                          ClockButton::ToggleFlyout(v80);
                */
                const unsigned int CLOCKBUTTON_OFFSET_IN_TRAYUI = 100;
                void* TrayUIInstance = *((INT64*)CTrayInstance + TRAYUI_OFFSET_IN_CTRAY);
                void* oldClockButtonInstance = *((INT64*)TrayUIInstance + CLOCKBUTTON_OFFSET_IN_TRAYUI);
                ClockButton_ToggleFlyoutCallback_Params* params = malloc(sizeof(ClockButton_ToggleFlyoutCallback_Params));
                if (params)
                {
                    *((INT64*)TrayUIInstance + CLOCKBUTTON_OFFSET_IN_TRAYUI) = ClockButtonInstance;
                    params->TrayUIInstance = TrayUIInstance;
                    params->CLOCKBUTTON_OFFSET_IN_TRAYUI = CLOCKBUTTON_OFFSET_IN_TRAYUI;
                    params->oldClockButtonInstance = oldClockButtonInstance;
                    return SendMessageCallbackW(hShellTray_Wnd, WM_TOGGLE_CLOCK_FLYOUT, 0, 0, ClockButton_ToggleFlyoutCallback, params);
                }
            }
            else
            {
                return PostMessageW(hShellTray_Wnd, WM_TOGGLE_CLOCK_FLYOUT, 0, 0);
            }
        }
    }
    return FALSE;
}
#endif
#pragma endregion


#pragma region "Open power user menu on Win+X"
#if WITH_MAIN_PATCHER
LRESULT explorer_SendMessageW(HWND hWndx, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == TB_GETTEXTROWS)
    {
        HWND hWnd = FindWindowEx(
            NULL,
            NULL,
            L"Shell_TrayWnd",
            NULL
        );
        if (hWnd)
        {
            hWnd = FindWindowEx(
                hWnd,
                NULL,
                L"RebarWindow32",
                NULL
            );
            if (hWnd)
            {
                hWnd = FindWindowEx(
                    hWnd,
                    NULL,
                    L"MSTaskSwWClass",
                    NULL
                );
                if (hWnd && hWnd == hWndx && wParam == -1)
                {
                    ToggleLauncherTipContextMenu();
                    return 0;
                }
            }
        }
    }
    return SendMessageW(hWndx, uMsg, wParam, lParam);
}
#endif
#pragma endregion


#pragma region "Set up taskbar button hooks, implement Weather widget"
#if WITH_MAIN_PATCHER

DWORD ShouldShowWidgetsInsteadOfCortana()
{
    DWORD dwVal = 0, dwSize = sizeof(DWORD);
    if (SHRegGetValueFromHKCUHKLMFunc && SHRegGetValueFromHKCUHKLMFunc(
        TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),
        TEXT("TaskbarDa"),
        SRRF_RT_REG_DWORD,
        NULL,
        &dwVal,
        (LPDWORD)(&dwSize)
    ) == ERROR_SUCCESS)
    {
        return dwVal;
    }
    return 0;
}

__int64 (*Widgets_OnClickFunc)(__int64 a1, __int64 a2) = 0;
__int64 Widgets_OnClickHook(__int64 a1, __int64 a2)
{
    if (ShouldShowWidgetsInsteadOfCortana() == 1)
    {
        ToggleWidgetsPanel();
        return 0;
    }
    else
    {
        if (Widgets_OnClickFunc)
        {
            return Widgets_OnClickFunc(a1, a2);
        }
        return 0;
    }
}

HRESULT (*Widgets_GetTooltipTextFunc)(__int64 a1, __int64 a2, __int64 a3, WCHAR* a4, UINT a5) = 0;
HRESULT WINAPI Widgets_GetTooltipTextHook(__int64 a1, __int64 a2, __int64 a3, WCHAR* a4, UINT a5)
{
    if (ShouldShowWidgetsInsteadOfCortana() == 1)
    {
        return SHLoadIndirectString(
            L"@{windows?ms-resource://Windows.UI.SettingsAppThreshold/SystemSettings/Resources/SystemSettings_DesktopTaskbar_Da2/DisplayName}",
            a4,
            a5,
            0
        );
    }
    else
    {
        if (Widgets_GetTooltipTextFunc)
        {
            return Widgets_GetTooltipTextFunc(a1, a2, a3, a4, a5);
        }
        return 0;
    }
}

/*int WINAPI explorer_LoadStringWHook(HINSTANCE hInstance, UINT uID, WCHAR* lpBuffer, UINT cchBufferMax)
{
    WCHAR wszBuffer[MAX_PATH];
    if (hInstance == GetModuleHandle(NULL) && uID == 912)// && SUCCEEDED(epw->lpVtbl->GetTitle(epw, MAX_PATH, wszBuffer)))
    {
        //sws_error_PrintStackTrace();
        int rez = LoadStringW(hInstance, uID, lpBuffer, cchBufferMax);
        //wprintf(L"%s\n", lpBuffer);
        return rez;
    }
    else
    {
        return LoadStringW(hInstance, uID, lpBuffer, cchBufferMax);
    }
}*/

void stub1(void* i)
{
}

#define WEATHER_FIXEDSIZE2_MAXWIDTH 192
BOOL explorer_DeleteMenu(HMENU hMenu, UINT uPosition, UINT uFlags)
{
    if (uPosition == 621 && uFlags == 0) // when removing News and interests
    {
        DeleteMenu(hMenu, 449, 0); // remove Cortana menu
        DeleteMenu(hMenu, 435, 0); // remove People menu
    }
    if (!IsWindows11() && uPosition == 445 && uFlags == 0) // when removing Cortana in Windows 10
    {
        DeleteMenu(hMenu, 435, 0);
    }
    return DeleteMenu(hMenu, uPosition, uFlags);
}

HWND hWndWeatherFlyout;
void RecomputeWeatherFlyoutLocation(HWND hWnd)
{
    RECT rcButton;
    GetWindowRect(PeopleButton_LastHWND, &rcButton);
    POINT pButton;
    pButton.x = rcButton.left;
    pButton.y = rcButton.top;

    RECT rcWeatherFlyoutWindow;
    GetWindowRect(hWnd, &rcWeatherFlyoutWindow);

    POINT pNewWindow;

    RECT rc;
    UINT tbPos = GetTaskbarLocationAndSize(pButton, &rc);
    if (tbPos == TB_POS_BOTTOM)
    {
        pNewWindow.y = rcButton.top - (rcWeatherFlyoutWindow.bottom - rcWeatherFlyoutWindow.top);
    }
    else if (tbPos == TB_POS_TOP)
    {
        pNewWindow.y = rcButton.bottom;
    }
    else if (tbPos == TB_POS_LEFT)
    {
        pNewWindow.x = rcButton.right;
    }
    if (tbPos == TB_POS_RIGHT)
    {
        pNewWindow.x = rcButton.left - (rcWeatherFlyoutWindow.right - rcWeatherFlyoutWindow.left);
    }

    if (tbPos == TB_POS_BOTTOM || tbPos == TB_POS_TOP)
    {
        pNewWindow.x = rcButton.left + ((rcButton.right - rcButton.left) / 2) - ((rcWeatherFlyoutWindow.right - rcWeatherFlyoutWindow.left) / 2);

        HMONITOR hMonitor = MonitorFromPoint(pButton, MONITOR_DEFAULTTOPRIMARY);
        if (hMonitor)
        {
            MONITORINFO mi;
            mi.cbSize = sizeof(MONITORINFO);
            if (GetMonitorInfoW(hMonitor, &mi))
            {
                if (mi.rcWork.right < pNewWindow.x + (rcWeatherFlyoutWindow.right - rcWeatherFlyoutWindow.left))
                {
                    pNewWindow.x = mi.rcWork.right - (rcWeatherFlyoutWindow.right - rcWeatherFlyoutWindow.left);
                }
                if (mi.rcWork.left > pNewWindow.x)
                {
                    pNewWindow.x = mi.rcWork.left;
                }
            }
        }
    }
    else if (tbPos == TB_POS_LEFT || tbPos == TB_POS_RIGHT)
    {
        pNewWindow.y = rcButton.top + ((rcButton.bottom - rcButton.top) / 2) - ((rcWeatherFlyoutWindow.bottom - rcWeatherFlyoutWindow.top) / 2);

        HMONITOR hMonitor = MonitorFromPoint(pButton, MONITOR_DEFAULTTOPRIMARY);
        if (hMonitor)
        {
            MONITORINFO mi;
            mi.cbSize = sizeof(MONITORINFO);
            if (GetMonitorInfoW(hMonitor, &mi))
            {
                if (mi.rcWork.bottom < pNewWindow.y + (rcWeatherFlyoutWindow.bottom - rcWeatherFlyoutWindow.top))
                {
                    pNewWindow.y = mi.rcWork.bottom - (rcWeatherFlyoutWindow.bottom - rcWeatherFlyoutWindow.top);
                }
                if (mi.rcWork.top > pNewWindow.y)
                {
                    pNewWindow.y = mi.rcWork.top;
                }
            }
        }
    }

    SetWindowPos(hWnd, NULL, pNewWindow.x, pNewWindow.y, 0, 0, SWP_NOSIZE | SWP_NOSENDCHANGING);
}

BOOL people_has_ellipsed = FALSE;
SIZE (*PeopleButton_CalculateMinimumSizeFunc)(void*, SIZE*);
SIZE WINAPI PeopleButton_CalculateMinimumSizeHook(void* _this, SIZE* pSz)
{
    SIZE ret = PeopleButton_CalculateMinimumSizeFunc(_this, pSz);
    BOOL bHasLocked = TryEnterCriticalSection(&lock_epw);
    if (bHasLocked && epw)
    {
        if (bWeatherFixedSize == 1)
        {
            int mul = 1;
            switch (dwWeatherViewMode)
            {
            case EP_WEATHER_VIEW_ICONTEXT:
                mul = 4;
                break;
            case EP_WEATHER_VIEW_TEXTONLY:
                mul = 3;
                break;
            case EP_WEATHER_VIEW_ICONTEMP:
                mul = 2;
                break;
            case EP_WEATHER_VIEW_ICONONLY:
            case EP_WEATHER_VIEW_TEMPONLY:
                mul = 1;
                break;
            }
            pSz->cx = pSz->cx * mul;
        }
        else
        {
            if (!prev_total_h)
            {
                pSz->cx = 10000;
            }
            else
            {
                pSz->cx = prev_total_h;
            }
        }
        //printf("[CalculateMinimumSize] %d %d\n", pSz->cx, pSz->cy);
        if (pSz->cy)
        {
            BOOL bIsInitialized = TRUE;
            HRESULT hr = epw->lpVtbl->IsInitialized(epw, &bIsInitialized);
            if (SUCCEEDED(hr))
            {
                int rt = MulDiv(48, pSz->cy, 60);
                if (!bIsInitialized)
                {
                    epw->lpVtbl->SetTerm(epw, MAX_PATH * sizeof(WCHAR), wszWeatherTerm);
                    epw->lpVtbl->SetLanguage(epw, MAX_PATH * sizeof(WCHAR), wszWeatherLanguage);
                    epw->lpVtbl->SetDevMode(epw, dwWeatherDevMode, FALSE);
                    epw->lpVtbl->SetIconPack(epw, dwWeatherIconPack, FALSE);
                    UINT dpiX = 0, dpiY = 0;
                    HMONITOR hMonitor = MonitorFromWindow(PeopleButton_LastHWND, MONITOR_DEFAULTTOPRIMARY);
                    HRESULT hr = GetDpiForMonitor(hMonitor, MDT_DEFAULT, &dpiX, &dpiY);
                    MONITORINFO mi;
                    ZeroMemory(&mi, sizeof(MONITORINFO));
                    mi.cbSize = sizeof(MONITORINFO);
                    if (GetMonitorInfoW(hMonitor, &mi))
                    {
                        DWORD dwTextScaleFactor = 0, dwSize = sizeof(DWORD);
                        if (SHRegGetValueFromHKCUHKLMFunc && SHRegGetValueFromHKCUHKLMFunc(
                            TEXT("SOFTWARE\\Microsoft\\Accessibility"),
                            TEXT("TextScaleFactor"),
                            SRRF_RT_REG_DWORD,
                            NULL,
                            &dwTextScaleFactor,
                            (LPDWORD)(&dwSize)
                        ) != ERROR_SUCCESS)
                        {
                            dwTextScaleFactor = 100;
                        }

                        RECT rcWeatherFlyoutWindow;
                        rcWeatherFlyoutWindow.left = mi.rcWork.left;
                        rcWeatherFlyoutWindow.top = mi.rcWork.top;
                        rcWeatherFlyoutWindow.right = rcWeatherFlyoutWindow.left + MulDiv(MulDiv(MulDiv(EP_WEATHER_WIDTH, dpiX, 96), dwTextScaleFactor, 100), dwWeatherZoomFactor, 100);
                        rcWeatherFlyoutWindow.bottom = rcWeatherFlyoutWindow.top + MulDiv(MulDiv(MulDiv(EP_WEATHER_HEIGHT, dpiX, 96), dwTextScaleFactor, 100), dwWeatherZoomFactor, 100);
                        int k = 0;
                        while (FAILED(hr = epw->lpVtbl->Initialize(epw, wszEPWeatherKillswitch, bAllocConsole, EP_WEATHER_PROVIDER_GOOGLE, rt, rt, dwWeatherTemperatureUnit, dwWeatherUpdateSchedule * 1000, rcWeatherFlyoutWindow, dwWeatherTheme, dwWeatherGeolocationMode, &hWndWeatherFlyout, dwWeatherZoomFactor ? dwWeatherZoomFactor : 100, dpiX, dpiY)))
                        {
                            BOOL bFailed = FALSE;
                            if (k == 0 && hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                            {
                                if (DownloadAndInstallWebView2Runtime())
                                {
                                    k++;
                                }
                                else
                                {
                                    bFailed = TRUE;
                                }
                            }
                            else
                            {
                                bFailed = TRUE;
                            }
                            if (bFailed)
                            {
                                prev_total_h = 0;
                                PostMessageW(FindWindowW(L"Shell_TrayWnd", NULL), WM_COMMAND, 435, 0);
                                //PostMessageW(FindWindowW(L"ExplorerPatcher_GUI_" _T(EP_CLSID), NULL), WM_USER + 1, 0, 0);
                                if (hServiceWindowThread) PostThreadMessageW(GetThreadId(hServiceWindowThread), WM_USER + 1, NULL, NULL);
                                break;
                            }
                        }
                        if (SUCCEEDED(hr))
                        {
                            epw->lpVtbl->SetWindowCornerPreference(epw, dwWeatherWindowCornerPreference);
                        }
                    }
                }
                else
                {
                    epw->lpVtbl->SetIconSize(epw, rt, rt);
                }
            }
            else
            {
                if (hr == 0x800706ba) // RPC server is unavailable
                {
                    //ReleaseSRWLockShared(&lock_epw);
                    /*AcquireSRWLockExclusive(&lock_epw);
                    epw = NULL;
                    prev_total_h = 0;
                    InvalidateRect(PeopleButton_LastHWND, NULL, TRUE);
                    ReleaseSRWLockExclusive(&lock_epw);*/
                    if (hServiceWindowThread) PostThreadMessageW(GetThreadId(hServiceWindowThread), WM_USER + 1, NULL, NULL);
                    //AcquireSRWLockShared(&lock_epw);
                }
            }
        }
        LeaveCriticalSection(&lock_epw);
    }
    else
    {
        if (bHasLocked)
        {
            LeaveCriticalSection(&lock_epw);
        }
    }
    return ret;
}

int PeopleBand_MulDivHook(int nNumber, int nNumerator, int nDenominator)
{
    if (nNumber != 46) // 46 = vertical taskbar, 48 = horizontal taskbar
    {
        //printf("[MulDivHook] %d %d %d\n", nNumber, nNumerator, nDenominator);
        BOOL bHasLocked = TryEnterCriticalSection(&lock_epw);
        if (bHasLocked && epw)
        {
            if (bWeatherFixedSize == 1)
            {
                int mul = 1;
                switch (dwWeatherViewMode)
                {
                case EP_WEATHER_VIEW_ICONTEXT:
                    mul = 4;
                    break;
                case EP_WEATHER_VIEW_TEXTONLY:
                    mul = 3;
                    break;
                case EP_WEATHER_VIEW_ICONTEMP:
                    mul = 2;
                    break;
                case EP_WEATHER_VIEW_ICONONLY:
                case EP_WEATHER_VIEW_TEMPONLY:
                    mul = 1;
                    break;
                }
                LeaveCriticalSection(&lock_epw);
                return MulDiv(nNumber * mul, nNumerator, nDenominator);
            }
            else
            {
                if (prev_total_h)
                {
                    LeaveCriticalSection(&lock_epw);
                    return prev_total_h;
                }
                else
                {
                    prev_total_h = MulDiv(nNumber, nNumerator, nDenominator);
                    LeaveCriticalSection(&lock_epw);
                    return prev_total_h;
                }
            }
            LeaveCriticalSection(&lock_epw);
        }
        else
        {
            if (bHasLocked)
            {
                LeaveCriticalSection(&lock_epw);
            }
        }
    }
    return MulDiv(nNumber, nNumerator, nDenominator);
}

DWORD epw_cbTemperature = 0;
DWORD epw_cbUnit = 0;
DWORD epw_cbCondition = 0;
DWORD epw_cbImage = 0;
WCHAR* epw_wszTemperature = NULL;
WCHAR* epw_wszUnit = NULL;
WCHAR* epw_wszCondition = NULL;
char* epw_pImage = NULL;
HRESULT (STDAPICALLTYPE *PeopleBand_DrawTextWithGlowFunc)(
    HDC hdc,
    LPCWSTR pszText,
    UINT cch,
    LPRECT prc,
    DWORD dwFlags,
    COLORREF crText,
    COLORREF crGlow,
    UINT nGlowRadius,
    UINT nGlowIntensity,
    BOOL fPreMultiply,
    DTT_CALLBACK_PROC pfnDrawTextCallback,
    LPARAM lParam);
__declspec(dllexport) HRESULT STDAPICALLTYPE PeopleBand_DrawTextWithGlowHook(
    HDC hdc,
    LPCWSTR pszText,
    UINT cch,
    LPRECT prc,
    DWORD dwFlags,
    COLORREF crText,
    COLORREF crGlow,
    UINT nGlowRadius,
    UINT nGlowIntensity,
    BOOL fPreMultiply,
    DTT_CALLBACK_PROC pfnDrawTextCallback,
    LPARAM lParam)
{
    BOOL bHasLocked = FALSE;
    if (cch == 1 && pszText[0] == L'\uE716' && dwFlags == (DT_CENTER | DT_SINGLELINE) && (bHasLocked = TryEnterCriticalSection(&lock_epw)) && epw)
    {
        people_has_ellipsed = FALSE;

        BOOL bUseCachedData = InSendMessage();
        BOOL bIsThemeActive = TRUE;
        if (!IsThemeActive() || IsHighContrast())
        {
            bIsThemeActive = FALSE;
        }
        HRESULT hr = S_OK;
        if (bUseCachedData ? TRUE : SUCCEEDED(hr = epw->lpVtbl->LockData(epw)))
        {
            UINT dpiX = 0, dpiY = 0;
            HRESULT hr = GetDpiForMonitor(MonitorFromWindow(PeopleButton_LastHWND, MONITOR_DEFAULTTOPRIMARY), MDT_DEFAULT, &dpiX, &dpiY);
            BOOL bShouldUnlockData = TRUE;
            DWORD cbTemperature = 0;
            DWORD cbUnit = 0;
            DWORD cbCondition = 0;
            DWORD cbImage = 0;
            BOOL bEmptyData = FALSE;
            if (bUseCachedData ? TRUE : SUCCEEDED(hr = epw->lpVtbl->GetDataSizes(epw, &cbTemperature, &cbUnit, &cbCondition, &cbImage)))
            {
                if (cbTemperature && cbUnit && cbCondition && cbImage)
                {
                    epw_cbTemperature = cbTemperature;
                    epw_cbUnit = cbUnit;
                    epw_cbCondition = cbCondition;
                    epw_cbImage = cbImage;
                }
                else
                {
                    if (!bUseCachedData)
                    {
                        bEmptyData = TRUE;
                        if (bShouldUnlockData)
                        {
                            epw->lpVtbl->UnlockData(epw);
                            bShouldUnlockData = FALSE;
                        }
                    }
                    else
                    {
                        bEmptyData = !epw_wszTemperature || !epw_wszUnit || !epw_wszCondition;
                    }
                    bUseCachedData = TRUE;
                }
                if (!bUseCachedData)
                {
                    if (epw_wszTemperature)
                    {
                        free(epw_wszTemperature);
                    }
                    epw_wszTemperature = calloc(1, epw_cbTemperature);
                    if (epw_wszUnit)
                    {
                        free(epw_wszUnit);
                    }
                    epw_wszUnit = calloc(1, epw_cbUnit);
                    if (epw_wszCondition)
                    {
                        free(epw_wszCondition);
                    }
                    epw_wszCondition = calloc(1, epw_cbCondition);
                    if (epw_pImage)
                    {
                        free(epw_pImage);
                    }
                    epw_pImage = calloc(1, epw_cbImage);
                }
                if (bUseCachedData ? TRUE : SUCCEEDED(hr = epw->lpVtbl->GetData(epw, epw_cbTemperature, epw_wszTemperature, epw_cbUnit, epw_wszUnit, epw_cbCondition, epw_wszCondition, epw_cbImage, epw_pImage)))
                {
                    if (!bUseCachedData)
                    {
                        WCHAR wszBuffer[MAX_PATH];
                        ZeroMemory(wszBuffer, sizeof(WCHAR) * MAX_PATH);
                        swprintf_s(wszBuffer, MAX_PATH, L"%s %s, %s, ", epw_wszTemperature, epw_wszUnit, epw_wszCondition);
                        int len = wcslen(wszBuffer);
                        epw->lpVtbl->GetTitle(epw, sizeof(WCHAR) * (MAX_PATH - len), wszBuffer + len, dwWeatherViewMode);
                        SetWindowTextW(PeopleButton_LastHWND, wszBuffer);

                        epw->lpVtbl->UnlockData(epw);
                        bShouldUnlockData = FALSE;
                    }

                    LOGFONTW logFont;
                    ZeroMemory(&logFont, sizeof(logFont));
                    LOGFONTW logFont2;
                    ZeroMemory(&logFont2, sizeof(logFont2));
                    NONCLIENTMETRICS ncm;
                    ZeroMemory(&ncm, sizeof(NONCLIENTMETRICS));
                    ncm.cbSize = sizeof(NONCLIENTMETRICS);
                    SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0, dpiX);
                    logFont = ncm.lfCaptionFont;
                    logFont.lfWeight = FW_NORMAL;
                    logFont2 = ncm.lfCaptionFont;
                    logFont2.lfWeight = FW_NORMAL;
                    logFont2.lfHeight += 1;
                    if (bEmptyData)
                    {
                        if (!dwTaskbarSmallIcons)
                        {
                            logFont.lfHeight *= 1.6;
                        }
                    }
                    else
                    {
                        if (dwWeatherViewMode == EP_WEATHER_VIEW_ICONTEXT)
                        {
                            //logFont.lfHeight = -12 * (dpiX / 96.0);
                        }
                    }
                    HFONT hFont = CreateFontIndirectW(&logFont);
                    HFONT hFont2 = CreateFontIndirectW(&logFont2);
                    if (hFont)
                    {
                        HDC hDC = CreateCompatibleDC(0);
                        if (hDC)
                        {
                            COLORREF rgbColor = RGB(0, 0, 0);
                            if (bIsThemeActive)
                            {
                                if ((global_rovi.dwBuildNumber < 18985) || (ShouldSystemUseDarkMode && ShouldSystemUseDarkMode()))
                                {
                                    rgbColor = RGB(255, 255, 255);
                                }
                            }
                            else
                            {
                                rgbColor = GetSysColor(COLOR_BTNTEXT);
                            }
                            HFONT hOldFont = SelectFont(hDC, hFont);
                            if (bEmptyData)
                            {
                                RECT rcText;
                                SetRect(&rcText, 0, 0, prc->right, prc->bottom);
                                SIZE size;
                                size.cx = rcText.right - rcText.left;
                                size.cy = rcText.bottom - rcText.top;
                                DWORD dwTextFlags = DT_SINGLELINE | DT_VCENTER | DT_HIDEPREFIX | DT_CENTER;
                                HBITMAP hBitmap = sws_WindowHelpers_CreateAlphaTextBitmap(L"\U0001f4f0", hFont, dwTextFlags, size, rgbColor);
                                if (hBitmap)
                                {
                                    HBITMAP hOldBMP = SelectBitmap(hDC, hBitmap);
                                    BITMAP BMInf;
                                    GetObjectW(hBitmap, sizeof(BITMAP), &BMInf);

                                    BLENDFUNCTION bf;
                                    bf.BlendOp = AC_SRC_OVER;
                                    bf.BlendFlags = 0;
                                    bf.SourceConstantAlpha = 0xFF;
                                    bf.AlphaFormat = AC_SRC_ALPHA;
                                    GdiAlphaBlend(hdc, 0, 0, BMInf.bmWidth, BMInf.bmHeight, hDC, 0, 0, BMInf.bmWidth, BMInf.bmHeight, bf);

                                    SelectBitmap(hDC, hOldBMP);
                                    DeleteBitmap(hBitmap);
                                }
                            }
                            else
                            {
                                DWORD dwWeatherSplit = (dwWeatherContentsMode && (dwWeatherViewMode == EP_WEATHER_VIEW_ICONTEXT || dwWeatherViewMode == EP_WEATHER_VIEW_TEXTONLY) && !dwTaskbarSmallIcons);

                                DWORD dwTextFlags = DT_SINGLELINE | DT_HIDEPREFIX;

                                WCHAR wszText1[MAX_PATH];
                                swprintf_s(wszText1, MAX_PATH, L"%s%s %s", bIsThemeActive ? L"" : L" ", epw_wszTemperature, dwWeatherTemperatureUnit == EP_WEATHER_TUNIT_FAHRENHEIT ? L"\u00B0F" : L"\u00B0C");// epw_wszUnit);
                                RECT rcText1;
                                SetRect(&rcText1, 0, 0, prc->right, dwWeatherSplit ? (prc->bottom / 2) : prc->bottom);
                                DrawTextW(hDC, wszText1, -1, &rcText1, dwTextFlags | DT_CALCRECT | (dwWeatherSplit ? DT_BOTTOM : DT_VCENTER));
                                rcText1.bottom = dwWeatherSplit ? (prc->bottom / 2) : prc->bottom;
                                WCHAR wszText2[MAX_PATH];
                                swprintf_s(wszText2, MAX_PATH, L"%s%s", bIsThemeActive ? L"" : L" ", epw_wszCondition);
                                RECT rcText2;
                                SetRect(&rcText2, 0, 0, prc->right, dwWeatherSplit ? (prc->bottom / 2) : prc->bottom);
                                DrawTextW(hDC, wszText2, -1, &rcText2, dwTextFlags | DT_CALCRECT | (dwWeatherSplit ? DT_TOP : DT_VCENTER));
                                rcText2.bottom = dwWeatherSplit ? (prc->bottom / 2) : prc->bottom;

                                if (bWeatherFixedSize)
                                {
                                    dwTextFlags |= DT_END_ELLIPSIS;
                                }

                                int addend = 0;
                                //int rt = MulDiv(48, a4->bottom, 60);
                                int rt = sqrt(epw_cbImage / 4);
                                int p = 0;// MulDiv(rt, 4, 64);
                                int margin_h = MulDiv(12, dpiX, 144);

                                BOOL bIsIconMode = (
                                    dwWeatherViewMode == EP_WEATHER_VIEW_ICONTEMP ||
                                    dwWeatherViewMode == EP_WEATHER_VIEW_ICONTEXT ||
                                    dwWeatherViewMode == EP_WEATHER_VIEW_ICONONLY);
                                switch (dwWeatherViewMode)
                                {
                                case EP_WEATHER_VIEW_ICONTEXT:
                                case EP_WEATHER_VIEW_TEXTONLY:
                                    if (dwWeatherSplit)
                                        addend = MAX((rcText1.right - rcText1.left), (rcText2.right - rcText2.left)) + margin_h;
                                    else
                                        addend = (rcText1.right - rcText1.left) + margin_h + (rcText2.right - rcText2.left) + margin_h;
                                    break;
                                case EP_WEATHER_VIEW_ICONTEMP:
                                case EP_WEATHER_VIEW_TEMPONLY:
                                    addend = (rcText1.right - rcText1.left) + margin_h;
                                    break;
                                case EP_WEATHER_VIEW_ICONONLY:
                                    addend = 0;
                                    break;
                                }
                                int margin_v = (prc->bottom - rt) / 2;
                                int total_h = (bIsIconMode ? ((margin_h - p) + rt + (margin_h - p)) : margin_h) + addend;
                                if (bWeatherFixedSize == 1)
                                {
                                    if (total_h > prc->right)
                                    {
                                        int diff = total_h - prc->right;
                                        rcText2.right -= diff - 2;
                                        people_has_ellipsed = TRUE;
                                        switch (dwWeatherViewMode)
                                        {
                                        case EP_WEATHER_VIEW_ICONTEXT:
                                        case EP_WEATHER_VIEW_TEXTONLY:
                                            if (dwWeatherSplit)
                                                addend = MAX((rcText1.right - rcText1.left), (rcText2.right - rcText2.left)) + margin_h;
                                            else
                                                addend = (rcText1.right - rcText1.left) + margin_h + (rcText2.right - rcText2.left) + margin_h;
                                            break;
                                        case EP_WEATHER_VIEW_ICONTEMP:
                                        case EP_WEATHER_VIEW_TEMPONLY: // should be impossible
                                            addend = (rcText1.right - rcText1.left) + margin_h;
                                            break;
                                        case EP_WEATHER_VIEW_ICONONLY:
                                            addend = 0;
                                            break;
                                        }
                                        total_h = (margin_h - p) + rt + (margin_h - p) + addend;
                                    }
                                }
                                int start_x = 0; // prev_total_h - total_h;
                                if (bWeatherFixedSize == 1)
                                {
                                    start_x = (prc->right - total_h) / 2;
                                }
                                if (bWeatherFixedSize == 2 && (total_h > MulDiv(192, dpiX, 96)))
                                {
                                    int diff = total_h - MulDiv(WEATHER_FIXEDSIZE2_MAXWIDTH, dpiX, 96);
                                    rcText2.right -= diff - 2;
                                    total_h = MulDiv(WEATHER_FIXEDSIZE2_MAXWIDTH, dpiX, 96);
                                    people_has_ellipsed = TRUE;
                                }

                                HBITMAP hBitmap = NULL, hOldBitmap = NULL;
                                void* pvBits = NULL;
                                SIZE size;

                                if (bIsIconMode)
                                {
                                    BITMAPINFOHEADER BMIH;
                                    ZeroMemory(&BMIH, sizeof(BITMAPINFOHEADER));
                                    BMIH.biSize = sizeof(BITMAPINFOHEADER);
                                    BMIH.biWidth = rt;
                                    BMIH.biHeight = -rt;
                                    BMIH.biPlanes = 1;
                                    BMIH.biBitCount = 32;
                                    BMIH.biCompression = BI_RGB;
                                    hBitmap = CreateDIBSection(hDC, &BMIH, 0, &pvBits, NULL, 0);
                                    if (hBitmap)
                                    {
                                        memcpy(pvBits, epw_pImage, epw_cbImage);
                                        hOldBitmap = SelectBitmap(hDC, hBitmap);

                                        BLENDFUNCTION bf;
                                        bf.BlendOp = AC_SRC_OVER;
                                        bf.BlendFlags = 0;
                                        bf.SourceConstantAlpha = 0xFF;
                                        bf.AlphaFormat = AC_SRC_ALPHA;
                                        GdiAlphaBlend(hdc, start_x + (margin_h - p), margin_v, rt, rt, hDC, 0, 0, rt, rt, bf);

                                        SelectBitmap(hDC, hOldBitmap);
                                        DeleteBitmap(hBitmap);
                                    }
                                }

                                if (dwWeatherViewMode == EP_WEATHER_VIEW_ICONTEMP || 
                                    dwWeatherViewMode == EP_WEATHER_VIEW_ICONTEXT ||
                                    dwWeatherViewMode == EP_WEATHER_VIEW_TEMPONLY ||
                                    dwWeatherViewMode == EP_WEATHER_VIEW_TEXTONLY
                                    )
                                {
                                    size.cx = rcText1.right - rcText1.left;
                                    size.cy = rcText1.bottom - rcText1.top;
                                    hBitmap = sws_WindowHelpers_CreateAlphaTextBitmap(wszText1, hFont, dwTextFlags | (dwWeatherSplit ? DT_BOTTOM : DT_VCENTER), size, rgbColor);
                                    if (hBitmap)
                                    {
                                        HBITMAP hOldBMP = SelectBitmap(hDC, hBitmap);
                                        BITMAP BMInf;
                                        GetObjectW(hBitmap, sizeof(BITMAP), &BMInf);

                                        BLENDFUNCTION bf;
                                        bf.BlendOp = AC_SRC_OVER;
                                        bf.BlendFlags = 0;
                                        bf.SourceConstantAlpha = 0xFF;
                                        bf.AlphaFormat = AC_SRC_ALPHA;
                                        GdiAlphaBlend(hdc, start_x + (bIsIconMode ? ((margin_h - p) + rt + (margin_h - p)) : margin_h), 0, BMInf.bmWidth, BMInf.bmHeight, hDC, 0, 0, BMInf.bmWidth, BMInf.bmHeight, bf);

                                        SelectBitmap(hDC, hOldBMP);
                                        DeleteBitmap(hBitmap);
                                    }
                                }

                                if (dwWeatherViewMode == EP_WEATHER_VIEW_ICONTEXT ||
                                    dwWeatherViewMode == EP_WEATHER_VIEW_TEXTONLY
                                    )
                                {
                                    size.cx = rcText2.right - rcText2.left;
                                    size.cy = rcText2.bottom - rcText2.top;
                                    hBitmap = sws_WindowHelpers_CreateAlphaTextBitmap(wszText2, (dwWeatherSplit && hFont2 ? hFont2 : hFont), dwTextFlags | (dwWeatherSplit ? DT_TOP : DT_VCENTER), size, rgbColor);
                                    if (hBitmap)
                                    {
                                        HBITMAP hOldBMP = SelectBitmap(hDC, hBitmap);
                                        BITMAP BMInf;
                                        GetObjectW(hBitmap, sizeof(BITMAP), &BMInf);

                                        BLENDFUNCTION bf;
                                        bf.BlendOp = AC_SRC_OVER;
                                        bf.BlendFlags = 0;
                                        bf.SourceConstantAlpha = 0xFF;
                                        bf.AlphaFormat = AC_SRC_ALPHA;
                                        GdiAlphaBlend(hdc, start_x + (bIsIconMode ? ((margin_h - p) + rt + (margin_h - p)) : margin_h) + (dwWeatherSplit ? -1 : (rcText1.right - rcText1.left) + margin_h), dwWeatherSplit ? (prc->bottom / 2 - 1) : 0, BMInf.bmWidth, BMInf.bmHeight, hDC, 0, 0, BMInf.bmWidth, BMInf.bmHeight, bf);

                                        SelectBitmap(hDC, hOldBMP);
                                        DeleteBitmap(hBitmap);
                                    }
                                }

                                if (bWeatherFixedSize == 1)
                                {

                                }
                                else
                                {
                                    if (total_h != prev_total_h)
                                    {
                                        prev_total_h = total_h;
                                        SendNotifyMessageW(HWND_BROADCAST, WM_WININICHANGE, 0, (LPARAM)L"TraySettings");
                                    }
                                }

                                /*
                                SetLastError(0);
                                LONG_PTR oldStyle = GetWindowLongPtrW(PeopleButton_LastHWND, GWL_EXSTYLE);
                                if (!GetLastError())
                                {
                                    LONG_PTR style;
                                    if (bIsThemeActive)
                                    {
                                        style = oldStyle & ~WS_EX_DLGMODALFRAME;
                                    }
                                    else
                                    {
                                        style = oldStyle | WS_EX_DLGMODALFRAME;
                                    }
                                    if (style != oldStyle)
                                    {
                                        SetWindowLongPtrW(PeopleButton_LastHWND, GWL_EXSTYLE, style);
                                    }
                                }
                                */
                            }
                            SelectFont(hDC, hOldFont);
                            DeleteDC(hDC);
                        }
                        DeleteFont(hFont);
                    }
                    if (hFont2)
                    {
                        DeleteFont(hFont2);
                    }
                    if (IsWindowVisible(hWndWeatherFlyout))
                    {
                        RecomputeWeatherFlyoutLocation(hWndWeatherFlyout);
                    }
                }
                /*free(epw_pImage);
                free(epw_wszCondition);
                free(epw_wszUnit);
                free(epw_wszTemperature);*/
            }
            if (!bUseCachedData && bShouldUnlockData)
            {
                epw->lpVtbl->UnlockData(epw);
            }
        }
        else
        {
            //printf("444444444444 0x%x\n", hr);
            if (hr == 0x800706ba) // RPC server is unavailable
            {
                //ReleaseSRWLockShared(&lock_epw);
                /*AcquireSRWLockExclusive(&lock_epw);
                epw = NULL;
                prev_total_h = 0;
                InvalidateRect(PeopleButton_LastHWND, NULL, TRUE); 
                ReleaseSRWLockExclusive(&lock_epw);*/
                if (hServiceWindowThread) PostThreadMessageW(GetThreadId(hServiceWindowThread), WM_USER + 1, NULL, NULL);
                //AcquireSRWLockShared(&lock_epw);
            }
        }

        //printf("hr %x\n", hr);

        LeaveCriticalSection(&lock_epw);
        return S_OK;
    }
    else
    {
        if (bHasLocked)
        {
            LeaveCriticalSection(&lock_epw);
        }
        return PeopleBand_DrawTextWithGlowFunc(hdc, pszText, cch, prc, dwFlags, crText, crGlow, nGlowRadius, nGlowIntensity, fPreMultiply, pfnDrawTextCallback, lParam);
    }
}

void(*PeopleButton_ShowTooltipFunc)(__int64 a1, unsigned __int8 bShow) = 0;
void WINAPI PeopleButton_ShowTooltipHook(__int64 _this, unsigned __int8 bShow)
{
    BOOL bHasLocked = TryEnterCriticalSection(&lock_epw);
    if (bHasLocked && epw)
    {
        if (bShow)
        {
            HRESULT hr = epw->lpVtbl->LockData(epw);
            if (SUCCEEDED(hr))
            {
                WCHAR wszBuffer[MAX_PATH];
                ZeroMemory(wszBuffer, sizeof(WCHAR) * MAX_PATH);
                DWORD mode = dwWeatherViewMode;
                if (bWeatherFixedSize && people_has_ellipsed)
                {
                    mode = EP_WEATHER_VIEW_ICONTEMP;
                }
                epw->lpVtbl->GetTitle(epw, sizeof(WCHAR) * MAX_PATH, wszBuffer, mode);
                if (wcsstr(wszBuffer, L"(null)"))
                {
                    HMODULE hModule = GetModuleHandleW(L"pnidui.dll");
                    if (hModule)
                    {
                        LoadStringW(hModule, 35, wszBuffer, MAX_PATH);
                    }
                }
                TTTOOLINFOW ti;
                ZeroMemory(&ti, sizeof(TTTOOLINFOW));
                ti.cbSize = sizeof(TTTOOLINFOW);
                ti.hwnd = *((INT64*)_this + 1);
                ti.uId = *((INT64*)_this + 1);
                ti.lpszText = wszBuffer;
                SendMessageW((HWND) * ((INT64*)_this + 10), TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
                epw->lpVtbl->UnlockData(epw);
            }
        }
        LeaveCriticalSection(&lock_epw);
    }
    else
    {
        if (bHasLocked)
        {
            LeaveCriticalSection(&lock_epw);
        }
        WCHAR wszBuffer[MAX_PATH];
        ZeroMemory(wszBuffer, sizeof(WCHAR) * MAX_PATH);
        LoadStringW(GetModuleHandle(NULL), 912, wszBuffer, MAX_PATH);
        if (wszBuffer[0])
        {
            TTTOOLINFOW ti;
            ZeroMemory(&ti, sizeof(TTTOOLINFOW));
            ti.cbSize = sizeof(TTTOOLINFOW);
            ti.hwnd = *((INT64*)_this + 1);
            ti.uId = *((INT64*)_this + 1);
            ti.lpszText = wszBuffer;
            SendMessageW((HWND) * ((INT64*)_this + 10), TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
        }
    }
    if (PeopleButton_ShowTooltipFunc)
    {
        return PeopleButton_ShowTooltipFunc(_this, bShow);
    }
    return 0;
}

__int64 (*PeopleButton_OnClickFunc)(__int64 a1, __int64 a2) = 0;
__int64 PeopleButton_OnClickHook(__int64 a1, __int64 a2)
{
    BOOL bHasLocked = TryEnterCriticalSection(&lock_epw);
    if (bHasLocked && epw)
    {
        if (!hWndWeatherFlyout)
        {
            epw->lpVtbl->GetWindowHandle(epw, &hWndWeatherFlyout);
        }
        if (hWndWeatherFlyout)
        {
            if (IsWindowVisible(hWndWeatherFlyout))
            {
                if (GetForegroundWindow() != hWndWeatherFlyout)
                {
                    SwitchToThisWindow(hWndWeatherFlyout, TRUE);
                }
                else
                {
                    epw->lpVtbl->Hide(epw);
                    //printf("HR %x\n", PostMessageW(hWnd, EP_WEATHER_WM_FETCH_DATA, 0, 0));
                }
            }
            else
            {
                RecomputeWeatherFlyoutLocation(hWndWeatherFlyout);

                epw->lpVtbl->Show(epw);

                SwitchToThisWindow(hWndWeatherFlyout, TRUE);
            }
        }
        LeaveCriticalSection(&lock_epw);
        return 0;
    }
    else
    {
        if (bHasLocked)
        {
            LeaveCriticalSection(&lock_epw);
        }
        if (PeopleButton_OnClickFunc)
        {
            return PeopleButton_OnClickFunc(a1, a2);
        }
        return 0;
    }
}

INT64 PeopleButton_SubclassProc(
    _In_ HWND   hWnd,
    _In_ UINT   uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    UINT_PTR    uIdSubclass,
    DWORD_PTR   dwRefData
)
{
    if (uMsg == WM_NCDESTROY)
    {
        RemoveWindowSubclass(hWnd, PeopleButton_SubclassProc, PeopleButton_SubclassProc);
        /*AcquireSRWLockExclusive(&lock_epw);
        if (epw)
        {
            epw->lpVtbl->Release(epw);
            epw = NULL;
            PeopleButton_LastHWND = NULL;
            prev_total_h = 0;
        }
        ReleaseSRWLockExclusive(&lock_epw);*/
        if (hServiceWindowThread) PostThreadMessageW(GetThreadId(hServiceWindowThread), WM_USER + 1, NULL, NULL);
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

static BOOL(*SetChildWindowNoActivateFunc)(HWND);
__declspec(dllexport) BOOL explorer_SetChildWindowNoActivateHook(HWND hWnd)
{
    TCHAR className[100];
    ZeroMemory(className, 100);
    GetClassNameW(hWnd, className, 100);
    if (!wcscmp(className, L"ControlCenterButton"))
    {
        if (bOldTaskbar < 2)
        {
            lpShouldDisplayCCButton = (BYTE*)(GetWindowLongPtrW(hWnd, 0) + 120);
            if (*lpShouldDisplayCCButton)
            {
                *lpShouldDisplayCCButton = !bHideControlCenterButton;
            }
        }
    }
    // get a look at vtable by searching for v_IsEnabled
    if (!wcscmp(className, L"TrayButton"))
    {
        uintptr_t Instance = *(uintptr_t*)GetWindowLongPtrW(hWnd, 0);
        if (Instance)
        {
            uintptr_t TrayButton_GetComponentName = *(INT_PTR(WINAPI**)())(Instance + 304); // 280 in versions of Windows 10 where this method exists
            wchar_t* wszComponentName = NULL;
            if (IsWindows11() && !IsBadCodePtr(TrayButton_GetComponentName))
            {
                wszComponentName = (const WCHAR*)(*(uintptr_t(**)(void))(Instance + 304))();
            }
            else
            {
                WCHAR title[MAX_PATH];
                GetWindowTextW(hWnd, title, MAX_PATH);
                WCHAR pbtitle[MAX_PATH];
                HMODULE hPeopleBand = LoadLibraryExW(L"PeopleBand.dll", NULL, LOAD_LIBRARY_AS_DATAFILE);
                if (hPeopleBand)
                {
                    LoadStringW(hPeopleBand, 256, pbtitle, 260);
                    FreeLibrary(hPeopleBand);
                }
                if (!wcscmp(pbtitle, title))
                {
                    wszComponentName = L"PeopleButton";
                }
            }
            if (wszComponentName)
            {
                if (!wcscmp(wszComponentName, L"CortanaButton"))
                {
                    if (bOldTaskbar < 2)
                    {
                        DWORD dwOldProtect;
                        VirtualProtect(Instance + 160, sizeof(uintptr_t), PAGE_READWRITE, &dwOldProtect);
                        if (!Widgets_OnClickFunc) Widgets_OnClickFunc = *(uintptr_t*)(Instance + 160);
                        *(uintptr_t*)(Instance + 160) = Widgets_OnClickHook;    // OnClick
                        VirtualProtect(Instance + 160, sizeof(uintptr_t), dwOldProtect, &dwOldProtect);
                        VirtualProtect(Instance + 216, sizeof(uintptr_t), PAGE_READWRITE, &dwOldProtect);
                        if (!Widgets_GetTooltipTextFunc) Widgets_GetTooltipTextFunc = *(uintptr_t*)(Instance + 216);
                        *(uintptr_t*)(Instance + 216) = Widgets_GetTooltipTextHook; // OnTooltipShow
                        VirtualProtect(Instance + 216, sizeof(uintptr_t), dwOldProtect, &dwOldProtect);
                    }
                }
                else if (!wcscmp(wszComponentName, L"MultitaskingButton"))
                {
                    if (bOldTaskbar < 2)
                    {
                        DWORD dwOldProtect;
                        VirtualProtect(Instance + 160, sizeof(uintptr_t), PAGE_READWRITE, &dwOldProtect);
                        *(uintptr_t*)(Instance + 160) = ToggleTaskView;    // OnClick
                        VirtualProtect(Instance + 160, sizeof(uintptr_t), dwOldProtect, &dwOldProtect);
                    }
                }
                else if (!wcscmp(wszComponentName, L"PeopleButton"))
                {
                    DWORD dwOldProtect;

                    uintptr_t PeopleButton_Instance = *((uintptr_t*)GetWindowLongPtrW(hWnd, 0) + 17);

                    VirtualProtect(PeopleButton_Instance + 32, sizeof(uintptr_t), PAGE_READWRITE, &dwOldProtect);
                    if (!PeopleButton_CalculateMinimumSizeFunc) PeopleButton_CalculateMinimumSizeFunc = *(uintptr_t*)(PeopleButton_Instance + 32);
                    *(uintptr_t*)(PeopleButton_Instance + 32) = PeopleButton_CalculateMinimumSizeHook; // CalculateMinimumSize
                    VirtualProtect(PeopleButton_Instance + 32, sizeof(uintptr_t), dwOldProtect, &dwOldProtect);

                    uintptr_t off_PeopleButton_ShowTooltip = 0;
                    if (bOldTaskbar >= 2 || IsWindows11())
                    {
                        off_PeopleButton_ShowTooltip = 224;
                    }
                    else
                    {
                        off_PeopleButton_ShowTooltip = 200;
                    }
                    VirtualProtect(Instance + off_PeopleButton_ShowTooltip, sizeof(uintptr_t), PAGE_READWRITE, &dwOldProtect);
                    if (!PeopleButton_ShowTooltipFunc) PeopleButton_ShowTooltipFunc = *(uintptr_t*)(Instance + off_PeopleButton_ShowTooltip);
                    *(uintptr_t*)(Instance + off_PeopleButton_ShowTooltip) = PeopleButton_ShowTooltipHook; // OnTooltipShow
                    VirtualProtect(Instance + off_PeopleButton_ShowTooltip, sizeof(uintptr_t), dwOldProtect, &dwOldProtect);

                    uintptr_t off_PeopleButton_OnClick = 0;
                    if (bOldTaskbar >= 2 || IsWindows11())
                    {
                        off_PeopleButton_OnClick = 160;
                    }
                    else
                    {
                        off_PeopleButton_OnClick = 136;
                    }
                    VirtualProtect(Instance + off_PeopleButton_OnClick, sizeof(uintptr_t), PAGE_READWRITE, &dwOldProtect);
                    if (!PeopleButton_OnClickFunc) PeopleButton_OnClickFunc = *(uintptr_t*)(Instance + off_PeopleButton_OnClick);
                    *(uintptr_t*)(Instance + off_PeopleButton_OnClick) = PeopleButton_OnClickHook;    // OnClick
                    VirtualProtect(Instance + off_PeopleButton_OnClick, sizeof(uintptr_t), dwOldProtect, &dwOldProtect);

                    PeopleButton_LastHWND = hWnd;
                    SetWindowSubclass(hWnd, PeopleButton_SubclassProc, PeopleButton_SubclassProc, 0);

                    EnterCriticalSection(&lock_epw);
                    if (!epw)
                    {
                        if (SUCCEEDED(CoCreateInstance(&CLSID_EPWeather, NULL, CLSCTX_LOCAL_SERVER, &IID_IEPWeather, &epw)) && epw)
                        {
                            epw->lpVtbl->SetNotifyWindow(epw, hWnd);

                            WCHAR wszBuffer[MAX_PATH];
                            ZeroMemory(wszBuffer, sizeof(WCHAR) * MAX_PATH);
                            HMODULE hModule = GetModuleHandleW(L"pnidui.dll");
                            if (hModule)
                            {
                                LoadStringW(hModule, 35, wszBuffer, MAX_PATH);
                            }
                            SetWindowTextW(hWnd, wszBuffer);
                        }
                    }
                    LeaveCriticalSection(&lock_epw);
                }
            }
        }
    }
    return SetChildWindowNoActivateFunc(hWnd);
}
#endif
#pragma endregion


#pragma region "Hide Show desktop button"
#if WITH_MAIN_PATCHER
DWORD GetTaskbarSd()
{
    DWORD dwVal = 1, dwSize = sizeof(DWORD);
    if (SHRegGetValueFromHKCUHKLMFunc && SHRegGetValueFromHKCUHKLMFunc(
        TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),
        TEXT("TaskbarSd"),
        SRRF_RT_REG_DWORD,
        NULL,
        &dwVal,
        &dwSize
    ) == ERROR_SUCCESS)
    {
        return dwVal;
    }
    return 1; // Visible
}

INT64 ShowDesktopSubclassProc(
    _In_ HWND   hWnd,
    _In_ UINT   uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam,
    UINT_PTR    uIdSubclass,
    DWORD_PTR   dwRefData
)
{
    switch (uMsg)
    {
        case WM_NCDESTROY:
        {
            RemoveWindowSubclass(hWnd, ShowDesktopSubclassProc, ShowDesktopSubclassProc);
            break;
        }
        case WM_PAINT:
        case WM_PRINTCLIENT:
        {
            HANDLE h_dwTaskbarSd = GetPropW(hWnd, L"EP_TaskbarSd");
            if (h_dwTaskbarSd)
            {
                DWORD dwTaskbarSd = (DWORD)h_dwTaskbarSd - 1;
                if (dwTaskbarSd == 2) // Invisible
                {
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hWnd, &ps);
                    if (hdc)
                    {
                        HDC hdcPaint;
                        HPAINTBUFFER hBufferedPaint = BeginBufferedPaint(hdc, &ps.rcPaint, BPBF_TOPDOWNDIB, NULL, &hdcPaint);
                        if (hBufferedPaint)
                        {
                            if (IsThemeActive())
                            {
                                DrawThemeParentBackground(hWnd, hdcPaint, NULL);
                            }
                            else
                            {
                                RECT rc;
                                GetClientRect(hWnd, &rc);
                                FillRect(hdc, &rc, (HBRUSH)(COLOR_BTNFACE + 1));
                            }
                            EndBufferedPaint(hBufferedPaint, TRUE);
                        }
                        EndPaint(hWnd, &ps);
                    }
                    return 0;
                }
            }
            break;
        }
        case WM_THEMECHANGED:
        case WM_SETTINGCHANGE:
        {
            LRESULT lRes = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            DWORD dwTaskbarSd = GetTaskbarSd();
            SetPropW(hWnd, L"EP_TaskbarSd", (HANDLE)(dwTaskbarSd + 1));
            ShowWindow(hWnd, dwTaskbarSd != 0 ? SW_SHOW : SW_HIDE);
            return lRes;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}
#endif
#pragma endregion


#pragma region "Notify shell ready"
#if WITH_MAIN_PATCHER
DWORD SignalShellReady(DWORD wait)
{
    printf("Started \"Signal shell ready\" thread.\n");
    //UpdateStartMenuPositioning(MAKELPARAM(TRUE, TRUE));

    while (!wait && TRUE)
    {
        HWND hShell_TrayWnd = FindWindowEx(
            NULL,
            NULL,
            L"Shell_TrayWnd",
            NULL
        );
        if (hShell_TrayWnd)
        {
            HWND hWnd = FindWindowEx(
                hShell_TrayWnd,
                NULL,
                L"Start",
                NULL
            );
            if (hWnd)
            {
                if (IsWindowVisible(hWnd))
                {
                    UpdateStartMenuPositioning(MAKELPARAM(TRUE, TRUE));
                    break;
                }
            }
        }
        Sleep(100);
    }

    Sleep(600);

    SetEvent(hCanStartSws);
    if (bOldTaskbar && (global_rovi.dwBuildNumber >= 22567))
    {
        PatchSndvolsso();
    }

    printf("Ended \"Signal shell ready\" thread.\n");
    return 0;
}
#endif
#pragma endregion


#pragma region "Window Switcher"
#if WITH_MAIN_PATCHER
DWORD sws_IsEnabled = FALSE;

void sws_ReadSettings(sws_WindowSwitcher* sws)
{
    HKEY hKey = NULL;
    DWORD dwSize = 0;

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        DWORD val = 0;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("AltTabSettings"),
            0,
            NULL,
            &val,
            &dwSize
        );
        sws_IsEnabled = (val == 2);
        RegCloseKey(hKey);
    }

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        TEXT(REGPATH) L"\\sws",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        if (sws)
        {
            sws_WindowSwitcher_InitializeDefaultSettings(sws);
            sws->dwWallpaperSupport = SWS_WALLPAPERSUPPORT_EXPLORER;
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("IncludeWallpaper"),
                0,
                NULL,
                &(sws->bIncludeWallpaper),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("RowHeight"),
                0,
                NULL,
                &(sws->dwRowHeight),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("MaxWidth"),
                0,
                NULL,
                &(sws->dwMaxWP),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("MaxHeight"),
                0,
                NULL,
                &(sws->dwMaxHP),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("ColorScheme"),
                0,
                NULL,
                &(sws->dwColorScheme),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("Theme"),
                0,
                NULL,
                &(sws->dwTheme),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("CornerPreference"),
                0,
                NULL,
                &(sws->dwCornerPreference),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("ShowDelay"),
                0,
                NULL,
                &(sws->dwShowDelay),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("PrimaryOnly"),
                0,
                NULL,
                &(sws->bPrimaryOnly),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("PerMonitor"),
                0,
                NULL,
                &(sws->bPerMonitor),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("MaxWidthAbs"),
                0,
                NULL,
                &(sws->dwMaxAbsoluteWP),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("MaxHeightAbs"),
                0,
                NULL,
                &(sws->dwMaxAbsoluteHP),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("NoPerApplicationList"),
                0,
                NULL,
                &(sws->bNoPerApplicationList),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("MasterPadding"),
                0,
                NULL,
                &(sws->dwMasterPadding),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("SwitcherIsPerApplication"),
                0,
                NULL,
                &(sws->bSwitcherIsPerApplication),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("AlwaysUseWindowTitleAndIcon"),
                0,
                NULL,
                &(sws->bAlwaysUseWindowTitleAndIcon),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("ScrollWheelBehavior"),
                0,
                NULL,
                &(sws->dwScrollWheelBehavior),
                &dwSize
            );
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("ScrollWheelInvert"),
                0,
                NULL,
                &(sws->bScrollWheelInvert),
                &dwSize
            );
            if (sws->bIsInitialized)
            {
                sws_WindowSwitcher_UnregisterHotkeys(sws);
                sws_WindowSwitcher_RegisterHotkeys(sws, NULL);
                sws_WindowSwitcher_RefreshTheme(sws);
            }
        }
        RegCloseKey(hKey);
    }
}

DWORD WindowSwitcher(DWORD unused)
{
    if (IsWindows11())
    {
        WaitForSingleObject(hCanStartSws, INFINITE);
    }
    if (!bOldTaskbar)
    {
        WaitForSingleObject(hWin11AltTabInitialized, INFINITE);
    }
    Sleep(1000);

    while (TRUE)
    {
        //Sleep(5000);
        while (!FindWindowExW(
            NULL,
            NULL,
            L"Shell_TrayWnd",
            NULL
        ))
        {
            printf("[sws] Waiting for taskbar...\n");
            Sleep(100);
        }
        Sleep(100);
        sws_ReadSettings(NULL);
        if (sws_IsEnabled)
        {
            sws_error_t err;
            sws_WindowSwitcher* sws = calloc(1, sizeof(sws_WindowSwitcher));
            if (!sws)
            {
                return 0;
            }
            sws_ReadSettings(sws);
            err = sws_error_Report(sws_error_GetFromInternalError(sws_WindowSwitcher_Initialize(&sws, FALSE)), NULL);
            if (err == SWS_ERROR_SUCCESS)
            {
                sws_WindowSwitcher_RefreshTheme(sws);
                HANDLE hEvents[3];
                hEvents[0] = sws->hEvExit;
                hEvents[1] = hSwsSettingsChanged;
                hEvents[2] = hSwsOpacityMaybeChanged;
                while (TRUE)
                {
                    DWORD dwRes = MsgWaitForMultipleObjectsEx(
                        3,
                        hEvents,
                        INFINITE,
                        QS_ALLINPUT,
                        MWMO_INPUTAVAILABLE
                    );
                    if (dwRes == WAIT_OBJECT_0 + 0)
                    {
                        break;
                    }
                    if (dwRes == WAIT_OBJECT_0 + 1)
                    {
                        sws_ReadSettings(sws);
                        if (!sws_IsEnabled)
                        {
                            break;
                        }
                    }
                    else if (dwRes == WAIT_OBJECT_0 + 2)
                    {
                        sws_WindowSwitcher_RefreshTheme(sws);
                    }
                    else if (dwRes == WAIT_OBJECT_0 + 3)
                    {
                        MSG msg;
                        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                            TranslateMessage(&msg);
                            DispatchMessage(&msg);
                        }
                    }
                    else
                    {
                        break;
                    }
                }
                sws_WindowSwitcher_Clear(sws);
                free(sws);
            }
            else
            {
                free(sws);
                return 0;
            }
        }
        else
        {
            WaitForSingleObject(
                hSwsSettingsChanged,
                INFINITE
            );
        }
    }
}
#endif
#pragma endregion


#pragma region "Load Settings from registry"
#define REFRESHUI_NONE         0b000000
#define REFRESHUI_GLOM         0b000001
#define REFRESHUI_ORB          0b000010
#define REFRESHUI_PEOPLE       0b000100
#define REFRESHUI_TASKBAR      0b001000
#define REFRESHUI_CENTER       0b010000
#define REFRESHUI_SPOTLIGHT    0b100000
void WINAPI LoadSettings(LPARAM lParam)
{
    BOOL bIsExplorer = LOWORD(lParam);
    BOOL bIsRefreshAllowed = HIWORD(lParam);
    DWORD dwRefreshUIMask = REFRESHUI_NONE;

    HKEY hKey = NULL;
    DWORD dwSize = 0, dwTemp = 0;

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        TEXT(REGPATH),
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_ALL_ACCESS | KEY_WOW64_64KEY,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
#if WITH_MAIN_PATCHER
        dwSize = sizeof(DWORD);
        dwTemp = 0;
        RegQueryValueExW(
            hKey,
            TEXT("MigratedFromOldSettings"),
            0,
            NULL,
            &dwTemp,
            &dwSize
        );
        if (!dwTemp)
        {
            HKEY hOldKey = NULL;
            RegOpenKeyExW(
                HKEY_CURRENT_USER,
                TEXT(REGPATH_OLD),
                REG_OPTION_NON_VOLATILE,
                KEY_ALL_ACCESS | KEY_WOW64_64KEY,
                &hOldKey
            );
            if (hOldKey == NULL || hOldKey == INVALID_HANDLE_VALUE)
            {
                hOldKey = NULL;
            }
            if (hOldKey)
            {
                dwSize = sizeof(DWORD);
                DWORD dw1 = 0;
                RegQueryValueExW(
                    hKey,
                    TEXT("OpenPropertiesAtNextStart"),
                    0,
                    NULL,
                    &dw1,
                    &dwSize
                );
                dwSize = sizeof(DWORD);
                DWORD dw2 = 0;
                RegQueryValueExW(
                    hKey,
                    TEXT("IsUpdatePending"),
                    0,
                    NULL,
                    &dw2,
                    &dwSize
                );
                if (RegCopyTreeW(hOldKey, NULL, hKey) == ERROR_SUCCESS)
                {
                    RegSetValueExW(
                        hKey,
                        TEXT("OpenPropertiesAtNextStart"),
                        0,
                        REG_DWORD,
                        &dw1,
                        sizeof(DWORD)
                    );
                    RegSetValueExW(
                        hKey,
                        TEXT("IsUpdatePending"),
                        0,
                        REG_DWORD,
                        &dw2,
                        sizeof(DWORD)
                    );
                    RegDeleteKeyExW(hKey, TEXT(STARTDOCKED_SB_NAME), KEY_WOW64_64KEY, 0);
                }
            }
            dwTemp = TRUE;
            RegSetValueExW(
                hKey,
                TEXT("MigratedFromOldSettings"),
                0,
                REG_DWORD,
                &dwTemp,
                sizeof(DWORD)
            );
        }
#endif
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("AllocConsole"),
            0,
            NULL,
            &bAllocConsole,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        dwTemp = 0;
        RegQueryValueExW(
            hKey,
            TEXT("Memcheck"),
            0,
            NULL,
            &dwTemp,
            &dwSize
        );
        if (dwTemp)
        {
#if defined(DEBUG) | defined(_DEBUG)
            printf("[Memcheck] Dumping memory leaks...\n");
            _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
            _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
            _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
            _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
            _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
            _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);
            _CrtDumpMemoryLeaks();
            printf("[Memcheck] Memory leak dump complete.\n");
            printf(
                "[Memcheck] Objects in use:\nGDI\tGDIp\tUSER\tUSERp\n%d\t%d\t%d\t%d\n",
                GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS),
                GetGuiResources(GetCurrentProcess(), GR_GDIOBJECTS_PEAK),
                GetGuiResources(GetCurrentProcess(), GR_USEROBJECTS),
                GetGuiResources(GetCurrentProcess(), GR_USEROBJECTS_PEAK)
            );
#endif
            dwTemp = 0;
            RegSetValueExW(
                hKey,
                TEXT("Memcheck"),
                0,
                REG_DWORD,
                &dwTemp,
                sizeof(DWORD)
            );
        }
        dwSize = sizeof(DWORD);
        dwTemp = 0;
        RegQueryValueExW(
            hKey,
            TEXT("OldTaskbarAl"),
            0,
            NULL,
            &dwTemp,
            &dwSize
        );
        if (dwTemp != dwOldTaskbarAl)
        {
            dwOldTaskbarAl = dwTemp;
            dwRefreshUIMask |= REFRESHUI_CENTER;
        }
        dwSize = sizeof(DWORD);
        dwTemp = 0;
        RegQueryValueExW(
            hKey,
            TEXT("MMOldTaskbarAl"),
            0,
            NULL,
            &dwTemp,
            &dwSize
        );
        if (dwTemp != dwMMOldTaskbarAl)
        {
            dwMMOldTaskbarAl = dwTemp;
            dwRefreshUIMask |= REFRESHUI_CENTER;
        }
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("HideExplorerSearchBar"),
            0,
            NULL,
            &bHideExplorerSearchBar,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("ShrinkExplorerAddressBar"),
            0,
            NULL,
            &bShrinkExplorerAddressBar,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("UseClassicDriveGrouping"),
            0,
            NULL,
            &bUseClassicDriveGrouping,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("FileExplorerCommandUI"),
            0,
            NULL,
            &dwFileExplorerCommandUI,
            &dwSize
        );
        if (dwFileExplorerCommandUI == 9999)
        {
            if (IsWindows11())
            {
                DWORD bIsWindows11CommandBarDisabled = (RegGetValueW(HKEY_CURRENT_USER, L"SOFTWARE\\Classes\\CLSID\\{d93ed569-3b3e-4bff-8355-3c44f6a52bb5}\\InProcServer32", L"", RRF_RT_REG_SZ, NULL, NULL, NULL) == ERROR_SUCCESS);
                RegSetValueExW(hKey, L"FileExplorerCommandUI", 0, REG_DWORD, &bIsWindows11CommandBarDisabled, sizeof(DWORD));
                dwFileExplorerCommandUI = bIsWindows11CommandBarDisabled;
            }
            else
            {
                dwFileExplorerCommandUI = 0;
            }
        }
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("LegacyFileTransferDialog"),
            0,
            NULL,
            &bLegacyFileTransferDialog,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("DisableImmersiveContextMenu"),
            0,
            NULL,
            &bDisableImmersiveContextMenu,
            &dwSize
        );
        dwTemp = FALSE;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("ClassicThemeMitigations"),
            0,
            NULL,
            &dwTemp,
            &dwSize
        );
        if (!bWasClassicThemeMitigationsSet)
        {
            bClassicThemeMitigations = dwTemp;
            bWasClassicThemeMitigationsSet = TRUE;
        }
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("SkinMenus"),
            0,
            NULL,
            &bSkinMenus,
            &dwSize
        );
        if (bIsExplorerProcess)
        {
            if (bAllocConsole)
            {
                FILE* conout;
                AllocConsole();
                freopen_s(
                    &conout,
                    "CONOUT$",
                    "w",
                    stdout
                );
            }
            else
            {
                FreeConsole();
            }
        }
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("DoNotRedirectSystemToSettingsApp"),
            0,
            NULL,
            &bDoNotRedirectSystemToSettingsApp,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("DoNotRedirectProgramsAndFeaturesToSettingsApp"),
            0,
            NULL,
            &bDoNotRedirectProgramsAndFeaturesToSettingsApp,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        dwTemp = 0;
        RegQueryValueExW(
            hKey,
            TEXT("MicaEffectOnTitlebar"),
            0,
            NULL,
            &dwTemp,
            &dwSize
        );
        if (dwTemp != bMicaEffectOnTitlebar)
        {
            bMicaEffectOnTitlebar = dwTemp;
            HMODULE hUxtheme = GetModuleHandleW(L"uxtheme.dll");
            if (hUxtheme)
            {
                if (bMicaEffectOnTitlebar)
                {
                    VnPatchDelayIAT(hUxtheme, "dwmapi.dll", "DwmExtendFrameIntoClientArea", uxtheme_DwmExtendFrameIntoClientAreaHook);
                }
                else
                {
                    //VnPatchDelayIAT(hUxtheme, "dwmapi.dll", "DwmExtendFrameIntoClientArea", DwmExtendFrameIntoClientArea);
                }
            }
        }
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("HideIconAndTitleInExplorer"),
            0,
            NULL,
            &bHideIconAndTitleInExplorer,
            &dwSize
        );
        if (!bIsExplorer)
        {
            RegCloseKey(hKey);
            return;
        }
        dwTemp = TRUE;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("OldTaskbar"),
            0,
            NULL,
            &dwTemp,
            &dwSize
        );
        if (!bWasOldTaskbarSet)
        {
            bOldTaskbar = dwTemp;
            AdjustTaskbarStyleValue(&bOldTaskbar);
            bWasOldTaskbarSet = TRUE;
        }
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("HideControlCenterButton"),
            0,
            NULL,
            &bHideControlCenterButton,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("FlyoutMenus"),
            0,
            NULL,
            &bFlyoutMenus,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("CenterMenus"),
            0,
            NULL,
            &bCenterMenus,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("SkinIcons"),
            0,
            NULL,
            &bSkinIcons,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("ReplaceNetwork"),
            0,
            NULL,
            &bReplaceNetwork,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("ArchiveMenu"),
            0,
            NULL,
            &bEnableArchivePlugin,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("ClockFlyoutOnWinC"),
            0,
            NULL,
            &bClockFlyoutOnWinC,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("DisableImmersiveContextMenu"),
            0,
            NULL,
            &bDisableImmersiveContextMenu,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("HookStartMenu"),
            0,
            NULL,
            &bHookStartMenu,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("PropertiesInWinX"),
            0,
            NULL,
            &bPropertiesInWinX,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("NoPropertiesInContextMenu"),
            0,
            NULL,
            &bNoPropertiesInContextMenu,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("NoMenuAccelerator"),
            0,
            NULL,
            &bNoMenuAccelerator,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("IMEStyle"),
            0,
            NULL,
            &dwIMEStyle,
            &dwSize
        );
        if (IsWindows11Version22H2OrHigher())
        {
            if (!dwIMEStyle) dwIMEStyle = 7;
            else if (dwIMEStyle == 7) dwIMEStyle = 0;
        }
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("UpdatePolicy"),
            0,
            NULL,
            &dwUpdatePolicy,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("IsUpdatePending"),
            0,
            NULL,
            &bShowUpdateToast,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("ToolbarSeparators"),
            0,
            NULL,
            &bToolbarSeparators,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("TaskbarAutohideOnDoubleClick"),
            0,
            NULL,
            &bTaskbarAutohideOnDoubleClick,
            &dwSize
        );
        dwTemp = ORB_STYLE_WINDOWS10;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("OrbStyle"),
            0,
            NULL,
            &dwTemp,
            &dwSize
        );
        if (bOldTaskbar && (dwTemp != dwOrbStyle))
        {
            dwOrbStyle = dwTemp;
            dwRefreshUIMask |= REFRESHUI_ORB;
        }
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("EnableSymbolDownload"),
            0,
            NULL,
            &bEnableSymbolDownload,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        dwTemp = 0;
        RegQueryValueExW(
            hKey,
            TEXT("OpenPropertiesAtNextStart"),
            0,
            NULL,
            &dwTemp,
            &dwSize
        );
        if (!IsAppRunningAsAdminMode() && dwTemp)
        {
#if WITH_MAIN_PATCHER
            LaunchPropertiesGUI(hModule);
#endif
        }

        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("DisableAeroSnapQuadrants"),
            0,
            NULL,
            &bDisableAeroSnapQuadrants,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("SnapAssistSettings"),
            0,
            NULL,
            &dwSnapAssistSettings,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("DoNotRedirectDateAndTimeToSettingsApp"),
            0,
            NULL,
            &bDoNotRedirectDateAndTimeToSettingsApp,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("DoNotRedirectNotificationIconsToSettingsApp"),
            0,
            NULL,
            &bDoNotRedirectNotificationIconsToSettingsApp,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("DisableOfficeHotkeys"),
            0,
            NULL,
            &bDisableOfficeHotkeys,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("DisableWinFHotkey"),
            0,
            NULL,
            &bDisableWinFHotkey,
            &dwSize
        );
        dwTemp = FALSE;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("SpotlightDisableIcon"),
            0,
            NULL,
            &dwTemp,
            &dwSize
        );
        if (dwTemp != bDisableSpotlightIcon)
        {
            bDisableSpotlightIcon = dwTemp;
#if WITH_MAIN_PATCHER
            if (IsSpotlightEnabled()) dwRefreshUIMask |= REFRESHUI_SPOTLIGHT;
#endif
        }
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("SpotlightDesktopMenuMask"),
            0,
            NULL,
            &dwSpotlightDesktopMenuMask,
            &dwSize
        );
        dwTemp = 0;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("SpotlightUpdateSchedule"),
            0,
            NULL,
            &dwTemp,
            &dwSize
        );
        if (dwTemp != dwSpotlightUpdateSchedule)
        {
            dwSpotlightUpdateSchedule = dwTemp;
#if WITH_MAIN_PATCHER
            if (IsSpotlightEnabled() && hWndServiceWindow)
            {
                if (dwSpotlightUpdateSchedule)
                {
                    SetTimer(hWndServiceWindow, 100, dwSpotlightUpdateSchedule * 1000, NULL);
                }
                else
                {
                    KillTimer(hWndServiceWindow, 100);
                }
            }
#endif
        }
        dwTemp = FALSE;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("PinnedItemsActAsQuickLaunch"),
            0,
            NULL,
            &dwTemp,
            &dwSize
        );
        if (!bWasPinnedItemsActAsQuickLaunch)
        {
            //if (dwTemp != bPinnedItemsActAsQuickLaunch)
            {
                bPinnedItemsActAsQuickLaunch = dwTemp;
                bWasPinnedItemsActAsQuickLaunch = TRUE;
                //dwRefreshUIMask |= REFRESHUI_TASKBAR;
            }
        }
        dwTemp = FALSE;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("RemoveExtraGapAroundPinnedItems"),
            0,
            NULL,
            &dwTemp,
            &dwSize
        );
        //if (!bWasRemoveExtraGapAroundPinnedItems)
        {
            if (dwTemp != bRemoveExtraGapAroundPinnedItems)
            {
                bRemoveExtraGapAroundPinnedItems = dwTemp;
                bWasRemoveExtraGapAroundPinnedItems = TRUE;
                dwRefreshUIMask |= REFRESHUI_TASKBAR;
            }
        }
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("UndeadStartCorner"),
            0,
            NULL,
            &dwUndeadStartCorner,
            &dwSize
        );

#if WITH_MAIN_PATCHER
        EnterCriticalSection(&lock_epw);

        DWORD dwOldWeatherTemperatureUnit = dwWeatherTemperatureUnit;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("WeatherTemperatureUnit"),
            0,
            NULL,
            &dwWeatherTemperatureUnit,
            &dwSize
        );
        if (dwWeatherTemperatureUnit != dwOldWeatherTemperatureUnit && epw)
        {
            epw->lpVtbl->SetTemperatureUnit(epw, dwWeatherTemperatureUnit);
            HWND hWnd = NULL;
            if (SUCCEEDED(epw->lpVtbl->GetWindowHandle(epw, &hWnd)) && hWnd)
            {
                SendMessageW(hWnd, EP_WEATHER_WM_FETCH_DATA, 0, 0);
            }
        }

        DWORD dwOldWeatherViewMode = dwWeatherViewMode;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("WeatherViewMode"),
            0,
            NULL,
            &dwWeatherViewMode,
            &dwSize
        );
        if (dwWeatherViewMode != dwOldWeatherViewMode && PeopleButton_LastHWND)
        {
            dwRefreshUIMask |= REFRESHUI_PEOPLE;
        }

        DWORD dwOldUpdateSchedule = dwWeatherUpdateSchedule;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("WeatherContentUpdateMode"),
            0,
            NULL,
            &dwWeatherUpdateSchedule,
            &dwSize
        );
        if (dwWeatherUpdateSchedule != dwOldUpdateSchedule && epw)
        {
            epw->lpVtbl->SetUpdateSchedule(epw, dwWeatherUpdateSchedule * 1000);
        }

        dwSize = MAX_PATH * sizeof(WCHAR);
        if (RegQueryValueExW(
            hKey,
            TEXT("WeatherLocation"),
            0,
            NULL,
            wszWeatherTerm,
            &dwSize
        ))
        {
            wcscpy_s(wszWeatherTerm, MAX_PATH, L"");
        }        
        else
        {
            if (wszWeatherTerm[0] == 0)
            {
                wcscpy_s(wszWeatherTerm, MAX_PATH, L"");
            }
        }
        if (epw)
        {
            epw->lpVtbl->SetTerm(epw, MAX_PATH * sizeof(WCHAR), wszWeatherTerm);
        }

        dwSize = MAX_PATH * sizeof(WCHAR);
        if (RegQueryValueExW(
            hKey,
            TEXT("WeatherLanguage"),
            0,
            NULL,
            wszWeatherLanguage,
            &dwSize
        ) != ERROR_SUCCESS || wszWeatherLanguage[0] == 0)
        {
            EP_L10N_GetCurrentUserLanguage(wszWeatherLanguage, MAX_PATH);
        }
        if (epw)
        {
            epw->lpVtbl->SetLanguage(epw, MAX_PATH * sizeof(WCHAR), wszWeatherLanguage);
        }

        DWORD bOldWeatherFixedSize = bWeatherFixedSize;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("WeatherFixedSize"),
            0,
            NULL,
            &bWeatherFixedSize,
            &dwSize
        );
        if (bWeatherFixedSize != bOldWeatherFixedSize && epw)
        {
            dwRefreshUIMask |= REFRESHUI_PEOPLE;
        }

        DWORD dwOldWeatherTheme = dwWeatherTheme;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("WeatherTheme"),
            0,
            NULL,
            &dwWeatherTheme,
            &dwSize
        );
        if (dwWeatherTheme != dwOldWeatherTheme && PeopleButton_LastHWND)
        {
            if (epw)
            {
                epw->lpVtbl->SetDarkMode(epw, (LONG64)dwWeatherTheme, TRUE);
            }
        }

        DWORD dwOldWeatherGeolocationMode = dwWeatherGeolocationMode;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("WeatherLocationType"),
            0,
            NULL,
            &dwWeatherGeolocationMode,
            &dwSize
        );
        if (dwWeatherGeolocationMode != dwOldWeatherGeolocationMode && PeopleButton_LastHWND)
        {
            if (epw)
            {
                epw->lpVtbl->SetGeolocationMode(epw, (LONG64)dwWeatherGeolocationMode);
            }
        }

        DWORD dwOldWeatherWindowCornerPreference = dwWeatherWindowCornerPreference;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("WeatherWindowCornerPreference"),
            0,
            NULL,
            &dwWeatherWindowCornerPreference,
            &dwSize
        );
        if (dwWeatherWindowCornerPreference != dwOldWeatherWindowCornerPreference && PeopleButton_LastHWND)
        {
            if (epw)
            {
                epw->lpVtbl->SetWindowCornerPreference(epw, (LONG64)dwWeatherWindowCornerPreference);
            }
        }

        DWORD dwOldWeatherDevMode = dwWeatherDevMode;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("WeatherDevMode"),
            0,
            NULL,
            &dwWeatherDevMode,
            &dwSize
        );
        if (dwWeatherDevMode != dwOldWeatherDevMode && PeopleButton_LastHWND)
        {
            if (epw)
            {
                epw->lpVtbl->SetDevMode(epw, (LONG64)dwWeatherDevMode, TRUE);
            }
        }

        DWORD dwOldWeatherIconPack = dwWeatherIconPack;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("WeatherIconPack"),
            0,
            NULL,
            &dwWeatherIconPack,
            &dwSize
        );
        if (dwWeatherIconPack != dwOldWeatherIconPack && PeopleButton_LastHWND)
        {
            if (epw)
            {
                epw->lpVtbl->SetIconPack(epw, (LONG64)dwWeatherIconPack, TRUE);
            }
        }

        DWORD dwOldWeatherToLeft = dwWeatherToLeft;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("WeatherToLeft"),
            0,
            NULL,
            &dwWeatherToLeft,
            &dwSize
        );
        if (dwWeatherToLeft != dwOldWeatherToLeft && PeopleButton_LastHWND)
        {
            dwRefreshUIMask |= REFRESHUI_CENTER;
        }

        DWORD dwOldWeatherContentsMode = dwWeatherContentsMode;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("WeatherContentsMode"),
            0,
            NULL,
            &dwWeatherContentsMode,
            &dwSize
        );
        if (dwWeatherContentsMode != dwOldWeatherContentsMode && PeopleButton_LastHWND)
        {
            dwRefreshUIMask |= REFRESHUI_CENTER;
        }

        DWORD dwOldWeatherZoomFactor = dwWeatherZoomFactor;
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("WeatherZoomFactor"),
            0,
            NULL,
            &dwWeatherZoomFactor,
            &dwSize
        );
        if (dwWeatherZoomFactor != dwOldWeatherZoomFactor && PeopleButton_LastHWND)
        {
            if (epw)
            {
                epw->lpVtbl->SetZoomFactor(epw, dwWeatherZoomFactor ? (LONG64)dwWeatherZoomFactor : 100);
            }
        }

        LeaveCriticalSection(&lock_epw);
#endif
        RegCloseKey(hKey);
    }

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ | KEY_WOW64_64KEY,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("AltTabSettings"),
            0,
            NULL,
            &dwAltTabSettings,
            &dwSize
        );
        RegCloseKey(hKey);
    }


    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ | KEY_WOW64_64KEY,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("MonitorOverride"),
            0,
            NULL,
            &bMonitorOverride,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("OpenAtLogon"),
            0,
            NULL,
            &bOpenAtLogon,
            &dwSize
        );
        RegCloseKey(hKey);
    }

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        TEXT(REGPATH) L"\\sws",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ | KEY_WOW64_64KEY,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        RegCloseKey(hKey);
    }

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MultitaskingView\\AltTabViewHost",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ | KEY_WOW64_64KEY,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        RegCloseKey(hKey);
    }

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ | KEY_WOW64_64KEY,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        RegCloseKey(hKey);
    }

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ | KEY_WOW64_64KEY,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        RegCloseKey(hKey);
    }

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced\\People",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ | KEY_WOW64_64KEY,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        RegCloseKey(hKey);
    }

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\TabletTip\\1.7",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ | KEY_WOW64_64KEY,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        RegCloseKey(hKey);
    }

    if (bIsRefreshAllowed && dwRefreshUIMask)
    {
        if (dwRefreshUIMask & REFRESHUI_GLOM)
        {
            Explorer_RefreshUI(0);
        }
        if ((dwRefreshUIMask & REFRESHUI_ORB) || (dwRefreshUIMask & REFRESHUI_PEOPLE))
        {
            HWND hwndTray = FindWindowW(L"Shell_TrayWnd", NULL);
            if (hwndTray)
            {
                SendMessageW(hwndTray, WM_WININICHANGE, 0, (LPARAM)L"TraySettings");
            }
            if (dwRefreshUIMask & REFRESHUI_ORB)
            {
                InvalidateRect(FindWindowW(L"ExplorerPatcher_GUI_" _T(EP_CLSID), NULL), NULL, FALSE);
            }
            if (dwRefreshUIMask & REFRESHUI_PEOPLE)
            {
                //if (epw_dummytext[0] == 0) epw_dummytext = L"\u2009";
                //else epw_dummytext = L"";
#if WITH_MAIN_PATCHER
                InvalidateRect(PeopleButton_LastHWND, NULL, TRUE);
#endif
            }
        }
        if (dwRefreshUIMask & REFRESHUI_TASKBAR)
        {
        }
        if (dwRefreshUIMask & REFRESHUI_CENTER)
        {
#if WITH_MAIN_PATCHER
            //ToggleTaskbarAutohide();
            //Sleep(1000);
            //ToggleTaskbarAutohide();
            FixUpCenteredTaskbar();
#endif
        }
        if (dwRefreshUIMask & REFRESHUI_SPOTLIGHT)
        {
            DWORD dwAttributes = 0; dwTemp = sizeof(DWORD);
            RegGetValueW(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\{2cc5ca98-6485-489a-920e-b3e88a6ccce3}\\ShellFolder", L"Attributes", RRF_RT_DWORD, NULL, &dwAttributes, &dwTemp);
            if (bDisableSpotlightIcon) dwAttributes |= SFGAO_NONENUMERATED;
            else dwAttributes &= ~SFGAO_NONENUMERATED;
            RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\Classes\\CLSID\\{2cc5ca98-6485-489a-920e-b3e88a6ccce3}\\ShellFolder", L"Attributes", REG_DWORD, &dwAttributes, sizeof(DWORD));
            SHFlushSFCache();
            SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
        }
    }
}

void Explorer_RefreshClockHelper(HWND hClockButton)
{
    INT64* ClockButtonInstance = (BYTE*)(GetWindowLongPtrW(hClockButton, 0)); // -> ClockButton
    // we call v_Initialize because all it does is to query the
    // registry and update the internal state to display seconds or not
    // to get the offset, simply inspect the vtable of ClockButton
    if (ClockButtonInstance)
    {
        ((void(*)(void*))(*(INT64*)((*(INT64*)ClockButtonInstance) + 6 * sizeof(uintptr_t))))(ClockButtonInstance); // v_Initialize
        // we need to refresh the button; for the text to actually change, we need to set this:
        // inspect ClockButton::v_OnTimer
        *((BYTE*)ClockButtonInstance + 547) = 1;
        // then, we simply invalidate the area
        InvalidateRect(hClockButton, NULL, TRUE);
    }
}

void Explorer_RefreshClock(int unused)
{
    HWND hShellTray_Wnd = FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL);
    if (hShellTray_Wnd)
    {
        HWND hTrayNotifyWnd = FindWindowExW(hShellTray_Wnd, NULL, L"TrayNotifyWnd", NULL);
        if (hTrayNotifyWnd)
        {
            HWND hClockButton = FindWindowExW(hTrayNotifyWnd, NULL, L"TrayClockWClass", NULL);
            if (hClockButton)
            {
                Explorer_RefreshClockHelper(hClockButton);
            }
        }
    }

    HWND hWnd = NULL;
    do
    {
        hWnd = FindWindowExW(
            NULL,
            hWnd,
            L"Shell_SecondaryTrayWnd",
            NULL
        );
        if (hWnd)
        {
            HWND hClockButton = FindWindowExW(hWnd, NULL, L"ClockButton", NULL);
            if (hClockButton)
            {
                Explorer_RefreshClockHelper(hClockButton);
            }
        }
    } while (hWnd);
}

void* TrayUI__UpdatePearlSizeFunc;

void UpdateSearchBox()
{
#if defined(_M_X64)
    if (!IsWindows11Version22H2OrHigher() || bOldTaskbar != 1)
        return;

    if (!TrayUI__UpdatePearlSizeFunc)
        return;

    PBYTE searchBegin = TrayUI__UpdatePearlSizeFunc;
    // 0F 84 ?? ?? ?? ?? 48 8B 81 ?? ?? ?? ?? 48 85 C0 74 04
    PBYTE match = FindPattern(
        searchBegin,
        256,
        "\x0F\x84\x00\x00\x00\x00\x48\x8B\x81\x00\x00\x00\x00\x48\x85\xC0\x74\x04",
        "xx????xxx????xxxxx"
    );
    if (match)
    {
        PBYTE overwriteBegin = match + 18;
        DWORD dwOldProtect;
        if (VirtualProtect(overwriteBegin, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            // Overwrite right after the pattern with
            // mov byte ptr [rax+58h], 0 // C6 40 58 00
            overwriteBegin[0] = 0xC6;
            overwriteBegin[1] = 0x40;
            overwriteBegin[2] = 0x58; // Offset to m_bEnabled
            overwriteBegin[3] = dwSearchboxTaskbarMode == 2 && !dwTaskbarSmallIcons; // Enable the search box?
            VirtualProtect(overwriteBegin, 4, dwOldProtect, &dwOldProtect);
        }
    }
#endif
}

int numTBButtons = 0;
void WINAPI Explorer_RefreshUI(int src)
{
    HKEY hKey = NULL;
    DWORD dwSize = 0, dwTemp = 0, dwRefreshMask = 0;
    if (src == 99 || src == 1)
    {
        RegCreateKeyExW(
            HKEY_CURRENT_USER,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_READ | KEY_WOW64_64KEY,
            NULL,
            &hKey,
            NULL
        );
        if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
        {
            hKey = NULL;
        }
        if (hKey)
        {
            dwTemp = 0;
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("TaskbarSmallIcons"),
                0,
                NULL,
                &dwTemp,
                &dwSize
            );
            if (dwTemp != dwTaskbarSmallIcons)
            {
                dwTaskbarSmallIcons = dwTemp;
                UpdateSearchBox();
            }
            dwTemp = 0;
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("ShowTaskViewButton"),
                0,
                NULL,
                &dwTemp,
                &dwSize
            );
            if (dwTemp != dwShowTaskViewButton)
            {
                dwShowTaskViewButton = dwTemp;
                dwRefreshMask |= REFRESHUI_CENTER;
            }
            dwTemp = 0;
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("TaskbarDa"),
                0,
                NULL,
                &dwTemp,
                &dwSize
            );
            if (dwTemp != dwTaskbarDa)
            {
                dwTaskbarDa = dwTemp;
                dwRefreshMask |= REFRESHUI_CENTER;
            }
            RegCloseKey(hKey);
            //SearchboxTaskbarMode
        }
    }
    if (src == 99 || src == 2)
    {
        RegCreateKeyExW(
            HKEY_CURRENT_USER,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search",
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_READ | KEY_WOW64_64KEY,
            NULL,
            &hKey,
            NULL
        );
        if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
        {
            hKey = NULL;
        }
        if (hKey)
        {
            dwTemp = 0;
            dwSize = sizeof(DWORD);
            RegQueryValueExW(
                hKey,
                TEXT("SearchboxTaskbarMode"),
                0,
                NULL,
                &dwTemp,
                &dwSize
            );
            if (dwTemp != dwSearchboxTaskbarMode)
            {
                dwSearchboxTaskbarMode = dwTemp;
                dwRefreshMask |= REFRESHUI_CENTER;
                UpdateSearchBox();
            }
        }
    }
    if (src == 99) return;
    HWND hwndTray = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hwndTray)
    {
        SendMessageW(hwndTray, WM_WININICHANGE, 0, (LPARAM)L"TraySettings");
    }
    Explorer_RefreshClock(0);
    if (dwRefreshMask & REFRESHUI_CENTER)
    {
#if WITH_MAIN_PATCHER
        FixUpCenteredTaskbar();
#endif
    }
}

void Explorer_TogglePeopleButton(int unused)
{
    HWND hShellTray_Wnd = FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL);
    if (hShellTray_Wnd)
    {
        INT64* CTrayInstance = (BYTE*)(GetWindowLongPtrW(hShellTray_Wnd, 0)); // -> CTray
        if (CTrayInstance)
        {
            const unsigned int TRAYUI_OFFSET_IN_CTRAY = 110;
            INT64* TrayUIInstance = *((INT64*)CTrayInstance + TRAYUI_OFFSET_IN_CTRAY);
            if (TrayUIInstance)
            {
                ((void(*)(void*))(*(INT64*)((*(INT64*)TrayUIInstance) + 57 * sizeof(uintptr_t))))(TrayUIInstance);
            }
        }
    }
}

void Explorer_ToggleTouchpad(int unused)
{
    HWND hShellTray_Wnd = FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL);
    if (hShellTray_Wnd)
    {
        INT64* CTrayInstance = (BYTE*)(GetWindowLongPtrW(hShellTray_Wnd, 0)); // -> CTray
        if (CTrayInstance)
        {
            const unsigned int TRAYUI_OFFSET_IN_CTRAY = 110;
            INT64* TrayUIInstance = *((INT64*)CTrayInstance + TRAYUI_OFFSET_IN_CTRAY);
            if (TrayUIInstance)
            {
                ((void(*)(void*))(*(INT64*)((*(INT64*)TrayUIInstance) + 60 * sizeof(uintptr_t))))(TrayUIInstance);
            }
        }
    }
}
#pragma endregion


#pragma region "Fix taskbar for classic theme and set Explorer window hooks"
HWND(*CreateWindowExWFunc)(
    DWORD     dwExStyle,
    LPCWSTR   lpClassName,
    LPCWSTR   lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam
    );
HWND CreateWindowExWHook(
    DWORD     dwExStyle,
    LPCWSTR   lpClassName,
    LPCWSTR   lpWindowName,
    DWORD     dwStyle,
    int       X,
    int       Y,
    int       nWidth,
    int       nHeight,
    HWND      hWndParent,
    HMENU     hMenu,
    HINSTANCE hInstance,
    LPVOID    lpParam
)
{
    if (bClassicThemeMitigations && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"TrayNotifyWnd"))
    {
        dwExStyle |= WS_EX_STATICEDGE;
    }
    if (bClassicThemeMitigations && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"NotifyIconOverflowWindow"))
    {
        dwExStyle |= WS_EX_STATICEDGE;
    }
    if (bClassicThemeMitigations && (*((WORD*)&(lpClassName)+1)) && (!wcscmp(lpClassName, L"SysListView32") || !wcscmp(lpClassName, L"SysTreeView32"))) // !wcscmp(lpClassName, L"FolderView")
    {
        wchar_t wszClassName[200];
        ZeroMemory(wszClassName, 200);
        GetClassNameW(GetAncestor(hWndParent, GA_ROOT), wszClassName, 200);
        if (!wcscmp(wszClassName, L"CabinetWClass"))
        {
            dwExStyle |= WS_EX_CLIENTEDGE;
        }
    }
    if (bIsExplorerProcess && bToolbarSeparators && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"ReBarWindow32"))
    {
        wchar_t wszClassName[200];
        ZeroMemory(wszClassName, 200);
        GetClassNameW(hWndParent, wszClassName, 200);
        if (!wcscmp(wszClassName, L"Shell_TrayWnd"))
        {
            dwStyle |= RBS_BANDBORDERS;
        }
    }
    HWND hWnd = CreateWindowExWFunc(
        dwExStyle,
        lpClassName,
        lpWindowName,
        dwStyle,
        X,
        Y,
        nWidth,
        nHeight,
        hWndParent,
        hMenu,
        hInstance,
        lpParam
    );
#if WITH_MAIN_PATCHER
    if (bIsExplorerProcess && (*((WORD*)&(lpClassName)+1)) && (!wcscmp(lpClassName, L"TrayClockWClass") || !wcscmp(lpClassName, L"ClockButton")))
    {
        SetWindowSubclass(hWnd, ClockButtonSubclassProc, ClockButtonSubclassProc, 0);
    }
    else if (bIsExplorerProcess && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"TrayShowDesktopButtonWClass"))
    {
        SetWindowSubclass(hWnd, ShowDesktopSubclassProc, ShowDesktopSubclassProc, 0);
    }
    else if (bIsExplorerProcess && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"Shell_TrayWnd"))
    {
        SetWindowSubclass(hWnd, Shell_TrayWndSubclassProc, Shell_TrayWndSubclassProc, TRUE);
        Shell_TrayWndMouseHook = SetWindowsHookExW(WH_MOUSE, Shell_TrayWndMouseProc, NULL, GetCurrentThreadId());
    }
    else if (bIsExplorerProcess && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"Shell_SecondaryTrayWnd"))
    {
        SetWindowSubclass(hWnd, Shell_TrayWndSubclassProc, Shell_TrayWndSubclassProc, FALSE);
    }
    else if (bIsExplorerProcess && (*((WORD*)&(lpClassName)+1)) && !_wcsicmp(lpClassName, L"ReBarWindow32") && hWndParent == FindWindowW(L"Shell_TrayWnd", NULL))
    {
        SetWindowSubclass(hWnd, ReBarWindow32SubclassProc, ReBarWindow32SubclassProc, FALSE);
    }
#endif
    /*
    if (bClassicThemeMitigations && (*((WORD*)&(lpClassName)+1)) && (!wcscmp(lpClassName, L"FolderView")))
    {
        wchar_t wszClassName[200];
        GetClassNameW(GetAncestor(hWndParent, GA_ROOT), wszClassName, 200);
        if (!wcscmp(wszClassName, L"CabinetWClass"))
        {
            SendMessageW(hWnd, 0x108, 0, 0);
        }
    }
    */
    //SetWindowTheme(hWnd, L" ", L" ");
    return hWnd;
}

LONG_PTR(*SetWindowLongPtrWFunc)(
    HWND     hWnd,
    int      nIndex,
    LONG_PTR dwNewLong
    );
LONG_PTR SetWindowLongPtrWHook(
    HWND     hWnd,
    int      nIndex,
    LONG_PTR dwNewLong
)
{
    WCHAR lpClassName[200];
    ZeroMemory(lpClassName, 200);
    GetClassNameW(hWnd, lpClassName, 200);
    HWND hWndParent = GetParent(hWnd);

    if (bClassicThemeMitigations && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"TrayNotifyWnd"))
    {
        if (nIndex == GWL_EXSTYLE)
        {
            dwNewLong |= WS_EX_STATICEDGE;
        }
    }
    if (bClassicThemeMitigations && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"NotifyIconOverflowWindow"))
    {
        if (nIndex == GWL_EXSTYLE)
        {
            dwNewLong |= WS_EX_STATICEDGE;
        }
    }
    if (bClassicThemeMitigations && (*((WORD*)&(lpClassName)+1)) && (!wcscmp(lpClassName, L"SysListView32") || !wcscmp(lpClassName, L"SysTreeView32"))) // !wcscmp(lpClassName, L"FolderView")
    {
        wchar_t wszClassName[200];
        ZeroMemory(wszClassName, 200);
        GetClassNameW(GetAncestor(hWndParent, GA_ROOT), wszClassName, 200);
        if (!wcscmp(wszClassName, L"CabinetWClass"))
        {
            if (nIndex == GWL_EXSTYLE)
            {
                dwNewLong |= WS_EX_CLIENTEDGE;
            }
        }
    }
    if (bIsExplorerProcess && bToolbarSeparators && (*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"ReBarWindow32"))
    {
        wchar_t wszClassName[200];
        ZeroMemory(wszClassName, 200);
        GetClassNameW(hWndParent, wszClassName, 200);
        if (!wcscmp(wszClassName, L"Shell_TrayWnd"))
        {
            if (nIndex == GWL_STYLE)
            {
                dwNewLong |= RBS_BANDBORDERS;
            }
        }
    }
    return SetWindowLongPtrWFunc(hWnd, nIndex, dwNewLong);
}

#if WITH_MAIN_PATCHER
HRESULT (*explorer_SetWindowThemeFunc)(
    HWND    hwnd,
    LPCWSTR pszSubAppName,
    LPCWSTR pszSubIdList
);
HRESULT explorer_SetWindowThemeHook(
    HWND    hwnd,
    LPCWSTR pszSubAppName,
    LPCWSTR pszSubIdList
)
{
    if (bClassicThemeMitigations)
    {
        printf("SetWindowTheme\n");
        return explorer_SetWindowThemeFunc(hwnd, L" ", L" ");
    }
    return explorer_SetWindowThemeFunc(hwnd, pszSubAppName, pszSubIdList);
}

HDPA hOrbCollection = NULL;
HRESULT explorer_DrawThemeBackground(
    HTHEME  hTheme,
    HDC     hdc,
    int     iPartId,
    int     iStateId,
    LPCRECT pRect,
    LPCRECT pClipRect
)
{
    if (dwOrbStyle && hOrbCollection)
    {
        for (unsigned int i = 0; i < DPA_GetPtrCount(hOrbCollection); ++i)
        {
            OrbInfo* oi = DPA_FastGetPtr(hOrbCollection, i);
            if (oi->hTheme == hTheme)
            {
                BITMAPINFO bi;
                ZeroMemory(&bi, sizeof(BITMAPINFO));
                bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bi.bmiHeader.biWidth = 1;
                bi.bmiHeader.biHeight = 1;
                bi.bmiHeader.biPlanes = 1;
                bi.bmiHeader.biBitCount = 32;
                bi.bmiHeader.biCompression = BI_RGB;
                RGBQUAD transparent = { 0, 0, 0, 0 };
                RGBQUAD color = { 0xFF, 0xFF, 0xFF, 0xFF };

                if (dwOrbStyle == ORB_STYLE_WINDOWS11)
                {
                    UINT separator = oi->dpi / 96;
                    //printf(">>> SEPARATOR %p %d %d\n", oi->hTheme, oi->dpi, separator);

                    // Background
                    StretchDIBits(hdc,
                        pRect->left + (separator % 2),
                        pRect->top + (separator % 2),
                        pRect->right - pRect->left - (separator % 2),
                        pRect->bottom - pRect->top - (separator % 2),
                        0, 0, 1, 1, &color, &bi,
                        DIB_RGB_COLORS, SRCCOPY);
                    // Middle vertical line
                    StretchDIBits(hdc,
                        pRect->left + ((pRect->right - pRect->left) / 2) - (separator / 2),
                        pRect->top,
                        separator,
                        pRect->bottom - pRect->top,
                        0, 0, 1, 1, &transparent, &bi,
                        DIB_RGB_COLORS, SRCCOPY);
                    // Middle horizontal line
                    StretchDIBits(hdc,
                        pRect->left,
                        pRect->top + ((pRect->bottom - pRect->top) / 2) - (separator / 2),
                        pRect->right - pRect->left,
                        separator,
                        0, 0, 1, 1, &transparent, &bi,
                        DIB_RGB_COLORS, SRCCOPY);
                }
                else if (dwOrbStyle == ORB_STYLE_TRANSPARENT)
                {
                    StretchDIBits(hdc,
                        pRect->left,
                        pRect->top,
                        pRect->right - pRect->left,
                        pRect->bottom - pRect->top,
                        0, 0, 1, 1, &transparent, &bi,
                        DIB_RGB_COLORS, SRCCOPY);
                }
                return S_OK;
            }
        }
    }
    if (bClassicThemeMitigations)
    {
        if (iPartId == 4 && iStateId == 1)
        {
            COLORREF bc = GetBkColor(hdc);
            COLORREF fc = GetTextColor(hdc);
            int mode = SetBkMode(hdc, TRANSPARENT);

            SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));

            NONCLIENTMETRICSW ncm;
            ncm.cbSize = sizeof(NONCLIENTMETRICSW);
            SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &ncm, 0);

            HFONT hFont = CreateFontIndirectW(&(ncm.lfCaptionFont));

            UINT dpiX, dpiY;
            HRESULT hr = GetDpiForMonitor(
                MonitorFromWindow(WindowFromDC(hdc), MONITOR_DEFAULTTOPRIMARY),
                MDT_DEFAULT,
                &dpiX,
                &dpiY
            );
            double dx = dpiX / 96.0, dy = dpiY / 96.0;

            HGDIOBJ hOldFont = SelectObject(hdc, hFont);
            DWORD dwTextFlags = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
            RECT rc = *pRect;
            rc.bottom -= 7 * dy;
            DrawTextW(
                hdc,
                L"\u2026",
                -1, 
                &rc,
                dwTextFlags
            );
            SelectObject(hdc, hOldFont);
            DeleteObject(hFont);
            SetBkColor(hdc, bc);
            SetTextColor(hdc, fc);
            SetBkMode(hdc, mode);
        }
        return S_OK;
    }
    return DrawThemeBackground(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

HRESULT explorer_CloseThemeData(HTHEME hTheme)
{
    HRESULT hr = CloseThemeData(hTheme);
    if (SUCCEEDED(hr) && hOrbCollection)
    {
        for (unsigned int i = 0; i < DPA_GetPtrCount(hOrbCollection); ++i)
        {
            OrbInfo* oi = DPA_FastGetPtr(hOrbCollection, i);
            if (oi->hTheme == hTheme)
            {
                //printf(">>> DELETE DPA %p %d\n", oi->hTheme, oi->dpi);
                DPA_DeletePtr(hOrbCollection, i);
                free(oi);
                break;
            }
        }
    }
    return hr;
}

HTHEME explorer_OpenThemeDataForDpi(
    HWND    hwnd,
    LPCWSTR pszClassList,
    UINT    dpi
)
{
    if ((*((WORD*)&(pszClassList)+1)) && !wcscmp(pszClassList, L"TaskbarPearl"))
    {
        if (!hOrbCollection)
        {
            hOrbCollection = DPA_Create(MAX_NUM_MONITORS);
        }

        HTHEME hTheme = OpenThemeDataForDpi(hwnd, pszClassList, dpi);
        if (hTheme && hOrbCollection)
        {
            OrbInfo* oi = malloc(sizeof(OrbInfo));
            if (oi)
            {
                oi->hTheme = hTheme;
                oi->dpi = dpi;
                //printf(">>> APPEND DPA %p %d\n", oi->hTheme, oi->dpi);
                DPA_AppendPtr(hOrbCollection, oi);
            }
        }
        return hTheme;
    }

    // task list - Taskband2 from CTaskListWnd::_HandleThemeChanged
    if (bClassicThemeMitigations && (*((WORD*)&(pszClassList)+1)) && !wcscmp(pszClassList, L"Taskband2"))
    {
        return 0xDeadBeef;
    }
    // system tray notification area more icons
    else if (bClassicThemeMitigations && (*((WORD*)&(pszClassList)+1)) && !wcscmp(pszClassList, L"TrayNotifyFlyout"))
    {
        return 0xABadBabe;
    }
    /*else if (bClassicThemeMitigations && (*((WORD*)&(pszClassList)+1)) && wcsstr(pszClassList, L"::Taskband2"))
    {
        wprintf(L"%s\n", pszClassList);
        return 0xB16B00B5;
    }*/
    return OpenThemeDataForDpi(hwnd, pszClassList, dpi);
}

HRESULT explorer_GetThemeMetric(
    HTHEME hTheme,
    HDC    hdc,
    int    iPartId,
    int    iStateId,
    int    iPropId,
    int* piVal
)
{
    if (!bClassicThemeMitigations || (hTheme != 0xABadBabe))
    {
        return GetThemeMetric(
            hTheme,
            hdc,
            iPartId,
            iStateId,
            iPropId,
            piVal
        );
    }
    const int TMT_WIDTH = 2416;
    const int TMT_HEIGHT = 2417;
    if (hTheme == 0xABadBabe && iPropId == TMT_WIDTH && iPartId == 3 && iStateId == 0)
    {
        *piVal = GetSystemMetrics(SM_CXICON);
    }
    else if (hTheme == 0xABadBabe && iPropId == TMT_HEIGHT && iPartId == 3 && iStateId == 0)
    {
        *piVal = GetSystemMetrics(SM_CYICON);
    }
    return S_OK;
}

HRESULT explorer_GetThemeMargins(
    HTHEME  hTheme,
    HDC     hdc,
    int     iPartId,
    int     iStateId,
    int     iPropId,
    LPCRECT prc,
    MARGINS* pMargins
)
{
    if (!bClassicThemeMitigations || (hTheme != 0xDeadBeef && hTheme != 0xABadBabe))
    {
        HRESULT hr = GetThemeMargins(
            hTheme,
            hdc,
            iPartId,
            iStateId,
            iPropId,
            prc,
            pMargins
        );
        return hr;
    }
    const int TMT_SIZINGMARGINS = 3601;
    const int TMT_CONTENTMARGINS = 3602;
    HRESULT hr = S_OK;
    if (hTheme)
    {
        hr = GetThemeMargins(
            hTheme,
            hdc,
            iPartId,
            iStateId,
            iPropId,
            prc,
            pMargins
        );
    }
    /*if (hTheme == 0xB16B00B5)
    {
        printf(
            "GetThemeMargins %d %d %d - %d %d %d %d\n", 
            iPartId, 
            iStateId, 
            iPropId, 
            pMargins->cxLeftWidth, 
            pMargins->cyTopHeight, 
            pMargins->cxRightWidth, 
            pMargins->cyBottomHeight
        );
    }*/
    if (hTheme == 0xDeadBeef && iPropId == TMT_CONTENTMARGINS && iPartId == 5 && iStateId == 1)
    {
        // task list button measurements
        pMargins->cxLeftWidth = 4;
        pMargins->cyTopHeight = 3;
        pMargins->cxRightWidth = 4;
        pMargins->cyBottomHeight = 3;
    }
    else if (hTheme == 0xDeadBeef && iPropId == TMT_CONTENTMARGINS && iPartId == 1 && iStateId == 0)
    {
        // task list measurements
        pMargins->cxLeftWidth = 0;
        pMargins->cyTopHeight = 0;
        pMargins->cxRightWidth = 4;
        pMargins->cyBottomHeight = 0;
    }
    else if (hTheme == 0xDeadBeef && iPropId == TMT_SIZINGMARGINS && iPartId == 5 && iStateId == 1)
    {
        pMargins->cxLeftWidth = 0;
        pMargins->cyTopHeight = 10;
        pMargins->cxRightWidth = 0;
        pMargins->cyBottomHeight = 10;
    }
    else if (hTheme = 0xABadBabe && iPropId == TMT_CONTENTMARGINS && iPartId == 3 && iStateId == 0)
    {
        pMargins->cxLeftWidth = 6;// GetSystemMetrics(SM_CXICONSPACING);
        pMargins->cyTopHeight = 6;// GetSystemMetrics(SM_CYICONSPACING);
        pMargins->cxRightWidth = 6;//GetSystemMetrics(SM_CXICONSPACING);
        pMargins->cyBottomHeight = 6;// GetSystemMetrics(SM_CYICONSPACING);
    }
    HWND hShell_TrayWnd = FindWindowEx(NULL, NULL, L"Shell_TrayWnd", NULL);
    if (hShell_TrayWnd)
    {
        LONG dwStyle = 0;
        dwStyle = GetWindowLongW(hShell_TrayWnd, GWL_STYLE);
        dwStyle |= WS_DLGFRAME;
        SetWindowLongW(hShell_TrayWnd, GWL_STYLE, dwStyle);
        dwStyle &= ~WS_DLGFRAME;
        SetWindowLongW(hShell_TrayWnd, GWL_STYLE, dwStyle);
    }
    HWND hWnd = NULL;
    do
    {
        hWnd = FindWindowEx(
            NULL,
            hWnd,
            L"Shell_SecondaryTrayWnd",
            NULL
        );
        if (hWnd)
        {
            LONG dwStyle = 0;
            dwStyle = GetWindowLongW(hWnd, GWL_STYLE);
            dwStyle |= WS_DLGFRAME;
            SetWindowLongW(hWnd, GWL_STYLE, dwStyle);
            dwStyle &= ~WS_DLGFRAME;
            SetWindowLongW(hWnd, GWL_STYLE, dwStyle);
        }
    } while (hWnd);
    return S_OK;
}

HRESULT explorer_DrawThemeTextEx(
    HTHEME        hTheme,
    HDC           hdc,
    int           iPartId,
    int           iStateId,
    LPCWSTR       pszText,
    int           cchText,
    DWORD         dwTextFlags,
    LPRECT        pRect,
    const DTTOPTS* pOptions
)
{
    if (!bClassicThemeMitigations)
    {
        return DrawThemeTextEx(
            hTheme,
            hdc,
            iPartId,
            iStateId,
            pszText,
            cchText,
            dwTextFlags,
            pRect,
            pOptions
        );
    }

    COLORREF bc = GetBkColor(hdc);
    COLORREF fc = GetTextColor(hdc);
    int mode = SetBkMode(hdc, TRANSPARENT);
    
    wchar_t text[200];
    GetWindowTextW(GetForegroundWindow(), text, 200);

    BOOL bIsActiveUnhovered = (iPartId == 5 && iStateId == 5);
    BOOL bIsInactiveUnhovered = (iPartId == 5 && iStateId == 1);
    BOOL bIsInactiveHovered = (iPartId == 5 && iStateId == 2);
    BOOL bIsActiveHovered = bIsInactiveHovered && !wcscmp(text, pszText);

    SetTextColor(hdc, GetSysColor(COLOR_BTNTEXT));

    NONCLIENTMETRICSW ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);
    SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSW), &ncm, 0);

    HFONT hFont = NULL;
    if (bIsActiveUnhovered || bIsActiveHovered)
    {
        ncm.lfCaptionFont.lfWeight = FW_BOLD;
    }
    else
    {
        ncm.lfCaptionFont.lfWeight = FW_NORMAL;
    }
    hFont = CreateFontIndirectW(&(ncm.lfCaptionFont));

    if (iPartId == 5 && iStateId == 0) // clock
    {
        pRect->top += 2;
    }

    HGDIOBJ hOldFont = SelectObject(hdc, hFont);
    DrawTextW(
        hdc,
        pszText,
        cchText, 
        pRect,
        dwTextFlags
    );
    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    SetBkColor(hdc, bc);
    SetTextColor(hdc, fc);
    SetBkMode(hdc, mode);
    return S_OK;
}
#endif
#pragma endregion


#pragma region "Change links"
int ExplorerFrame_CompareStringOrdinal(const WCHAR* a1, int a2, const WCHAR* a3, int a4, BOOL bIgnoreCase)
{
    void* pRedirects[10] =
    {
        L"::{BB06C0E4-D293-4F75-8A90-CB05B6477EEE}", // System                     (default: redirected to Settings app)
        L"::{7B81BE6A-CE2B-4676-A29E-EB907A5126C5}", // Programs and Features      (default: not redirected)
        NULL,
        // The following are unused but available for the future
        L"::{D450A8A1-9568-45C7-9C0E-B4F9FB4537BD}", // Installed Updates          (default: not redirected)
        L"::{17CD9488-1228-4B2F-88CE-4298E93E0966}", // Default Programs           (default: not redirected)
        L"::{8E908FC9-BECC-40F6-915B-F4CA0E70D03D}", // Network and Sharing Center (default: not redirected)
        L"::{7007ACC7-3202-11D1-AAD2-00805FC1270E}", // Network Connections        (default: not redirected)
        L"Advanced",                                 // Network and Sharing Center -> Change advanced sharing options (default: not redirected)
        L"::{A8A91A66-3A7D-4424-8D24-04E180695C7A}", // Devices and Printers       (default: not redirected)
        NULL
    };
    int ret = CompareStringOrdinal(a1, a2, a3, a4, bIgnoreCase);
    if (ret != CSTR_EQUAL)
    {
        return ret;
    }

    int i = 0;
    while (1)
    {
        BOOL bCond = FALSE;
        if (i == 0) bCond = bDoNotRedirectSystemToSettingsApp;
        else if (i == 1) bCond = bDoNotRedirectProgramsAndFeaturesToSettingsApp;
        if (CompareStringOrdinal(a3, -1, pRedirects[i], -1, FALSE) == CSTR_EQUAL && bCond) break;
        i++;
        if (pRedirects[i] == NULL)
        {
            return ret;
        }
    }

    return CSTR_GREATER_THAN;
}

#if WITH_MAIN_PATCHER
DEFINE_GUID(IID_EnumExplorerCommand,
    0xA88826F8,
    0x186F, 0x4987, 0xAA, 0xDE,
    0xEA, 0x0C, 0xEF, 0x8F, 0xBF, 0xE8
);

typedef interface EnumExplorerCommand EnumExplorerCommand;

typedef struct EnumExplorerCommandVtbl
{
    BEGIN_INTERFACE

    HRESULT(STDMETHODCALLTYPE* QueryInterface)(
        EnumExplorerCommand* This,
        /* [in] */ REFIID riid,
        /* [annotation][iid_is][out] */
        _COM_Outptr_  void** ppvObject);

    ULONG(STDMETHODCALLTYPE* AddRef)(
        EnumExplorerCommand* This);

    ULONG(STDMETHODCALLTYPE* Release)(
        EnumExplorerCommand* This);

    HRESULT(STDMETHODCALLTYPE* Next)(
        EnumExplorerCommand* This,
        unsigned int a2,
        void** a3,
        void* a4);

    END_INTERFACE
} EnumExplorerCommandVtbl;

interface EnumExplorerCommand
{
    CONST_VTBL struct EnumExplorerCommandVtbl* lpVtbl;
};

typedef interface UICommand UICommand;

typedef struct UICommandVtbl
{
    BEGIN_INTERFACE

    HRESULT(STDMETHODCALLTYPE* QueryInterface)(
        UICommand* This,
        /* [in] */ REFIID riid,
        /* [annotation][iid_is][out] */
        _COM_Outptr_  void** ppvObject);

    ULONG(STDMETHODCALLTYPE* AddRef)(
        UICommand* This);

    ULONG(STDMETHODCALLTYPE* Release)(
        UICommand* This);

    HRESULT(STDMETHODCALLTYPE* GetTitle)(
        UICommand* This);

    HRESULT(STDMETHODCALLTYPE* GetIcon)(
        UICommand* This);

    HRESULT(STDMETHODCALLTYPE* GetTooltip)(
        UICommand* This);

    HRESULT(STDMETHODCALLTYPE* GetCanonicalName)(
        UICommand* This,
        GUID* guid);

    HRESULT(STDMETHODCALLTYPE* GetState)(
        UICommand* This);

    HRESULT(STDMETHODCALLTYPE* Invoke)(
        UICommand* This,
        void* a2,
        void* a3);

    HRESULT(STDMETHODCALLTYPE* GetFlags)(
        UICommand* This);

    HRESULT(STDMETHODCALLTYPE* EnumSubCommands)(
        UICommand* This);

    END_INTERFACE
} UICommandVtbl;

interface UICommand
{
    CONST_VTBL struct UICommandVtbl* lpVtbl;
};

DEFINE_GUID(GUID_UICommand_System,
    0x4C202CF0,
    0xC4DC, 0x4251, 0xA3, 0x71,
    0xB6, 0x22, 0xB4, 0x3D, 0x59, 0x2B
);
DEFINE_GUID(GUID_UICommand_ProgramsAndFeatures,
    0xA2E6D9CC,
    0xF866, 0x40B6, 0xA4, 0xB2,
    0xEE, 0x9E, 0x10, 0x04, 0xBD, 0xFC
);
HRESULT(*shell32_UICommand_InvokeFunc)(UICommand*, void*, void*);
HRESULT shell32_UICommand_InvokeHook(UICommand* _this, void* a2, void* a3)
{
    // Guid = {A2E6D9CC-F866-40B6-A4B2-EE9E1004BDFC} Programs and Features
    // Guid = {4C202CF0-C4DC-4251-A371-B622B43D592B} System
    GUID guid;
    ZeroMemory(&guid, sizeof(GUID));
    _this->lpVtbl->GetCanonicalName(_this, &guid);
    BOOL bIsSystem = bDoNotRedirectSystemToSettingsApp && IsEqualGUID(&guid, &GUID_UICommand_System);
    BOOL bIsProgramsAndFeatures = bDoNotRedirectProgramsAndFeaturesToSettingsApp && IsEqualGUID(&guid, &GUID_UICommand_ProgramsAndFeatures);
    if (bIsSystem || bIsProgramsAndFeatures)
    {
        IOpenControlPanel* pOpenControlPanel = NULL;
        CoCreateInstance(
            &CLSID_OpenControlPanel,
            NULL,
            CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
            &IID_OpenControlPanel,
            &pOpenControlPanel
        );
        if (pOpenControlPanel)
        {
            WCHAR* pszWhat = L"";
            if (bIsSystem)
            {
                pszWhat = L"Microsoft.System";
            }
            else if (bIsProgramsAndFeatures)
            {
                pszWhat = L"Microsoft.ProgramsAndFeatures";
            }
            pOpenControlPanel->lpVtbl->Open(pOpenControlPanel, pszWhat, NULL, NULL);
            pOpenControlPanel->lpVtbl->Release(pOpenControlPanel);
            return S_OK;
        }
    }
    return shell32_UICommand_InvokeFunc(_this, a2, a3);
}

BOOL explorer_ShellExecuteExW(SHELLEXECUTEINFOW* pExecInfo)
{
    if (bDoNotRedirectSystemToSettingsApp && pExecInfo && pExecInfo->lpFile && !wcscmp(pExecInfo->lpFile, L"ms-settings:about"))
    {
        IOpenControlPanel* pOpenControlPanel = NULL;
        CoCreateInstance(
            &CLSID_OpenControlPanel,
            NULL,
            CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
            &IID_OpenControlPanel,
            &pOpenControlPanel
        );
        if (pOpenControlPanel)
        {
            pOpenControlPanel->lpVtbl->Open(pOpenControlPanel, L"Microsoft.System", NULL, NULL);
            pOpenControlPanel->lpVtbl->Release(pOpenControlPanel);
            return 1;
        }
    }
    return ShellExecuteExW(pExecInfo);
}

HINSTANCE explorer_ShellExecuteW(
    HWND    hwnd,
    LPCWSTR lpOperation,
    LPCWSTR lpFile,
    LPCWSTR lpParameters,
    LPCWSTR lpDirectory,
    INT     nShowCmd
)
{
    if (bDoNotRedirectNotificationIconsToSettingsApp && !wcscmp(lpFile, L"ms-settings:notifications"))
    {
        return ShellExecuteW(
            hwnd, lpOperation,
            L"shell:::{05d7b0f4-2121-4eff-bf6b-ed3f69b894d9}",
            lpParameters, lpDirectory, nShowCmd
        );
    }
    else if (bDoNotRedirectDateAndTimeToSettingsApp && !wcscmp(lpFile, L"ms-settings:dateandtime"))
    {
        return ShellExecuteW(
            hwnd, lpOperation,
            L"shell:::{E2E7934B-DCE5-43C4-9576-7FE4F75E7480}",
            lpParameters, lpDirectory, nShowCmd
        );
    }
    /*else if (!wcscmp(lpFile, L"ms-settings:taskbar"))
    {
        LaunchPropertiesGUI(hModule);
        return 0;
    }*/
    return ShellExecuteW(hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
}
#endif
#pragma endregion


#pragma region "Classic Drive Grouping"
#if WITH_MAIN_PATCHER
const struct { DWORD dwDescriptionId; UINT uResourceId; } driveCategoryMap[] = {
    { SHDID_FS_DIRECTORY,        9338 }, //shell32
    { SHDID_COMPUTER_SHAREDDOCS, 9338 }, //shell32
    { SHDID_COMPUTER_FIXED,      IDS_DRIVECATEGORY_HARDDISKDRIVES },
    { SHDID_COMPUTER_DRIVE35,    IDS_DRIVECATEGORY_REMOVABLESTORAGE },
    { SHDID_COMPUTER_REMOVABLE,  IDS_DRIVECATEGORY_REMOVABLESTORAGE },
    { SHDID_COMPUTER_CDROM,      IDS_DRIVECATEGORY_REMOVABLESTORAGE },
    { SHDID_COMPUTER_DRIVE525,   IDS_DRIVECATEGORY_REMOVABLESTORAGE },
    { SHDID_COMPUTER_NETDRIVE,   9340 }, //shell32
    { SHDID_COMPUTER_OTHER,      IDS_DRIVECATEGORY_OTHER },
    { SHDID_COMPUTER_RAMDISK,    IDS_DRIVECATEGORY_OTHER },
    { SHDID_COMPUTER_IMAGING,    IDS_DRIVECATEGORY_IMAGING },
    { SHDID_COMPUTER_AUDIO,      IDS_DRIVECATEGORY_PORTABLEMEDIADEVICE },
    { SHDID_MOBILE_DEVICE,       IDS_DRIVECATEGORY_PORTABLEDEVICE }
};

//Represents the true data structure that is returned from shell32!DllGetClassObject
typedef struct {
    const IClassFactoryVtbl* lpVtbl;
    ULONG flags;
    REFCLSID rclsid;
    HRESULT(*pfnCreateInstance)(IUnknown*, REFIID, void**);
} Shell32ClassFactoryEntry;

//Represents a custom ICategorizer/IShellExtInit
typedef struct _EPCategorizer
{
    ICategorizerVtbl* categorizer;
    IShellExtInitVtbl* shellExtInit;

    ULONG ulRefCount;
    IShellFolder2* pShellFolder;
} EPCategorizer;

#pragma region "EPCategorizer: ICategorizer"

HRESULT STDMETHODCALLTYPE EPCategorizer_ICategorizer_QueryInterface(ICategorizer* _this, REFIID riid, void** ppvObject)
{
    if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_ICategorizer))
    {
        *ppvObject = _this;
    }
    else if (IsEqualIID(riid, &IID_IShellExtInit))
    {
        *ppvObject = &((EPCategorizer*) _this)->shellExtInit;
    }
    else
    {
        ppvObject = NULL;
        return E_NOINTERFACE;
    }

    _this->lpVtbl->AddRef(_this);

    return S_OK;
}

ULONG STDMETHODCALLTYPE EPCategorizer_ICategorizer_AddRef(ICategorizer* _this)
{
    return InterlockedIncrement(&((EPCategorizer*)_this)->ulRefCount);
}

ULONG STDMETHODCALLTYPE EPCategorizer_ICategorizer_Release(ICategorizer* _this)
{
    ULONG ulNewCount = InterlockedDecrement(&((EPCategorizer*)_this)->ulRefCount);

    //When the window is closed or refreshed the object is finally freed
    if (ulNewCount == 0)
    {
        EPCategorizer* epCategorizer = (EPCategorizer*)_this;

        if (epCategorizer->pShellFolder != NULL)
        {
            epCategorizer->pShellFolder->lpVtbl->Release(epCategorizer->pShellFolder);
            epCategorizer->pShellFolder = NULL;
        }

        free(epCategorizer);
    }

    return ulNewCount;
}

HRESULT STDMETHODCALLTYPE EPCategorizer_ICategorizer_GetDescription(ICategorizer* _this, LPWSTR pszDesc, UINT cch)
{
    //As of writing returns the string "Type". Same implementation as shell32!CStorageSystemTypeCategorizer::GetDescription
    LoadStringW(hShell32, 0x3105, pszDesc, cch);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE EPCategorizer_ICategorizer_GetCategory(ICategorizer* _this, UINT cidl, PCUITEMID_CHILD_ARRAY apidl, DWORD* rgCategoryIds)
{
    EPCategorizer* epCategorizer = (EPCategorizer*)_this;

    HRESULT hr = S_OK;

    for (UINT i = 0; i < cidl; i++)
    {
        rgCategoryIds[i] = IDS_DRIVECATEGORY_OTHER;

        PROPERTYKEY key = { FMTID_ShellDetails, PID_DESCRIPTIONID };
        VARIANT variant;
        VariantInit(&variant);

        hr = epCategorizer->pShellFolder->lpVtbl->GetDetailsEx(epCategorizer->pShellFolder, apidl[i], &key, &variant);

        if (SUCCEEDED(hr))
        {
            SHDESCRIPTIONID did;

            if (SUCCEEDED(VariantToBuffer(&variant, &did, sizeof(did))))
            {
                for (int j = 0; j < ARRAYSIZE(driveCategoryMap); j++)
                {
                    if (did.dwDescriptionId == driveCategoryMap[j].dwDescriptionId)
                    {
                        rgCategoryIds[i] = driveCategoryMap[j].uResourceId;
                        break;
                    }
                }
            }

            VariantClear(&variant);
        }
    }

    return hr;
}

HRESULT STDMETHODCALLTYPE EPCategorizer_ICategorizer_GetCategoryInfo(ICategorizer* _this, DWORD dwCategoryId, CATEGORY_INFO* pci)
{
    //Now retrieve the display name to use for the resource ID dwCategoryId.
    //pci is already populated with most of the information it needs, we just need to fill in the wszName
    if (!LoadStringW(hModule, dwCategoryId, pci->wszName, ARRAYSIZE(pci->wszName)))
        LoadStringW(hShell32, dwCategoryId, pci->wszName, ARRAYSIZE(pci->wszName));

    return S_OK;
}

HRESULT STDMETHODCALLTYPE EPCategorizer_ICategorizer_CompareCategory(ICategorizer* _this, CATSORT_FLAGS csfFlags, DWORD dwCategoryId1, DWORD dwCategoryId2)
{
    //Typically a categorizer would use the resource IDs containing the names of each category as the "category ID" as well. In our case however, we're using
    //a combination of resource/category IDs provided by shell32 and resource/category IDs we're overriding ourselves. As a result, we are forced to compare
    //not by the value of the category/resource IDs themselves, but by their _position_ within our category ID map

    int categoryArraySize = ARRAYSIZE(driveCategoryMap);

    int firstPos = -1;
    int secondPos = -1;

    for (int i = 0; i < categoryArraySize; i++)
    {
        if (driveCategoryMap[i].uResourceId == dwCategoryId1)
        {
            firstPos = i;
            break;
        }
    }

    for (int i = 0; i < categoryArraySize; i++)
    {
        if (driveCategoryMap[i].uResourceId == dwCategoryId2)
        {
            secondPos = i;
            break;
        }
    }

    int diff = firstPos - secondPos;

    if (diff < 0)
        return 0xFFFF;

    return diff > 0;
}

#pragma endregion
#pragma region "EPCategorizer: IShellExtInit"

//Adjustor Thunks: https://devblogs.microsoft.com/oldnewthing/20040206-00/?p=40723
HRESULT STDMETHODCALLTYPE EPCategorizer_IShellExtInit_QueryInterface(IShellExtInit* _this, REFIID riid, void** ppvObject)
{
    return EPCategorizer_ICategorizer_QueryInterface((ICategorizer*)((char*)_this - sizeof(IShellExtInitVtbl*)), riid, ppvObject);
}

ULONG STDMETHODCALLTYPE EPCategorizer_IShellExtInit_AddRef(IShellExtInit* _this)
{
    return EPCategorizer_ICategorizer_AddRef((ICategorizer*)((char*)_this - sizeof(IShellExtInitVtbl*)));
}

ULONG STDMETHODCALLTYPE EPCategorizer_IShellExtInit_Release(IShellExtInit* _this)
{
    return EPCategorizer_ICategorizer_Release((ICategorizer*)((char*)_this - sizeof(IShellExtInitVtbl*)));
}

HRESULT STDMETHODCALLTYPE EPCategorizer_IShellExtInit_Initialize(IShellExtInit* _this, PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID)
{
    EPCategorizer* epCategorizer = (EPCategorizer*)((char*)_this - sizeof(IShellExtInitVtbl*));

    return SHBindToObject(NULL, pidlFolder, NULL, &IID_IShellFolder2, (void**)&epCategorizer->pShellFolder);
}

#pragma endregion

const ICategorizerVtbl EPCategorizer_categorizerVtbl = {
    EPCategorizer_ICategorizer_QueryInterface,
    EPCategorizer_ICategorizer_AddRef,
    EPCategorizer_ICategorizer_Release,
    EPCategorizer_ICategorizer_GetDescription,
    EPCategorizer_ICategorizer_GetCategory,
    EPCategorizer_ICategorizer_GetCategoryInfo,
    EPCategorizer_ICategorizer_CompareCategory
};

const IShellExtInitVtbl EPCategorizer_shellExtInitVtbl = {
    EPCategorizer_IShellExtInit_QueryInterface,
    EPCategorizer_IShellExtInit_AddRef,
    EPCategorizer_IShellExtInit_Release,
    EPCategorizer_IShellExtInit_Initialize
};

HRESULT(STDMETHODCALLTYPE *shell32_DriveTypeCategorizer_CreateInstanceFunc)(IUnknown* pUnkOuter, REFIID riid, void** ppvObject);

HRESULT shell32_DriveTypeCategorizer_CreateInstanceHook(IUnknown* pUnkOuter, REFIID riid, void** ppvObject)
{
    if (bUseClassicDriveGrouping && IsEqualIID(riid, &IID_ICategorizer))
    {
        EPCategorizer* epCategorizer = (EPCategorizer*) malloc(sizeof(EPCategorizer));
        epCategorizer->categorizer = &EPCategorizer_categorizerVtbl;
        epCategorizer->shellExtInit = &EPCategorizer_shellExtInitVtbl;
        epCategorizer->ulRefCount = 1;
        epCategorizer->pShellFolder = NULL;

        *ppvObject = epCategorizer;

        return S_OK;
    }

    return shell32_DriveTypeCategorizer_CreateInstanceFunc(pUnkOuter, riid, ppvObject);
}
#endif
#pragma endregion


#pragma region "File Explorer command bar and ribbon support"
DEFINE_GUID(CLSID_XamlIslandViewAdapter,
    0x6480100B,
    0x5A83, 0x4D1E, 0x9F, 0x69,
    0x8A, 0xE5, 0xA8, 0x8E, 0x9A, 0x33
);

DEFINE_GUID(CLSID_UIRibbonFramework,
    0x926749FA,
    0x2615, 0x4987, 0x88, 0x45,
    0xC3, 0x3E, 0x65, 0xF2, 0xB9, 0x57
);

DEFINE_GUID(IID_IUIRibbonFramework,
    0xF4F0385D,
    0x6872, 0x43A8, 0xAD, 0x09,
    0x4C, 0x33, 0x9C, 0xB3, 0xF5, 0xC5
);

HRESULT ExplorerFrame_CoCreateInstanceHook(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID* ppv)
{
    if (IsEqualCLSID(rclsid, &CLSID_XamlIslandViewAdapter))
    {
        if (dwFileExplorerCommandUI != 0 && dwFileExplorerCommandUI != 3 && dwFileExplorerCommandUI != 4)
            return REGDB_E_CLASSNOTREG;
    }
    else if (IsEqualCLSID(rclsid, &CLSID_UIRibbonFramework) && IsEqualIID(riid, &IID_IUIRibbonFramework))
    {
        if (dwFileExplorerCommandUI == 2)
            return REGDB_E_CLASSNOTREG;
    }
    return CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}
#pragma endregion


#pragma region "Change language UI style + Enable old taskbar"
#if WITH_MAIN_PATCHER
DEFINE_GUID(CLSID_TrayUIComponent,
    0x88FC85D3,
    0x7090, 0x4F53, 0x8F, 0x7A,
    0xEB, 0x02, 0x68, 0x16, 0x27, 0x88
);

HRESULT EPTrayUIComponent_CreateInstance(REFIID riid, void** ppvObject);

__declspec(dllexport) HRESULT explorer_CoCreateInstanceHook(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, void** ppv)
{
    if (IsEqualCLSID(rclsid, &CLSID_InputSwitchControl) && IsEqualIID(riid, &IID_IInputSwitchControl))
    {
        HRESULT hr = CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
        if (SUCCEEDED(hr) && bOldTaskbar && dwIMEStyle)
        {
            // The commented method below is no longer required as I have now came to creating a wrapper class.
            // Also, make sure to read the explanation below as well, it's useful for understanding how this worked.
            // Note: Other than in explorer.exe!CTrayInputIndicator::_RegisterInputSwitch, we're also called by
            // explorer.exe!HostAppEnvironment::_SetupInputSwitchServer which passes ISCT_IDL_USEROOBE (6) as the style.
            if (IsWindows11Version22H2OrHigher())
            {
                hr = CInputSwitchControlProxySV2_CreateInstance(*ppv, riid, ppv);
            }
            else
            {
                hr = CInputSwitchControlProxy_CreateInstance(*ppv, riid, ppv);
            }
            // Pff... how this works:
            // 
            // * This `CoCreateInstance` call will get a pointer to an IInputSwitchControl interface
            // (the call to here is made from `explorer!CTrayInputIndicator::_RegisterInputSwitch`);
            // the next call on this pointer will be on the `IInputSwitchControl::Init` function.
            // 
            // * `IInputSwitchControl::Init`'s second parameter is a number (x) which tells which
            // language switcher UI to prepare (check `IsUtil::MapClientTypeToString` in
            // `InputSwitch.dll`). "explorer" requests the "DESKTOP" UI (x = 0), which is the new
            // Windows 11 UI; if we replace that number with something else, some other UI will
            // be created
            // 
            // * ~~We cannot patch the vtable of the COM object because the executable is protected
            // by control flow guard and we would make a jump to an invalid site (maybe there is
            // some clever workaround fpr this as well, somehow telling the compiler to place a certain
            // canary before our trampoline, so it matches with what the runtime support for CFG expects,
            // but we'd have to keep that canary in sync with the one in explorer.exe, so not very
            // future proof).~~ Edit: Not true after all.
            // 
            // * Taking advantage of the fact that the call to `IInputSwitchControl::Init` is the thing
            // that happens right after we return from here, and looking on the disassembly, we see nothing
            // else changes `rdx` (which is the second argument to a function call), basically x, besides the
            // very `xor edx, edx` instruction before the call. Thus, we patch that out, and we also do
            // `mov edx, whatever` here; afterwards, we do NOTHING else, but just return and hope that
            // edx will stick
            // 
            // * Needless to say this is **HIGHLY** amd64
            /*
            char pattern[2] = {0x33, 0xD2};
            DWORD dwOldProtect;
            char* p_mov_edx_val = mov_edx_val;
            if (!ep_pf)
            {
                ep_pf = memmem(_ReturnAddress(), 200, pattern, 2);
                if (ep_pf)
                {
                    // Cancel out `xor edx, edx`
                    VirtualProtect(ep_pf, 2, PAGE_EXECUTE_READWRITE, &dwOldProtect);
                    memset(ep_pf, 0x90, 2);
                    VirtualProtect(ep_pf, 2, dwOldProtect, &dwOldProtect);
                }
                VirtualProtect(p_mov_edx_val, 6, PAGE_EXECUTE_READWRITE, &dwOldProtect);
            }
            if (ep_pf)
            {
                // Craft a "function" which does `mov edx, whatever; ret` and call it
                DWORD* pVal = mov_edx_val + 1;
                *pVal = dwIMEStyle;
                void(*pf_mov_edx_val)() = p_mov_edx_val;
                pf_mov_edx_val();
            }
            */
        }
        return hr;
    }
    else if (IsEqualCLSID(rclsid, &CLSID_TrayUIComponent) && IsEqualIID(riid, &IID_ITrayUIComponent))
    {
        if (bOldTaskbar && explorer_TrayUI_CreateInstanceFunc)
        {
            return EPTrayUIComponent_CreateInstance(riid, ppv);
        }
    }
    return CoCreateInstance(rclsid, pUnkOuter, dwClsContext, riid, ppv);
}

bool(*DisableWin10TaskbarIsEnabledFunc)(void* _this);
bool DisableWin10TaskbarIsEnabledHook(void* _this)
{
    return false;
}
#endif
#pragma endregion


#pragma region "Explorer Registry Hooks"
LSTATUS explorer_RegCreateKeyExW(
    HKEY hKey,
    LPCWSTR lpSubKey,
    DWORD Reserved,
    LPWSTR lpClass,
    DWORD dwOptions,
    REGSAM samDesired,
    const LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    PHKEY phkResult,
    LPDWORD lpdwDisposition
)
{
    if (!lstrcmpW(lpSubKey, L"MMStuckRects3"))
    {
        lpSubKey = L"MMStuckRectsLegacy";
    }
    else if (!lstrcmpW(lpSubKey, L"StuckRects3"))
    {
        lpSubKey = L"StuckRectsLegacy";
    }

    return RegCreateKeyExW(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

LSTATUS explorer_SHGetValueW(HKEY hkey, LPCWSTR pszSubKey, LPCWSTR pszValue, DWORD* pdwType, void* pvData, DWORD* pcbData)
{
    if (!lstrcmpW(pszSubKey, L"MMStuckRects3"))
    {
        pszSubKey = L"MMStuckRectsLegacy";
    }
    else if (!lstrcmpW(pszSubKey, L"StuckRects3"))
    {
        pszSubKey = L"StuckRectsLegacy";
    }

    return SHGetValueW(hkey, pszSubKey, pszValue, pdwType, pvData, pcbData);
}

IStream* explorer_OpenRegStream(HKEY hkey, PCWSTR pszSubkey, PCWSTR pszValue, DWORD grfMode)
{
    DWORD flOldProtect;

    if (!lstrcmpiW(pszValue, L"TaskbarWinXP")
        && VirtualProtect(pszValue, 0xC8, PAGE_EXECUTE_READWRITE, &flOldProtect))
    {
        lstrcpyW(pszValue, L"TaskbarWinEP");
        VirtualProtect(pszValue, 0xC8, flOldProtect, &flOldProtect);
    }

    return OpenRegStream(hkey, pszSubkey, pszValue, grfMode);
}

LSTATUS explorer_RegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, HKEY* phkResult)
{
    DWORD flOldProtect;

    if (!lstrcmpiW(lpSubKey, L"Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\TrayNotify")
        && VirtualProtect(lpSubKey, 0xC8ui64, 0x40u, &flOldProtect))
    {
        lstrcpyW(lpSubKey, L"Software\\Classes\\Local Settings\\Software\\Microsoft\\Windows\\CurrentVersion\\TrayNotSIB");
        VirtualProtect(lpSubKey, 0xC8ui64, flOldProtect, &flOldProtect);
    }

    return RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

LSTATUS explorer_RegSetValueExW(
    HKEY       hKey,
    LPCWSTR    lpValueName,
    DWORD      Reserved,
    DWORD      dwType,
    const BYTE* lpData,
    DWORD      cbData
)
{
    if (IsWindows11() && !lstrcmpW(lpValueName, L"ShowCortanaButton"))
    {
        if (cbData == sizeof(DWORD) && *(DWORD*)lpData == 1)
        {
            DWORD dwData = 2;
            return RegSetValueExW(hKey, L"TaskbarDa", Reserved, dwType, &dwData, cbData);
        }
        return RegSetValueExW(hKey, L"TaskbarDa", Reserved, dwType, lpData, cbData);
    }

    return RegSetValueExW(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

LSTATUS explorer_RegGetValueW(
    HKEY    hkey,
    LPCWSTR lpSubKey,
    LPCWSTR lpValue,
    DWORD   dwFlags,
    LPDWORD pdwType,
    PVOID   pvData,
    LPDWORD pcbData
)
{
    DWORD flOldProtect;
    BOOL bShowTaskViewButton = FALSE;
    LSTATUS lRes;

    if (IsWindows11() && !lstrcmpW(lpValue, L"ShowCortanaButton"))
    {
        lRes = RegGetValueW(hkey, lpSubKey, L"TaskbarDa", dwFlags, pdwType, pvData, pcbData);
        if (*(DWORD*)pvData == 2)
        {
            *(DWORD*)pvData = 1;
        }
    }
    /*else if (!lstrcmpW(lpValue, L"PeopleBand"))
    {
        lRes = RegGetValueW(hkey, lpSubKey, L"TaskbarMn", dwFlags, pdwType, pvData, pcbData);
    }*/
    else
    {
        lRes = RegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    }

    /*if (IsWindows11() && !lstrcmpW(lpValue, L"SearchboxTaskbarMode"))
    {
        if (*(DWORD*)pvData)
        {
            *(DWORD*)pvData = 1;
        }

        lRes = ERROR_SUCCESS;
    }*/

    return lRes;
}

HRESULT (*explorer_SHCreateStreamOnModuleResourceWFunc)(
    HMODULE hModule,
    LPCWSTR pwszName,
    LPCWSTR pwszType,
    IStream** ppStream
);

HRESULT WINAPI explorer_SHCreateStreamOnModuleResourceWHook(
    HMODULE hModule,
    LPCWSTR pwszName,
    LPCWSTR pwszType,
    IStream** ppStream
)
{
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(hModule, path, MAX_PATH);
    if ((*((WORD*)&(pwszName)+1)))
    {
        wprintf(L"%s - %s %s\n", path, pwszName, pwszType);
    }
    else
    {
        UINT uId = (UINT)(UINT_PTR)pwszName;
        wprintf(L"%s - %d %s\n", path, uId, pwszType);

        IStream* pStream = NULL;
        if (uId < 124)
        {
            if (S_Icon_Dark_TaskView)
            {
                pStream = SHCreateMemStream(P_Icon_Dark_TaskView, S_Icon_Dark_TaskView);
                if (pStream)
                {
                    *ppStream = pStream;
                    return S_OK;
                }
            }
        }
        else if (uId >= 151)
        {
            if (uId < 163)
            {
                if (S_Icon_Dark_Search)
                {
                    pStream = SHCreateMemStream(P_Icon_Dark_Search, S_Icon_Dark_Search);
                    if (pStream)
                    {
                        *ppStream = pStream;
                        return S_OK;
                    }
                }
            }

            if (uId < 201)
            {
                if (S_Icon_Light_Search)
                {
                    pStream = SHCreateMemStream(P_Icon_Light_Search, S_Icon_Light_Search);
                    if (pStream)
                    {
                        *ppStream = pStream;
                        return S_OK;
                    }
                }
            }

            if (uId < 213)
            {
                if (S_Icon_Dark_Widgets)
                {
                    printf(">>> %p %d\n", P_Icon_Dark_Widgets, S_Icon_Dark_Widgets);
                    pStream = SHCreateMemStream(P_Icon_Dark_Widgets, S_Icon_Dark_Widgets);
                    if (pStream)
                    {
                        *ppStream = pStream;
                        return S_OK;
                    }
                }
            }

            if (uId < 251)
            {
                if (S_Icon_Light_Widgets)
                {
                    pStream = SHCreateMemStream(P_Icon_Light_Widgets, S_Icon_Light_Widgets);
                    if (pStream)
                    {
                        *ppStream = pStream;
                        return S_OK;
                    }
                }
            }
        }
        else if (uId < 307)
        {
            if (S_Icon_Light_TaskView)
            {
                pStream = SHCreateMemStream(P_Icon_Light_TaskView, S_Icon_Light_TaskView);
                if (pStream)
                {
                    *ppStream = pStream;
                    return S_OK;
                }
            }
        }
    }
    return explorer_SHCreateStreamOnModuleResourceWFunc(hModule, pwszName, pwszType, ppStream);
}
#pragma endregion


#pragma region "Remember primary taskbar positioning"
BOOL bTaskbarFirstTimePositioning = FALSE;
BOOL bTaskbarSet = FALSE;

BOOL explorer_SetRect(LPRECT lprc, int xLeft, int yTop, int xRight, int yBottom)
{
    BOOL bIgnore = FALSE;
    if (bTaskbarFirstTimePositioning)
    {
        bIgnore = bTaskbarSet;
    }
    else
    {
        bTaskbarFirstTimePositioning = TRUE;
        bIgnore = (GetSystemMetrics(SM_CMONITORS) == 1);
        bTaskbarSet = bIgnore;
    }

    if (bIgnore)
    {
        return SetRect(lprc, xLeft, yTop, xRight, yBottom);
    }
    if (xLeft)
    {
        return SetRect(lprc, xLeft, yTop, xRight, yBottom);
    }
    if (yTop)
    {
        return SetRect(lprc, xLeft, yTop, xRight, yBottom);
    }
    if (xRight != GetSystemMetrics(SM_CXSCREEN))
    {
        return SetRect(lprc, xLeft, yTop, xRight, yBottom);
    }
    if (yBottom != GetSystemMetrics(SM_CYSCREEN))
    {
        return SetRect(lprc, xLeft, yTop, xRight, yBottom);
    }

    bTaskbarSet = TRUE;
    
    StuckRectsData srd;
    DWORD pcbData = sizeof(StuckRectsData);
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StuckRectsLegacy",
        L"Settings",
        REG_BINARY,
        NULL,
        &srd,
        &pcbData);

    if (pcbData != sizeof(StuckRectsData))
    {
        return SetRect(lprc, xLeft, yTop, xRight, yBottom);
    }

    if (srd.pvData[0] != sizeof(StuckRectsData))
    {
        return SetRect(lprc, xLeft, yTop, xRight, yBottom);
    }

    if (srd.pvData[1] != -2)
    {
        return SetRect(lprc, xLeft, yTop, xRight, yBottom);
    }

    HMONITOR hMonitor = MonitorFromRect(&(srd.rc), MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi;
    ZeroMemory(&mi, sizeof(MONITORINFO));
    mi.cbSize = sizeof(MONITORINFO);
    if (!GetMonitorInfoW(hMonitor, &mi))
    {
        return SetRect(lprc, xLeft, yTop, xRight, yBottom);
    }

    if (lprc)
    {
        *lprc = mi.rcMonitor;
        return TRUE;
    }

    return FALSE;
}
#pragma endregion


#pragma region "Disable Office Hotkeys"
const UINT office_hotkeys[10] = { 0x57, 0x54, 0x59, 0x4F, 0x50, 0x44, 0x4C, 0x58, 0x4E, 0x20 };
BOOL explorer_RegisterHotkeyHook(HWND hWnd, int id, UINT fsModifiers, UINT vk)
{
    if (bDisableOfficeHotkeys && fsModifiers == (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN | MOD_NOREPEAT) && (
        vk == office_hotkeys[0] ||
        vk == office_hotkeys[1] ||
        vk == office_hotkeys[2] ||
        vk == office_hotkeys[3] ||
        vk == office_hotkeys[4] ||
        vk == office_hotkeys[5] ||
        vk == office_hotkeys[6] ||
        vk == office_hotkeys[7] ||
        vk == office_hotkeys[8] ||
        vk == office_hotkeys[9] ||
        !vk))
    {
        SetLastError(ERROR_HOTKEY_ALREADY_REGISTERED);
        return FALSE;
    }

    BOOL result = RegisterHotKey(hWnd, id, fsModifiers, vk);

    static BOOL bWinBHotkeyRegistered = FALSE;
    if (!bWinBHotkeyRegistered && fsModifiers == (MOD_WIN | MOD_NOREPEAT) && vk == 'D') // right after Win+D
    {
#if USE_MOMENT_3_FIXES_ON_MOMENT_2
        BOOL bPerformMoment2Patches = IsWindows11Version22H2Build1413OrHigher() && bOldTaskbar;
#else
        BOOL bPerformMoment2Patches = bOldTaskbar ? bOldTaskbar == 1 ? IsWindows11Version22H2Build2134OrHigher() : IsWindows11Version22H2Build1413OrHigher() : FALSE;
#endif
        if (bPerformMoment2Patches)
        {
            // Might be better if we scan the GlobalKeylist array to prevent hardcoded numbers?
            RegisterHotKey(hWnd, 500, MOD_WIN | MOD_NOREPEAT, 'A');
            RegisterHotKey(hWnd, 514, MOD_WIN | MOD_NOREPEAT, 'B');
            RegisterHotKey(hWnd, 591, MOD_WIN | MOD_NOREPEAT, 'N');
            printf("Registered Win+A, Win+B, and Win+N\n");
        }
        bWinBHotkeyRegistered = TRUE;
    }

    return result;
}

BOOL twinui_RegisterHotkeyHook(HWND hWnd, int id, UINT fsModifiers, UINT vk)
{
    if (fsModifiers == (MOD_WIN | MOD_NOREPEAT) && vk == 'F')
    {
        SetLastError(ERROR_HOTKEY_ALREADY_REGISTERED);
        return FALSE;
    }
    return RegisterHotKey(hWnd, id, fsModifiers, vk);
}
#pragma endregion


#pragma region "Fix taskbar thumbnails and acrylic in newer OS builds (22572+)"
#if WITH_MAIN_PATCHER
HRESULT explorer_DwmUpdateThumbnailPropertiesHook(HTHUMBNAIL hThumbnailId, DWM_THUMBNAIL_PROPERTIES* ptnProperties)
{
    if (ptnProperties->dwFlags == 0 || ptnProperties->dwFlags == DWM_TNP_RECTSOURCE)
    {
        ptnProperties->dwFlags |= DWM_TNP_SOURCECLIENTAREAONLY;
        ptnProperties->fSourceClientAreaOnly = TRUE;
    }
    return DwmUpdateThumbnailProperties(hThumbnailId, ptnProperties);
}

void UpdateWindowAccentProperties_PatchAttribData(WINCOMPATTRDATA* pAttrData);

BOOL WINAPI explorer_SetWindowCompositionAttribute(HWND hWnd, WINCOMPATTRDATA* pData)
{
    if (bClassicThemeMitigations)
    {
        return TRUE;
    }
    if (bOldTaskbar && global_rovi.dwBuildNumber >= 22581 &&
        (GetClassWord(hWnd, GCW_ATOM) == RegisterWindowMessageW(L"Shell_TrayWnd") || 
         GetClassWord(hWnd, GCW_ATOM) == RegisterWindowMessageW(L"Shell_SecondaryTrayWnd")) && 
        pData->nAttribute == 19 && pData->pData && pData->ulDataSize == sizeof(ACCENTPOLICY))
    {
        UpdateWindowAccentProperties_PatchAttribData(pData);
    }
    return SetWindowCompositionAttribute(hWnd, pData);
}
#endif
#pragma endregion


#pragma region "Revert legacy copy dialog"
BOOL(*SHELL32_CanDisplayWin8CopyDialogFunc)();
BOOL SHELL32_CanDisplayWin8CopyDialogHook()
{
    if (bLegacyFileTransferDialog) return FALSE;
    return SHELL32_CanDisplayWin8CopyDialogFunc();
}
#pragma endregion


#pragma region "Windows Spotlight customization"
#if WITH_MAIN_PATCHER

HKEY hKeySpotlight1 = NULL;
HKEY hKeySpotlight2 = NULL;
BOOL bSpotlightIsDesktopContextMenu = FALSE;

LSTATUS shell32_RegCreateKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD Reserved, LPWSTR lpClass, DWORD dwOptions, REGSAM samDesired, const LPSECURITY_ATTRIBUTES lpSecurityAttributes, PHKEY phkResult, LPDWORD lpdwDisposition)
{
    if (bDisableSpotlightIcon && hKey == HKEY_CURRENT_USER && !_wcsicmp(lpSubKey, L"Software\\Classes\\CLSID\\{2cc5ca98-6485-489a-920e-b3e88a6ccce3}"))
    {
        LSTATUS lRes = RegCreateKeyExW(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
        if (lRes == ERROR_SUCCESS) hKeySpotlight1 = *phkResult;
        return lRes;
    }
    else if (hKeySpotlight1 && hKey == hKeySpotlight1 && !_wcsicmp(lpSubKey, L"ShellFolder"))
    {
        LSTATUS lRes = RegCreateKeyExW(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
        if (lRes == ERROR_SUCCESS) hKeySpotlight2 = *phkResult;
        return lRes;
    }
    return RegCreateKeyExW(hKey, lpSubKey, Reserved, lpClass, dwOptions, samDesired, lpSecurityAttributes, phkResult, lpdwDisposition);
}

LSTATUS shell32_RegSetValueExW(HKEY hKey, LPCWSTR lpValueName, DWORD Reserved, DWORD dwType, const BYTE* lpData, DWORD cbData)
{
    if (hKeySpotlight1 && hKeySpotlight2 && hKey == hKeySpotlight2 && !_wcsicmp(lpValueName, L"Attributes"))
    {
        hKeySpotlight1 = NULL;
        hKeySpotlight2 = NULL;
        DWORD dwAttributes = *(DWORD*)lpData | SFGAO_NONENUMERATED;
        SHFlushSFCache();
        return RegSetValueExW(hKey, lpValueName, Reserved, dwType, &dwAttributes, cbData);
    }
    return RegSetValueExW(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

BOOL shell32_DeleteMenu(HMENU hMenu, UINT uPosition, UINT uFlags)
{
    if (uPosition == 0x7053 && IsSpotlightEnabled() && dwSpotlightDesktopMenuMask) bSpotlightIsDesktopContextMenu = TRUE;
    return DeleteMenu(hMenu, uPosition, uFlags);
}

BOOL shell32_TrackPopupMenu(HMENU hMenu, UINT uFlags, int x, int y, int nReserved, HWND hWnd, const RECT* prcRect)
{
    if (IsSpotlightEnabled() && dwSpotlightDesktopMenuMask && (GetPropW(GetParent(hWnd), L"DesktopWindow") && (RegisterWindowMessageW(L"WorkerW") == GetClassWord(GetParent(hWnd), GCW_ATOM) || RegisterWindowMessageW(L"Progman") == GetClassWord(GetParent(hWnd), GCW_ATOM))) && bSpotlightIsDesktopContextMenu)
    {
        SpotlightHelper(dwSpotlightDesktopMenuMask, hWnd, hMenu, NULL);
    }
    bSpotlightIsDesktopContextMenu = FALSE;
    BOOL bRet = TrackPopupMenuHook(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    if (IsSpotlightEnabled() && dwSpotlightDesktopMenuMask)
    {
        MENUITEMINFOW mii;
        mii.cbSize = sizeof(MENUITEMINFOW);
        mii.fMask = MIIM_FTYPE | MIIM_DATA;
        if (GetMenuItemInfoW(hMenu, bRet, FALSE, &mii) && mii.dwItemData >= SPOP_CLICKMENU_FIRST && mii.dwItemData <= SPOP_CLICKMENU_LAST)
        {
            SpotlightHelper(mii.dwItemData, hWnd, hMenu, NULL);
        }
    }
    return bRet;
}
#endif
#pragma endregion


#pragma region "Fix Windows 10 taskbar high DPI button width bug"
#if WITH_MAIN_PATCHER
int patched_GetSystemMetrics(int nIndex)
{
    if ((bOldTaskbar && nIndex == SM_CXMINIMIZED) || nIndex == SM_CXICONSPACING || nIndex == SM_CYICONSPACING)
    {
        wchar_t wszDim[MAX_PATH + 4];
        ZeroMemory(wszDim, sizeof(wchar_t) * (MAX_PATH + 4));
        DWORD dwSize = MAX_PATH;
        wchar_t* pVal = L"MinWidth";
        if (nIndex == SM_CXICONSPACING) pVal = L"IconSpacing";
        else if (nIndex == SM_CYICONSPACING) pVal = L"IconVerticalSpacing";
        RegGetValueW(HKEY_CURRENT_USER, L"Control Panel\\Desktop\\WindowMetrics", pVal, SRRF_RT_REG_SZ, NULL, wszDim, &dwSize);
        int dwDim = _wtoi(wszDim);
        if (dwDim <= 0)
        {
            if (nIndex == SM_CXMINIMIZED) return 160;
            else if (dwDim < 0) return MulDiv(dwDim, GetDpiForSystem(), -1440);
        }
    }
    return GetSystemMetrics(nIndex);
}
#endif
#pragma endregion


#pragma region "Fix Windows 10 taskbar redraw problem on OS builds 22621+"
HWND Windows11v22H2_explorer_CreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    if ((*((WORD*)&(lpClassName)+1)) && !wcscmp(lpClassName, L"Shell_TrayWnd")) dwStyle |= WS_CLIPCHILDREN;
    return CreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
}
#pragma endregion


#pragma region "Shrink File Explorer address bar height"
int explorerframe_GetSystemMetricsForDpi(int nIndex, UINT dpi)
{
    if (bShrinkExplorerAddressBar && nIndex == SM_CYFIXEDFRAME) return IsWindows11() ? -3 : -1;
    return GetSystemMetricsForDpi(nIndex, dpi);
}

#if defined(_M_X64)
static void PatchAddressBarSizing(const MODULEINFO* mi)
{
    // <- means inlined

    PBYTE match;
    DWORD dwOldProtect;

    // Patch address bar positioning
    if (IsWindows11())
    {
        // CAddressBand::_PositionChildWindows()
        // 83 45 ?? ?? 83 6D ?? ?? 48
        //          xx To 03    xx To 01
        match = FindPattern(
            mi->lpBaseOfDll,
            mi->SizeOfImage,
            "\x83\x45\x00\x00\x83\x6D\x00\x00\x48",
            "xx??xx??x"
        );
        if (match && VirtualProtect(match, 9, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            match[3] = 3;
            match[7] = 1;
            VirtualProtect(match, 9, dwOldProtect, &dwOldProtect);
        }

        // CAddressBand::_AddressBandWndProc()
        // 83 45 ?? ?? 83 6D ?? ?? 0F
        //          xx To 03    xx To 01
        match = FindPattern(
            mi->lpBaseOfDll,
            mi->SizeOfImage,
            "\x83\x45\x00\x00\x83\x6D\x00\x00\x0F",
            "xx??xx??x"
        );
        if (match && VirtualProtect(match, 9, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            match[3] = 3;
            match[7] = 1;
            VirtualProtect(match, 9, dwOldProtect, &dwOldProtect);
        }
    }
    else
    {
        // Contaminated with some remnants of the ReportUsage of "SearchSuggestions" feature
        // CAddressBand::_PositionChildWindows()
        // 83 45 ?? ?? 48 8D 0D ?? ?? ?? ?? 45 33 C0 B2 01 E8 ?? ?? ?? ?? 83 6D ?? ?? 48
        //          xx To 03                                                       xx To 01
        match = FindPattern(
            mi->lpBaseOfDll,
            mi->SizeOfImage,
            "\x83\x45\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\x45\x33\xC0\xB2\x01\xE8\x00\x00\x00\x00\x83\x6D\x00\x00\x48",
            "xx??xxx????xxxxxx????xx??x"
        );
        if (match && VirtualProtect(match, 25, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            match[3] = 3;
            match[24] = 1;
            VirtualProtect(match, 25, dwOldProtect, &dwOldProtect);
        }

        // 83 45 ?? ?? 45 33 C0 B2 01 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 6D ?? ?? 0F
        //          xx To 03                                                       xx To 01
        match = FindPattern(
            mi->lpBaseOfDll,
            mi->SizeOfImage,
            "\x83\x45\x00\x00\x45\x33\xC0\xB2\x01\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\x6D\x00\x00\x0F",
            "xx??xxxxxxxx????x????xx??x"
        );
        if (match && VirtualProtect(match, 25, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            match[3] = 3;
            match[24] = 1;
            VirtualProtect(match, 25, dwOldProtect, &dwOldProtect);
        }
    }

    // Patch address bar height
    // CAddressBand::GetBandInfo() <- CAddressBand::CalculateBandHeight() <- BandSizing::BandSizingHelper::GetCalculatedBandHeight()
    // 41 8D 48 AA 48 FF 15 ?? ?? ?? ?? 0F 1F 44 00 ?? 03 F8 // 22621.2506/2715
    //          xx To 9E
    /*match = FindPattern(
        mi->lpBaseOfDll,
        mi->SizeOfImage,
        "\x41\x8D\x48\xAA\x48\xFF\x15\x00\x00\x00\x00\x0F\x1F\x44\x00\x00\x03\xF8",
        "xxxxxxx????xxxx?xx"
    );
    if (match && VirtualProtect(match, 7, PAGE_EXECUTE_READWRITE, &dwOldProtect))
    {
        match[3] = IsWindows11() ? 0x9E : 0xA0; // -98 : -96 -- -2 on Windows 11, 0 on Windows 10
        VirtualProtect(match, 7, dwOldProtect, &dwOldProtect);
    }
    else
    {
        // CAddressBand::GetBandInfo() <- CAddressBand::CalculateBandHeight() <- BandSizing::BandSizingHelper::GetCalculatedBandHeight()
    }

    // CAddressBand::CalculateBandHeight() <- BandSizing::BandSizingHelper::GetCalculatedBandHeight()
    // 41 8D 48 AA 48 FF 15 ?? ?? ?? ?? 0F 1F 44 00 ?? 48 8B // 22621.2506/2715
    //          xx To 9E
    match = FindPattern(
        mi->lpBaseOfDll,
        mi->SizeOfImage,
        "\x41\x8D\x48\xAA\x48\xFF\x15\x00\x00\x00\x00\x0F\x1F\x44\x00\x00\x03\xF8",
        "xxxxxxx????xxxx?xx"
    );
    if (match && VirtualProtect(match, 7, PAGE_EXECUTE_READWRITE, &dwOldProtect))
    {
        match[3] = IsWindows11() ? 0x9E : 0xA0; // -98 : -96 -- -2 on Windows 11, 0 on Windows 10
        VirtualProtect(match, 7, dwOldProtect, &dwOldProtect);
    }*/

    // Patch address band height
    // CAddressBand::GetBandInfo() <- CAddressBand::GetBandHeight()
    // 83 C7 10 45 85 ED 4C 8B 6C 24 // 22621.2506/2715
    // 83 C7 07 45 85 E4 4C 8B A4 24 // 19045.3393
    // 83 C7 ?? 45 85 ?? 4C 8B ?? 24
    //       xx To 04
    match = FindPattern(
        mi->lpBaseOfDll,
        mi->SizeOfImage,
        "\x83\xC7\x00\x45\x85\x00\x4C\x8B\x00\x24",
        "xx?xx?xx?x"
    );
    if (!match)
    {
        // CAddressBand::GetBandInfo() <- CAddressBand::GetBandHeight()
        // 83 C7 ?? 83 7C 24 ?? ?? 74 // 22621.1992
        //       xx To 04          ^^ short jnz
        match = FindPattern(
            mi->lpBaseOfDll,
            mi->SizeOfImage,
            "\x83\xC7\x00\x83\x7C\x24\x00\x00\x74",
            "xx?xxx??x"
        );
        if (!match)
        {
            // CAddressBand::GetBandInfo() <- CAddressBand::GetBandHeight()
            // 83 C7 ?? 83 7C 24 ?? ?? 0F 85 // 23560.1000
            //       xx To 04          ^^^^^ long jnz
            match = FindPattern(
                mi->lpBaseOfDll,
                mi->SizeOfImage,
                "\x83\xC7\x00\x83\x7C\x24\x00\x00\x0F\x85",
                "xx?xxx??xx"
            );
            if (!match)
            {
                // CAddressBand::GetBandHeight()
                // 8D 43 ?? 48 8B 4C 24 ?? 48 33 CC E8 // 25951
                //       xx To 04
                match = FindPattern(
                    mi->lpBaseOfDll,
                    mi->SizeOfImage,
                    "\x8D\x43\x00\x48\x8B\x4C\x24\x00\x48\x33\xCC\xE8",
                    "xx?xxxx?xxxx"
                );
            }
        }
    }
    if (match)
    {
        if (VirtualProtect(match, 3, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            match[2] = 4;
            VirtualProtect(match, 3, dwOldProtect, &dwOldProtect);
        }
    }

    /*// Patch search box height
    // BandSizing::BandSizingHelper::GetCalculatedBandHeight()
    // 8D 4D AA 44 8B C5 48 FF 15
    //       xx To 9E
    match = FindPattern(
        mi->lpBaseOfDll,
        mi->SizeOfImage,
        "\x8D\x4D\xAA\x44\x8B\xC5\x48\xFF\x15",
        "xxxxxxxxx"
    );
    if (match)
    {
        if (VirtualProtect(match, 9, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            match[2] = IsWindows11() ? 0x9E : 0xA0; // -98 : -96 -- -2 on Windows 11, 0 on Windows 10
            VirtualProtect(match, 9, dwOldProtect, &dwOldProtect);
        }
    }
    else
    {
        // B9 0A 00 00 00 48 FF 15 // Windows 10 and Windows 11 25951
        //    xxxxxxxxxxx To 0
        match = FindPattern(
            mi->lpBaseOfDll,
            mi->SizeOfImage,
            "\xB9\x0A\x00\x00\x00\x48\xFF\x15",
            "xxxxxxxx"
        );
        if (match && VirtualProtect(match, 8, PAGE_EXECUTE_READWRITE, &dwOldProtect))
        {
            *(int*)(match + 1) = IsWindows11() ? -2 : 0;
            VirtualProtect(match, 8, dwOldProtect, &dwOldProtect);
        }
    }*/

    if (IsWindows11())
    {
        // Windows 11 - Patch Up button size
        // CUpBand::ScaleAndSetPadding()
        // Inlined SHLogicalToPhysicalDPI()
        // 41 B8 60 00 00 00 41 8D 58 B1
        //                            xx To A8
        match = FindPattern(
            mi->lpBaseOfDll,
            mi->SizeOfImage,
            "\x41\xB8\x60\x00\x00\x00\x41\x8D\x58\xB1",
            "xxxxxxxxxx"
        );
        if (match)
        {
            if (VirtualProtect(match, 10, PAGE_EXECUTE_READWRITE, &dwOldProtect))
            {
                match[9] = 0xA8; // -88: 96 - 88 = 8
                VirtualProtect(match, 10, dwOldProtect, &dwOldProtect);
            }
        }
        else
        {
            // Non-inlined SHLogicalToPhysicalDPI()
            // 48 8B F1 48 8B 49 70 ?? ?? ?? ?? B8 ?? ?? ?? ?? // 23590, 25951
            //                                     xxxxxxxxxxx To 8
            match = FindPattern(
                mi->lpBaseOfDll,
                mi->SizeOfImage,
                "\x48\x8B\xF1\x48\x8B\x49\x70\x00\x00\x00\x00\xB8",
                "xxxxxxx????x"
            );
            if (match && VirtualProtect(match, 16, PAGE_EXECUTE_READWRITE, &dwOldProtect))
            {
                *(int*)(match + 12) = 8;
                VirtualProtect(match, 16, dwOldProtect, &dwOldProtect);
            }
        }
    }
}
#else
static void PatchAddressBarSizing(MODULEINFO* mi) {}
#endif
#pragma endregion


#pragma region "Fix taskbar cascade and tile windows options not working"
WORD explorer_TileWindows(
    HWND hwndParent,
    UINT wHow,
    const RECT* lpRect,
    UINT cKids,
    const HWND* lpKids
)
{
    return TileWindows((hwndParent == GetShellWindow()) ? GetDesktopWindow() : hwndParent, wHow, lpRect, cKids, lpKids);
}

WORD explorer_CascadeWindows(
    HWND hwndParent,
    UINT wHow,
    const RECT* lpRect,
    UINT cKids,
    const HWND* lpKids
)
{
    return CascadeWindows((hwndParent == GetShellWindow()) ? GetDesktopWindow() : hwndParent, wHow, lpRect, cKids, lpKids);
}
#pragma endregion


#pragma region "Fix explorer crashing on Windows builds lower than 25158"
HWND(*NtUserFindWindowEx)(HWND hWndParent, HWND hWndChildAfter, LPCWSTR lpszClass, LPCWSTR lpszWindow, DWORD dwType);
HWND user32_NtUserFindWindowExHook(HWND hWndParent, HWND hWndChildAfter, LPCWSTR lpszClass, LPCWSTR lpszWindow, DWORD dwType) {
    if (!NtUserFindWindowEx) NtUserFindWindowEx = GetProcAddress(GetModuleHandleW(L"win32u.dll"), "NtUserFindWindowEx");
    HWND hWnd = NULL;
    for (int i = 0; i < 5; ++i) {
        if (hWnd) break;
        hWnd = NtUserFindWindowEx(hWndParent, hWndChildAfter, lpszClass, lpszWindow, dwType);
    }
    return hWnd;
}
#pragma endregion


#pragma region "Infrastructure for reporting which OS features are enabled"
#pragma pack(push, 1)
struct RTL_FEATURE_CONFIGURATION {
    unsigned int featureId;
    unsigned __int32 group : 4;
    unsigned __int32 enabledState : 2;
    unsigned __int32 enabledStateOptions : 1;
    unsigned __int32 unused1 : 1;
    unsigned __int32 variant : 6;
    unsigned __int32 variantPayloadKind : 2;
    unsigned __int32 unused2 : 16;
    unsigned int payload;
};
#pragma pack(pop)

int (*RtlQueryFeatureConfigurationFunc)(UINT32 featureId, int sectionType, INT64* changeStamp, struct RTL_FEATURE_CONFIGURATION* buffer);
int RtlQueryFeatureConfigurationHook(UINT32 featureId, int sectionType, INT64* changeStamp, struct RTL_FEATURE_CONFIGURATION* buffer) {
    int rv = RtlQueryFeatureConfigurationFunc(featureId, sectionType, changeStamp, buffer);
    switch (featureId)
    {
#if !USE_MOMENT_3_FIXES_ON_MOMENT_2
        case 26008830: // STTest
        {
            if (bOldTaskbar == 1)
            {
                // Disable tablet optimized taskbar feature when using the Windows 10 taskbar
                //
                // For now, this fixes Task View and Win-Tab, Alt-Tab breaking after pressing Win-Tab,
                // flyouts alignment, notification center alignment, Windows key shortcuts on
                // OS builds 22621.1413+
                //
                // Removed in 22621.2134+
                //
                buffer->enabledState = FEATURE_ENABLED_STATE_DISABLED;
            }
            break;
        }
#endif
        case 37634385: // TIFE "Tabs in File Explorer"
        {
            if (dwFileExplorerCommandUI == 1     // Windows 10 Ribbon     <-- fixes saving of window position and size
                || dwFileExplorerCommandUI == 2  // Windows 7 Command Bar <-- fixes menu bar behavior
                || dwFileExplorerCommandUI == 3) // Windows 11 Command Bar (no Tabs, classic Address Bar) <-- provides option to disable tabs in File Explorer
            {
                // Removed in 23575.1000+
                // Crashing on 22635.2915
                if ((global_rovi.dwBuildNumber >= 22621 && global_rovi.dwBuildNumber <= 22635) && global_ubr >= 2915)
                    break;
                buffer->enabledState = FEATURE_ENABLED_STATE_DISABLED;
            }
            break;
        }
        case 40729001: // WASDKInFileExplorer - Removed in 22635.2915+
        case 42295138: // XAMLFolderViewSupport - Depends on WASDKInFileExplorer
        {
            if (dwFileExplorerCommandUI == 1     // Windows 10 Ribbon     <-- fixes crashing when navigating back to a WASDK view
                || dwFileExplorerCommandUI == 2  // Windows 7 Command Bar <-- ditto
                || dwFileExplorerCommandUI == 3) // Windows 11 Command Bar (no Tabs, classic Address Bar) <-- fixes crashing when opening an Explorer window
            {
                // Disable the new Windows App SDK views (in Home and Gallery) when not using the Windows 11 command bar
                //
                // There is an issue where Explorer crashes when one goes to a page with WASDK, goes to another page
                // without WASDK, and returning to a page with WASDK.
                //
                // However this also disables the new Gallery page altogether.
                // TODO- We have to find a way to either fix the crashing or make Gallery use the non WASDK view in the
                // TODO  same way as when Explorer is opened into Control Panel then going to Gallery.
                //
                // TODO- We cannot rely on feature flag patches because they will eventually be removed.
                //
                buffer->enabledState = FEATURE_ENABLED_STATE_DISABLED;
            }
            break;
        }
        case 40950262: // FEMNB "File Explorer Modern Navigation Bar"
        {
            if (dwFileExplorerCommandUI == 3     // Windows 11 Command Bar (no Tabs, classic Address Bar)
                || dwFileExplorerCommandUI == 4) // Windows 11 Command Bar (classic Address Bar)
            {
                // Option to disable the new navigation bar
                // Crashing on 22635.2915
                if ((global_rovi.dwBuildNumber >= 22621 && global_rovi.dwBuildNumber <= 22635) && global_ubr >= 2915)
                    break;
                buffer->enabledState = FEATURE_ENABLED_STATE_DISABLED;
            }
            break;
        }
#if 0
        case 42537950: // DisableWin10Taskbar
        {
            if (bOldTaskbar)
            {
                // Sorry Microsoft, but we need more time. Peace ✌️
                buffer->enabledState = FEATURE_ENABLED_STATE_DISABLED;
            }
            break;
        }
#endif
        case 44656322: // ID44656322
        {
            if (bOldTaskbar)
            {
                // This feature flag when enabled makes the flyouts disregard the left and right offsets, so that they
                // appear over the Copilot sidebar instead of beside it. Disabling this fixes start menu positioning
                // when the taskbar is at the left or right side, but it will make that behavior occur again.
                buffer->enabledState = FEATURE_ENABLED_STATE_DISABLED;
            }
            break;
        }
    }
    return rv;
}
#pragma endregion


DWORD InjectBasicFunctions(BOOL bIsExplorer, BOOL bInstall)
{
    //Sleep(150);

    HMODULE hShlwapi = LoadLibraryW(L"Shlwapi.dll");
    if (hShlwapi)
    {
        if (bInstall)
        {
            SHRegGetValueFromHKCUHKLMFunc = GetProcAddress(hShlwapi, "SHRegGetValueFromHKCUHKLM");
        }
        else
        {
            FreeLibrary(hShlwapi);
            FreeLibrary(hShlwapi);
        }
    }

    HANDLE hShell32 = LoadLibraryW(L"shell32.dll");
    if (hShell32)
    {
        if (bInstall)
        {
#if WITH_MAIN_PATCHER
            if (DoesOSBuildSupportSpotlight())
            {
                VnPatchIAT(hShell32, "user32.dll", "TrackPopupMenu", shell32_TrackPopupMenu);
            }
            else
            {
#endif
                VnPatchIAT(hShell32, "user32.dll", "TrackPopupMenu", TrackPopupMenuHook);
#if WITH_MAIN_PATCHER
            }
#endif
            if (bIsExplorerProcess)
            {
                HOOK_IMMERSIVE_MENUS(Shell32);
            }
            VnPatchIAT(hShell32, "user32.dll", "SystemParametersInfoW", DisableImmersiveMenus_SystemParametersInfoW);
            if (!bIsExplorer)
            {
                CreateWindowExWFunc = CreateWindowExW;
                VnPatchIAT(hShell32, "user32.dll", "CreateWindowExW", CreateWindowExWHook);
                SetWindowLongPtrWFunc = SetWindowLongPtrW;
                VnPatchIAT(hShell32, "user32.dll", "SetWindowLongPtrW", SetWindowLongPtrWHook);
            }
        }
        else
        {
            VnPatchIAT(hShell32, "user32.dll", "TrackPopupMenu", TrackPopupMenu);
            VnPatchIAT(hShell32, "user32.dll", "SystemParametersInfoW", SystemParametersInfoW);
            if (!bIsExplorer)
            {
                VnPatchIAT(hShell32, "user32.dll", "CreateWindowExW", CreateWindowExW);
                VnPatchIAT(hShell32, "user32.dll", "SetWindowLongPtrW", SetWindowLongPtrW);
            }
            FreeLibrary(hShell32);
            FreeLibrary(hShell32);
        }
    }

    HANDLE hShcore = LoadLibraryW(L"shcore.dll");
    if (hShcore)
    {
        if (bInstall)
        {
            explorerframe_SHCreateWorkerWindowFunc = GetProcAddress(hShcore, (LPCSTR)188);
        }
        else
        {
            FreeLibrary(hShcore);
            FreeLibrary(hShcore);
        }
    }

    HANDLE hExplorerFrame = LoadLibraryW(L"ExplorerFrame.dll");
    if (hExplorerFrame)
    {
        if (bInstall)
        {
            VnPatchIAT(hExplorerFrame, "user32.dll", "TrackPopupMenu", TrackPopupMenuHook);
            if (bIsExplorerProcess)
            {
                HOOK_IMMERSIVE_MENUS(ExplorerFrame);
            }
            VnPatchIAT(hExplorerFrame, "user32.dll", "SystemParametersInfoW", DisableImmersiveMenus_SystemParametersInfoW);
            VnPatchIAT(hExplorerFrame, "shcore.dll", (LPCSTR)188, explorerframe_SHCreateWorkerWindowHook);  // <<<SAB>>>
            if (!bIsExplorer)
            {
                CreateWindowExWFunc = CreateWindowExW;
                VnPatchIAT(hExplorerFrame, "user32.dll", "CreateWindowExW", CreateWindowExWHook);
                SetWindowLongPtrWFunc = SetWindowLongPtrW;
                VnPatchIAT(hExplorerFrame, "user32.dll", "SetWindowLongPtrW", SetWindowLongPtrWHook);
            }
            VnPatchIAT(hExplorerFrame, "API-MS-WIN-CORE-STRING-L1-1-0.DLL", "CompareStringOrdinal", ExplorerFrame_CompareStringOrdinal);
            VnPatchIAT(hExplorerFrame, "user32.dll", "GetSystemMetricsForDpi", explorerframe_GetSystemMetricsForDpi);
#if WITH_MAIN_PATCHER
            MODULEINFO mi;
            GetModuleInformation(GetCurrentProcess(), hExplorerFrame, &mi, sizeof(MODULEINFO));
            if (bShrinkExplorerAddressBar)
            {
                if ((global_rovi.dwBuildNumber >= 19041 && global_rovi.dwBuildNumber <= 19045 && global_ubr < 3754) || IsWindows11())
                {
                    PatchAddressBarSizing(&mi);
                }
            }
#endif
            VnPatchIAT(hExplorerFrame, "api-ms-win-core-com-l1-1-0.dll", "CoCreateInstance", ExplorerFrame_CoCreateInstanceHook);
        }
        else
        {
            VnPatchIAT(hExplorerFrame, "user32.dll", "TrackPopupMenu", TrackPopupMenu);
            VnPatchIAT(hExplorerFrame, "user32.dll", "SystemParametersInfoW", SystemParametersInfoW);
            VnPatchIAT(hExplorerFrame, "shcore.dll", (LPCSTR)188, explorerframe_SHCreateWorkerWindowFunc);
            if (!bIsExplorer)
            {
                VnPatchIAT(hExplorerFrame, "user32.dll", "CreateWindowExW", CreateWindowExW);
                VnPatchIAT(hExplorerFrame, "user32.dll", "SetWindowLongPtrW", SetWindowLongPtrW);
            }
            VnPatchIAT(hExplorerFrame, "API-MS-WIN-CORE-STRING-L1-1-0.DLL", "CompareStringOrdinal", CompareStringOrdinal);
            VnPatchIAT(hExplorerFrame, "user32.dll", "GetSystemMetricsForDpi", GetSystemMetricsForDpi);
            VnPatchIAT(hExplorerFrame, "api-ms-win-core-com-l1-1-0.dll", "CoCreateInstance", CoCreateInstance);
            FreeLibrary(hExplorerFrame);
            FreeLibrary(hExplorerFrame);
        }
    }

    HANDLE hWindowsUIFileExplorer = LoadLibraryW(L"Windows.UI.FileExplorer.dll");
    if (hWindowsUIFileExplorer)
    {
        if (bInstall)
        {
            VnPatchDelayIAT(hWindowsUIFileExplorer, "user32.dll", "TrackPopupMenu", TrackPopupMenuHook);
            VnPatchDelayIAT(hWindowsUIFileExplorer, "user32.dll", "SystemParametersInfoW", DisableImmersiveMenus_SystemParametersInfoW);
            if (!bIsExplorer)
            {
                CreateWindowExWFunc = CreateWindowExW;
                VnPatchIAT(hWindowsUIFileExplorer, "user32.dll", "CreateWindowExW", CreateWindowExWHook);
                SetWindowLongPtrWFunc = SetWindowLongPtrW;
                VnPatchIAT(hWindowsUIFileExplorer, "user32.dll", "SetWindowLongPtrW", SetWindowLongPtrWHook);
            }
        }
        else
        {
            VnPatchDelayIAT(hWindowsUIFileExplorer, "user32.dll", "TrackPopupMenu", TrackPopupMenu);
            VnPatchDelayIAT(hWindowsUIFileExplorer, "user32.dll", "SystemParametersInfoW", SystemParametersInfoW);
            if (!bIsExplorer)
            {
                VnPatchIAT(hWindowsUIFileExplorer, "user32.dll", "CreateWindowExW", CreateWindowExW);
                VnPatchIAT(hWindowsUIFileExplorer, "user32.dll", "SetWindowLongPtrW", SetWindowLongPtrW);
            }
            FreeLibrary(hWindowsUIFileExplorer);
            FreeLibrary(hWindowsUIFileExplorer);
        }
    }

    if (bInstall)
    {
        DWORD dwSize = sizeof(DWORD);
        RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", L"Start_ShowClassicMode", RRF_RT_DWORD, NULL, &dwStartShowClassicMode, &dwSize);
        if (!DoesWindows10StartMenuExist())
        {
            dwStartShowClassicMode = 0;
        }
    }

#if WITH_MAIN_PATCHER
    // As of writing, this function is never invoked with bInstall=TRUE, so we don't handle the case if it's false for now
    if (bIsExplorerProcess)
    {
        RtlQueryFeatureConfigurationFunc = GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "RtlQueryFeatureConfiguration");
        int rv = -1;
        if (RtlQueryFeatureConfigurationFunc)
        {
            rv = funchook_prepare(
                funchook,
                (void**)&RtlQueryFeatureConfigurationFunc,
                RtlQueryFeatureConfigurationHook
            );
        }
        if (rv != 0)
        {
            printf("Failed to hook RtlQueryFeatureConfiguration(). rv = %d\n", rv);
        }

        if (!bIsExplorer && IsXamlSoundsEnabled())
        {
            HANDLE hCombase = LoadLibraryW(L"combase.dll");
            VnPatchIAT(hCombase, "api-ms-win-core-libraryloader-l1-2-0.dll", "LoadLibraryExW", combase_LoadLibraryExW);
        }
    }
#endif

    return 0;
}


#pragma region "Enable EP weather on Windows Server SKUs"
#if WITH_MAIN_PATCHER
BOOL PeopleBand_IsOS(DWORD dwOS)
{
    if (dwOS == OS_ANYSERVER) return FALSE;
    return IsOS(dwOS);
}

BOOL explorer_IsOS(DWORD dwOS)
{
    if (dwOS == OS_ANYSERVER)
    {
        unsigned char buf[16];
        memcpy(buf, (uintptr_t)_ReturnAddress() - 16, 16);
        // check if call is made from explorer!PeopleBarPolicy::IsPeopleBarDisabled
        if (buf[0] == 0x48 && buf[1] == 0x83 && buf[2] == 0xec && buf[3] == 0x28 && buf[4] == 0xb9 &&
            buf[5] == 0x1d && buf[6] == 0x00 && buf[7] == 0x00 && buf[8] == 0x00) // sub rsp, 28h; mov ecx, 1dh
        {
            buf[0] = 0x48; buf[1] = 0x31; buf[2] = 0xc0; buf[3] = 0xc3; // xor rax, rax; ret
            DWORD flOldProtect = 0;
            if (VirtualProtect((uintptr_t)_ReturnAddress() - 16, 4, PAGE_EXECUTE_READWRITE, &flOldProtect))
            {
                memcpy((uintptr_t)_ReturnAddress() - 16, buf, 4);
                VirtualProtect((uintptr_t)_ReturnAddress() - 16, 4, flOldProtect, &flOldProtect);
                VnPatchIAT(GetModuleHandleW(NULL), "api-ms-win-shcore-sysinfo-l1-1-0.dll", "IsOS", IsOS);
                return FALSE;
            }
        }
    }
    return IsOS(dwOS);
}
#endif
#pragma endregion


#pragma region "Find offsets of needed functions when symbols are not available"
#if WITH_MAIN_PATCHER
void TryToFindExplorerOffsets(HANDLE hExplorer, MODULEINFO* pmiExplorer, DWORD* pOffsets)
{
    if (!pOffsets[0] || pOffsets[0] == 0xFFFFFFFF)
    {
        // ImmersiveTray::AttachWindowToTray()
#if defined(_M_X64)
        // 48 8B 93 ?? ?? ?? ?? 48 8B 8B ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B 4B
        //                                              ^^^^^^^^^^^
        // Ref: CTaskListThumbnailWnd::SetSite()
        PBYTE match = FindPattern(
            hExplorer, pmiExplorer->SizeOfImage,
            "\x48\x8B\x93\x00\x00\x00\x00\x48\x8B\x8B\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\x4B",
            "xxx????xxx????x????xxx"
        );
        if (match)
        {
            match += 14;
            pOffsets[0] = match + 5 + *(int*)(match + 1) - (PBYTE)hExplorer;
            printf("explorer.exe!ImmersiveTray::AttachWindowToTray() = %lX\n", pOffsets[0]);
        }
#endif
    }

    if (!pOffsets[1] || pOffsets[1] == 0xFFFFFFFF)
    {
        // ImmersiveTray::RaiseWindow()
#if defined(_M_X64)
        // 41 B9 02 00 00 00 48 8B 8B ?? ?? ?? ?? E8 ?? ?? ?? ?? 85 C0
        //                                           ^^^^^^^^^^^
        // Ref: CTaskListThumbnailWnd::_RaiseWindowForLivePreviewIfNeeded()
        PBYTE match = FindPattern(
            hExplorer, pmiExplorer->SizeOfImage,
            "\x41\xB9\x02\x00\x00\x00\x48\x8B\x8B\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x85\xC0",
            "xxxxxxxxx????x????xx"
        );
        if (match)
        {
            match += 13;
            pOffsets[1] = match + 5 + *(int*)(match + 1) - (PBYTE)hExplorer;
            printf("explorer.exe!ImmersiveTray::RaiseWindow() = %lX\n", pOffsets[1]);
        }
#endif
    }

    if (!pOffsets[2] || pOffsets[2] == 0xFFFFFFFF)
    {
        // CTaskBand_CreateInstance()
#if defined(_M_X64)
        // Pre-24H2 (output variable uninitialized)
        // Tested: 19041.3758, 22000.51, 22621.1992
        // 48 8B F1 4C 8D 44 24 ?? 48 8B 49 ?? 33 D2 E8 ?? ?? ?? ??
        //                                              ^^^^^^^^^^^
        // Ref: CTrayBandSite::_AddRequiredBands()
        PBYTE match = FindPattern(
            hExplorer, pmiExplorer->SizeOfImage,
            "\x48\x8B\xF1\x4C\x8D\x44\x24\x00\x48\x8B\x49\x00\x33\xD2\xE8",
            "xxxxxxx?xxx?xxx"
        );
        if (match)
        {
            match += 14;
            pOffsets[2] = match + 5 + *(int*)(match + 1) - (PBYTE)hExplorer;
        }
        else
        {
            // 24H2 (output variable initialized to 0)
            // Tested: 25951, 26080
            // 4C 8D 40 ?? 48 8B F1 33 D2 48 8B 49 ?? E8 ?? ?? ?? ??
            //                                           ^^^^^^^^^^^
            // Ref: CTrayBandSite::_AddRequiredBands()
            match = FindPattern(
                hExplorer, pmiExplorer->SizeOfImage,
                "\x4C\x8D\x40\x00\x48\x8B\xF1\x33\xD2\x48\x8B\x49\x00\xE8",
                "xxx?xxxxxxxx?x"
            );
            if (match)
            {
                match += 13;
                pOffsets[2] = match + 5 + *(int*)(match + 1) - (PBYTE)hExplorer;
            }
        }
        if (match)
        {
            printf("explorer.exe!CTaskBand_CreateInstance() = %lX\n", pOffsets[2]);
        }
#endif
    }

    if (!pOffsets[3] || pOffsets[3] == 0xFFFFFFFF)
    {
        // HandleFirstTimeLegacy()
#if defined(_M_X64)
        // Short Jump
        // Tested: 19045.3758, 22000.51, 25951, 26080
        // 4D 85 ?? 74 ?? 49 83 ?? 01 75 ??             E8 ?? ?? ?? ??
        //                                                 ^^^^^^^^^^^
        // Ref: TrayUI::WndProc()
        PBYTE match = FindPattern(
            hExplorer, pmiExplorer->SizeOfImage,
            "\x4D\x85\x00\x74\x00\x49\x83\x00\x01\x75\x00\xE8",
            "xx?x?xx?xx?x"
        );
        if (match)
        {
            match += 11;
            pOffsets[3] = match + 5 + *(int*)(match + 1) - (PBYTE)hExplorer;
        }
        else
        {
            // Long Jump
            // Tested: 22621.1992
            // 4D 85 ?? 74 ?? 49 83 ?? 01 0F 85 ?? ?? ?? ?? E8 ?? ?? ?? ??
            //                                                 ^^^^^^^^^^^
            match = FindPattern(
                hExplorer, pmiExplorer->SizeOfImage,
                "\x4D\x85\x00\x74\x00\x49\x83\x00\x01\x0F\x85\x00\x00\x00\x00\xE8",
                "xx?x?xx?xxx????x"
            );
            if (match)
            {
                match += 15;
                pOffsets[3] = match + 5 + *(int*)(match + 1) - (PBYTE)hExplorer;
            }
        }
        if (match)
        {
            printf("explorer.exe!HandleFirstTimeLegacy() = %lX\n", pOffsets[3]);
        }
#endif
    }

    if (!pOffsets[4] || pOffsets[4] == 0xFFFFFFFF)
    {
#if defined(_M_X64)
        // SetColorPreferenceForLogonUI()
        // Ref: TrayUI::_HandleSettingChange()
        // 48 8B F9 E8 ?? ?? ?? ?? 8B D8 85 C0 78 ?? 48 8B CF E8 ?? ?? ?? ??
        //                                                       ^^^^^^^^^^^
        PBYTE match = FindPattern(
            hExplorer, pmiExplorer->SizeOfImage,
            "\x48\x8B\xF9\xE8\x00\x00\x00\x00\x8B\xD8\x85\xC0\x78\x00\x48\x8B\xCF\xE8",
            "xxxx????xxxxx?xxxx"
        );
        if (match)
        {
            match += 17;
            pOffsets[4] = match + 5 + *(int*)(match + 1) - (PBYTE)hExplorer;
            printf("explorer.exe!SetColorPreferenceForLogonUI() = %lX\n", pOffsets[4]);
        }
#endif
    }
}
#endif
#pragma endregion


#pragma region "Fix Pin to Start from Explorer not working when using Windows 10 start menu"
#if WITH_MAIN_PATCHER
extern HRESULT AppResolver_StartTileData_RoGetActivationFactory(HSTRING activatableClassId, REFIID iid, void** factory);

typedef struct CCacheShortcut CCacheShortcut;
extern HRESULT(*AppResolver_CAppResolverCacheBuilder__AddUserPinnedShortcutToStartFunc)(void* _this, const CCacheShortcut* a2, const void* a3);
extern HRESULT AppResolver_CAppResolverCacheBuilder__AddUserPinnedShortcutToStart(void* _this, const CCacheShortcut* a2, const void* a3);

static void PatchAppResolver()
{
    HANDLE hAppResolver = LoadLibraryW(L"AppResolver.dll");
    MODULEINFO miAppResolver;
    GetModuleInformation(GetCurrentProcess(), hAppResolver, &miAppResolver, sizeof(MODULEINFO));

    VnPatchDelayIAT(hAppResolver, "api-ms-win-core-winrt-l1-1-0.dll", "RoGetActivationFactory", AppResolver_StartTileData_RoGetActivationFactory);

    // CAppResolverCacheBuilder::_AddUserPinnedShortcutToStart()
#if defined(_M_X64)
    // 8B ? 48 8B D3 E8 ? ? ? ? 48 8B 8D
    //                  ^^^^^^^
    // Ref: CAppResolverCacheBuilder::_AddShortcutToCache()
    PBYTE match = FindPattern(
        hAppResolver,
        miAppResolver.SizeOfImage,
        "\x8B\x00\x48\x8B\xD3\xE8\x00\x00\x00\x00\x48\x8B\x8D",
        "x?xxxx????xxx"
    );
    if (match)
    {
        match += 5;
        match += 5 + *(int*)(match + 1);
    }
#elif defined(_M_ARM64)
    // 7F 23 03 D5  FD 7B BC A9  F3 53 01 A9  F5 5B 02 A9  F7 1B 00 F9  FD 03 00 91  ?? ?? ?? ??  FF 43 01 D1  F7 03 00 91  30 00 80 92  F0 1A 00 F9  ?? 03 01 AA  ?? 03 02 AA  FF ?? 00 F9
    // ----------- PACIBSP, don't scan for this because it's everywhere
    PBYTE match = FindPattern(
        hAppResolver,
        miAppResolver.SizeOfImage,
        "\xFD\x7B\xBC\xA9\xF3\x53\x01\xA9\xF5\x5B\x02\xA9\xF7\x1B\x00\xF9\xFD\x03\x00\x91\x00\x00\x00\x00\xFF\x43\x01\xD1\xF7\x03\x00\x91\x30\x00\x80\x92\xF0\x1A\x00\xF9\x00\x03\x01\xAA\x00\x03\x02\xAA\xFF\x00\x00\xF9",
        "xxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxxxxxx?xxx?xxxx?xx"
    );
    if (match)
    {
        match -= 4;
    }
#endif
    if (match)
    {
        AppResolver_CAppResolverCacheBuilder__AddUserPinnedShortcutToStartFunc = match;
        printf("CAppResolverCacheBuilder::_AddUserPinnedShortcutToStart() = %llX\n", match - (PBYTE)hAppResolver);
    }

    int rv = -1;
    if (AppResolver_CAppResolverCacheBuilder__AddUserPinnedShortcutToStartFunc)
    {
        rv = funchook_prepare(
            funchook,
            (void**)&AppResolver_CAppResolverCacheBuilder__AddUserPinnedShortcutToStartFunc,
            AppResolver_CAppResolverCacheBuilder__AddUserPinnedShortcutToStart
        );
    }
    if (rv != 0)
    {
        printf("Failed to hook CAppResolverCacheBuilder::_AddUserPinnedShortcutToStart(). rv = %d\n", rv);
    }
}

extern HRESULT PatchStartTileDataFurther(HMODULE hModule, BOOL bSMEH);

static void PatchStartTileData(BOOL bSMEH)
{
    HANDLE hStartTileData = LoadLibraryW(L"StartTileData.dll");

    VnPatchIAT(hStartTileData, "api-ms-win-core-winrt-l1-1-0.dll", "RoGetActivationFactory", AppResolver_StartTileData_RoGetActivationFactory);

    if (!bSMEH)
        return;

    if ((global_rovi.dwBuildNumber >= 22621 && global_rovi.dwBuildNumber <= 22635) && global_ubr >= 3420
        || global_rovi.dwBuildNumber >= 25169)
    {
        PatchStartTileDataFurther(hStartTileData, bSMEH);
        /*HRESULT hr = CoInitialize(NULL);
        if (SUCCEEDED(hr))
        {
            PatchStartTileDataFurther(hStartTileData, bSMEH);
        }
        if (hr == S_OK)
        {
            CoUninitialize();
        }*/
    }
}
#endif
#pragma endregion


#pragma region "Crash counter system"
#if WITH_MAIN_PATCHER
typedef struct CrashCounterSettings
{
    BOOL bDisabled;
    int counter;
    int thresholdTime;
    int threshold;
} CrashCounterSettings;

void GetCrashCounterSettings(CrashCounterSettings* out)
{
    out->bDisabled = FALSE;
    out->counter = 0;
    out->thresholdTime = 10000;
    out->threshold = 3;

    DWORD dwTCSize = sizeof(DWORD);
    RegGetValueW(HKEY_CURRENT_USER, _T(REGPATH), L"CrashCounterDisabled", RRF_RT_DWORD, NULL, &out->bDisabled, &dwTCSize);
    if (out->bDisabled != FALSE && out->bDisabled != TRUE) out->bDisabled = FALSE;

    dwTCSize = sizeof(DWORD);
    RegGetValueW(HKEY_CURRENT_USER, _T(REGPATH), L"CrashCounter", RRF_RT_DWORD, NULL, &out->counter, &dwTCSize);
    if (out->counter < 0) out->counter = 0;

    dwTCSize = sizeof(DWORD);
    RegGetValueW(HKEY_CURRENT_USER, _T(REGPATH), L"CrashThresholdTime", RRF_RT_DWORD, NULL, &out->thresholdTime, &dwTCSize);
    if (out->thresholdTime < 100 || out->thresholdTime > 60000) out->thresholdTime = 10000;

    dwTCSize = sizeof(DWORD);
    RegGetValueW(HKEY_CURRENT_USER, _T(REGPATH), L"CrashCounterThreshold", RRF_RT_DWORD, NULL, &out->threshold, &dwTCSize);
    if (out->threshold <= 1 || out->threshold >= 10) out->threshold = 3;
}

BOOL IsCrashCounterEnabled()
{
    CrashCounterSettings cfg;
    GetCrashCounterSettings(&cfg);
    return !cfg.bDisabled;
}

HRESULT InformUserAboutCrashCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, LONG_PTR lpRefData)
{
    if (msg == TDN_HYPERLINK_CLICKED)
    {
        wchar_t* link = (wchar_t*)lParam;
        if (wcschr(link, L'\''))
        {
            size_t len = wcslen(link);
            for (size_t i = 0; i < len; ++i)
            {
                if (link[i] == L'\'')
                    link[i] = L'"';
            }
            STARTUPINFO si;
            PROCESS_INFORMATION pi;
            ZeroMemory(&si, sizeof(si));
            si.cb = sizeof(si);
            ZeroMemory(&pi, sizeof(pi));
            if (CreateProcessW(NULL, link, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
            {
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            }
            for (size_t i = 0; i < len; ++i)
            {
                if (link[i] == L'"')
                    link[i] = L'\'';
            }
        }
        else if (!wcsncmp(link, L"eplink://update", 15))
        {
            if (!wcsncmp(link, L"eplink://update/stable", 22))
            {
                DWORD no = 0;
                RegSetKeyValueW(HKEY_CURRENT_USER, _T(REGPATH), L"UpdatePreferStaging", REG_DWORD, &no, sizeof(DWORD));
            }
            else if (!wcsncmp(link, L"eplink://update/staging", 23))
            {
                DWORD yes = 1;
                RegSetKeyValueW(HKEY_CURRENT_USER, _T(REGPATH), L"UpdatePreferStaging", REG_DWORD, &yes, sizeof(DWORD));
            }
            SetLastError(0);
            HANDLE sigInstallUpdates = CreateEventW(NULL, FALSE, FALSE, L"EP_Ev_InstallUpdates_" _T(EP_CLSID));
            if (sigInstallUpdates && GetLastError() == ERROR_ALREADY_EXISTS)
            {
                SetEvent(sigInstallUpdates);
            }
            else
            {
                CreateThread(NULL, 0, CheckForUpdatesThread, 0, 0, NULL);
                Sleep(100);
                SetLastError(0);
                sigInstallUpdates = CreateEventW(NULL, FALSE, FALSE, L"EP_Ev_InstallUpdates_" _T(EP_CLSID));
                if (sigInstallUpdates && GetLastError() == ERROR_ALREADY_EXISTS)
                {
                    SetEvent(sigInstallUpdates);
                }
            }
        }
        else
        {
            ShellExecuteW(NULL, L"open", link, L"", NULL, SW_SHOWNORMAL);
        }
        return S_FALSE;
    }
    return S_OK;

}

DWORD InformUserAboutCrash(LPVOID unused)
{
    CrashCounterSettings cfg;
    GetCrashCounterSettings(&cfg);

    EP_L10N_ApplyPreferredLanguageForCurrentThread();
    HMODULE hEPGui = LoadGuiModule();
    if (!hEPGui)
    {
        return 0; // Can't load the strings, nothing to display
    }

    wchar_t title[100];
    LoadStringW(hEPGui, IDS_CRASH_TITLE, title, ARRAYSIZE(title));

    wchar_t times[32];
    ZeroMemory(times, sizeof(times));
    if (cfg.threshold == 1)
    {
        LoadStringW(hEPGui, IDS_CRASH_ONCE, times, ARRAYSIZE(times));
    }
    else
    {
        wchar_t format[32];
        if (LoadStringW(hEPGui, IDS_CRASH_MULTIPLE, format, ARRAYSIZE(format)) != 0)
        {
            swprintf_s(times, ARRAYSIZE(times), format, cfg.threshold);
        }
    }

    wchar_t uninstallLink[MAX_PATH];
    ZeroMemory(uninstallLink, sizeof(uninstallLink));
    uninstallLink[0] = L'\'';
    SHGetFolderPathW(NULL, SPECIAL_FOLDER, NULL, SHGFP_TYPE_CURRENT, uninstallLink + 1);
    wcscat_s(uninstallLink, MAX_PATH, _T(APP_RELATIVE_PATH) L"\\" _T(SETUP_UTILITY_NAME) L"' /uninstall");

    wchar_t bodyFormat[10000];
    if (LoadStringW(hEPGui, IDS_CRASH_BODY, bodyFormat, ARRAYSIZE(bodyFormat)) == 0)
    {
        FreeLibrary(hEPGui);
        return 0;
    }

    wchar_t msg[10000];
    _swprintf_p(msg, ARRAYSIZE(msg), bodyFormat,
        times, cfg.thresholdTime / 1000,                                // 1 & 2: (once|X times) in less than Y seconds
        L"eplink://update",                                             // 3: [update ExplorerPatcher and restart File Explorer]
        L"https://github.com/valinet/ExplorerPatcher/releases",         // 4: [view releases]
        L"https://github.com/valinet/ExplorerPatcher/discussions/1102", // 5: [check the current status]
        L"https://github.com/valinet/ExplorerPatcher/discussions",      // 6: [discuss]
        L"https://github.com/valinet/ExplorerPatcher/issues",           // 7: [review the latest issues]
        uninstallLink                                                   // 8: [uninstall ExplorerPatcher]
    );

    wchar_t dismiss[32];
    LoadStringW(hEPGui, IDS_CRASH_DISMISS, dismiss, ARRAYSIZE(dismiss));

    TASKDIALOG_BUTTON buttons[1];
    buttons[0].nButtonID = IDNO;
    buttons[0].pszButtonText = dismiss;

    TASKDIALOGCONFIG td;
    ZeroMemory(&td, sizeof(td));
    td.cbSize = sizeof(TASKDIALOGCONFIG);
    td.hInstance = hModule;
    td.hwndParent = NULL;
    td.dwFlags = TDF_SIZE_TO_CONTENT | TDF_ENABLE_HYPERLINKS;
    td.pszWindowTitle = L"ExplorerPatcher";
    td.pszMainInstruction = title;
    td.pszContent = msg;
    td.cButtons = ARRAYSIZE(buttons);
    td.pButtons = buttons;
    td.cRadioButtons = 0;
    td.pRadioButtons = NULL;
    td.cxWidth = 0;
    td.pfCallback = InformUserAboutCrashCallback;

    HMODULE hComCtl32 = NULL;
    HRESULT(*pfTaskDialogIndirect)(const TASKDIALOGCONFIG*, int*, int*, BOOL*) = NULL;
    int res = td.nDefaultButton;
    if (!(hComCtl32 = GetModuleHandleA("Comctl32.dll")) ||
        !(pfTaskDialogIndirect = GetProcAddress(hComCtl32, "TaskDialogIndirect")) ||
        FAILED(pfTaskDialogIndirect(&td, &res, NULL, NULL)))
    {
        wcscat_s(msg, 10000, L" Would you like to open the ExplorerPatcher status web page on GitHub in your default browser?");
        res = MessageBoxW(NULL, msg, L"ExplorerPatcher", MB_ICONASTERISK | MB_YESNO);
    }
    if (res == IDYES)
    {
        ShellExecuteW(NULL, L"open", L"https://github.com/valinet/ExplorerPatcher/discussions/1102", L"", NULL, SW_SHOWNORMAL);
    }
    FreeLibrary(hEPGui);
    return 0;
}

DWORD WINAPI ClearCrashCounter(INT64 timeout)
{
    Sleep(timeout);
    DWORD zero = 0;
    RegSetKeyValueW(HKEY_CURRENT_USER, _T(REGPATH), L"CrashCounter", REG_DWORD, &zero, sizeof(DWORD));
    return 0;
}

BOOL CrashCounterHandleEntryPoint()
{
    CrashCounterSettings cfg;
    GetCrashCounterSettings(&cfg);
    if (!cfg.bDisabled)
    {
        // Ctrl + Shift + Alt
        BOOL bKeyCombinationTriggered = GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_SHIFT) & 0x8000 && GetAsyncKeyState(VK_MENU) & 0x8000;
        if (cfg.counter >= cfg.threshold || bKeyCombinationTriggered)
        {
            cfg.counter = 0;
            RegSetKeyValueW(HKEY_CURRENT_USER, _T(REGPATH), L"CrashCounter", REG_DWORD, &cfg.counter, sizeof(DWORD));
            SHCreateThread(InformUserAboutCrash, NULL, 0, NULL);
            return TRUE;
        }
        cfg.counter++;
        RegSetKeyValueW(HKEY_CURRENT_USER, _T(REGPATH), L"CrashCounter", REG_DWORD, &cfg.counter, sizeof(DWORD));
        SHCreateThread(ClearCrashCounter, cfg.thresholdTime, 0, NULL);
    }
    return FALSE;
}
#endif
#pragma endregion


#pragma region "Loader for alternate taskbar implementation"
#if WITH_MAIN_PATCHER
BOOL CheckExplorerSymbols(symbols_addr* symbols_PTRS)
{
    BOOL bAllValid = TRUE;
    /*for (SIZE_T j = 0; j < ARRAYSIZE(symbols_PTRS->explorer_PTRS) - 1; ++j)
    {
        DWORD i = symbols_PTRS->explorer_PTRS[j];
        bAllValid &= i && i != 0xFFFFFFFF;
        if (!bAllValid)
            break;
    }*/
    return bAllValid;
}

const WCHAR* GetTaskbarDllChecked(symbols_addr* symbols_PTRS)
{
    const WCHAR* pszTaskbarDll = PickTaskbarDll();
    if (!pszTaskbarDll)
    {
        wprintf(L"[TB] Unsupported build\n");
        return NULL;
    }
    if (!CheckExplorerSymbols(symbols_PTRS))
    {
        wprintf(L"[TB] Missing offsets\n");
        return NULL;
    }
    return pszTaskbarDll;
}

// Behavior based on selected Taskbar style:
// - Windows 11: Load our taskbar DLL with LOAD_LIBRARY_AS_DATAFILE for the old context menu
// - Windows 10: Skip loading
// - Windows 10 (ExplorerPatcher): Load it fully
typedef enum _EP_TASKBAR_FEATURES
{
    EPTF_None,
    EPTF_Taskbar = 0x1,
    EPTF_ClassicContextMenu = 0x2,
    EPTF_WinBlueLauncher = 0x4,
    EPTF_AudioFlyout = 0x8,

    EPTF_FullLoad =
        EPTF_Taskbar
      | EPTF_WinBlueLauncher
      | EPTF_AudioFlyout
} EP_TASKBAR_FEATURES;

EP_TASKBAR_FEATURES GetEPTaskbarFeatures()
{
    EP_TASKBAR_FEATURES eptf = EPTF_None;

    if (IsWindows11() && bOldTaskbar == 0)
    {
        eptf |= EPTF_ClassicContextMenu;
    }
    if (bOldTaskbar >= 2)
    {
        eptf |= EPTF_Taskbar;
    }

    BOOL fValue = FALSE;
    if (SUCCEEDED(SHRegGetBOOLWithREGSAM(HKEY_CURRENT_USER, L"Software\\ExplorerPatcher", L"UseImmersiveLauncher", 0, &fValue)) && fValue)
    {
        eptf |= EPTF_WinBlueLauncher;
    }

    DWORD dwValue = 0;
    if (SUCCEEDED(SHRegGetDWORD(HKEY_CURRENT_USER, L"Software\\ExplorerPatcher", L"AudioFlyoutStyle", &dwValue)) && dwValue == 1)
    {
        eptf |= EPTF_AudioFlyout;
    }

    return eptf;
}

HMODULE PrepareAlternateTaskbarImplementation(symbols_addr* symbols_PTRS, const WCHAR* pszTaskbarDll)
{
    if (!symbols_PTRS || !pszTaskbarDll)
    {
        return NULL;
    }

    EP_TASKBAR_FEATURES eptf = GetEPTaskbarFeatures();
    if (eptf == EPTF_None)
    {
        return NULL;
    }

    wchar_t szPath[MAX_PATH];
    ZeroMemory(szPath, sizeof(szPath));
    SHGetFolderPathW(NULL, SPECIAL_FOLDER, NULL, SHGFP_TYPE_CURRENT, szPath);
    wcscat_s(szPath, MAX_PATH, _T(APP_RELATIVE_PATH) L"\\");
    wcscat_s(szPath, MAX_PATH, pszTaskbarDll);
    BOOL bFullLoad = (eptf & EPTF_FullLoad) != 0;
    HMODULE hMyTaskbar = bFullLoad ? LoadLibraryW(szPath) : LoadLibraryExW(szPath, NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (!hMyTaskbar)
    {
        wprintf(L"[TB] '%s' not found\n", pszTaskbarDll);
        return NULL;
    }
    g_hMyTaskbar = hMyTaskbar;

    if (!bFullLoad)
    {
        return NULL; // Prevent IAT hooks from being carried out
    }

    typedef DWORD (*GetVersion_t)();
    GetVersion_t GetVersion = (GetVersion_t)GetProcAddress(hMyTaskbar, "GetVersion");
    DWORD version = GetVersion ? GetVersion() : 0;
    if (version != 2)
    {
        wprintf(L"[TB] '%s' with version %d is not compatible\n", pszTaskbarDll, version);
        FreeLibrary(hMyTaskbar);
        return NULL;
    }

    if ((eptf & EPTF_Taskbar) != 0)
    {
        TrayUI_CreateInstance_t pfnMyTrayUICreateInstance = (TrayUI_CreateInstance_t)GetProcAddress(hMyTaskbar, "EP_TrayUI_CreateInstance");
        if (pfnMyTrayUICreateInstance)
        {
            if (IsWindows11())
            {
                explorer_TrayUI_CreateInstanceFunc = pfnMyTrayUICreateInstance;
            }
            else if (explorer_TrayUI_CreateInstanceFunc)
            {
                funchook_prepare(
                    funchook,
                    (void**)&explorer_TrayUI_CreateInstanceFunc,
                    pfnMyTrayUICreateInstance
                );
            }
            else
            {
                printf("[TB] Failed to hook TrayUI_CreateInstance()\n");
                FreeLibrary(hMyTaskbar);
                return NULL;
            }
        }
    }

    if ((eptf & EPTF_WinBlueLauncher) != 0)
    {
        typedef HRESULT (WINAPI *EP_Launcher_PatchTwinUIPCShell_t)();
        EP_Launcher_PatchTwinUIPCShell_t pfnEP_Launcher_PatchTwinUIPCShell = (EP_Launcher_PatchTwinUIPCShell_t)GetProcAddress(hMyTaskbar, "EP_Launcher_PatchTwinUIPCShell");
        if (pfnEP_Launcher_PatchTwinUIPCShell)
        {
            HRESULT hr = pfnEP_Launcher_PatchTwinUIPCShell();
            if (FAILED(hr))
            {
                printf("[TB] EP_Launcher_PatchTwinUIPCShell failed\n");
            }
        }
    }

    if ((eptf & EPTF_AudioFlyout) != 0)
    {
        typedef HRESULT (WINAPI *EP_AudioFlyout_PatchTwinUI_t)();
        EP_AudioFlyout_PatchTwinUI_t pfnEP_AudioFlyout_PatchTwinUI = (EP_AudioFlyout_PatchTwinUI_t)GetProcAddress(hMyTaskbar, "EP_AudioFlyout_PatchTwinUI");
        if (pfnEP_AudioFlyout_PatchTwinUI)
        {
            HRESULT hr = pfnEP_AudioFlyout_PatchTwinUI();
            if (FAILED(hr))
            {
                printf("[TB] EP_AudioFlyout_PatchTwinUI failed\n");
            }
        }
    }

    typedef void (*CopyExplorerSymbols_t)(symbols_addr* symbols);
    CopyExplorerSymbols_t CopyExplorerSymbols = (CopyExplorerSymbols_t)GetProcAddress(hMyTaskbar, "CopyExplorerSymbols");
    if (CopyExplorerSymbols)
    {
        CopyExplorerSymbols(symbols_PTRS);
    }

    typedef void (*SetImmersiveMenuFunctions_t)(void* a, void* b, void* c);
    SetImmersiveMenuFunctions_t SetImmersiveMenuFunctions = (SetImmersiveMenuFunctions_t)GetProcAddress(hMyTaskbar, "SetImmersiveMenuFunctions");
    if (SetImmersiveMenuFunctions)
    {
        SetImmersiveMenuFunctions(
            CImmersiveContextMenuOwnerDrawHelper_s_ContextMenuWndProcFunc,
            ImmersiveContextMenuHelper_ApplyOwnerDrawToMenuFunc,
            ImmersiveContextMenuHelper_RemoveOwnerDrawFromMenuFunc
        );
    }

    wprintf(L"[TB] Using '%s'\n", pszTaskbarDll);
    return hMyTaskbar;
}
#endif
#pragma endregion


#pragma region "Restore network icon on builds without pnidui.dll shipped"
#if WITH_MAIN_PATCHER
typedef struct SSOEntry
{
    GUID* pguid;
    int sharedThread;
    DWORD dwFlags;
    bool (*pfnCheckEnabled)();
} SSOEntry;

void PatchStobject(HANDLE hStobject)
{
    if (!hStobject)
        return;

    PBYTE beginRData = NULL;
    DWORD sizeRData = 0;

    // Our target is in .rdata
    PIMAGE_DOS_HEADER dosHeader = hStobject;
    if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE)
    {
        PIMAGE_NT_HEADERS64 ntHeader = (PIMAGE_NT_HEADERS64)((u_char*)dosHeader + dosHeader->e_lfanew);
        if (ntHeader->Signature == IMAGE_NT_SIGNATURE)
        {
            for (unsigned int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i)
            {
                PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeader) + i;
                if (!strncmp(section->Name, ".rdata", 6))
                {
                    beginRData = (PBYTE)dosHeader + section->VirtualAddress;
                    sizeRData = section->SizeOfRawData;
                    break;
                }
            }
        }
    }
    if (!beginRData || !sizeRData)
    {
        return;
    }

    // We'll sacrifice the Windows To Go SSO for this
    GUID* pguidTarget = memmem(beginRData, sizeRData, &CLSID_WindowsToGoSSO, sizeof(GUID));
    if (!pguidTarget)
    {
        return;
    }
    printf("[SSO] pguidTarget = %llX\n", (PBYTE)pguidTarget - (PBYTE)hStobject);

    // Find where it's used
    SSOEntry* pssoEntryTarget = NULL;
    SIZE_T searchSize = (SIZE_T)sizeRData - sizeof(SSOEntry);
    for (SIZE_T i = 0; i < searchSize; i += 8) // We know the struct is aligned, save some iterations
    {
        SSOEntry* current = (SSOEntry*)(beginRData + i);
        if (current->pguid == pguidTarget && current->sharedThread == 0 && current->dwFlags == 0 && current->pfnCheckEnabled)
        {
            pssoEntryTarget = current;
            break;
        }
    }
    if (!pssoEntryTarget)
    {
        return;
    }
    printf("[SSO] pssoEntryTarget = %llX\n", (PBYTE)pssoEntryTarget - (PBYTE)hStobject);

    // Make sure it's really not used
    if (pssoEntryTarget->pfnCheckEnabled && pssoEntryTarget->pfnCheckEnabled())
    {
        return;
    }

    // Modify the GUID
    DWORD dwOldProtect = 0;
    if (VirtualProtect(pguidTarget, sizeof(GUID), PAGE_EXECUTE_READWRITE, &dwOldProtect))
    {
        *pguidTarget = CLSID_NetworkTraySSO;
        VirtualProtect(pguidTarget, sizeof(GUID), dwOldProtect, &dwOldProtect);
    }

    // Modify the SSOEntry
    if (VirtualProtect(pssoEntryTarget, sizeof(SSOEntry), PAGE_EXECUTE_READWRITE, &dwOldProtect))
    {
        pssoEntryTarget->sharedThread = 1;
        pssoEntryTarget->dwFlags = 0;
        pssoEntryTarget->pfnCheckEnabled = NULL;
        VirtualProtect(pssoEntryTarget, sizeof(SSOEntry), dwOldProtect, &dwOldProtect);
    }
}

LSTATUS pnidui_RegGetValueW(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData)
{
    LSTATUS status = RegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
    if (!lstrcmpW(lpValue, L"ReplaceVan") && status == ERROR_FILE_NOT_FOUND)
    {
        *(DWORD*)pvData = 0;
        status = ERROR_SUCCESS;
    }
    return status;
}

void PatchPnidui(HMODULE hPnidui)
{
    VnPatchIAT(hPnidui, "api-ms-win-core-com-l1-1-0.dll", "CoCreateInstance", pnidui_CoCreateInstanceHook);
    if (global_rovi.dwBuildNumber >= 25236)
    {
        VnPatchIAT(hPnidui, "api-ms-win-core-registry-l1-1-0.dll", "RegGetValueW", pnidui_RegGetValueW);
    }
    VnPatchIAT(hPnidui, "user32.dll", "TrackPopupMenu", pnidui_TrackPopupMenuHook);
    HOOK_IMMERSIVE_MENUS(Pnidui);
#ifdef USE_PRIVATE_INTERFACES
    if (bSkinIcons)
    {
        VnPatchIAT(hPnidui, "user32.dll", "LoadImageW", SystemTray_LoadImageWHook);
    }
#endif
    printf("Setup pnidui functions done\n");
}
#endif
#pragma endregion

DWORD Inject(BOOL bIsExplorer)
{
#if defined(DEBUG) | defined(_DEBUG)
    FILE* conout;
    AllocConsole();
    freopen_s(
        &conout, 
        "CONOUT$",
        "w", 
        stdout
    );
#endif

    int rv;

    if (bIsExplorer)
    {
#if WITH_MAIN_PATCHER
        InitializeCriticalSection(&lock_epw);
#endif
        wszWeatherLanguage = malloc(sizeof(WCHAR) * MAX_PATH);
        wszWeatherTerm = malloc(sizeof(WCHAR) * MAX_PATH);
    }

    LoadSettings(MAKELPARAM(bIsExplorer, FALSE));
    Explorer_RefreshUI(99);

#if WITH_MAIN_PATCHER
    if (bIsExplorerProcess)
    {
        funchook = funchook_create();
        // printf("funchook create %d\n", funchook != 0);
    }
#endif

    if (bIsExplorer)
    {
        hSwsSettingsChanged = CreateEventW(NULL, FALSE, FALSE, NULL);
        hSwsOpacityMaybeChanged = CreateEventW(NULL, FALSE, FALSE, NULL);
    }

    unsigned int numSettings = bIsExplorer ? 12 : 2;
    Setting* settings = calloc(numSettings, sizeof(Setting));
    if (settings)
    {
        unsigned int cs = 0;

        if (cs < numSettings)
        {
            settings[cs].callback = NULL;
            settings[cs].data = NULL;
            settings[cs].hEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
            settings[cs].hKey = NULL;
            ZeroMemory(settings[cs].name, MAX_PATH);
            settings[cs].origin = NULL;
            cs++;
        }

        if (cs < numSettings)
        {
            settings[cs].callback = LoadSettings;
            settings[cs].data = MAKELPARAM(bIsExplorer, TRUE);
            settings[cs].hEvent = NULL;
            settings[cs].hKey = NULL;
            wcscpy_s(settings[cs].name, MAX_PATH, TEXT(REGPATH));
            settings[cs].origin = HKEY_CURRENT_USER;
            cs++;
        }

        if (cs < numSettings)
        {
            settings[cs].callback = LoadSettings;
            settings[cs].data = MAKELPARAM(bIsExplorer, FALSE);
            settings[cs].hEvent = NULL;
            settings[cs].hKey = NULL;
            wcscpy_s(settings[cs].name, MAX_PATH, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage");
            settings[cs].origin = HKEY_CURRENT_USER;
            cs++;
        }

        if (cs < numSettings)
        {
            settings[cs].callback = SetEvent;
            settings[cs].data = hSwsSettingsChanged;
            settings[cs].hEvent = NULL;
            settings[cs].hKey = NULL;
            wcscpy_s(settings[cs].name, MAX_PATH, TEXT(REGPATH) L"\\sws");
            settings[cs].origin = HKEY_CURRENT_USER;
            cs++;
        }

        if (cs < numSettings)
        {
            settings[cs].callback = SetEvent;
            settings[cs].data = hSwsOpacityMaybeChanged;
            settings[cs].hEvent = NULL;
            settings[cs].hKey = NULL;
            wcscpy_s(settings[cs].name, MAX_PATH, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MultitaskingView\\AltTabViewHost");
            settings[cs].origin = HKEY_CURRENT_USER;
            cs++;
        }

        if (cs < numSettings)
        {
            settings[cs].callback = Explorer_RefreshUI;
            settings[cs].data = 1;
            settings[cs].hEvent = NULL;
            settings[cs].hKey = NULL;
            wcscpy_s(settings[cs].name, MAX_PATH, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced");
            settings[cs].origin = HKEY_CURRENT_USER;
            cs++;
        }

        if (cs < numSettings)
        {
            settings[cs].callback = Explorer_RefreshUI;
            settings[cs].data = 2;
            settings[cs].hEvent = NULL;
            settings[cs].hKey = NULL;
            wcscpy_s(settings[cs].name, MAX_PATH, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Search");
            settings[cs].origin = HKEY_CURRENT_USER;
            cs++;
        }

        if (cs < numSettings)
        {
            settings[cs].callback = Explorer_RefreshUI;
            settings[cs].data = NULL;
            settings[cs].hEvent = NULL;
            settings[cs].hKey = NULL;
            wcscpy_s(settings[cs].name, MAX_PATH, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced\\People");
            settings[cs].origin = HKEY_CURRENT_USER;
            cs++;
        }

        if (cs < numSettings)
        {
            settings[cs].callback = Explorer_RefreshUI;
            settings[cs].data = NULL;
            settings[cs].hEvent = NULL;
            settings[cs].hKey = NULL;
            wcscpy_s(settings[cs].name, MAX_PATH, L"SOFTWARE\\Microsoft\\TabletTip\\1.7");
            settings[cs].origin = HKEY_CURRENT_USER;
            cs++;
        }

        if (cs < numSettings)
        {
            settings[cs].callback = SetEvent;
            settings[cs].data = hSwsSettingsChanged;
            settings[cs].hEvent = NULL;
            settings[cs].hKey = NULL;
            wcscpy_s(settings[cs].name, MAX_PATH, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer");
            settings[cs].origin = HKEY_CURRENT_USER;
            cs++;
        }

        if (cs < numSettings)
        {
            settings[cs].callback = UpdateStartMenuPositioning;
            settings[cs].data = MAKELPARAM(FALSE, TRUE);
            settings[cs].hEvent = NULL;
            settings[cs].hKey = NULL;
            wcscpy_s(settings[cs].name, MAX_PATH, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced");
            settings[cs].origin = HKEY_CURRENT_USER;
            cs++;
        }

        if (cs < numSettings)
        {
            settings[cs].callback = LoadSettings;
            settings[cs].data = MAKELPARAM(bIsExplorer, FALSE);
            settings[cs].hEvent = NULL;
            settings[cs].hKey = NULL;
            wcscpy_s(settings[cs].name, MAX_PATH, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer");
            settings[cs].origin = HKEY_CURRENT_USER;
            cs++;
        }

        SettingsChangeParameters* settingsParams = calloc(1, sizeof(SettingsChangeParameters));
        if (settingsParams)
        {
            settingsParams->settings = settings;
            settingsParams->size = numSettings;
            settingsParams->hThread = CreateThread(
                0,
                0,
                MonitorSettings,
                settingsParams,
                0,
                0
            );
        }
        else
        {
            if (numSettings && settings[0].hEvent)
            {
                CloseHandle(settings[0].hEvent);
            }
            free(settings);
            settings = NULL;
        }
    }

    InjectBasicFunctions(bIsExplorer, TRUE);
    //if (!hDelayedInjectionThread)
    //{
    //    hDelayedInjectionThread = CreateThread(0, 0, InjectBasicFunctions, 0, 0, 0);
    //}

    if (!bIsExplorer)
    {
#if WITH_MAIN_PATCHER
        if (bIsExplorerProcess)
        {
            rv = funchook_install(funchook, 0);
            if (rv != 0)
            {
                printf("Failed to install hooks. rv = %d\n", rv);
            }
            funchook_destroy(funchook);
            funchook = NULL;
        }
#endif
        return 0;
    }

#if WITH_MAIN_PATCHER
    extern void InitializeWilLogCallback();
    InitializeWilLogCallback();
    wprintf(L"Running on Windows %d, OS Build %d.%d.%d.%d.\n", IsWindows11() ? 11 : 10, global_rovi.dwMajorVersion, global_rovi.dwMinorVersion, global_rovi.dwBuildNumber, global_ubr);
#endif

    WCHAR wszPath[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, wszPath);
    wcscat_s(wszPath, MAX_PATH, _T(APP_RELATIVE_PATH));
    if (!PathFileExistsW(wszPath))
    {
        CreateDirectoryW(wszPath, NULL);
    }

#if WITH_MAIN_PATCHER
    wszEPWeatherKillswitch = calloc(sizeof(WCHAR), MAX_PATH);
    srand(time(NULL));
    rand_string(wszEPWeatherKillswitch, MAX_PATH / 2 - 1);
    swprintf_s(wszEPWeatherKillswitch, sizeof(_T(EP_Weather_Killswitch)) / sizeof(WCHAR), L"%s", _T(EP_Weather_Killswitch));
    wszEPWeatherKillswitch[wcslen(wszEPWeatherKillswitch)] = L'_';
    //wprintf(L"%s\n", wszEPWeatherKillswitch);
    hEPWeatherKillswitch = CreateMutexW(NULL, TRUE, wszEPWeatherKillswitch);
    /*while (TRUE)
    {
        hEPWeatherKillswitch = CreateMutexW(NULL, TRUE, wszEPWeatherKillswitch);
        if (GetLastError() == ERROR_ALREADY_EXISTS)
        {
            WaitForSingleObject(hEPWeatherKillswitch, INFINITE);
            CloseHandle(hEPWeatherKillswitch);
        }
        else
        {
            break;
        }
    }*/
#endif


#if WITH_MAIN_PATCHER
    hCanStartSws = CreateEventW(NULL, FALSE, FALSE, NULL);
    hWin11AltTabInitialized = CreateEventW(NULL, FALSE, FALSE, NULL);
    CreateThread(
        0,
        0,
        WindowSwitcher,
        0,
        0,
        0
    );


#ifdef USE_PRIVATE_INTERFACES
    P_Icon_Dark_Search = ReadFromFile(L"C:\\Users\\root\\Downloads\\pri\\resources\\Search_Dark\\png\\32.png", &S_Icon_Dark_Search);
    P_Icon_Light_Search = ReadFromFile(L"C:\\Users\\root\\Downloads\\pri\\resources\\Search_Light\\png\\32.png", &S_Icon_Light_Search);
    P_Icon_Dark_TaskView = ReadFromFile(L"C:\\Users\\root\\Downloads\\pri\\resources\\TaskView_Dark\\png\\32.png", &S_Icon_Dark_TaskView);
    P_Icon_Light_TaskView = ReadFromFile(L"C:\\Users\\root\\Downloads\\pri\\resources\\TaskView_Light\\png\\32.png", &S_Icon_Light_TaskView);
    P_Icon_Dark_Widgets = ReadFromFile(L"C:\\Users\\root\\Downloads\\pri\\resources\\Widgets_Dark\\png\\32.png", &S_Icon_Dark_Widgets);
    P_Icon_Light_Widgets = ReadFromFile(L"C:\\Users\\root\\Downloads\\pri\\resources\\Widgets_Light\\png\\32.png", &S_Icon_Dark_Widgets);
#endif


    symbols_addr symbols_PTRS;
    ZeroMemory(
        &symbols_PTRS,
        sizeof(symbols_addr)
    );
    LoadSymbolsResult loadSymbolsResult = LoadSymbols(&symbols_PTRS);
    if (loadSymbolsResult.bSuccess)
    {
        if (NeedToDownloadSymbols(&loadSymbolsResult))
        {
            if (bEnableSymbolDownload)
            {
                printf("Attempting to download symbol data; for now, the program may have limited functionality.\n");
                DownloadSymbolsParams* params = malloc(sizeof(DownloadSymbolsParams));
                params->hModule = hModule;
                params->bVerbose = FALSE;
                params->loadResult = loadSymbolsResult;
                CreateThread(0, 0, DownloadSymbols, params, 0, 0);
            }
        }
        else
        {
            printf("Loaded symbols\n");
        }
    }


    HANDLE hUser32 = LoadLibraryW(L"user32.dll");
    CreateWindowInBand = GetProcAddress(hUser32, "CreateWindowInBand");
    GetWindowBand = GetProcAddress(hUser32, "GetWindowBand");
    SetWindowBand = GetProcAddress(hUser32, "SetWindowBand");
    SetWindowCompositionAttribute = GetProcAddress(hUser32, "SetWindowCompositionAttribute");
    if (!IsWindows11BuildHigherThan25158()) {
        VnPatchIAT(hUser32, "win32u.dll", "NtUserFindWindowEx", user32_NtUserFindWindowExHook);
    }
    printf("Setup user32 functions done\n");


    HANDLE hExplorer = GetModuleHandleW(NULL);
    MODULEINFO miExplorer;
    GetModuleInformation(GetCurrentProcess(), hExplorer, &miExplorer, sizeof(MODULEINFO));

    if (IsWindows11Version22H2OrHigher())
    {
        TryToFindExplorerOffsets(hExplorer, &miExplorer, symbols_PTRS.explorer_PTRS);

#if 0
        if (global_rovi.dwBuildNumber >= 26002)
        {
#if defined(_M_X64)
            // Please Microsoft 🙏
            // 48 8B ?? 78 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 0F 85
            //                                     ^^^^^^^^^^^
            // 26040.1000: C28AE
            // 26052.1000: BF052
            // 26058.1000: BEFA2
            // 26080.1   : 11795C
            PBYTE match = FindPattern(
                hExplorer,
                miExplorer.SizeOfImage,
                "\x48\x8B\x00\x78\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x84\xC0\x0F\x85",
                "xx?xxxx????x????xxxx"
            );
            if (match)
            {
                match += 11;
                match += 5 + *(int*)(match + 1);
            }
#elif defined(_M_ARM64)
            PBYTE match = NULL;
#endif
            if (match)
            {
                DisableWin10TaskbarIsEnabledFunc = match;
                printf("wil::details::FeatureImpl<__WilFeatureTraits_Feature_DisableWin10Taskbar>::__private_IsEnabled() = %llX\n", match - (PBYTE)hExplorer);

                funchook_prepare(
                    funchook,
                    (void**)&DisableWin10TaskbarIsEnabledFunc,
                    DisableWin10TaskbarIsEnabledHook
                );
            }
        }
#endif
    }

    const WCHAR* pszTaskbarDll = GetTaskbarDllChecked(&symbols_PTRS);
    if (bOldTaskbar >= 2 && !pszTaskbarDll)
    {
        bOldTaskbar = 1;
        AdjustTaskbarStyleValue(&bOldTaskbar);
    }

    SetChildWindowNoActivateFunc = GetProcAddress(GetModuleHandleW(L"user32.dll"), (LPCSTR)2005);
    if (bOldTaskbar)
    {
        VnPatchIAT(hExplorer, "user32.dll", (LPCSTR)2005, explorer_SetChildWindowNoActivateHook);
        VnPatchDelayIAT(hExplorer, "ext-ms-win-rtcore-ntuser-window-ext-l1-1-0.dll", "SendMessageW", explorer_SendMessageW);
        VnPatchIAT(hExplorer, "shell32.dll", "ShellExecuteW", explorer_ShellExecuteW);
        VnPatchIAT(hExplorer, "shell32.dll", "ShellExecuteExW", explorer_ShellExecuteExW);
        VnPatchIAT(hExplorer, "API-MS-WIN-CORE-REGISTRY-L1-1-0.DLL", "RegGetValueW", explorer_RegGetValueW);
        VnPatchIAT(hExplorer, "API-MS-WIN-CORE-REGISTRY-L1-1-0.DLL", "RegSetValueExW", explorer_RegSetValueExW);
        VnPatchIAT(hExplorer, "API-MS-WIN-CORE-REGISTRY-L1-1-0.DLL", "RegCreateKeyExW", explorer_RegCreateKeyExW);
        VnPatchIAT(hExplorer, "API-MS-WIN-SHCORE-REGISTRY-L1-1-0.DLL", "SHGetValueW", explorer_SHGetValueW);
        VnPatchIAT(hExplorer, "user32.dll", "LoadMenuW", explorer_LoadMenuW);
        VnPatchIAT(hExplorer, "api-ms-win-core-shlwapi-obsolete-l1-1-0.dll", "QISearch", explorer_QISearch);
        if (IsOS(OS_ANYSERVER)) VnPatchIAT(hExplorer, "api-ms-win-shcore-sysinfo-l1-1-0.dll", "IsOS", explorer_IsOS);
        if (IsWindows11Version22H2OrHigher()) VnPatchDelayIAT(hExplorer, "ext-ms-win-rtcore-ntuser-window-ext-l1-1-0.dll", "CreateWindowExW", Windows11v22H2_explorer_CreateWindowExW);
    }
    VnPatchIAT(hExplorer, "user32.dll", "TileWindows", explorer_TileWindows);
    VnPatchIAT(hExplorer, "user32.dll", "CascadeWindows", explorer_CascadeWindows);
    VnPatchIAT(hExplorer, "API-MS-WIN-CORE-REGISTRY-L1-1-0.DLL", "RegOpenKeyExW", explorer_RegOpenKeyExW);
    VnPatchIAT(hExplorer, "shell32.dll", (LPCSTR)85, explorer_OpenRegStream);
    VnPatchIAT(hExplorer, "user32.dll", "TrackPopupMenuEx", explorer_TrackPopupMenuExHook);
    HOOK_IMMERSIVE_MENUS(Explorer);
    VnPatchIAT(hExplorer, "uxtheme.dll", "OpenThemeDataForDpi", explorer_OpenThemeDataForDpi);
    VnPatchIAT(hExplorer, "uxtheme.dll", "DrawThemeBackground", explorer_DrawThemeBackground);
    VnPatchIAT(hExplorer, "uxtheme.dll", "CloseThemeData", explorer_CloseThemeData);
    if (IsWindows11())
    {
        // Fix Windows 10 taskbar high DPI button width bug
        VnPatchIAT(hExplorer, "api-ms-win-ntuser-sysparams-l1-1-0.dll", "GetSystemMetrics", patched_GetSystemMetrics);

        // Fix Pin to Start/Unpin from Start
        PatchAppResolver();
        PatchStartTileData(FALSE);
    }
    //VnPatchIAT(hExplorer, "api-ms-win-core-libraryloader-l1-2-0.dll", "LoadStringW", explorer_LoadStringWHook);
    if (bClassicThemeMitigations)
    {
        /*explorer_SetWindowThemeFunc = SetWindowTheme;
        rv = funchook_prepare(
            funchook,
            (void**)&explorer_SetWindowThemeFunc,
            explorer_SetWindowThemeHook
        );
        if (rv != 0)
        {
            FreeLibraryAndExitThread(hModule, rv);
            return rv;
        }*/
        VnPatchIAT(hExplorer, "uxtheme.dll", "DrawThemeTextEx", explorer_DrawThemeTextEx);
        VnPatchIAT(hExplorer, "uxtheme.dll", "GetThemeMargins", explorer_GetThemeMargins);
        VnPatchIAT(hExplorer, "uxtheme.dll", "GetThemeMetric", explorer_GetThemeMetric);
        //VnPatchIAT(hExplorer, "uxtheme.dll", "OpenThemeDataForDpi", explorer_OpenThemeDataForDpi);
        //VnPatchIAT(hExplorer, "uxtheme.dll", "DrawThemeBackground", explorer_DrawThemeBackground);
        VnPatchIAT(hExplorer, "user32.dll", "SetWindowCompositionAttribute", explorer_SetWindowCompositionAttribute);
    }
    //VnPatchDelayIAT(hExplorer, "ext-ms-win-rtcore-ntuser-window-ext-l1-1-0.dll", "CreateWindowExW", explorer_CreateWindowExW);
    if (bOldTaskbar)
    {
        VnPatchIAT(hExplorer, "api-ms-win-core-com-l1-1-0.dll", "CoCreateInstance", explorer_CoCreateInstanceHook);
        VnPatchIAT(hExplorer, "API-MS-WIN-NTUSER-RECTANGLE-L1-1-0.DLL", "SetRect", explorer_SetRect);
        VnPatchIAT(hExplorer, "USER32.DLL", "DeleteMenu", explorer_DeleteMenu);
        if (global_rovi.dwBuildNumber >= 22572)
        {
            VnPatchIAT(hExplorer, "dwmapi.dll", "DwmUpdateThumbnailProperties", explorer_DwmUpdateThumbnailPropertiesHook);
            if (!bClassicThemeMitigations)
            {
                VnPatchIAT(hExplorer, "user32.dll", "SetWindowCompositionAttribute", explorer_SetWindowCompositionAttribute);
            }
        }
    }
    if (IsWindows11())
    {
        // Find pointers to various stuff needed to have a working Windows 10 taskbar and Windows 10 taskbar context menu on Windows 11 taskbar
#if defined(_M_X64)
        // 4C 8D 05 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 48 8B
        //                               ^^^^^^^^^^^    ^^^^^^^^^^^
        // Ref: CTray::Init()
        PBYTE match = FindPattern(
            hExplorer,
            miExplorer.SizeOfImage,
            "\x4C\x8D\x05\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B",
            "xxx????xxx????x????xx"
        );
        if (match)
        {
            match += 7; // Point to 48
            g_pTrayUIHost = (ITrayUIHost*)(match + 7 + *(int*)(match + 3));
            match += 7; // Point to E8
            explorer_TrayUI_CreateInstanceFunc = (TrayUI_CreateInstance_t)(match + 5 + *(int*)(match + 1));
        }
#elif defined(_M_ARM64)
        // TODO Add support for ARM64
#endif
    }
    else
    {
#if defined(_M_X64)
        // 4C 8D 05 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? E8 ?? ?? ?? ?? 85 C0
        //                               ^^^^^^^^^^^    ^^^^^^^^^^^
        // Ref: CTray::Init()
        PBYTE match = FindPattern(
            hExplorer,
            miExplorer.SizeOfImage,
            "\x4C\x8D\x05\x00\x00\x00\x00\x48\x8D\x0D\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x85\xC0",
            "xxx????xxx????x????xx"
        );
        if (match)
        {
            match += 7; // Point to 48
            g_pTrayUIHost = (ITrayUIHost*)(match + 7 + *(int*)(match + 3));
            match += 7; // Point to E8
            explorer_TrayUI_CreateInstanceFunc = (TrayUI_CreateInstance_t)(match + 5 + *(int*)(match + 1));
        }
#endif
    }
    if (g_pTrayUIHost)
    {
        printf("ITrayUIHost = %llX\n", (PBYTE)g_pTrayUIHost - (PBYTE)hExplorer);
    }
    if (explorer_TrayUI_CreateInstanceFunc)
    {
        printf("explorer.exe!TrayUI_CreateInstance() = %llX\n", (PBYTE)explorer_TrayUI_CreateInstanceFunc - (PBYTE)hExplorer);
    }

    // Enable Windows 10 taskbar search box on 22621+
    if (IsWindows11Version22H2OrHigher())
    {
        if (symbols_PTRS.explorer_PTRS[5] && symbols_PTRS.explorer_PTRS[5] != 0xFFFFFFFF)
        {
            TrayUI__UpdatePearlSizeFunc = (PBYTE)hExplorer + symbols_PTRS.explorer_PTRS[5];
        }
        UpdateSearchBox();
    }

    HANDLE hShcore = LoadLibraryW(L"shcore.dll");
    SHWindowsPolicy = GetProcAddress(hShcore, (LPCSTR)190);
#ifdef USE_PRIVATE_INTERFACES
    explorer_SHCreateStreamOnModuleResourceWFunc = GetProcAddress(hShcore, (LPCSTR)109);
    VnPatchIAT(hExplorer, "shcore.dll", (LPCSTR)0x6D, explorer_SHCreateStreamOnModuleResourceWHook);
#endif

    printf("Setup explorer functions done\n");




    CreateWindowExWFunc = CreateWindowExW;
    rv = funchook_prepare(
        funchook,
        (void**)&CreateWindowExWFunc,
        CreateWindowExWHook
    );
    if (rv != 0)
    {
        FreeLibraryAndExitThread(hModule, rv);
        return rv;
    }
    SetWindowLongPtrWFunc = SetWindowLongPtrW;
    rv = funchook_prepare(
        funchook,
        (void**)&SetWindowLongPtrWFunc,
        SetWindowLongPtrWHook
    );
    if (rv != 0)
    {
        FreeLibraryAndExitThread(hModule, rv);
        return rv;
    }




    HANDLE hUxtheme = LoadLibraryW(L"uxtheme.dll");
    GetThemeName = (GetThemeName_t)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(74));
    RefreshImmersiveColorPolicyState = (RefreshImmersiveColorPolicyState_t)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(104));
    GetIsImmersiveColorUsingHighContrast = (GetIsImmersiveColorUsingHighContrast_t)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(106));
    GetUserColorPreference = (GetUserColorPreference_t)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(120));
    GetColorFromPreference = (GetColorFromPreference_t)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(121));
    PeopleBand_DrawTextWithGlowFunc = GetProcAddress(hUxtheme, MAKEINTRESOURCEA(126));
    ShouldAppsUseDarkMode = (ShouldAppsUseDarkMode_t)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(132));
    AllowDarkModeForWindow = (AllowDarkModeForWindow_t)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(133));
    SetPreferredAppMode = (SetPreferredAppMode_t)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(135));
    ShouldSystemUseDarkMode = (ShouldSystemUseDarkMode_t)GetProcAddress(hUxtheme, MAKEINTRESOURCEA(138));
    if (bOldTaskbar)
    {
        VnPatchIAT(hExplorer, "uxtheme.dll", MAKEINTRESOURCEA(126), PeopleBand_DrawTextWithGlowHook);
    }
    // DwmExtendFrameIntoClientArea hooked in LoadSettings
    printf("Setup uxtheme functions done\n");

    RunTwinUIPCShellPatches(&symbols_PTRS);

    HMODULE hMyTaskbar = PrepareAlternateTaskbarImplementation(&symbols_PTRS, pszTaskbarDll);
    if (hMyTaskbar)
    {
        VnPatchIAT(hMyTaskbar, "user32.dll", "DeleteMenu", explorer_DeleteMenu);
        VnPatchIAT(hMyTaskbar, "user32.dll", "LoadMenuW", explorer_LoadMenuW);
        VnPatchIAT(hMyTaskbar, "user32.dll", "SendMessageW", explorer_SendMessageW);
        VnPatchIAT(hMyTaskbar, "user32.dll", "SetRect", explorer_SetRect);
        VnPatchIAT(hMyTaskbar, "user32.dll", "TrackPopupMenuEx", explorer_TrackPopupMenuExHook);
    }

    HANDLE hCombase = LoadLibraryW(L"combase.dll");
    if (IsWindows11())
    {
        if (IsWindows11Version22H2OrHigher())
        {
            // Fixed a bug that crashed Explorer when a folder window was opened after a first one was closed on OS builds 22621+
            VnPatchIAT(hCombase, "api-ms-win-core-libraryloader-l1-2-0.dll", "LoadLibraryExW", Windows11v22H2_combase_LoadLibraryExW);
        }
    }
    if (!IsWindows11Version22H2OrHigher() && IsXamlSoundsEnabled())
    {
        VnPatchIAT(hCombase, "api-ms-win-core-libraryloader-l1-2-0.dll", "LoadLibraryExW", combase_LoadLibraryExW);
    }
    printf("Setup combase functions done\n");


    HANDLE hTwinui = LoadLibraryW(L"twinui.dll");
    if (!IsWindows11())
    {
        VnPatchIAT(hTwinui, "user32.dll", "TrackPopupMenu", twinui_TrackPopupMenuHook);
    }
    if (bDisableWinFHotkey)
    {
        VnPatchIAT(hTwinui, "user32.dll", "RegisterHotKey", twinui_RegisterHotkeyHook);
    }
    printf("Setup twinui functions done\n");


    HANDLE hStobject = LoadLibraryW(L"stobject.dll");
    if (hStobject)
    {
        VnPatchIAT(hStobject, "api-ms-win-core-registry-l1-1-0.dll", "RegGetValueW", stobject_RegGetValueW);
        VnPatchIAT(hStobject, "api-ms-win-core-com-l1-1-0.dll", "CoCreateInstance", stobject_CoCreateInstanceHook);
        if (IsWindows11())
        {
            VnPatchDelayIAT(hStobject, "user32.dll", "TrackPopupMenu", stobject_TrackPopupMenuHook);
            VnPatchDelayIAT(hStobject, "user32.dll", "TrackPopupMenuEx", stobject_TrackPopupMenuExHook);
        }
        else
        {
            VnPatchIAT(hStobject, "user32.dll", "TrackPopupMenu", stobject_TrackPopupMenuHook);
            VnPatchIAT(hStobject, "user32.dll", "TrackPopupMenuEx", stobject_TrackPopupMenuExHook);
        }
        if (global_rovi.dwBuildNumber >= 25236 && bOldTaskbar)
        {
            PatchStobject(hStobject);
        }
    }
#ifdef USE_PRIVATE_INTERFACES
    if (bSkinIcons)
    {
        VnPatchDelayIAT(hStobject, "user32.dll", "LoadImageW", SystemTray_LoadImageWHook);
    }
#endif
    printf("Setup stobject functions done\n");



    HANDLE hBthprops = LoadLibraryW(L"bthprops.cpl");
    VnPatchIAT(hBthprops, "user32.dll", "TrackPopupMenuEx", bthprops_TrackPopupMenuExHook);
#ifdef USE_PRIVATE_INTERFACES
    if (bSkinIcons)
    {
        VnPatchIAT(hBthprops, "user32.dll", "LoadImageW", SystemTray_LoadImageWHook);
    }
#endif
    printf("Setup bthprops functions done\n");



    if (global_rovi.dwBuildNumber < 25236)
    {
        HANDLE hPnidui = LoadLibraryW(L"pnidui.dll");
        if (hPnidui)
        {
            PatchPnidui(hPnidui);
        }
    }


#if WITH_MAIN_PATCHER
    if (global_rovi.dwBuildNumber < 22567)
    {
        PatchSndvolsso();
    }
#endif


    hShell32 = GetModuleHandleW(L"shell32.dll");
    if (hShell32)
    {
        // Patch ribbon to handle redirects to classic CPLs
        HRESULT(*SHELL32_Create_IEnumUICommand)(IUnknown*, int*, int, IUnknown**) = GetProcAddress(hShell32, (LPCSTR)0x2E8);
        if (SHELL32_Create_IEnumUICommand)
        {
            char WVTASKITEM[80];
            ZeroMemory(WVTASKITEM, 80);
            IUnknown* pEnumUICommand = NULL;
            SHELL32_Create_IEnumUICommand(NULL, WVTASKITEM, 1, &pEnumUICommand);
            if (pEnumUICommand)
            {
                EnumExplorerCommand* pEnumExplorerCommand = NULL;
                pEnumUICommand->lpVtbl->QueryInterface(pEnumUICommand, &IID_EnumExplorerCommand, &pEnumExplorerCommand);
                pEnumUICommand->lpVtbl->Release(pEnumUICommand);
                if (pEnumExplorerCommand)
                {
                    UICommand* pUICommand = NULL;
                    pEnumExplorerCommand->lpVtbl->Next(pEnumExplorerCommand, 1, &pUICommand, NULL);
                    pEnumExplorerCommand->lpVtbl->Release(pEnumExplorerCommand);
                    if (pUICommand)
                    {
                        DWORD flOldProtect = 0;
                        if (VirtualProtect(pUICommand->lpVtbl, sizeof(UICommandVtbl), PAGE_EXECUTE_READWRITE, &flOldProtect))
                        {
                            shell32_UICommand_InvokeFunc = pUICommand->lpVtbl->Invoke;
                            pUICommand->lpVtbl->Invoke = shell32_UICommand_InvokeHook;
                            VirtualProtect(pUICommand->lpVtbl, sizeof(UICommandVtbl), flOldProtect, &flOldProtect);
                        }
                        pUICommand->lpVtbl->Release(pUICommand);
                    }
                }
            }
        }

        // Allow clasic drive groupings in This PC
        DllGetClassObject_t SHELL32_DllGetClassObject = (DllGetClassObject_t)GetProcAddress(hShell32, "DllGetClassObject");
        if (SHELL32_DllGetClassObject)
        {
            IClassFactory* pClassFactory = NULL;
            SHELL32_DllGetClassObject(&CLSID_DriveTypeCategorizer, &IID_IClassFactory, (LPVOID*)&pClassFactory);

            if (pClassFactory)
            {
                //DllGetClassObject hands out a unique "factory entry" data structure for each type of CLSID, containing a pointer to an IClassFactoryVtbl as well as some other members including
                //the _true_ create instance function that should be called (in this instance, shell32!CDriveTypeCategorizer_CreateInstance). When the IClassFactory::CreateInstance method is called,
                //shell32!ECFCreateInstance will cast the IClassFactory* passed to it back into a factory entry, and then invoke the pfnCreateInstance function defined in that entry directly.
                //Thus, rather than hooking the shared shell32!ECFCreateInstance function found on the IClassFactoryVtbl* shared by all class objects returned by shell32!DllGetClassObject, we get the real
                //CreateInstance function that will be called and hook that instead
                Shell32ClassFactoryEntry* pClassFactoryEntry = (Shell32ClassFactoryEntry*)pClassFactory;

                DWORD flOldProtect = 0;

                if (VirtualProtect(pClassFactoryEntry, sizeof(Shell32ClassFactoryEntry), PAGE_EXECUTE_READWRITE, &flOldProtect))
                {
                    shell32_DriveTypeCategorizer_CreateInstanceFunc = pClassFactoryEntry->pfnCreateInstance;
                    pClassFactoryEntry->pfnCreateInstance = shell32_DriveTypeCategorizer_CreateInstanceHook;
                    VirtualProtect(pClassFactoryEntry, sizeof(Shell32ClassFactoryEntry), flOldProtect, &flOldProtect);
                }

                pClassFactory->lpVtbl->Release(pClassFactory);
            }
        }

        // Disable Windows Spotlight icon
        if (DoesOSBuildSupportSpotlight())
        {
            VnPatchIAT(hShell32, "API-MS-WIN-CORE-REGISTRY-L1-1-0.DLL", "RegCreateKeyExW", shell32_RegCreateKeyExW);
            VnPatchIAT(hShell32, "API-MS-WIN-CORE-REGISTRY-L1-1-0.DLL", "RegSetValueExW", shell32_RegSetValueExW);
            VnPatchIAT(hShell32, "user32.dll", "DeleteMenu", shell32_DeleteMenu);
        }

        // Fix high DPI wrong (desktop) icon spacing bug
        if (IsWindows11())
        {
            VnPatchIAT(hShell32, "user32.dll", "GetSystemMetrics", patched_GetSystemMetrics);
        }
    }
    printf("Setup shell32 functions done\n");


    HANDLE hWindowsStorage = LoadLibraryW(L"windows.storage.dll");
    SHELL32_CanDisplayWin8CopyDialogFunc = GetProcAddress(hShell32, "SHELL32_CanDisplayWin8CopyDialog");
    if (SHELL32_CanDisplayWin8CopyDialogFunc) VnPatchDelayIAT(hWindowsStorage, "ext-ms-win-shell-exports-internal-l1-1-0.dll", "SHELL32_CanDisplayWin8CopyDialog", SHELL32_CanDisplayWin8CopyDialogHook);
    printf("Setup windows.storage functions done\n");


    if (IsWindows11())
    {
        HANDLE hInputSwitch = LoadLibraryW(L"InputSwitch.dll");
        if (bOldTaskbar)
        {
            printf("[IME] Context menu patch status: %d\n", PatchContextMenuOfNewMicrosoftIME(NULL));
        }
        if (hInputSwitch)
        {
            VnPatchIAT(hInputSwitch, "user32.dll", "TrackPopupMenuEx", inputswitch_TrackPopupMenuExHook);
            HOOK_IMMERSIVE_MENUS(InputSwitch);
            printf("Setup inputswitch functions done\n");
        }

        HANDLE hWindowsudkShellcommon = LoadLibraryW(L"windowsudk.shellcommon.dll");
        if (hWindowsudkShellcommon)
        {
            HANDLE hSLC = LoadLibraryW(L"slc.dll");
            if (hSLC)
            {
                SLGetWindowsInformationDWORDFunc = GetProcAddress(hSLC, "SLGetWindowsInformationDWORD");

                if (SLGetWindowsInformationDWORDFunc)
                {
                    VnPatchDelayIAT(hWindowsudkShellcommon, "ext-ms-win-security-slc-l1-1-0.dll", "SLGetWindowsInformationDWORD", windowsudkshellcommon_SLGetWindowsInformationDWORDHook);
                }
            }
        }
    }



    HANDLE hPeopleBand = LoadLibraryW(L"PeopleBand.dll");
    if (hPeopleBand)
    {
        if (IsOS(OS_ANYSERVER)) VnPatchIAT(hPeopleBand, "SHLWAPI.dll", (LPCSTR)437, PeopleBand_IsOS);
        VnPatchIAT(hPeopleBand, "api-ms-win-core-largeinteger-l1-1-0.dll", "MulDiv", PeopleBand_MulDivHook);
        printf("Setup peopleband functions done\n");
    }



    if (AreLogonLogoffShutdownSoundsEnabled())
    {
        HookLogonSound();
    }

    rv = funchook_install(funchook, 0);
    if (rv != 0)
    {
        FreeLibraryAndExitThread(hModule, rv);
        return rv;
    }
    // funchook_destroy(funchook);
    // funchook = NULL;
    printf("Installed hooks.\n");




    if (IsWindows11())
    {
        if (bOldTaskbar)
        {
            CreateThread(0, 0, SignalShellReady, 0, 0, 0);
        }
        else
        {
            CreateThread(0, 0, FixTaskbarAutohide, 0, 0, 0);
        }
    }

    /*if (IsWindows11Version22H2OrHigher() && bOldTaskbar)
    {
        DWORD dwRes = 1;
        DWORD dwSize = sizeof(DWORD);
        if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Search", L"SearchboxTaskbarMode", RRF_RT_DWORD, NULL, &dwRes, &dwSize) != ERROR_SUCCESS)
        {
            RegSetKeyValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Search", L"SearchboxTaskbarMode", REG_DWORD, &dwRes, sizeof(DWORD));
        }
    }*/


    /*
    if (IsWindows11())
    {
        DWORD dwSize = 0;
        if (SHRegGetValueFromHKCUHKLMFunc && SHRegGetValueFromHKCUHKLMFunc(
            L"Control Panel\\Desktop\\WindowMetrics",
            L"MinWidth",
            SRRF_RT_REG_SZ,
            NULL,
            NULL,
            (LPDWORD)(&dwSize)
        ) != ERROR_SUCCESS)
        {
            RegSetKeyValueW(
                HKEY_CURRENT_USER,
                L"Control Panel\\Desktop\\WindowMetrics",
                L"MinWidth",
                REG_SZ,
                L"38",
                sizeof(L"38")
            );
        }
    }
    */



    CreateThread(
        0,
        0,
        OpenStartOnCurentMonitorThread,
        0,
        0,
        0
    );
    printf("Open Start on monitor thread\n");



    hServiceWindowThread = CreateThread(
        0,
        0,
        EP_ServiceWindowThread,
        0,
        0,
        0
    );
    printf("EP Service Window thread\n");



    // if (bDisableOfficeHotkeys)
    {
        VnPatchIAT(hExplorer, "user32.dll", "RegisterHotKey", explorer_RegisterHotkeyHook);
    }



    if (bEnableArchivePlugin)
    {
        ArchiveMenuThreadParams* params = calloc(1, sizeof(ArchiveMenuThreadParams));
        params->CreateWindowInBand = CreateWindowInBand;
        params->hWnd = &archivehWnd;
        params->wndProc = CLauncherTipContextMenu_WndProc;
        CreateThread(
            0,
            0,
            ArchiveMenuThread,
            params,
            0,
            0
        );
    }



    CreateThread(NULL, 0, CheckForUpdatesThread, 5000, 0, NULL);



    WCHAR wszExtraLibPath[MAX_PATH];
    if (GetWindowsDirectoryW(wszExtraLibPath, MAX_PATH))
    {
        wcscat_s(wszExtraLibPath, MAX_PATH, L"\\ep_extra.dll");
        if (FileExistsW(wszExtraLibPath))
        {
            HMODULE hExtra = LoadLibraryW(wszExtraLibPath);
            if (hExtra)
            {
                printf("[Extra] Found library: %p.\n", hExtra);
                FARPROC ep_extra_entrypoint = GetProcAddress(hExtra, "ep_extra_EntryPoint");
                if (ep_extra_entrypoint)
                {
                    printf("[Extra] Running entry point...\n");
                    ep_extra_entrypoint();
                    printf("[Extra] Finished running entry point.\n");
                }
            }
            else
            {
                printf("[Extra] LoadLibraryW failed with 0x%x.", GetLastError());
            }
        }
    }



    /*if (bHookStartMenu)
    {
        HookStartMenuParams* params2 = calloc(1, sizeof(HookStartMenuParams));
        params2->dwTimeout = 1000;
        params2->hModule = hModule;
        params2->proc = InjectStartFromExplorer;
        GetModuleFileNameW(hModule, params2->wszModulePath, MAX_PATH);
        CreateThread(0, 0, HookStartMenu, params2, 0, 0);
    }*/




    VnPatchDelayIAT(hExplorer, "ext-ms-win-rtcore-ntuser-window-ext-l1-1-0.dll", "GetClientRect", TaskbarCenter_GetClientRectHook);
    if (hMyTaskbar)
        VnPatchIAT(hMyTaskbar, "USER32.dll", "GetClientRect", TaskbarCenter_GetClientRectHook);
    VnPatchIAT(hExplorer, "SHCORE.dll", (LPCSTR)190, TaskbarCenter_SHWindowsPolicy);
    printf("Initialized taskbar centering module.\n");




    //CreateThread(0, 0, PositionStartMenuTimeout, 0, 0, 0);

    /*else
    {
        if (bIsExplorer)
        {
            // deinject all

            rv = funchook_uninstall(funchook, 0);
            if (rv != 0)
            {
                FreeLibraryAndExitThread(hModule, rv);
                return rv;
            }

            rv = funchook_destroy(funchook);
            if (rv != 0)
            {
                FreeLibraryAndExitThread(hModule, rv);
                return rv;
            }
        }

        //SetEvent(hExitSettingsMonitor);
        //WaitForSingleObject(hSettingsMonitorThread, INFINITE);
        //CloseHandle(hExitSettingsMonitor);
        //free(settingsParams);
        //free(settings);
        //InjectBasicFunctions(FALSE, FALSE);
        FreeLibraryAndExitThread(hModule, 0);
    }*/
#endif
    return 0;
}

#if WITH_MAIN_PATCHER
char VisibilityChangedEventArguments_GetVisible(__int64 a1)
{
    int v1;
    char v3[8];
    ZeroMemory(v3, 8);

    v1 = (*(__int64(__fastcall**)(__int64, char*))(*(INT64*)a1 + 48))(a1, v3);
    if (v1 < 0)
        return 0;

    return v3[0];
}

DWORD Start_NoStartMenuMorePrograms = 0;
DWORD Start_ForceStartSize = 0;
DWORD StartMenu_maximumFreqApps = 6;
DWORD StartMenu_ShowAllApps = 0;
DWORD StartDocked_DisableRecommendedSection = FALSE;
DWORD StartDocked_DisableRecommendedSectionApply = TRUE;
DWORD StartUI_EnableRoundedCorners = FALSE;
DWORD StartUI_EnableRoundedCornersApply = TRUE;
DWORD StartUI_ShowMoreTiles = FALSE;
HKEY hKey_StartUI_TileGrid = NULL;

void StartMenu_LoadSettings(BOOL bRestartIfChanged)
{
    HKEY hKey = NULL;
    DWORD dwSize, dwVal;

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("MakeAllAppsDefault"),
            0,
            NULL,
            &StartMenu_ShowAllApps,
            &dwSize
        );
        dwSize = sizeof(DWORD);
        RegQueryValueExW(
            hKey,
            TEXT("MonitorOverride"),
            0,
            NULL,
            &bMonitorOverride,
            &dwSize
        );
        RegCloseKey(hKey);
    }
    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        TEXT(REGPATH_STARTMENU),
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        dwSize = sizeof(DWORD);
        dwVal = 6;
        RegQueryValueExW(
            hKey,
            TEXT("Start_MaximumFrequentApps"),
            0,
            NULL,
            &dwVal,
            &dwSize
        );
        if (bRestartIfChanged && dwVal != StartMenu_maximumFreqApps)
        {
            exit(0);
        }
        StartMenu_maximumFreqApps = dwVal;

        dwSize = sizeof(DWORD);
        dwVal = FALSE;
        RegQueryValueExW(
            hKey,
            TEXT("StartDocked_DisableRecommendedSection"),
            0,
            NULL,
            &dwVal,
            &dwSize
        );
        if (dwVal != StartDocked_DisableRecommendedSection)
        {
            StartDocked_DisableRecommendedSectionApply = TRUE;
        }
        StartDocked_DisableRecommendedSection = dwVal;

        dwSize = sizeof(DWORD);
        dwVal = FALSE;
        RegQueryValueExW(
            hKey,
            TEXT("StartUI_EnableRoundedCorners"),
            0,
            NULL,
            &dwVal,
            &dwSize
        );
        if (dwVal != StartUI_EnableRoundedCorners)
        {
            StartUI_EnableRoundedCornersApply = TRUE;
        }
        StartUI_EnableRoundedCorners = dwVal;

        dwSize = sizeof(DWORD);
        dwVal = FALSE;
        RegQueryValueExW(
            hKey,
            TEXT("StartUI_ShowMoreTiles"),
            0,
            NULL,
            &dwVal,
            &dwSize
        );
        if (bRestartIfChanged && dwStartShowClassicMode && dwVal != StartUI_ShowMoreTiles)
        {
            exit(0);
        }
        StartUI_ShowMoreTiles = dwVal;

        RegCloseKey(hKey);
    }

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        dwSize = sizeof(DWORD);
        if (IsWindows11()) dwVal = 0;
        else dwVal = 1;
        RegQueryValueExW(
            hKey,
            TEXT("Start_ShowClassicMode"),
            0,
            NULL,
            &dwVal,
            &dwSize
        );
        if (!DoesWindows10StartMenuExist())
        {
            dwVal = 0;
        }
        if (bRestartIfChanged && dwVal != dwStartShowClassicMode)
        {
            exit(0);
        }
        dwStartShowClassicMode = dwVal;

        dwSize = sizeof(DWORD);
        if (IsWindows11()) dwVal = 1;
        else dwVal = 0;
        RegQueryValueExW(
            hKey,
            TEXT("TaskbarAl"),
            0,
            NULL,
            &dwVal,
            &dwSize
        );
        if (InterlockedExchange(&dwTaskbarAl, dwVal) != dwVal)
        {
            StartUI_EnableRoundedCornersApply = TRUE;
            StartDocked_DisableRecommendedSectionApply = TRUE;
        }

        RegCloseKey(hKey);
    }

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Policies\\Microsoft\\Windows\\Explorer",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        dwSize = sizeof(DWORD);
        dwVal = 0;
        RegQueryValueExW(
            hKey,
            TEXT("ForceStartSize"),
            0,
            NULL,
            &dwVal,
            &dwSize
        );
        if (bRestartIfChanged && dwVal != Start_ForceStartSize)
        {
            exit(0);
        }
        Start_ForceStartSize = dwVal;

        RegCloseKey(hKey);
    }

    RegCreateKeyExW(
        HKEY_CURRENT_USER,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer",
        0,
        NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_READ,
        NULL,
        &hKey,
        NULL
    );
    if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
    {
        hKey = NULL;
    }
    if (hKey)
    {
        dwSize = sizeof(DWORD);
        dwVal = 0;
        RegQueryValueExW(
            hKey,
            TEXT("NoStartMenuMorePrograms"),
            0,
            NULL,
            &dwVal,
            &dwSize
        );
        if (bRestartIfChanged && dwVal != Start_NoStartMenuMorePrograms)
        {
            exit(0);
        }
        Start_NoStartMenuMorePrograms = dwVal;

        RegCloseKey(hKey);
    }
}

static INT64(*StartDocked_LauncherFrame_OnVisibilityChangedFunc)(void*, INT64, void*) = NULL;

static INT64(*StartDocked_LauncherFrame_ShowAllAppsFunc)(void* _this) = NULL;

INT64 StartDocked_LauncherFrame_OnVisibilityChangedHook(void* _this, INT64 a2, void* VisibilityChangedEventArguments)
{
    INT64 r = 0;
    if (StartDocked_LauncherFrame_OnVisibilityChangedFunc)
    {
        r = StartDocked_LauncherFrame_OnVisibilityChangedFunc(_this, a2, VisibilityChangedEventArguments);
    }
    if (StartMenu_ShowAllApps)
    {
        //if (VisibilityChangedEventArguments_GetVisible(VisibilityChangedEventArguments))
        {
            if (StartDocked_LauncherFrame_ShowAllAppsFunc)
            {
                StartDocked_LauncherFrame_ShowAllAppsFunc(_this);
            }
        }
    }
    return r;
}

INT64(*StartDocked_SystemListPolicyProvider_GetMaximumFrequentAppsFunc)(void*) = NULL;

INT64 StartDocked_SystemListPolicyProvider_GetMaximumFrequentAppsHook(void* _this)
{
    return StartMenu_maximumFreqApps;
}

INT64(*StartUI_SystemListPolicyProvider_GetMaximumFrequentAppsFunc)(void*) = NULL;

INT64 StartUI_SystemListPolicyProvider_GetMaximumFrequentAppsHook(void* _this)
{
    return StartMenu_maximumFreqApps;
}

INT64(*StartDocked_StartSizingFrame_StartSizingFrameFunc)(void* _this) = NULL;

INT64 StartDocked_StartSizingFrame_StartSizingFrameHook(void* _this)
{
    INT64 rv = StartDocked_StartSizingFrame_StartSizingFrameFunc(_this);
    HMODULE hModule = LoadLibraryW(L"Shlwapi.dll");
    if (hModule)
    {
        DWORD dwStatus = 0, dwSize = sizeof(DWORD);
        t_SHRegGetValueFromHKCUHKLM SHRegGetValueFromHKCUHKLMFunc = GetProcAddress(hModule, "SHRegGetValueFromHKCUHKLM");
        if (!SHRegGetValueFromHKCUHKLMFunc || SHRegGetValueFromHKCUHKLMFunc(
            TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"),
            TEXT("TaskbarAl"),
            SRRF_RT_REG_DWORD,
            NULL,
            &dwStatus,
            &dwSize
        ) != ERROR_SUCCESS)
        {
            dwStatus = 0;
        }
        FreeLibrary(hModule);
        *(((char*)_this + 387)) = dwStatus;
    }
    return rv;
}

typedef enum Parser_XamlBufferType
{
    XBT_Text,
    XBT_Binary,
    XBT_MemoryMappedResource
} Parser_XamlBufferType;

typedef struct Parser_XamlBuffer
{
    unsigned int m_count;
    Parser_XamlBufferType m_bufferType;
    const unsigned __int8* m_buffer;
} Parser_XamlBuffer;

static BOOL StartMenu_FillParserBuffer(Parser_XamlBuffer* pBuffer, int resourceId)
{
    HRSRC hRscr = FindResource(hModule, MAKEINTRESOURCE(resourceId), RT_RCDATA);
    if (!hRscr)
        return FALSE;

    HGLOBAL hgRscr = LoadResource(hModule, hRscr);
    if (!hgRscr)
        return FALSE;

    pBuffer->m_buffer = LockResource(hgRscr);
    pBuffer->m_count = SizeofResource(hModule, hRscr);
    pBuffer->m_bufferType = XBT_Binary;
    return TRUE;
}

Parser_XamlBuffer g_EmptyRefreshedStylesXbfBuffer;

HRESULT(*CCoreServices_TryLoadXamlResourceHelperFunc)(void* _this, void* pUri, bool* pfHasBinaryFile, void** ppMemory, Parser_XamlBuffer* pBuffer, void** ppPhysicalUri);
HRESULT CCoreServices_TryLoadXamlResourceHelperHook(void* _this, void* pUri, bool* pfHasBinaryFile, void** ppMemory, Parser_XamlBuffer* pBuffer, void** ppPhysicalUri)
{
    HRESULT(*Clone)(void* _this, void** ppUri); // index 3
    HRESULT(*GetCanonical)(void* _this, unsigned int* pBufferLength, wchar_t* pszBuffer); // index 7
    void** vtable = *(void***)pUri;
    Clone = vtable[3];
    GetCanonical = vtable[7];
    wchar_t szCanonical[MAX_PATH];
    unsigned int len = MAX_PATH;
    GetCanonical(pUri, &len, szCanonical);
    // OutputDebugStringW(szCanonical); OutputDebugStringW(L"<<<<<\n");

    if (!wcscmp(szCanonical, L"ms-appx://Windows.UI.ShellCommon/JumpViewUI/RefreshedStyles.xaml"))
    {
        *pfHasBinaryFile = true;
        *pBuffer = g_EmptyRefreshedStylesXbfBuffer;
        if (ppPhysicalUri)
            Clone(pUri, ppPhysicalUri);
        return pBuffer->m_buffer ? S_OK : E_FAIL;
    }

    return CCoreServices_TryLoadXamlResourceHelperFunc(_this, pUri, pfHasBinaryFile, ppMemory, pBuffer, ppPhysicalUri);
}

static BOOL StartMenu_FixContextMenuXbfHijackMethod()
{
    HANDLE hWindowsUIXaml = LoadLibraryW(L"Windows.UI.Xaml.dll");
    if (!hWindowsUIXaml)
        return FALSE;

    MODULEINFO mi;
    GetModuleInformation(GetCurrentProcess(), hWindowsUIXaml, &mi, sizeof(mi));

    if (!StartMenu_FillParserBuffer(&g_EmptyRefreshedStylesXbfBuffer, IDR_REFRESHEDSTYLES_XBF))
        return FALSE;

#if defined(_M_X64)
    // 49 89 43 C8 E8 ?? ?? ?? ?? 85 C0
    //                ^^^^^^^^^^^
    // Ref: CCoreServices::LoadXamlResource()
    PBYTE match = FindPattern(
        mi.lpBaseOfDll,
        mi.SizeOfImage,
        "\x49\x89\x43\xC8\xE8\x00\x00\x00\x00\x85\xC0",
        "xxxxx????xx"
    );
    if (!match)
        return FALSE;

    match += 4;
    match += 5 + *(int*)(match + 1);
#elif defined(_M_ARM64)
    // E1 0B 40 F9 05 00 80 D2 04 00 80 D2 E3 03 ?? AA E2 03 ?? AA E0 03 ?? AA ?? ?? ?? 97
    //                                                                         ^^^^^^^^^^^
    // Ref: CoreServices_TryGetApplicationResource()
    PBYTE match = FindPattern(
        mi.lpBaseOfDll,
        mi.SizeOfImage,
        "\xE1\x0B\x40\xF9\x05\x00\x80\xD2\x04\x00\x80\xD2\xE3\x03\x00\xAA\xE2\x03\x00\xAA\xE0\x03\x00\xAA\x00\x00\x00\x97",
        "xxxxxxxxxxxxxx?xxx?xxx?x???x"
    );
    if (!match)
        return FALSE;

    match += 24;
    match = (PBYTE)ARM64_FollowBL((DWORD*)match);
    if (!match)
        return FALSE;
#endif

    CCoreServices_TryLoadXamlResourceHelperFunc = match;
    funchook_prepare(
        funchook,
        (void**)&CCoreServices_TryLoadXamlResourceHelperFunc,
        CCoreServices_TryLoadXamlResourceHelperHook
    );
    return TRUE;
}

// void StartUI::UserTileView::AppendMenuFlyoutItemCommand(class Windows::UI::Xaml::Controls::MenuFlyout^, class Windows::Internal::Shell::StartUI::UserTileCommand^, enum Windows::Internal::Shell::StartUI::UserTileCommandId)
void (*StartUI_UserTileView_AppendMenuFlyoutItemCommandFunc)(void* _this, void* menuFlyout, void* userTileCommand, int id);
void StartUI_UserTileView_AppendMenuFlyoutItemCommandHook(void* _this, void* menuFlyout, void* userTileCommand, int id)
{
    // 4 = UserTile_LaunchAccountBadging
    // 5 = UserTile_AccountBadgingSecondary
    if (id == 4 || id == 5)
    {
        return;
    }
    StartUI_UserTileView_AppendMenuFlyoutItemCommandFunc(_this, menuFlyout, userTileCommand, id);
}

static void StartMenu_FixUserTileMenu(MODULEINFO* mi)
{
#if defined(_M_X64)
    // 41 B9 03 00 00 00 4D 8B C4 ?? 8B D6 49 8B CD E8 ?? ?? ?? ??
    //                                                 ^^^^^^^^^^^
    // Ref: <lambda_3a9b433356e31b02e54fffbca0ecf3fa>::operator()
    PBYTE match = FindPattern(
        mi->lpBaseOfDll,
        mi->SizeOfImage,
        "\x41\xB9\x03\x00\x00\x00\x4D\x8B\xC4\x00\x8B\xD6\x49\x8B\xCD\xE8",
        "xxxxxxxxx?xxxxxx"
    );
    if (match)
    {
        match += 15;
        match += 5 + *(int*)(match + 1);
    }
#elif defined(_M_ARM64)
    // 63 00 80 52 E2 03 1B AA E1 03 14 AA E0 03 19 AA ?? ?? ?? 94
    //                                                 ^^^^^^^^^^^
    // Ref: <lambda_3a9b433356e31b02e54fffbca0ecf3fa>::operator()
    PBYTE match = FindPattern(
        mi->lpBaseOfDll,
        mi->SizeOfImage,
        "\x63\x00\x80\x52\xE2\x03\x1B\xAA\xE1\x03\x14\xAA\xE0\x03\x19\xAA\x00\x00\x00\x94",
        "xxxxxxxxxxxxxxxx???x"
    );
    if (match)
    {
        match += 16;
        match = (PBYTE)ARM64_FollowBL((DWORD*)match);
    }
#endif

    if (match)
    {
        StartUI_UserTileView_AppendMenuFlyoutItemCommandFunc = match;
        funchook_prepare(
            funchook,
            (void**)&StartUI_UserTileView_AppendMenuFlyoutItemCommandFunc,
            StartUI_UserTileView_AppendMenuFlyoutItemCommandHook
        );
    }
}

LSTATUS StartUI_RegOpenKeyExW(HKEY hKey, LPCWSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
    if (wcsstr(lpSubKey, L"$start.tilegrid$windows.data.curatedtilecollection.tilecollection\\Current"))
    {
        LSTATUS lRes = RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
        if (lRes == ERROR_SUCCESS)
        {
            hKey_StartUI_TileGrid = *phkResult;
        }
        return lRes;
    }
    return RegOpenKeyExW(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}

LSTATUS StartUI_RegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
    if (hKey == hKey_StartUI_TileGrid)
    {
        if (!_wcsicmp(lpValueName, L"Data"))
        {
            LSTATUS lRes = RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
            if (lRes == ERROR_SUCCESS && lpData && *lpcbData >= 26)
            {
                lpData[25] = (StartUI_ShowMoreTiles ? 16 : 12);
            }
            return lRes;
        }
    }
    return RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

LSTATUS StartUI_RegCloseKey(HKEY hKey)
{
    if (hKey == hKey_StartUI_TileGrid)
    {
        hKey_StartUI_TileGrid = NULL;
    }
    return RegCloseKey(hKey);
}

int Start_SetWindowRgn(HWND hWnd, HRGN hRgn, BOOL bRedraw)
{
    WCHAR wszDebug[MAX_PATH];
    BOOL bIsWindowVisible = FALSE;
    HRESULT hr = IsThreadCoreWindowVisible(&bIsWindowVisible);
    if (SUCCEEDED(hr))
    {
#if WITH_SMA_PATCH_REPORT
        if (dwStartShowClassicMode && IsWindows11())
        {
            HANDLE hAnimationsPatched = OpenMutexW(SYNCHRONIZE, FALSE, _T(EPStart10_AnimationsPatched));
            if (hAnimationsPatched)
            {
                CloseHandle(hAnimationsPatched);
            }
            else
            {
                ShowWindow(hWnd, bIsWindowVisible ? SW_SHOW : SW_HIDE);
            }
        }
#endif
        DWORD TaskbarAl = InterlockedAdd(&dwTaskbarAl, 0);
        if (bIsWindowVisible && (!TaskbarAl ? (dwStartShowClassicMode ? StartUI_EnableRoundedCornersApply : StartDocked_DisableRecommendedSectionApply) : 1))
        {
            HWND hWndTaskbar = NULL;
            if (TaskbarAl)
            {
                HMONITOR hMonitorOfStartMenu = NULL;
                if (bMonitorOverride == 1 || !bMonitorOverride) {
                    POINT pt;
                    if (!bMonitorOverride) GetCursorPos(&pt);
                    else {
                        pt.x = 0; pt.y = 0;
                    }
                    hMonitorOfStartMenu = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
                }
                else {
                    MonitorOverrideData mod;
                    mod.cbIndex = 2;
                    mod.dwIndex = bMonitorOverride;
                    mod.hMonitor = NULL;
                    EnumDisplayMonitors(NULL, NULL, ExtractMonitorByIndex, &mod);
                    if (mod.hMonitor == NULL)
                    {
                        POINT pt; pt.x = 0; pt.y = 0;
                        hMonitorOfStartMenu = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
                    }
                    else
                    {
                        hMonitorOfStartMenu = mod.hMonitor;
                    }
                }

                HWND hWndTemp = NULL;

                HWND hShellTray_Wnd = FindWindowExW(NULL, NULL, L"Shell_TrayWnd", NULL);
                if (hShellTray_Wnd && !hWndTaskbar && hMonitorOfStartMenu == MonitorFromWindow(hShellTray_Wnd, MONITOR_DEFAULTTOPRIMARY) && dwOldTaskbarAl)
                {
                    hWndTaskbar = hShellTray_Wnd;
                }

                if (!hWndTaskbar)
                {
                    do
                    {
                        hWndTemp = FindWindowExW(
                            NULL,
                            hWndTemp,
                            L"Shell_SecondaryTrayWnd",
                            NULL
                        );
                        if (hWndTemp && !hWndTaskbar && hMonitorOfStartMenu == MonitorFromWindow(hWndTemp, MONITOR_DEFAULTTOPRIMARY) && dwMMOldTaskbarAl)
                        {
                            hWndTaskbar = hWndTemp;
                            break;
                        }
                    } while (hWndTemp);
                }

                if(!hWndTaskbar)
                {
                    hWndTaskbar = hShellTray_Wnd;
                }
            }
            MONITORINFO mi;
            ZeroMemory(&mi, sizeof(MONITORINFO));
            mi.cbSize = sizeof(MONITORINFO);
            GetMonitorInfoW(MonitorFromWindow(hWndTaskbar ? hWndTaskbar : hWnd, MONITOR_DEFAULTTOPRIMARY), &mi);
            DWORD dwPos = 0;
            RECT rcC;
            if (hWndTaskbar)
            {
                GetWindowRect(hWndTaskbar, &rcC);
                rcC.left -= mi.rcMonitor.left;
                rcC.right -= mi.rcMonitor.left;
                rcC.top -= mi.rcMonitor.top;
                rcC.bottom -= mi.rcMonitor.top;
                if (rcC.left < 5 && rcC.top > 5)
                {
                    dwPos = TB_POS_BOTTOM;
                }
                else if (rcC.left < 5 && rcC.top < 5 && rcC.right > rcC.bottom)
                {
                    dwPos = TB_POS_TOP;
                }
                else if (rcC.left < 5 && rcC.top < 5 && rcC.right < rcC.bottom)
                {
                    dwPos = TB_POS_LEFT;
                }
                else if (rcC.left > 5 && rcC.top < 5)
                {
                    dwPos = TB_POS_RIGHT;
                }
            }
            RECT rc;
            if (dwStartShowClassicMode)
            {
                LVT_StartUI_EnableRoundedCorners(hWnd, StartUI_EnableRoundedCorners, dwPos, hWndTaskbar, &rc);
                if (!StartUI_EnableRoundedCorners)
                {
                    StartUI_EnableRoundedCornersApply = FALSE;
                }
            }
            else
            {
                LVT_StartDocked_DisableRecommendedSection(hWnd, StartDocked_DisableRecommendedSection, &rc);
                //StartDocked_DisableRecommendedSectionApply = FALSE;
            }
            if (hWndTaskbar)
            {
                if (rcC.left < 5 && rcC.top > 5)
                {
                    if (dwStartShowClassicMode)
                    {
                        SetWindowPos(hWnd, NULL, mi.rcMonitor.left + (((mi.rcMonitor.right - mi.rcMonitor.left) - (rc.right - rc.left)) / 2), mi.rcMonitor.top, 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
                    }
                    else
                    {
                        // Windows 11 Start menu knows how to center itself when the taskbar is at the bottom of the screen
                        SetWindowPos(hWnd, NULL, mi.rcMonitor.left, mi.rcMonitor.top, 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
                    }
                }
                else if (rcC.left < 5 && rcC.top < 5 && rcC.right > rcC.bottom)
                {
                    SetWindowPos(hWnd, NULL, mi.rcMonitor.left + (((mi.rcMonitor.right - mi.rcMonitor.left) - (rc.right - rc.left)) / 2), mi.rcMonitor.top + (rcC.bottom - rcC.top), 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
                }
                else if (rcC.left < 5 && rcC.top < 5 && rcC.right < rcC.bottom)
                {
                    SetWindowPos(hWnd, NULL, mi.rcMonitor.left + (rcC.right - rcC.left), mi.rcMonitor.top + (((mi.rcMonitor.bottom - mi.rcMonitor.top) - (rc.bottom - rc.top)) / 2), 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
                }
                else if (rcC.left > 5 && rcC.top < 5)
                {
                    SetWindowPos(hWnd, NULL, mi.rcMonitor.left, mi.rcMonitor.top + (((mi.rcMonitor.bottom - mi.rcMonitor.top) - (rc.bottom - rc.top)) / 2), 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
                }
            }
            else
            {
                SetWindowPos(hWnd, NULL, mi.rcWork.left, mi.rcWork.top, 0, 0, SWP_NOSIZE | SWP_FRAMECHANGED | SWP_ASYNCWINDOWPOS);
            }
        }
        if (bIsWindowVisible && dwStartShowClassicMode && IsWindows11Version22H2Build2134OrHigher())
        {
            extern void NeedsRo_SyncSettingsFromRegToCDS();
            NeedsRo_SyncSettingsFromRegToCDS();
        }
    }
    return SetWindowRgn(hWnd, hRgn, bRedraw);
}

int WINAPI SetupMessage(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
    return 0;
    LPCWSTR lpOldText = lpText;
    LPCWSTR lpOldCaption = lpCaption;
    wchar_t wszText[MAX_PATH];
    ZeroMemory(wszText, MAX_PATH * sizeof(wchar_t));
    wchar_t wszCaption[MAX_PATH];
    ZeroMemory(wszCaption, MAX_PATH * sizeof(wchar_t));
    LoadStringW(hModule, IDS_PRODUCTNAME, wszCaption, MAX_PATH);
    switch (Code)
    {
    case 1:
        LoadStringW(hModule, IDS_INSTALL_SUCCESS_TEXT, wszText, MAX_PATH);
        break;
    case -1:
        LoadStringW(hModule, IDS_INSTALL_ERROR_TEXT, wszText, MAX_PATH);
        break;
    case 2:
        LoadStringW(hModule, IDS_UNINSTALL_SUCCESS_TEXT, wszText, MAX_PATH);
        break;
    case -2:
        LoadStringW(hModule, IDS_UNINSTALL_ERROR_TEXT, wszText, MAX_PATH);
        break;
    default:
        LoadStringW(hModule, IDS_OPERATION_NONE, wszText, MAX_PATH);
        break;
    }
    int ret = MessageBoxW(hWnd, wszText, wszCaption, uType);
    lpText = lpOldText;
    lpOldCaption = lpOldCaption;
    return ret;
}

void Setup_Regsvr32(BOOL bInstall)
{
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    if (!IsAppRunningAsAdminMode())
    {
        wchar_t wszPath[MAX_PATH];
        ZeroMemory(wszPath, ARRAYSIZE(wszPath));
        wchar_t wszCurrentDirectory[MAX_PATH];
        ZeroMemory(wszCurrentDirectory, ARRAYSIZE(wszCurrentDirectory));
        if (GetModuleFileNameW(NULL, wszPath, ARRAYSIZE(wszPath)) &&
            GetCurrentDirectoryW(ARRAYSIZE(wszCurrentDirectory), wszCurrentDirectory + (bInstall ? 1 : 4)))
        {
            wszCurrentDirectory[0] = L'"';
            if (!bInstall)
            {
                wszCurrentDirectory[0] = L'/';
                wszCurrentDirectory[1] = L'u';
                wszCurrentDirectory[2] = L' ';
                wszCurrentDirectory[3] = L'"';
            }
#if defined(_M_X64)
            wcscat_s(wszCurrentDirectory, ARRAYSIZE(wszCurrentDirectory), L"\\ExplorerPatcher.amd64.dll\"");
#elif defined(_M_ARM64)
            wcscat_s(wszCurrentDirectory, ARRAYSIZE(wszCurrentDirectory), L"\\ExplorerPatcher.arm64.dll\"");
#else
#error "Unsupported architecture"
#endif
            SHELLEXECUTEINFOW sei;
            ZeroMemory(&sei, sizeof(SHELLEXECUTEINFOW));
            sei.cbSize = sizeof(sei);
            sei.lpVerb = L"runas";
            sei.lpFile = wszPath;
            sei.lpParameters = wszCurrentDirectory;
            sei.hwnd = NULL;
            sei.nShow = SW_NORMAL;
            if (!ShellExecuteExW(&sei))
            {
                DWORD dwError = GetLastError();
                if (dwError == ERROR_CANCELLED)
                {
                    wchar_t wszText[MAX_PATH];
                    ZeroMemory(wszText, MAX_PATH * sizeof(wchar_t));
                    wchar_t wszCaption[MAX_PATH];
                    ZeroMemory(wszCaption, MAX_PATH * sizeof(wchar_t));
                    LoadStringW(hModule, IDS_PRODUCTNAME, wszCaption, MAX_PATH);
                    LoadStringW(hModule, IDS_INSTALL_ERROR_TEXT, wszText, MAX_PATH);
                    MessageBoxW(0, wszText, wszCaption, MB_ICONINFORMATION);
                }
            }
            exit(0);
        }
    }

    VnPatchDelayIAT(GetModuleHandle(NULL), "ext-ms-win-ntuser-dialogbox-l1-1-0.dll", "MessageBoxW", SetupMessage);
}

#ifdef _WIN64
#pragma comment(linker, "/export:DllRegisterServer=_DllRegisterServer")
#endif
HRESULT WINAPI _DllRegisterServer()
{
    DWORD dwLastError = ERROR_SUCCESS;
    HKEY hKey = NULL;
    DWORD dwSize = 0;
    wchar_t wszFilename[MAX_PATH];
    wchar_t wszInstallPath[MAX_PATH];

    Setup_Regsvr32(TRUE);

    if (!dwLastError)
    {
        if (!GetModuleFileNameW(hModule, wszFilename, MAX_PATH))
        {
            dwLastError = GetLastError();
        }
    }
    if (!dwLastError)
    {
        dwLastError = RegCreateKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Classes\\CLSID\\" TEXT(EP_CLSID) L"\\InProcServer32",
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_WRITE | KEY_WOW64_64KEY,
            NULL,
            &hKey,
            NULL
        );
        if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
        {
            hKey = NULL;
        }
        if (hKey)
        {
            dwLastError = RegSetValueExW(
                hKey,
                NULL,
                0,
                REG_SZ,
                wszFilename,
                (wcslen(wszFilename) + 1) * sizeof(wchar_t)
            );
            dwLastError = RegSetValueExW(
                hKey,
                L"ThreadingModel",
                0,
                REG_SZ,
                L"Apartment",
                10 * sizeof(wchar_t)
            );
            RegCloseKey(hKey);
        }
    }
    if (!dwLastError)
    {
        PathRemoveExtensionW(wszFilename);
        PathRemoveExtensionW(wszFilename);
        wcscat_s(wszFilename, MAX_PATH, L".IA-32.dll");
    }
    if (!dwLastError)
    {
        dwLastError = RegCreateKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\WOW6432Node\\Classes\\CLSID\\" TEXT(EP_CLSID) L"\\InProcServer32",
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_WRITE | KEY_WOW64_64KEY,
            NULL,
            &hKey,
            NULL
        );
        if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
        {
            hKey = NULL;
        }
        if (hKey)
        {
            dwLastError = RegSetValueExW(
                hKey,
                NULL,
                0,
                REG_SZ,
                wszFilename,
                (wcslen(wszFilename) + 1) * sizeof(wchar_t)
            );
            dwLastError = RegSetValueExW(
                hKey,
                L"ThreadingModel",
                0,
                REG_SZ,
                L"Apartment",
                10 * sizeof(wchar_t)
            );
            RegCloseKey(hKey);
        }
    }
    if (!dwLastError)
    {
        dwLastError = RegCreateKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Classes\\Drive\\shellex\\FolderExtensions\\" TEXT(EP_CLSID),
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_WRITE | KEY_WOW64_64KEY,
            NULL,
            &hKey,
            NULL
        );
        if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
        {
            hKey = NULL;
        }
        if (hKey)
        {
            DWORD dwDriveMask = 255;
            dwLastError = RegSetValueExW(
                hKey,
                L"DriveMask",
                0,
                REG_DWORD,
                &dwDriveMask,
                sizeof(DWORD)
            );
            RegCloseKey(hKey);
        }
    }
    /*if (!dwLastError)
    {
        dwLastError = RegCreateKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\" TEXT(EP_CLSID),
            0,
            NULL,
            REG_OPTION_NON_VOLATILE,
            KEY_WRITE | KEY_WOW64_64KEY,
            NULL,
            &hKey,
            NULL
        );
        if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
        {
            hKey = NULL;
        }
        if (hKey)
        {
            DWORD dwNoInternetExplorer = 1;
            dwLastError = RegSetValueExW(
                hKey,
                L"NoInternetExplorer",
                0,
                REG_DWORD,
                &dwNoInternetExplorer,
                sizeof(DWORD)
            );
            RegCloseKey(hKey);
        }
    }*/
    Code = 1;
    if (dwLastError) Code = -Code;

    //ZZRestartExplorer(0, 0, 0, 0);

    return dwLastError == 0 ? S_OK : HRESULT_FROM_WIN32(dwLastError);
}

#ifdef _WIN64
#pragma comment(linker, "/export:DllUnregisterServer=_DllUnregisterServer")
#endif
HRESULT WINAPI _DllUnregisterServer()
{
    DWORD dwLastError = ERROR_SUCCESS;
    HKEY hKey = NULL;
    DWORD dwSize = 0;
    wchar_t wszFilename[MAX_PATH];

    Setup_Regsvr32(FALSE);

    if (!dwLastError)
    {
        if (!GetModuleFileNameW(hModule, wszFilename, MAX_PATH))
        {
            dwLastError = GetLastError();
        }
    }
    if (!dwLastError)
    {
        dwLastError = RegOpenKeyW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Classes\\CLSID\\" TEXT(EP_CLSID),
            &hKey
        );
        if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
        {
            hKey = NULL;
        }
        if (hKey)
        {
            dwLastError = RegDeleteTreeW(
                hKey,
                0
            );
            RegCloseKey(hKey);
            if (!dwLastError)
            {
                RegDeleteTreeW(
                    HKEY_LOCAL_MACHINE,
                    L"SOFTWARE\\Classes\\CLSID\\" TEXT(EP_CLSID)
                );
            }
        }
    }
    if (!dwLastError)
    {
        PathRemoveExtensionW(wszFilename);
        PathRemoveExtensionW(wszFilename);
        wcscat_s(wszFilename, MAX_PATH, L".IA-32.dll");
    }
    if (!dwLastError)
    {
        dwLastError = RegOpenKeyW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\WOW6432Node\\Classes\\CLSID\\" TEXT(EP_CLSID),
            &hKey
        );
        if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
        {
            hKey = NULL;
        }
        if (hKey)
        {
            dwLastError = RegDeleteTreeW(
                hKey,
                0
            );
            RegCloseKey(hKey);
            if (!dwLastError)
            {
                RegDeleteTreeW(
                    HKEY_LOCAL_MACHINE,
                    L"SOFTWARE\\WOW6432Node\\Classes\\CLSID\\" TEXT(EP_CLSID)
                );
            }
        }
    }
    if (!dwLastError)
    {
        dwLastError = RegOpenKeyW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Classes\\Drive\\shellex\\FolderExtensions\\" TEXT(EP_CLSID),
            &hKey
        );
        if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
        {
            hKey = NULL;
        }
        if (hKey)
        {
            dwLastError = RegDeleteTreeW(
                hKey,
                0
            );
            RegCloseKey(hKey);
            if (!dwLastError)
            {
                RegDeleteTreeW(
                    HKEY_LOCAL_MACHINE,
                    L"SOFTWARE\\Classes\\Drive\\shellex\\FolderExtensions\\" TEXT(EP_CLSID)
                );
            }
        }
    }
    /*if (!dwLastError)
    {
        dwLastError = RegOpenKeyW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\" TEXT(EP_CLSID),
            &hKey
        );
        if (hKey == NULL || hKey == INVALID_HANDLE_VALUE)
        {
            hKey = NULL;
        }
        if (hKey)
        {
            dwLastError = RegDeleteTreeW(
                hKey,
                0
            );
            RegCloseKey(hKey);
            if (!dwLastError)
            {
                RegDeleteTreeW(
                    HKEY_LOCAL_MACHINE,
                    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Browser Helper Objects\\" TEXT(EP_CLSID)
                );
            }
        }
    }*/
    Code = 2;
    if (dwLastError) Code = -Code;

    //ZZRestartExplorer(0, 0, 0, 0);

    return dwLastError == 0 ? S_OK : HRESULT_FROM_WIN32(dwLastError);
}
#endif

#ifdef _WIN64
#pragma comment(linker, "/export:DllCanUnloadNow=_DllCanUnloadNow")
#else
#pragma comment(linker, "/export:DllCanUnloadNow=__DllCanUnloadNow@0")
#endif
HRESULT WINAPI _DllCanUnloadNow()
{
    return S_FALSE;
}

DWORD InjectStartMenu()
{
#if WITH_MAIN_PATCHER
    funchook = funchook_create();

    HANDLE hStartDocked = NULL;
    HANDLE hStartUI = NULL;

    if (!IsWindows11()) dwTaskbarAl = 0;

    StartMenu_LoadSettings(FALSE);

    if (dwStartShowClassicMode || !IsWindows11())
    {
        hStartUI = LoadLibraryW(L"StartUI.dll");
        if (!hStartUI)
            hStartUI = LoadLibraryW(L"StartUI_.dll");

        // Fixes hang when Start menu closes
        VnPatchDelayIAT(hStartUI, "ext-ms-win-ntuser-draw-l1-1-0.dll", "SetWindowRgn", Start_SetWindowRgn);

        if (IsWindows11())
        {
            MODULEINFO miStartUI;
            GetModuleInformation(GetCurrentProcess(), hStartUI, &miStartUI, sizeof(miStartUI));

            // Fixes Pin to Start/Unpin from Start
            PatchAppResolver();
            PatchStartTileData(TRUE);

            // Fixes context menu crashes
            StartMenu_FixContextMenuXbfHijackMethod();

            // Fixes user tile menu
            StartMenu_FixUserTileMenu(&miStartUI);

            // Enables "Show more tiles" setting
            LoadLibraryW(L"Windows.CloudStore.dll");
            HANDLE hWindowsCloudStore = GetModuleHandleW(L"Windows.CloudStore.dll");
            VnPatchIAT(hWindowsCloudStore, "api-ms-win-core-registry-l1-1-0.dll", "RegOpenKeyExW", StartUI_RegOpenKeyExW);
            VnPatchIAT(hWindowsCloudStore, "api-ms-win-core-registry-l1-1-0.dll", "RegQueryValueExW", StartUI_RegQueryValueExW);
            VnPatchIAT(hWindowsCloudStore, "api-ms-win-core-registry-l1-1-0.dll", "RegCloseKey", StartUI_RegCloseKey);
        }
    }
    else
    {
        LoadLibraryW(L"StartDocked.dll");
        hStartDocked = GetModuleHandleW(L"StartDocked.dll");

        VnPatchDelayIAT(hStartDocked, "ext-ms-win-ntuser-draw-l1-1-0.dll", "SetWindowRgn", Start_SetWindowRgn);
    }

    Setting* settings = calloc(6, sizeof(Setting));
    settings[0].callback = NULL;
    settings[0].data = NULL;
    settings[0].hEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    settings[0].hKey = NULL;
    ZeroMemory(settings[0].name, MAX_PATH);
    settings[0].origin = NULL;
    settings[1].callback = StartMenu_LoadSettings;
    settings[1].data = FALSE;
    settings[1].hEvent = NULL;
    settings[1].hKey = NULL;
    wcscpy_s(settings[1].name, MAX_PATH, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StartPage");
    settings[1].origin = HKEY_CURRENT_USER;
    settings[2].callback = StartMenu_LoadSettings;
    settings[2].data = TRUE;
    settings[2].hEvent = NULL;
    settings[2].hKey = NULL;
    wcscpy_s(settings[2].name, MAX_PATH, TEXT(REGPATH_STARTMENU));
    settings[2].origin = HKEY_CURRENT_USER;
    settings[3].callback = StartMenu_LoadSettings;
    settings[3].data = TRUE;
    settings[3].hEvent = NULL;
    settings[3].hKey = NULL;
    wcscpy_s(settings[3].name, MAX_PATH, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced");
    settings[3].origin = HKEY_CURRENT_USER;
    settings[4].callback = StartMenu_LoadSettings;
    settings[4].data = TRUE;
    settings[4].hEvent = NULL;
    settings[4].hKey = NULL;
    wcscpy_s(settings[4].name, MAX_PATH, L"SOFTWARE\\Policies\\Microsoft\\Windows\\Explorer");
    settings[4].origin = HKEY_CURRENT_USER;
    settings[5].callback = StartMenu_LoadSettings;
    settings[5].data = TRUE;
    settings[5].hEvent = NULL;
    settings[5].hKey = NULL;
    wcscpy_s(settings[5].name, MAX_PATH, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer");
    settings[5].origin = HKEY_CURRENT_USER;

    SettingsChangeParameters* params = calloc(1, sizeof(SettingsChangeParameters));
    params->settings = settings;
    params->size = 6;
    CreateThread(
        0,
        0,
        MonitorSettings,
        params,
        0,
        0
    );

    int rv;
    DWORD dwVal0 = 0, dwVal1 = 0, dwVal2 = 0, dwVal3 = 0, dwVal4 = 0;

    HMODULE hModule = LoadLibraryW(L"Shlwapi.dll");
    if (hModule)
    {
        DWORD dwStatus = 0, dwSize = sizeof(DWORD);
        t_SHRegGetValueFromHKCUHKLM SHRegGetValueFromHKCUHKLM = GetProcAddress(hModule, "SHRegGetValueFromHKCUHKLM");

        if (SHRegGetValueFromHKCUHKLM)
        {
            dwSize = sizeof(DWORD);
            SHRegGetValueFromHKCUHKLM(
                TEXT(REGPATH_STARTMENU) TEXT("\\") TEXT(STARTDOCKED_SB_NAME),
                TEXT(STARTDOCKED_SB_0),
                SRRF_RT_REG_DWORD,
                NULL,
                &dwVal0,
                &dwSize
            );
            SHRegGetValueFromHKCUHKLM(
                TEXT(REGPATH_STARTMENU) TEXT("\\") TEXT(STARTDOCKED_SB_NAME),
                TEXT(STARTDOCKED_SB_1),
                SRRF_RT_REG_DWORD,
                NULL,
                &dwVal1,
                &dwSize
            );
            SHRegGetValueFromHKCUHKLM(
                TEXT(REGPATH_STARTMENU) TEXT("\\") TEXT(STARTDOCKED_SB_NAME),
                TEXT(STARTDOCKED_SB_2),
                SRRF_RT_REG_DWORD,
                NULL,
                &dwVal2,
                &dwSize
            );
            SHRegGetValueFromHKCUHKLM(
                TEXT(REGPATH_STARTMENU) TEXT("\\") TEXT(STARTDOCKED_SB_NAME),
                TEXT(STARTDOCKED_SB_3),
                SRRF_RT_REG_DWORD,
                NULL,
                &dwVal3,
                &dwSize
            );
            SHRegGetValueFromHKCUHKLM(
                TEXT(REGPATH_STARTMENU) TEXT("\\") TEXT(STARTUI_SB_NAME),
                TEXT(STARTUI_SB_0),
                SRRF_RT_REG_DWORD,
                NULL,
                &dwVal4,
                &dwSize
            );
        }
        FreeLibrary(hModule);
    }

    if (dwVal1 && dwVal1 != 0xFFFFFFFF && hStartDocked)
    {
        StartDocked_LauncherFrame_ShowAllAppsFunc = (INT64(*)(void*))
            ((uintptr_t)hStartDocked + dwVal1);
    }
    if (dwVal2 && dwVal2 != 0xFFFFFFFF && hStartDocked)
    {
        StartDocked_LauncherFrame_OnVisibilityChangedFunc = (INT64(*)(void*, INT64, void*))
            ((uintptr_t)hStartDocked + dwVal2);
        rv = funchook_prepare(
            funchook,
            (void**)&StartDocked_LauncherFrame_OnVisibilityChangedFunc,
            StartDocked_LauncherFrame_OnVisibilityChangedHook
        );
        if (rv != 0)
        {
            FreeLibraryAndExitThread(hModule, rv);
            return rv;
        }
    }
    if (dwVal3 && dwVal3 != 0xFFFFFFFF && hStartDocked)
    {
        StartDocked_SystemListPolicyProvider_GetMaximumFrequentAppsFunc = (INT64(*)(void*, INT64, void*))
            ((uintptr_t)hStartDocked + dwVal3);
        rv = funchook_prepare(
            funchook,
            (void**)&StartDocked_SystemListPolicyProvider_GetMaximumFrequentAppsFunc,
            StartDocked_SystemListPolicyProvider_GetMaximumFrequentAppsHook
        );
        if (rv != 0)
        {
            FreeLibraryAndExitThread(hModule, rv);
            return rv;
        }
    }
    if (dwVal4 && dwVal4 != 0xFFFFFFFF && hStartUI)
    {
        StartUI_SystemListPolicyProvider_GetMaximumFrequentAppsFunc = (INT64(*)(void*, INT64, void*))
            ((uintptr_t)hStartUI + dwVal4);
        rv = funchook_prepare(
            funchook,
            (void**)&StartUI_SystemListPolicyProvider_GetMaximumFrequentAppsFunc,
            StartUI_SystemListPolicyProvider_GetMaximumFrequentAppsHook
        );
        if (rv != 0)
        {
            FreeLibraryAndExitThread(hModule, rv);
            return rv;
        }
    }

    rv = funchook_install(funchook, 0);
    if (rv != 0)
    {
        FreeLibraryAndExitThread(hModule, rv);
        return rv;
    }
    funchook_destroy(funchook);
    funchook = NULL;
#endif
    return 0;
}

void InjectShellExperienceHost()
{
#if WITH_MAIN_PATCHER
    HKEY hKey;
    if (RegOpenKeyW(HKEY_CURRENT_USER, _T(SEH_REGPATH), &hKey) != ERROR_SUCCESS)
    {
        return;
    }
    RegCloseKey(hKey);
    HMODULE hQA = LoadLibraryW(L"Windows.UI.QuickActions.dll");
    if (hQA)
    {
        PIMAGE_DOS_HEADER dosHeader = hQA;
        if (dosHeader->e_magic == IMAGE_DOS_SIGNATURE)
        {
            PIMAGE_NT_HEADERS64 ntHeader = (PIMAGE_NT_HEADERS64)((u_char*)dosHeader + dosHeader->e_lfanew);
            if (ntHeader->Signature == IMAGE_NT_SIGNATURE)
            {
                PBYTE pSEHPatchArea = NULL;
                BYTE seh_pattern1[14] =
                {
                    // mov al, 1
                    0xB0, 0x01,
                    // jmp + 2
                    0xEB, 0x02,
                    // xor al, al
                    0x32, 0xC0,
                    // add rsp, 0x20
                    0x48, 0x83, 0xC4, 0x20,
                    // pop rdi
                    0x5F,
                    // pop rsi
                    0x5E,
                    // pop rbx
                    0x5B,
                    // ret
                    0xC3
                };
                BYTE seh_off = 12;
                BYTE seh_pattern2[5] =
                {
                    // mov r8b, 3
                    0x41, 0xB0, 0x03,
                    // mov dl, 1
                    0xB2, 0x01
                };
                BOOL bTwice = FALSE;
                PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(ntHeader);
                for (unsigned int i = 0; i < ntHeader->FileHeader.NumberOfSections; ++i)
                {
                    if (section->Characteristics & IMAGE_SCN_CNT_CODE)
                    {
                        if (section->SizeOfRawData && !bTwice)
                        {
                            PBYTE pSectionBegin = (PBYTE)hQA + section->VirtualAddress;
                            //DWORD dwOldProtect;
                            //VirtualProtect(pSectionBegin, section->SizeOfRawData, PAGE_EXECUTE_READWRITE, &dwOldProtect);
                            PBYTE pCandidate = NULL;
                            while (TRUE)
                            {
                                pCandidate = memmem(
                                    !pCandidate ? pSectionBegin : pCandidate,
                                    !pCandidate ? section->SizeOfRawData : (uintptr_t)section->SizeOfRawData - (uintptr_t)(pCandidate - pSectionBegin),
                                    seh_pattern1,
                                    sizeof(seh_pattern1)
                                );
                                if (!pCandidate)
                                {
                                    break;
                                }
                                PBYTE pCandidate2 = pCandidate - seh_off - sizeof(seh_pattern2);
                                if (pCandidate2 > section->VirtualAddress)
                                {
                                    if (memmem(pCandidate2, sizeof(seh_pattern2), seh_pattern2, sizeof(seh_pattern2)))
                                    {
                                        if (!pSEHPatchArea)
                                        {
                                            pSEHPatchArea = pCandidate;
                                        }
                                        else
                                        {
                                            bTwice = TRUE;
                                        }
                                    }
                                }
                                pCandidate += sizeof(seh_pattern1);
                            }
                            //VirtualProtect(pSectionBegin, section->SizeOfRawData, dwOldProtect, &dwOldProtect);
                        }
                    }
                    section++;
                }
                if (pSEHPatchArea && !bTwice)
                {
                    DWORD dwOldProtect;
                    VirtualProtect(pSEHPatchArea, sizeof(seh_pattern1), PAGE_EXECUTE_READWRITE, &dwOldProtect);
                    pSEHPatchArea[2] = 0x90;
                    pSEHPatchArea[3] = 0x90;
                    VirtualProtect(pSEHPatchArea, sizeof(seh_pattern1), dwOldProtect, &dwOldProtect);
                }
            }
        }
    }
#endif
}

// On 22H2 builds, the Windows 10 flyouts for network and battery can be enabled
// by patching either of the following functions in ShellExperienceHost. I didn't
// see any differences when patching with any of the 3 methods, although
// `SharedUtilities::IsWindowsLite` seems to be invoked in more places, whereas `GetProductInfo`
// and `RtlGetDeviceFamilyInfoEnum` are only called in `FlightHelper::CalculateRepaintEnabled`
// and either seems to get the job done. YMMV

LSTATUS SEH_RegGetValueW(HKEY hkey, LPCWSTR lpSubKey, LPCWSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData) {
    if (!lstrcmpW(lpValue, L"UseLiteLayout")) { *(DWORD*)pvData = 1; return ERROR_SUCCESS; }
    return RegGetValueW(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}

BOOL SEH_RtlGetDeviceFamilyInfoEnum(INT64 u0, PDWORD u1, INT64 u2) {
    *u1 = 10;
    return TRUE;
}

BOOL SEH_GetProductInfo(DWORD dwOSMajorVersion, DWORD dwOSMinorVersion, DWORD dwSpMajorVersion, DWORD dwSpMinorVersion, PDWORD pdwReturnedProductType) {
    *pdwReturnedProductType = 119;
    return TRUE;
}

void InjectShellExperienceHostFor22H2OrHigher() {
#if WITH_MAIN_PATCHER
    if (!IsWindows11Version22H2Build1413OrHigher())
    {
        HKEY hKey;
        if (RegOpenKeyW(HKEY_CURRENT_USER, _T(SEH_REGPATH), &hKey) == ERROR_SUCCESS)
        {
            RegCloseKey(hKey);
            HMODULE hQA = LoadLibraryW(L"Windows.UI.QuickActions.dll");
            if (hQA) VnPatchIAT(hQA, "api-ms-win-core-sysinfo-l1-2-0.dll", "GetProductInfo", SEH_GetProductInfo);
            // if (hQA) VnPatchIAT(hQA, "ntdll.dll", "RtlGetDeviceFamilyInfoEnum", SEH_RtlGetDeviceFamilyInfoEnum);
            // if (hQA) VnPatchIAT(hQA, "api-ms-win-core-registry-l1-1-0.dll", "RegGetValueW", SEH_RegGetValueW);
        }
    }
#endif
}

#if WITH_MAIN_PATCHER
bool IsUserOOBE()
{
    BOOL b = FALSE;
    SHRegGetBOOLWithREGSAM(
        HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\OOBE",
        L"LaunchUserOOBE",
        0,
        &b
    );
    return b;
}

bool IsCredentialReset()
{
    BOOL b = FALSE;
    SHRegGetBOOLWithREGSAM(
        HKEY_LOCAL_MACHINE,
        L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Authentication\\CFL\\ExperienceManagerData",
        L"LaunchCflScenario",
        0,
        &b
    );
    return b;
}

bool IsUserOOBEOrCredentialReset()
{
    return IsUserOOBE() || IsCredentialReset();
}
#endif

#define DLL_INJECTION_METHOD_DXGI 0
#define DLL_INJECTION_METHOD_COM 1
#define DLL_INJECTION_METHOD_START_INJECTION 2
HRESULT EntryPoint(DWORD dwMethod)
{
    if (bInstanced)
    {
        return E_NOINTERFACE;
    }

    InitializeGlobalVersionAndUBR();

    TCHAR exePath[MAX_PATH], dllName[MAX_PATH];
    GetModuleFileNameW(hModule, dllName, MAX_PATH);
    PathStripPathW(dllName);
    BOOL bIsDllNameDXGI = !_wcsicmp(dllName, L"dxgi.dll");
    if (dwMethod == DLL_INJECTION_METHOD_DXGI && !bIsDllNameDXGI)
    {
        return E_NOINTERFACE;
    }

    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION,
        FALSE,
        GetCurrentProcessId()
    );
    if (!hProcess)
    {
        return E_NOINTERFACE;
    }
    DWORD dwLength = MAX_PATH;
    QueryFullProcessImageNameW(
        hProcess,
        0,
        exePath,
        &dwLength
    );
    CloseHandle(hProcess);

    TCHAR wszSearchIndexerPath[MAX_PATH];
    GetSystemDirectoryW(wszSearchIndexerPath, MAX_PATH);
    wcscat_s(wszSearchIndexerPath, MAX_PATH, L"\\SearchIndexer.exe");
    if (!_wcsicmp(exePath, wszSearchIndexerPath))
    {
        return E_NOINTERFACE;
    }

    TCHAR wszExplorerExpectedPath[MAX_PATH];
    GetWindowsDirectoryW(wszExplorerExpectedPath, MAX_PATH);
    wcscat_s(wszExplorerExpectedPath, MAX_PATH, L"\\explorer.exe");
    BOOL bIsThisExplorer = !_wcsicmp(exePath, wszExplorerExpectedPath);

    TCHAR wszStartExpectedPath[MAX_PATH];
    GetWindowsDirectoryW(wszStartExpectedPath, MAX_PATH);
    wcscat_s(wszStartExpectedPath, MAX_PATH, L"\\SystemApps\\Microsoft.Windows.StartMenuExperienceHost_cw5n1h2txyewy\\StartMenuExperienceHost.exe");
    BOOL bIsThisStartMEH = !_wcsicmp(exePath, wszStartExpectedPath);

    TCHAR wszShellExpectedPath[MAX_PATH];
    GetWindowsDirectoryW(wszShellExpectedPath, MAX_PATH);
    wcscat_s(wszShellExpectedPath, MAX_PATH, L"\\SystemApps\\ShellExperienceHost_cw5n1h2txyewy\\ShellExperienceHost.exe");
    BOOL bIsThisShellEH = !_wcsicmp(exePath, wszShellExpectedPath);

    if (dwMethod == DLL_INJECTION_METHOD_DXGI)
    {
        if (!(bIsThisExplorer || bIsThisStartMEH || bIsThisShellEH))
        {
            return E_NOINTERFACE;
        }
        TCHAR wszRealDXGIPath[MAX_PATH];
        GetSystemDirectoryW(wszRealDXGIPath, MAX_PATH);
        wcscat_s(wszRealDXGIPath, MAX_PATH, L"\\dxgi.dll");
#if WITH_MAIN_PATCHER
        SetupDXGIImportFunctions(LoadLibraryW(wszRealDXGIPath));
#endif
    }
    if (dwMethod == DLL_INJECTION_METHOD_COM && (bIsThisExplorer || bIsThisStartMEH || bIsThisShellEH))
    {
        return E_NOINTERFACE;
    }
    if (dwMethod == DLL_INJECTION_METHOD_START_INJECTION && !bIsThisStartMEH)
    {
        return E_NOINTERFACE;
    }

    bIsExplorerProcess = bIsThisExplorer;
    if (bIsThisExplorer)
    {
#if WITH_MAIN_PATCHER
        if (GetSystemMetrics(SM_CLEANBOOT) != 0 || IsUserOOBEOrCredentialReset())
        {
            IncrementDLLReferenceCount(hModule);
            bInstanced = TRUE;
            return E_NOINTERFACE;
        }
#endif
        BOOL desktopExists = IsDesktopWindowAlreadyPresent();
#if WITH_MAIN_PATCHER
        if (!desktopExists && CrashCounterHandleEntryPoint())
        {
            IncrementDLLReferenceCount(hModule);
            bInstanced = TRUE;
            return E_NOINTERFACE;
        }
#endif
        Inject(!desktopExists);
        IncrementDLLReferenceCount(hModule);
        bInstanced = TRUE;
    }
    else if (bIsThisStartMEH)
    {
        InjectStartMenu();
#if WITH_MAIN_PATCHER
        if (IsXamlSoundsEnabled())
        {
            HMODULE hWindowsUIXaml = LoadLibraryW(L"Windows.UI.Xaml.dll");
            ForceEnableXamlSounds(hWindowsUIXaml);
        }
#endif
        IncrementDLLReferenceCount(hModule);
        bInstanced = TRUE;
    }
    else if (bIsThisShellEH)
    {
        if (IsWindows11Version22H2OrHigher())
        {
            InjectShellExperienceHostFor22H2OrHigher();
        }
        else if (IsWindows11())
        {
            InjectShellExperienceHost();
        }
#if WITH_MAIN_PATCHER
        if (IsXamlSoundsEnabled())
        {
            HMODULE hWindowsUIXaml = LoadLibraryW(L"Windows.UI.Xaml.dll");
            ForceEnableXamlSounds(hWindowsUIXaml);
        }
#endif
        IncrementDLLReferenceCount(hModule);
        bInstanced = TRUE;
    }
    else if (dwMethod == DLL_INJECTION_METHOD_COM)
    {
        Inject(FALSE);
        IncrementDLLReferenceCount(hModule);
        bInstanced = TRUE;
    }

    return E_NOINTERFACE;
}

#if WITH_MAIN_PATCHER
// for explorer.exe and ShellExperienceHost.exe
__declspec(dllexport) HRESULT DXGIDeclareAdapterRemovalSupport()
{
    EntryPoint(DLL_INJECTION_METHOD_DXGI);
    return DXGIDeclareAdapterRemovalSupportFunc();
}
// for StartMenuExperienceHost.exe via DXGI
__declspec(dllexport) HRESULT CreateDXGIFactory1(void* p1, void** p2)
{
    EntryPoint(DLL_INJECTION_METHOD_DXGI);
    return CreateDXGIFactory1Func(p1, p2);
}
// for StartMenuExperienceHost.exe via injection from explorer
HRESULT InjectStartFromExplorer()
{
    EntryPoint(DLL_INJECTION_METHOD_START_INJECTION);
    return HRESULT_FROM_WIN32(GetLastError());
}
#endif

#ifdef _WIN64
#pragma comment(linker, "/export:DllGetClassObject=_DllGetClassObject")
#else
#pragma comment(linker, "/export:DllGetClassObject=__DllGetClassObject@12")
#endif
// for everything else
HRESULT WINAPI _DllGetClassObject(
    REFCLSID rclsid,
    REFIID   riid,
    LPVOID* ppv
)
{
    return EntryPoint(DLL_INJECTION_METHOD_COM);
}

BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD     fdwReason,
    _In_ LPVOID    lpvReserved
)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDLL);
        hModule = hinstDLL;
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#if WITH_MAIN_PATCHER
__declspec(dllexport) int ZZGUI(HWND hWnd, HINSTANCE hInstance, LPSTR lpszCmdLine, int nCmdShow)
{
    // Forward to ep_gui.dll
    LaunchPropertiesGUI(hModule);
    return 0;
}
#endif
