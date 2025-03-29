from app import db
from sqlalchemy.sql import func


# デバイスのデータテーブルの定義
class IoTData(db.Model):
    __tablename__ = "iot_data"

    id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    device_id = db.Column(db.Integer, nullable=False)
    temperature = db.Column(db.Float, nullable=False)
    humidity = db.Column(db.Float, nullable=False)
    timestamp = db.Column(db.DateTime, default=func.now(), nullable=False)

    def __repr__(self):
        return f"<DeviceData device_id={self.device_id} temperature={self.temperature} humidity={self.humidity} timestamp={self.timestamp}>"


class OrderData(db.Model):
    __tablename__ = "order_data"

    id = db.Column(db.Integer, primary_key=True, autoincrement=True)
    menus = db.Column(db.String(1024))
    ideal_temperature = db.Column(db.Float, nullable=False)
    ideal_humidity = db.Column(db.Float, nullable=False)
    timestamp = db.Column(db.DateTime, default=func.now(), nullable=False)
