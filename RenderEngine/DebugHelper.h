#pragma once

#include<Windows.h>
#include<iostream>
#include<tchar.h>



#ifndef __DEBUG_HELPER__
#define __DEBUG_HELPER__

namespace DesktopCPPDebugger {

	// �����������̨�������ڵ������Ŵ�
	void UseCosoleBoard() {
		//----begin console output
		//----create console
		AllocConsole();   // ��������̨
		SetConsoleTitle(_T("Debug Output"));      // ���ÿ���̨���ڱ���
		// �ض��� STDIN
		{
			FILE*fp;
			errno_t eno = freopen_s(&fp, "CONIN$", "r+t", stdin);
			if (eno != 0) { // ���ɹ�
				MessageBox(NULL, _T("�ض���stdinʧ��!"), _T("error"), MB_OK | MB_ICONEXCLAMATION);
				exit(0);
			}
		}
		// �ض���STDOUT
		{
			FILE*fp;
			errno_t eno = freopen_s(&fp, "CONOUT$", "w+t", stdout);
			if (eno != 0) {//���ɹ�
				MessageBox(NULL, _T("�ض���stdoutʧ��!"), _T("error"), MB_OK | MB_ICONEXCLAMATION);
				exit(0);
			}

		}
		//----end console output
	}
}

#endif // !__DEBUG_HELPER__
