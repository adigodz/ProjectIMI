import time
import socket
import numpy as np
from brainflow.board_shim import BoardShim, BrainFlowInputParams, BoardIds

# ------------------------------
# CONFIG
# ------------------------------
UDP_IP = "127.0.0.1"     # หาก LabVIEW อยู่เครื่องเดียวกัน
UDP_PORT = 5005          # พอร์ตที่ LabVIEW จะเปิดรับ
DURATION = 60            # ระยะเวลาบันทึก (วินาที)
# ------------------------------

def main():
    # ตั้งค่าบอร์ด
    BoardShim.enable_dev_board_logger()
    params = BrainFlowInputParams()
    params.serial_port = "COM5"  # เปลี่ยนตามพอร์ตของคุณ

    board_id = BoardIds.GANGLION_BOARD
    board = BoardShim(board_id, params)

    # เตรียม session
    board.prepare_session()
    board.start_stream()

    print(f"Start streaming data from Ganglion for {DURATION} seconds...")

    # สร้าง UDP socket
    udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    start_time = time.time()
    while (time.time() - start_time) < DURATION:
        # ดึงข้อมูลใหม่ที่ยังไม่ได้อ่าน
        data = board.get_board_data()

        if data.shape[1] > 0:
            eeg_channels = BoardShim.get_eeg_channels(board_id)
            eeg_values = [data[ch][-1] for ch in eeg_channels]  # ค่าล่าสุดของแต่ละ channel

            # แปลงเป็น string ส่งออก เช่น "120.5,118.9,123.4,121.7"
            message = ",".join([f"{v:.3f}" for v in eeg_values])
            udp.sendto(message.encode(), (UDP_IP, UDP_PORT))

            print(f"Sent: {message}")
        time.sleep(0.05)  # ประมาณ 20Hz (ส่งทุก 50 มิลลิวินาที)

    # หยุดการทำงาน
    board.stop_stream()
    board.release_session()
    udp.close()

    print("\nStop streaming and close connection.")


if __name__ == "__main__":
    main()
