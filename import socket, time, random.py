import socket, time, random

udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
while True:
    fake_data = [random.uniform(100, 200) for _ in range(4)]
    msg = ",".join([f"{v:.2f}" for v in fake_data])
    udp.sendto(msg.encode(), ("127.0.0.1", 5005))
    print(f"Sent: {msg}")
    time.sleep(0.1)