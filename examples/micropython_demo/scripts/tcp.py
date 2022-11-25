import network
import umachine as machine
import usocket as socket
import utime as time    

WIFI_SSID='KYzhang'
WIFI_PASSWORD='kyzhangx'
SERVER_HOST='117.50.111.72'
SERVER_PORT=8001

if __name__ == '__main__':

    # register esp8266 as NIC
    uart0 = machine.UART(0)             # BearPi LPUART1
    esp8266 = network.ESP8266(uart0)
    print("esp8266 init done")
    esp8266.connect(ssid=WIFI_SSID, pwd=WIFI_PASSWORD)
    print("esp8266 connect done")

    # get socket and establish tcp connection
    tcp_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print("socket init done")
    tcp_socket.connect((SERVER_HOST, SERVER_PORT))
    print("socket connect done")

    # tcp send and receive
    cnt = 0
    while cnt < 5:
        send_len = tcp_socket.send("This is TCP test!\n")
        print(f"send len:{send_len}")
        data = tcp_socket.recv(1024)
        if not data:
            break
        print(f"recv data:{data}")
        cnt += 1
        time.sleep(0.5)
