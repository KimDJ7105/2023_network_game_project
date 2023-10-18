#include <iostream>
#include <cstring>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32")

#define BUFSIZE 256

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // 서버 소켓 생성
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "소켓 생성 실패" << std::endl;
        return 1;
    }

    // 서버 주소 설정
    int retval;
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(8080);
    retval = bind(serverSocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        std::cerr << "소켓 바인드 실패" << std::endl;
    }

    // 리스닝
 
    retval = listen(serverSocket, SOMAXCONN);
    if (retval == SOCKET_ERROR) {
        std::cerr << "소켓 리스닝 실패" << std::endl;
    }

    std::cout << "서버 대기 중..." << std::endl;

    SOCKET client_sock;
    struct sockaddr_in clientaddr;
    int addrlen;
    char buf[BUFSIZE + 1];

    while (true) {
        // 클라이언트 연결 받기
       // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(serverSocket, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            std::cerr << "클라이언트 소켓 억셉트 실패" << std::endl;
            break;
        }

        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
            addr, ntohs(clientaddr.sin_port));

        // 클라이언트와 데이터 통신
        while (1) {
            // 데이터 받기
            retval = recv(client_sock, buf, BUFSIZE, 0);
            if (retval == SOCKET_ERROR) {
                std::cout << "데이터 수신 에러" << std::endl;
                break;
            }
            else if (retval == 0)
                break;

            // 받은 데이터 출력
            buf[retval] = '\0';
            printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

            // 데이터 보내기
            retval = send(client_sock, buf, retval, 0);
            if (retval == SOCKET_ERROR) {
                std::cout << "데이터 송신 에러" << std::endl;
                break;
            }
        }


        // 클라이언트 소켓 닫기
        closesocket(client_sock);

        printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
            addr, ntohs(clientaddr.sin_port));
    }

    // 서버 소켓 닫기 (여기까지 오지 않음)
    closesocket(serverSocket);

    return 0;
}