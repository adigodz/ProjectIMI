import argparse
import time
import brainflow
import numpy as np
from pyfirmata import Arduino, util
from time import sleep
import sys
import pandas as pd
import matplotlib.pyplot as plt
import csv 
from statistics import mean

from brainflow.board_shim import BoardShim, BrainFlowInputParams, LogLevels, BoardIds
from brainflow.data_filter import DataFilter, FilterTypes, AggOperations, WindowFunctions, DetrendOperations




import mne
from mne.channels import read_layout

import mne
from mne.channels import read_layout



    
def main():
    
    # use cyton board for demo
    params = BrainFlowInputParams()
    params.serial_port="COM7"
    
    board_id = BoardIds.GANGLION_BOARD
    
    board_descr = BoardShim.get_board_descr(board_id)
    sampling_rate = int(board_descr['sampling_rate'])
    board = BoardShim(board_id, params)
    
    board.prepare_session()
    
    board.config_board("/3")
    

    
    #Auditory P300
    board.start_stream()
    startTime1 = time.time()
    eeg_channels = BoardShim.get_eeg_channels(board_id)
    BoardShim.log_message(LogLevels.LEVEL_INFO.value, 'start record in the main thread')
    nfft = DataFilter.get_nearest_power_of_two(sampling_rate)
    
    infinit=0
    #Speech imagery
    print("start")
    while infinit==0 :
        t_beta=0
        while t_beta==0 :    

            time.sleep(10)
            nfft = DataFilter.get_nearest_power_of_two(sampling_rate)
            data = board.get_board_data()


            eeg_channels = board_descr['eeg_channels']
            print(eeg_channels)
            eeg_channel = eeg_channels[1]
            DataFilter.detrend(data[eeg_channel], DetrendOperations.LINEAR.value)
            psd = DataFilter.get_psd_welch(data[eeg_channel], nfft, nfft // 2, sampling_rate,3)


            band_power_alpha = DataFilter.get_band_power(psd, 7.0, 13.0)
            band_power_beta = DataFilter.get_band_power(psd, 14.0, 30.0)
            if band_power_beta > 0:
                current_ratio = band_power_alpha/band_power_beta
            else:
                current_ratio = 0
            
            # ตรรกะทดสอบสมาธิ:
            if current_ratio < 1.0: # ตัวเลข 1.0 เป็นตัวอย่างของ 'Low Ratio'
                print("🟢 สมาธิดีมาก! (Beta Activity สูง)")
                # เพิ่มคะแนนในเกม/ทำให้วัตถุเคลื่อนที่เร็วขึ้น
                infinit=1
                t_beta=1
            elif current_ratio < 2.0:
                print("🟡 สมาธิปานกลาง (กำลังผ่อนคลาย/โฟกัส)")
            else: # Ratio สูง
                print("🔴 สมาธิหลุด/ผ่อนคลายมาก (Alpha Activity สูง)")
                # ลดคะแนน/แสดงภาพ/เสียงเตือน
               
                
           

    board.stop_stream()
    board.release_session()
    print("stop record") 

if __name__ == "__main__":
    
    main()