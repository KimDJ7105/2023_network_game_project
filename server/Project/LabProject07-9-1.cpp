// LabProject07-9-1.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

//-------------------------------------------------------
// Worker Thread�� Recv Thread ���� ����,
// Worker Thread���� ���ڸ� �Ѱܹ޴� �κ� ���찡 Ȯ�� �ʿ�
// Recv Thread ���鶧 Handle �� arry�� ��������. ���� �� �� ������ �Ұ�
//-------------------------------------------------------

#include "stdafx.h"
#include "LabProject07-9-1.h"
#include "GameFramework.h"

#define MAX_LOADSTRING 100
#pragma comment(lib,"ws2_32")

HINSTANCE						ghAppInstance;
TCHAR							szTitle[MAX_LOADSTRING];
TCHAR							szWindowClass[MAX_LOADSTRING];

CGameFramework					gGameFramework;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

typedef struct threadarg
{
	HINSTANCE *hInstance;
	HINSTANCE *hPrevInstance;
	LPTSTR *lpCmdLine;
	int *nCmdShow;
} threadarg;

//----���� ����
std::array<SOCKET, 2> client_socket;
std::queue<EVENT> InputEventQueue;
extern CRITICAL_SECTION cs;

//sendthread�� workerthread�� ������ �����ϱ� ���� �̺�Ʈ �ڵ�
HANDLE hSendEvent;
HANDLE hWorkerEvent;

void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[%s] %s\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER
		| FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[%s] %s\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

DWORD WINAPI WorkerThread(LPVOID arg)
{
	struct threadarg *th_arg = (threadarg*)arg;

	HINSTANCE hInstance = *(th_arg->hInstance);
	HINSTANCE hPrevInstance = *(th_arg->hPrevInstance);
	LPTSTR lpCmdLine = *(th_arg->lpCmdLine);
	int nCmdShow = *(th_arg->nCmdShow);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	::LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	::LoadString(hInstance, IDC_LABPROJECT0791, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow)) return(FALSE);

	hAccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABPROJECT0791));

	int retval;

	while (1)
	{

		// 11.11 �߰� - �Ӱ迵�� ���� �ʿ�
		//HandleInputEvent(InputEventQueue);
		// �Ӱ迵�� Ż�� �ʿ�

		retval = WaitForSingleObject(hSendEvent, INFINITE); //������ ��Ⱑ ���� �����Ѱ�?
		if (retval == WAIT_OBJECT_0)
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) break;
			if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
		}
		else
		{
			gGameFramework.FrameAdvance();
			SetEvent(hWorkerEvent);
		}
	}
	gGameFramework.OnDestroy();

	return((int)msg.wParam);
}

DWORD WINAPI RecvThread(LPVOID arg) 
{
	int c_id = *((int*)arg);

	while (true) {
		char buf[BUFSIZE];

		int retval = recv(client_socket[c_id], buf, BUFSIZE, MSG_WAITALL);
		if (retval == 0) return 0;

		EnterCriticalSection(&cs);
		InputEventQueue.push({ (int)buf, c_id });
		LeaveCriticalSection(&cs);
	}
}

DWORD WINAPI SendThread(LPVOID arg)
{
	int c_id = *((int*)arg);
	int retval;

	while (true) {
		retval = WaitForSingleObject(hWorkerEvent, INFINITE);
		if (retval == WAIT_OBJECT_0) break;

		// �� ������ ���� ��� ����

		SetEvent(hSendEvent);
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	
	//����ȭ�� ���� �ʱ�ȭ��
	InitializeCriticalSection(&cs);
	hSendEvent = CreateEvent(NULL, false, false, NULL); //workerthread�� ó�� ���۽� block �Ǹ� �ȵ�. ���۽� ��ȣ ���·� �����ǰ� ���� �ʿ�
	hWorkerEvent = CreateEvent(NULL, false, false, NULL);

	//���� ����
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	//���ε�
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// ����
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	//accpet�� �ʿ��� ����
	int addrlen;
	struct sockaddr_in clientaddr;
	int numOfclient = 0;
	std::array<HANDLE, 2> handle_arry;

	while (numOfclient < 2) {
		//accept
		addrlen = sizeof(clientaddr);
		client_socket[numOfclient] = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (client_socket[numOfclient] == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// ������ Ŭ���̾�Ʈ ���� ���
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
			addr, ntohs(clientaddr.sin_port));

		//Ŭ���̾�Ʈ ���� RecvThread ����
		handle_arry[numOfclient] = CreateThread(NULL, 0, RecvThread, &numOfclient, 0, NULL);

		//Ŭ���̾�Ʈ ���� SendThread ����
		CreateThread(NULL, 0, SendThread, &numOfclient, 0, NULL);

		if (handle_arry[numOfclient] == NULL) {
			printf("RecvThread Fail");
			return numOfclient;
		}

		//������ Ŭ���̾�Ʈ �� ����
		++numOfclient;
	}

	struct threadarg arg;
	arg.hInstance = &hInstance;
	arg.hPrevInstance = &hPrevInstance;
	arg.lpCmdLine = &lpCmdLine;
	arg.nCmdShow = &nCmdShow;

	HANDLE WorkerHandle = CreateThread(NULL, 0, WorkerThread, &arg, 0, NULL);
	if (WorkerHandle == NULL) {
		printf("WorkerThread Fail");
		return 1;
	}

	WaitForSingleObject(WorkerHandle, INFINITE);
	WaitForSingleObject(handle_arry[0], INFINITE);
	WaitForSingleObject(handle_arry[1], INFINITE);

	DeleteCriticalSection(&cs);

	closesocket(listen_sock);
	WSACleanup();
	return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LABPROJECT0791));
	wcex.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;//MAKEINTRESOURCE(IDC_LABPROJECT0791);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = ::LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return ::RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	ghAppInstance = hInstance;

	RECT rc ={ 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_BORDER;
	AdjustWindowRect(&rc, dwStyle, FALSE);
	HWND hMainWnd = CreateWindow(szWindowClass, szTitle, dwStyle, 0, 0, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance, NULL);

	if (!hMainWnd) return(FALSE);

	gGameFramework.OnCreate(hInstance, hMainWnd);

	::ShowWindow(hMainWnd, nCmdShow);
	::UpdateWindow(hMainWnd);

	return(TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_SIZE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
	case WM_KEYUP:
		gGameFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			::DialogBox(ghAppInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			::DestroyWindow(hWnd);
			break;
		default:
			return(::DefWindowProc(hWnd, message, wParam, lParam));
		}
		break;
	case WM_PAINT:
		hdc = ::BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	default:
		return(::DefWindowProc(hWnd, message, wParam, lParam));
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return((INT_PTR)TRUE);
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			::EndDialog(hDlg, LOWORD(wParam));
			return((INT_PTR)TRUE);
		}
		break;
	}
	return((INT_PTR)FALSE);
}
