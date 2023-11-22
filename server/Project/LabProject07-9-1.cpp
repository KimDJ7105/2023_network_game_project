#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

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

//----전역변수
std::queue<EVENT> InputEventQueue;
extern CRITICAL_SECTION cs;

//sendthread와 workerthread 동기화를 위한 이벤트 핸들
HANDLE hWorkerEvent;
HANDLE hSendEvent;

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
	//struct threadarg *th_arg = (threadarg*)arg;

	//HINSTANCE hInstance = *(th_arg->hInstance);
	//HINSTANCE hPrevInstance = *(th_arg->hPrevInstance);
	//LPTSTR lpCmdLine = *(th_arg->lpCmdLine);
	//int nCmdShow = *(th_arg->nCmdShow);

	//UNREFERENCED_PARAMETER(hPrevInstance);
	//UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	//HACCEL hAccelTable;

	//::LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	//::LoadString(hInstance, IDC_LABPROJECT0791, szWindowClass, MAX_LOADSTRING);
	//MyRegisterClass(hInstance);

	//if (!InitInstance(hInstance, nCmdShow)) return(FALSE);

	//hAccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABPROJECT0791));

	int retval;

	gGameFramework.BuildObjects();
	while (1)
	{

		//if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		//{
		//	if (msg.message == WM_QUIT) break;
		//	if (!::TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		//	{
		//		::TranslateMessage(&msg);
		//		::DispatchMessage(&msg);
		//	}
		//}
		//else
		//{
		//	
		//}

		gGameFramework.FrameAdvance();
		retval = WaitForSingleObject(hSendEvent, INFINITE); //무한 대기가 올바를까?
		if (retval == WAIT_OBJECT_0) continue;
	}
	gGameFramework.OnDestroy();

	return((int)msg.wParam);
}

DWORD WINAPI RecvThread(LPVOID arg) 
{
	int c_id = *((int*)arg);

	while (true) {
		char buf[BUFSIZE];

		int retval = recv(Define::sock[c_id], buf, BUFSIZE, MSG_WAITALL);
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

	auto objmgr = Define::SceneManager->GetCurrentScene()->objectManager;

	while (true) {
		retval = WaitForSingleObject(hWorkerEvent, INFINITE);
		if (retval == WAIT_OBJECT_0) break;

		{
			auto createPack = objmgr->GetCreatePack();
			int createPackSize = createPack.size();
			send(Define::sock[c_id], (char*)createPackSize, sizeof(int), 0);
			for (auto pack : createPack)
				send(Define::sock[c_id], (char*)&pack, sizeof(sc_create_object_packet), 0);
			createPack.clear();
		}

		{
			auto deletePack = objmgr->GetDeletePack();
			int deletePackSize = deletePack.size();
			send(Define::sock[c_id], (char*)deletePackSize, sizeof(int), 0);
			for (auto pack : deletePack)
				send(Define::sock[c_id], (char*)&pack, sizeof(sc_delete_object_packet), 0);
			deletePack.clear();
		}

		{
			auto packList = objmgr->AllTrnasformToPacket();
			int objectSize = packList.size();
			send(Define::sock[c_id], (char*)objectSize, sizeof(int), 0);
			for (auto pack : packList)
				send(Define::sock[c_id], (char*)&pack, sizeof(sc_object_transform_packet), 0);
		}

		SetEvent(hSendEvent);
	}
	return 0;
}

int main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return 1;
	
	//동기화 객체 생성
	InitializeCriticalSection(&cs);
	hSendEvent = CreateEvent(NULL, false, false, NULL); //workerthread가 처음 실행 될 수 있도록 처음엔 신호 상태로 생성해야함
	hWorkerEvent = CreateEvent(NULL, false, true, NULL);

	//소켓 생성
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");

	//bind
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");

	// listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");

	//accpet에 필요한 변수들
	int addrlen;
	struct sockaddr_in clientaddr;
	int numOfclient = 0;
	std::array<HANDLE, 2> handle_arry;

	while (numOfclient < 2) {
		//accept
		addrlen = sizeof(clientaddr);
		Define::sock[numOfclient] = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (Define::sock[numOfclient] == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP] client addr: IP addr=%s, port num=%d\n",
			addr, ntohs(clientaddr.sin_port));

		//클라이언트 전용 RecvThread 생성
		handle_arry[numOfclient] = CreateThread(NULL, 0, RecvThread, &numOfclient, 0, NULL);
		if (handle_arry[numOfclient] == NULL) {
			printf("RecvThread Fail");
			return numOfclient;
		}

		//클라이언트 전용 SendThread 생성
		CreateThread(NULL, 0, SendThread, &numOfclient, 0, NULL);

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
