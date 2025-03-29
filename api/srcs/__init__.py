from flask import Flask
from flask_sqlalchemy import SQLAlchemy
import os

# アプリケーションの初期化
app = Flask(__name__, static_folder="/app/app/static")

# データベースの設定
DATABASE_URI = "sqlite:///db.sqlite3"
app.config["SQLALCHEMY_DATABASE_URI"] = DATABASE_URI
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False

# SQLAlchemy 初期化
db = SQLAlchemy(app)


# リクエストを受け付ける際に必ずDBが存在する状態にする
@app.before_request
def create_db():
    if not os.path.exists("db.sqlite3"):
        db.create_all()


# モデル、ルートのインポート
from app import routes, models
