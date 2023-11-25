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
SOCKET sock[2];

//sendthread와 workerthread 동기화를 위한 이벤트 핸들
HANDLE hWorkerEvent;
HANDLE hSendEvent[2];
HANDLE hRecvReadyEvent;

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
	MSG msg;
	int retval;

	auto objmgr = Define::SceneManager->GetCurrentScene()->objectManager;

	gGameFramework.BuildObjects();
	while (1)
	{
		gGameFramework.FrameAdvance();
		WaitForMultipleObjects(2, hSendEvent,true, INFINITE);

		objmgr->GetDeletePack().clear();
		objmgr->GetCreatePack().clear();
	}
	gGameFramework.OnDestroy();

	return((int)msg.wParam);
}

DWORD WINAPI RecvThread(LPVOID arg) 
{
	int c_id = *((int*)arg);

	WaitForSingleObject(hRecvReadyEvent, INFINITE);

	while (true) {
		cs_player_input_packet pack;
		int retval = recv(sock[c_id], (char*)&pack, sizeof(cs_player_input_packet), MSG_WAITALL);
		if (retval == 0) return 0;
		if (retval == SOCKET_ERROR) err_quit("recv()");
		else if (retval != 0) {
			printf("id - %d : We got some msg of %d - %d / size is : %d\n", c_id, pack.packet_type, pack.input_event, retval);
		}

		/*struct sockaddr_in myaddr;
		int client_len = sizeof(myaddr);
		getpeername(Define::sock[c_id], (struct sockaddr*)&myaddr, &client_len);

		printf("Port    : %d\n", ntohs(myaddr.sin_port));
		printf("address : %s\n", inet_ntoa(myaddr.sin_addr));*/

		EnterCriticalSection(&cs);
		InputEventQueue.push({ pack.input_event, c_id });
		LeaveCriticalSection(&cs);
	}
}

DWORD WINAPI SendThread(LPVOID arg)
{
	int c_id = *((int*)arg);
	int retval;

	auto objmgr = Define::SceneManager->GetCurrentScene()->objectManager;

	while (true) {
		WaitForSingleObject(hWorkerEvent, INFINITE);

		{
			auto createPack = objmgr->GetCreatePack();
			int createPackSize = createPack.size();
			//printf("(createpacket)%d socket : %d EA\n", c_id, createPackSize);
			retval = send(sock[c_id], (char*)createPackSize, sizeof(int), 0);
			if (retval == SOCKET_ERROR) err_quit("send()1 - 1");

			for (auto pack : createPack) {
				retval = send(sock[c_id], (char*)&pack, sizeof(sc_create_object_packet), 0);
				if (retval == SOCKET_ERROR) err_quit("send()1 - 2");
			}
		}

		{
			auto deletePack = objmgr->GetDeletePack();
			int deletePackSize = deletePack.size();
			//printf("(deletepacket)%d socket : %d EA\n", c_id, deletePackSize);
			retval = send(sock[c_id], (char*)deletePackSize, sizeof(int), 0);
			if(retval == SOCKET_ERROR) err_quit("send()2 - 1");
			for (auto pack : deletePack) {
				retval = send(sock[c_id], (char*)&pack, sizeof(sc_delete_object_packet), 0);
				if (retval == SOCKET_ERROR) err_quit("send()2 - 2");
			}
		}

		{
			auto packList = objmgr->AllTrnasformToPacket();
			int objectSize = packList.size();
			//printf("(transformpacket)%d socket : %d EA\n", c_id, objectSize);
			retval = send(sock[c_id], (char*)objectSize, sizeof(int), 0);
			if (retval == SOCKET_ERROR) err_quit("send()3 - 1");
			for (auto pack : packList) {
				retval = send(sock[c_id], (char*)&pack, sizeof(sc_object_transform_packet), 0);
				if (retval == SOCKET_ERROR) err_quit("send()3 - 2");
			}
		}

		SetEvent(hSendEvent[c_id]);
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
	hSendEvent[1] = CreateEvent(NULL, false, true, NULL); //workerthread가 처음 실행 될 수 있도록 처음엔 신호 상태로 생성해야함
	hSendEvent[0] = CreateEvent(NULL, false, true, NULL); //workerthread가 처음 실행 될 수 있도록 처음엔 신호 상태로 생성해야함
	hWorkerEvent = CreateEvent(NULL, true, false, NULL);
	hRecvReadyEvent = CreateEvent(NULL, true, false, NULL);

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
	std::array<HANDLE, 2> handle_arry;

	for (int i = 0; i < 2; i++)
	{
		//accept
		addrlen = sizeof(clientaddr);
		sock[i] = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
		if (sock[i] == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
		printf("\n[TCP] client addr: IP addr=%s, port num=%d\n",
			addr, ntohs(clientaddr.sin_port));

		//클라이언트 전용 RecvThread 생성
		int player_id_index = i;
		handle_arry[i] = CreateThread(NULL, 0, RecvThread, &player_id_index, 0, NULL);
		if (handle_arry[i] == NULL) {
			printf("RecvThread Fail");
			return 0;
		}

		//클라이언트 전용 SendThread 생성
		CreateThread(NULL, 0, SendThread, &player_id_index, 0, NULL);
	}

	SetEvent(hRecvReadyEvent);


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
