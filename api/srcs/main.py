from flask import Flask, jsonify

app = Flask(__name__, static_folder="static")


# APIエンドポイント
@app.route("/api/hello")
def hello():
    return jsonify({"message": "Hello, World!"})


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=8080, debug=True)
