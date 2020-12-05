#pragma once

#include<Windows.h>
#include<iostream>
#include<tchar.h>



#ifndef __DEBUG_HELPER__
#define __DEBUG_HELPER__

namespace DesktopCPPDebugger {

	// 建立输出控制台，有助于调试与排错
	void UseCosoleBoard() {
		//----begin console output
		//----create console
		AllocConsole();   // 建立控制台
		SetConsoleTitle(_T("Debug Output"));      // 设置控制台窗口标题
		// 重定向 STDIN
		{
			FILE*fp;
			errno_t eno = freopen_s(&fp, "CONIN$", "r+t", stdin);
			if (eno != 0) { // 不成功
				MessageBox(NULL, _T("重定向stdin失败!"), _T("error"), MB_OK | MB_ICONEXCLAMATION);
				exit(0);
			}
		}
		// 重定向STDOUT
		{
			FILE*fp;
			errno_t eno = freopen_s(&fp, "CONOUT$", "w+t", stdout);
			if (eno != 0) {//不成功
				MessageBox(NULL, _T("重定向stdout失败!"), _T("error"), MB_OK | MB_ICONEXCLAMATION);
				exit(0);
			}

		}
		//----end console output
	}
}

#endif // !__DEBUG_HELPER__
