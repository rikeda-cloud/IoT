from flask import request, jsonify
from app import app, db
from app.models import IoTData, OrderData
from app.utils.fetch_open_ai import fetch_open_ai
import json


# INFO IoTデバイスのIDは仮の値を用いる
DEVICE_ID = 1


@app.route("/")
def index():
    # index.htmlを静的に提供
    return app.send_static_file("index.html")


@app.route("/api/iot-data", methods=["POST"])
def add_iot_data():
    """IoTデバイスから温度・湿度データを受け取りDBに保存"""
    data = request.get_json()
    temperature = data.get("temperature")
    humidity = data.get("humidity")

    if not all([temperature, humidity]):
        return jsonify({
            "error": "Missing 'temperature' or 'humidity' in the request"
        }), 400

    # 新しいデータを作成
    new_data = IoTData(device_id=DEVICE_ID, temperature=temperature, humidity=humidity)

    try:
        db.session.add(new_data)
        db.session.commit()
        return jsonify({"message": "Device data added successfully"}), 201
    except Exception as e:
        db.session.rollback()
        return jsonify({"error": str(e)}), 500


@app.route("/api/iot-data", methods=["GET"])
def list_iot_data():
    """指定したデバイスIDに関連するすべてのデータを取得"""
    try:
        # DEVICE_IDに関連するすべてのデータを取得
        data = IoTData.query.filter_by(device_id=DEVICE_ID).all()

        # データが存在しない場合
        if not data:
            return jsonify({"message": "No data found for the given device ID"}), 404

        # データをリストに変換
        result = [
            {
                "temperature": d.temperature,
                "humidity": d.humidity,
                "timestamp": d.timestamp,
            }
            for d in data
        ]

        return jsonify(result), 200

    except Exception as e:
        return jsonify({"error": str(e)}), 500


@app.route("/api/order/assigned", methods=["POST"])
def handle_order_assigned():
    """
    menuServiceから叩かれる
    商品名情報を含んだリクエストを処理
    """
    data = request.get_json()
    menus = data.get("menus")

    if not menus:
        return jsonify({"error": "Missing 'menus' in the request"}), 400

    try:
        prompt_system = "あなたは食品のデリバリーの専門家です。"
        prompt_user = "デリバリーバッグに最適な温度と湿度を教えてください。"
        prompt_user += f"メニューは、{menus}です。"
        prompt_user += 'メッセージの形式はJSON形式で{"temperature": 12.34, "humidity": 12.34}この形式を厳守してください！'
        prompt_user += "JSONのデータだけで良いので、他の文字は一切送らないでください"
        res_data = fetch_open_ai(prompt_system, prompt_user)
        res_json = res_data["choices"][0]["message"]["content"]
        json_data = json.loads(res_json)
        ideal_temperature = json_data["temperature"]
        ideal_humidity = json_data["humidity"]

        menus_string = ",".join(menus)
        new_data = OrderData(
            menus=menus_string,
            ideal_temperature=ideal_temperature,
            ideal_humidity=ideal_humidity,
        )
        db.session.add(new_data)
        db.session.commit()

        return jsonify(res_json), 201
    except Exception as e:
        return jsonify({"error": str(e)}), 500


@app.route("/api/order", methods=["GET"])
def fetch_order():
    """オーダーのデータを取得"""
    try:
        data = OrderData.query.order_by(OrderData.id.desc()).first()

        # データが存在しない場合
        if not data:
            return jsonify({"message": "注文が行われていません"}), 404

        iot_data = IoTData.query.filter_by(device_id=DEVICE_ID).all()

        # データが存在しない場合
        if not iot_data:
            return jsonify({"message": "No data found for the given device ID"}), 404

        # データをリストに変換
        iot_data_list = [
            {
                "temperature": d.temperature,
                "humidity": d.humidity,
                "timestamp": d.timestamp,
            }
            for d in iot_data
        ]

        prompt_system = "あなたは食品のデリバリーの専門家です。"
        prompt_user = f"{iot_data_list}"
        prompt_user += (
            "このような形でフードボックス内の中身の温度と湿度の現在のデータが送られます"
        )
        prompt_user += f"そして私はフード配達員で{data.menus}を運んでいます。"
        prompt_user += f"{data.menus}を理想的に食べる温度は{data.ideal_temperature}度でと湿度は{data.ideal_humidity}%です。"
        prompt_user += "私はフード配達員として何をすべきですか？"
        res_data = fetch_open_ai(prompt_system, prompt_user)
        res_json = res_data["choices"][0]["message"]["content"]
        # json_data = json.loads(res_json)

        result = {
            "menus": data.menus,
            "ideal_temperature": data.ideal_temperature,
            "ideal_humidity": data.ideal_humidity,
            "report": res_json,
        }
        return jsonify(result), 200

    except Exception as e:
        return jsonify({"error": str(e)}), 500
