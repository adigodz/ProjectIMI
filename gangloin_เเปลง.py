import time
import socket
import numpy as np
from brainflow.board_shim import BoardShim, BrainFlowInputParams, BoardIds

# ------------------------------
# CONFIG
# ------------------------------
UDP_IP = "127.0.0.1"     # หาก LabVIEW อยู่เครื่องเดียวกัน
UDP_PORT = 5005          # พอร์ตที่ LabVIEW จะเปิดรับ
DURATION = 600           # ระยะเวลาบันทึก (วินาที)
# ------------------------------

# Ganglion scale factor (แปลง raw counts เป็น µV)
# ตาม Datasheet: 24-bit ADC, 24-bit range ±24mV → scale ~0.02235 µV/count
SCALE_FACTOR = 0.02235

def main():
    # ตั้งค่าบอร์ด
    BoardShim.enable_dev_board_logger()
    params = BrainFlowInputParams()
    params.serial_port = "COM7"  # เปลี่ยนตามพอร์ตของคุณ

    board_id = BoardIds.GANGLION_BOARD
    board = BoardShim(board_id, params)

    # เตรียม session
    board.prepare_session()
    board.start_stream()

    print(f"Start streaming data from Ganglion for {DURATION} seconds...")

    # สร้าง UDP socket
    udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # กำหนด index ของ channels ที่ต้องการส่ง (Ganglion board: CH1-CH4)
    # สมมุติ mapping: CH1=Fz, CH2=Cz, CH3=O1
    selected_channels = [0, 1, 2, 3]  

    start_time = time.time()
    while (time.time() - start_time) < DURATION:
        data = board.get_board_data()

        if data.shape[1] > 0:
            # ดึงค่าล่าสุดของแต่ละ channel ที่เลือก + แปลงเป็น µV
            eeg_values_uv = [data[ch][-1] * SCALE_FACTOR for ch in selected_channels]

            # แปลงเป็น string ส่งออก เช่น "3.8,2.5,1.2"
            message = ",".join([f"{v:.3f}" for v in eeg_values_uv])
            udp.sendto(message.encode(), (UDP_IP, UDP_PORT))

            print(f"Sent (µV): {message}")

        time.sleep(0.05)  # ประมาณ 20Hz

    # หยุดการทำงาน
    board.stop_stream()
    board.release_session()
    udp.close()

    print("\nStop streaming and close connection.")


if __name__ == "__main__":
    main()
