import socket

# 서버 정보
server_ip = '127.0.0.1'
server_port = 8080

# 소켓 생성
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 서버에 연결
client_socket.connect((server_ip, server_port))


# 전송할 데이터
print("Hello, Successfully connected to server")

while 1 :
    print("정수 2개를 입력하세요, q를 입력하면 종료합니다. ")
    
    int_data1 = input("정수1 : ")
    
    if int_data1 == 'q' :
        break;
    
    int_data2 = input("정수2 : ")
    
    int_data1 = int(int_data1)
    int_data2 = int(int_data2)
    
    data_to_send = int_data1.to_bytes(4, byteorder='little') + int_data2.to_bytes(4, byteorder='little') #두 정수를 little endian으로 정렬해서 합치기
    client_socket.send(data_to_send)
    
    byte_data = client_socket.recv(4)
    data = int.from_bytes(byte_data,byteorder='little') #받은 바이트 형태의 데이터를 int 형태로 변환
    print("서버로부터 받은 데이터:", data)


# 소켓 닫기
client_socket.close()