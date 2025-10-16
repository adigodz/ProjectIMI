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
import socket, random
from statistics import mean

from brainflow.board_shim import BoardShim, BrainFlowInputParams, LogLevels, BoardIds
from brainflow.data_filter import DataFilter, FilterTypes, AggOperations, WindowFunctions, DetrendOperations

import mne
from mne.channels import read_layout

import mne
def main():
    
    # use cyton board for demo
    params = BrainFlowInputParams()
    params.serial_port = "COM7" #อย่าลืมเปลี่ยน
    
    board_id = BoardIds.GANGLION_BOARD
    board = BoardShim(board_id, params)

    board.prepare_session()
    board.config_board("/3")
    board.start_stream()

    print("Recording 10 seconds")
    time.sleep(10)

    #Auditory P300
    data = board.get_board_data()
    board_descr = BoardShim.get_board_descr(board_id)
    eeg_channels = board_descr['eeg_channels']
    sampling_rate = int(board_descr['sampling_rate'])
    nfft = DataFilter.get_nearest_power_of_two(sampling_rate)

    print(f"\nEEG Channels: {eeg_channels}")
    print(f"sampling rate: {sampling_rate} Hz\n")

    ratios = []

    #pip install brainflow numpy pandas matplotlib mne pyfirmata <โหลด library>
    #4 channels
    for ch in eeg_channels:
        DataFilter.detrend(data[ch], DetrendOperations.LINEAR.value)

        psd = DataFilter.get_psd_welch(
            data[ch], nfft, nfft // 2, sampling_rate, 3
        )
        print(psd)
        #alpha (7–13 Hz) and beta (14–30 Hz) band power
        band_power_alpha = DataFilter.get_band_power(psd, 7.0, 13.0)
        band_power_beta = DataFilter.get_band_power(psd, 14.0, 30.0)

        ratio = band_power_alpha / band_power_beta if band_power_beta != 0 else np.nan
        ratios.append(ratio)

        print(f"Channel {ch}: alpha/beta ratio = {ratio:.4f}")

    #average ratios
    valid_ratios = [r for r in ratios if not np.isnan(r)]
    if valid_ratios:
        avg_ratio = np.mean(valid_ratios)
        print(f"\nAverage alpha/beta ratio across all channels: {avg_ratio:.4f}")
    else:
        print("\nNo valid ratios could be computed.")
 

    board.stop_stream()
    board.release_session()
    print("\nStop record")


if __name__ == "__main__":
    main()
