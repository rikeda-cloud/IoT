import serial

# シリアルポートを開く（例：/dev/ttyUSB0）
ser = serial.Serial("/dev/ttyUSB0", 115200)

while True:
    # シリアルポートからデータを受信
    data = ser.readline().decode("utf-8").strip()
    print(f"Received: {data}")
