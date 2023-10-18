import socket

# 서버 정보
server_ip = '127.0.0.1'
server_port = 8080

# 소켓 생성
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# 서버에 연결
client_socket.connect((server_ip, server_port))


# 전송할 데이터
message = "Hello from Python Client"

# 데이터 전송
client_socket.send(message.encode())  # 문자열을 바이트로 인코딩하여 전송

# 데이터 받기
data = client_socket.recv(256)
print("서버로부터 받은 데이터:", data.decode()) #받은 바이트 문자열을 decode

while 1 :
    print("메시지를 입력하세요, q를 입력하면 종료합니다. ")
    
    message = input("메시지 : ")
    
    if message == 'q' :
        break;
    
    client_socket.send(message.encode())
    
    data = client_socket.recv(256)
    print("서버로부터 받은 데이터:", data.decode())


# 소켓 닫기
client_socket.close()