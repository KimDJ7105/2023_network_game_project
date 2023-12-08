// LabProject07-9-1.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "LabProject07-9-1.h"
#include "GameFramework.h"
#include "CSyncObjectManager.h"
#include "WindowDialog.h"

#pragma comment(lib,"ws2_32")
#define MAX_LOADSTRING 100

#define SERVERIP "127.0.0.1"

HINSTANCE						ghAppInstance;
TCHAR							szTitle[MAX_LOADSTRING];
TCHAR							szWindowClass[MAX_LOADSTRING];

CGameFramework					gGameFramework;

HANDLE hRecvHandle;
HANDLE hWoker;

HINSTANCE hInst; // �ν��Ͻ� �ڵ�
HWND hEdit; // ����Ʈ ��Ʈ��

DWORD WINAPI RecvThread(LPVOID arg);
void RecvInitObject();
void KeyControl();
void MouseControl();

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

void err_quit(const char* msg);
void err_display(const char* msg);

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	int retval;
	// ���� �ʱ�ȭ
	hInst = hInstance;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	hRecvHandle = CreateEvent(NULL, true, true, NULL);
	hWoker = CreateEvent(NULL, true, false, NULL);

	// ���� ����
	Define::sock = socket(AF_INET, SOCK_STREAM, 0);
	if (Define::sock == INVALID_SOCKET) err_quit("socket()");

	// connect()
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_pton(AF_INET, SERVERIP, &serveraddr.sin_addr);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(Define::sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("connect()");

	// ���° Ŭ������ ���Źޱ�
	recv(Define::sock, (char*)&Define::ClientIndex, sizeof(int), 0);

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	::LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	::LoadString(hInstance, IDC_LABPROJECT0791, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	RecvInitObject();
	if (!InitInstance(hInstance, nCmdShow)) return(FALSE);

	hAccelTable = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LABPROJECT0791));

	CreateThread(NULL, 0, RecvThread, NULL, 0, NULL);
	bool end = false;
	while (Define::GameRunnig)
	{
		
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
			//WaitForSingleObject(hWoker, INFINITE);
			KeyControl();
			MouseControl();
			gGameFramework.FrameAdvance(); 

			if (!end)
			{
				if (Define::Players[0]->GetRoot()->GetActive() == false)
				{
					DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
					if (Define::ClientIndex == 0)
						SetWindowTextW(handleDictionary["Text"], L"YOU LOSE");
					else
						SetWindowTextW(handleDictionary["Text"], L"YOU WIN");
					end = true;
				}
				else if (Define::Players[1]->GetRoot()->GetActive() == false)
				{
					DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
					if (Define::ClientIndex == 1)
						SetWindowTextW(handleDictionary["Text"], L"YOU LOSE");
					else
						SetWindowTextW(handleDictionary["Text"], L"YOU WIN");
					end = true;
				}
			}

			//ResetEvent(hWoker);
			//SetEvent(hRecvHandle);
		}
	}
	gGameFramework.OnDestroy();

	return((int)msg.wParam);
}

DWORD WINAPI RecvThread(LPVOID arg)
{
	int reval;
	vector<sc_object_transform_packet> packList;
	EVENT flag;
	flag.client_id = Define::ClientIndex;
	flag.event_id = RECV_DONE;
	
	while (true) {
		//WaitForSingleObject(hRecvHandle, INFINITE);
		{
			int transformPackSize = 0;
			reval = recv(Define::sock, (char*)&transformPackSize, sizeof(int), 0);
			if (reval == SOCKET_ERROR) continue;
			for (int i = 0; i < transformPackSize; i++)
			{
				sc_object_transform_packet pack;
				reval = recv(Define::sock, (char*)&pack, sizeof(sc_object_transform_packet), 0);
				if (reval == SOCKET_ERROR) continue;

				packList.emplace_back(pack);
			}

			send(Define::sock, (char*)&flag, sizeof(EVENT), 0);

			Define::SyncObjectManager->SetTransformPack(packList);
			packList.clear();
		}
		//ResetEvent(hRecvHandle);
		//SetEvent(hWoker);
	}
}

void RecvInitObject()
{
	int createPackSize = 0;
	recv(Define::sock, (char*)&createPackSize, sizeof(int), 0);

	for (int i = 0; i < createPackSize; i++)
	{
		sc_create_object_packet pack;
		recv(Define::sock, (char*)&pack, sizeof(sc_create_object_packet), 0);
		Define::SyncObjectManager->AddCreatePack(pack);
	}

	//NetworkConverter nc;
	//nc.Recv(Define::sock);
	//
	//auto* packList = nc.ReadBuffer<sc_create_object_packet>();
	//Define::SyncObjectManager->AddRangeCreatePack(packList);
}

void KeyControl()
{
	//if (Define::Input->GetKeyAny()) return;
	//�� �ڵ尡 ������ ���� �۵���.

	auto recvKey = [](int key, int key_state) {
		EVENT e{ Define::ClientIndex, -1, -1, {-1,-1} };

		e.event_id = key;
		e.state = key_state;
		send(Define::sock, (char*)&e, sizeof(EVENT), 0);
	};

	if (Define::Input->IsKeyDown())
	{
		if (Define::Input->GetKeyDown(KeyCode::W))
			recvKey(KEY_W, KEY_DOWN);
		if (Define::Input->GetKeyDown(KeyCode::S))
			recvKey(KEY_S, KEY_DOWN);
		if (Define::Input->GetKeyDown(KeyCode::D))
			recvKey(KEY_D, KEY_DOWN);
		if (Define::Input->GetKeyDown(KeyCode::A))
			recvKey(KEY_A, KEY_DOWN);
		if (Define::Input->GetKeyDown(KeyCode::Space))
			recvKey(KEY_SPACE, KEY_DOWN);
	}
	if (Define::Input->IsKeyUp())
	{
		if (Define::Input->GetKeyUp(KeyCode::W))
			recvKey(KEY_W, KEY_UP);
		if (Define::Input->GetKeyUp(KeyCode::S))
			recvKey(KEY_S, KEY_UP);
		if (Define::Input->GetKeyUp(KeyCode::D))
			recvKey(KEY_D, KEY_UP);
		if (Define::Input->GetKeyUp(KeyCode::A))
			recvKey(KEY_A, KEY_UP);
		if (Define::Input->GetKeyUp(KeyCode::Space))
			recvKey(KEY_SPACE, KEY_UP);
	}
}

void MouseControl()
{
	if (Define::Input->GetMousePress(MouseButton::Left))
	{
		POINT mouseAxis = Define::Input->GetMouseAxis();
		mouseAxis.x *= 10;
		if (mouseAxis.x != 0)
		{
			EVENT e{ Define::ClientIndex, MOUSE_LEFT, MOUSE_DOWN, mouseAxis };
			send(Define::sock, (char*)&e, sizeof(EVENT), 0);
		}
	}
	else if (Define::Input->GetMouseUp(MouseButton::Left))
	{
		EVENT e{ Define::ClientIndex, MOUSE_LEFT, MOUSE_UP, {0,0} };
		send(Define::sock, (char*)&e, sizeof(EVENT), 0);
	}

	if (Define::Input->GetMousePress(MouseButton::Right))
	{
		POINT mouseAxis = Define::Input->GetMouseAxis();
		if (mouseAxis.y != 0)
		{
			EVENT e{ Define::ClientIndex, MOUSE_RIGHT, MOUSE_DOWN, mouseAxis };
			send(Define::sock, (char*)&e, sizeof(EVENT), 0);
		}
	}
	else if (Define::Input->GetMouseUp(MouseButton::Right))
	{
		EVENT e{ Define::ClientIndex, MOUSE_RIGHT, MOUSE_UP, {0,0} };
		send(Define::sock, (char*)&e, sizeof(EVENT), 0);
	}
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
