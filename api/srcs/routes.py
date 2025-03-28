from flask import request, jsonify
from app import app, db
from app.models import IoTData
from app.utils.fetch_open_ai import fetch_open_ai


# INFO IoTデバイスのIDは仮の値を用いる
DEVICE_ID = 1


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
        prompt_user += 'メッセージの形式は{"temperature": 12.34, "humidity": 12.34}のようなJSON形式でお願いします。'
        prompt_user += f"メニューは、{menus}です。"
        res_data = fetch_open_ai(prompt_system, prompt_user)
        return jsonify({"message": res_data["choices"][0]["message"]["content"]}), 201
    except Exception as e:
        return jsonify({"error": str(e)}), 500
