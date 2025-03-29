import serial
import requests

# シリアルポートを開く（例：/dev/ttyUSB0）
ser = serial.Serial("/dev/ttyUSB0", 115200)

url = "http://127.0.0.1:8080/api/iot-data"

while True:
    # シリアルポートからデータを受信
    data = ser.readline().decode("utf-8").strip()
    requests.post(url, data=data)
    response = requests.post(
        url, data=data, headers={"Content-Type": "application/json"}
    )
    print(data)
