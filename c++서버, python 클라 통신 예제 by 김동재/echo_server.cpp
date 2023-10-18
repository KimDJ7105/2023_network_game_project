#include <iostream>
#include <cstring>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32")

#define BUFSIZE 256

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // ���� ���� ����
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "���� ���� ����" << std::endl;
        return 1;
    }

    // ���� �ּ� ����
    int retval;
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(8080);
    retval = bind(serverSocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        std::cerr << "���� ���ε� ����" << std::endl;
    }

    // ������
 
    retval = listen(serverSocket, SOMAXCONN);
    if (retval == SOCKET_ERROR) {
        std::cerr << "���� ������ ����" << std::endl;
    }

    std::cout << "���� ��� ��..." << std::endl;

    SOCKET client_sock;
    struct sockaddr_in clientaddr;
    int addrlen;
    char buf[BUFSIZE + 1];

    while (true) {
        // Ŭ���̾�Ʈ ���� �ޱ�
       // accept()
        addrlen = sizeof(clientaddr);
        client_sock = accept(serverSocket, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_sock == INVALID_SOCKET) {
            std::cerr << "Ŭ���̾�Ʈ ���� ���Ʈ ����" << std::endl;
            break;
        }

        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
            addr, ntohs(clientaddr.sin_port));

        // Ŭ���̾�Ʈ�� ������ ���
        while (1) {
            // ������ �ޱ�
            retval = recv(client_sock, buf, BUFSIZE, 0);
            if (retval == SOCKET_ERROR) {
                std::cout << "������ ���� ����" << std::endl;
                break;
            }
            else if (retval == 0)
                break;

            // ���� ������ ���
            buf[retval] = '\0';
            printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), buf);

            // ������ ������
            retval = send(client_sock, buf, retval, 0);
            if (retval == SOCKET_ERROR) {
                std::cout << "������ �۽� ����" << std::endl;
                break;
            }
        }


        // Ŭ���̾�Ʈ ���� �ݱ�
        closesocket(client_sock);

        printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
            addr, ntohs(clientaddr.sin_port));
    }

    // ���� ���� �ݱ� (������� ���� ����)
    closesocket(serverSocket);

    return 0;
}