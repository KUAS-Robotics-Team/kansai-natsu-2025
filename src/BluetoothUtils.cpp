#include "BluetoothUtils.h"
#include "esp_bt_device.h"
#include "esp_err.h"
#include <Arduino.h>

void removePairedDevices() {
    uint8_t pairedDeviceBtAddr[20][6];
    int count = esp_bt_gap_get_bond_device_num();
    esp_bt_gap_get_bond_device_list(&count, pairedDeviceBtAddr);
    for (int i = 0; i < count; i++) {
        esp_bt_gap_remove_bond_device(pairedDeviceBtAddr[i]);
    }
}

void printDeviceAddress() {
    const uint8_t* point = esp_bt_dev_get_address();
    for (int i = 0; i < 6; i++) {
        char str[3];
        sprintf(str, "%02x", (int)point[i]);
        Serial.print(str);
        if (i < 5) Serial.print(":");
    }
}
