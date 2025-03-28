import requests
import os
import json


def fetch_open_ai(prompt_system: str, prompt_user: str):
    # API キーを環境変数から取得
    api_key = os.getenv("OPENAI_API_KEY")
    url = "https://api.openai.com/v1/chat/completions"

    headers = {"Content-Type": "application/json", "Authorization": f"Bearer {api_key}"}

    data = {
        "model": "gpt-4o",
        "messages": [
            {"role": "developer", "content": prompt_system},
            {"role": "user", "content": prompt_user},
        ],
    }

    res = requests.post(url, headers=headers, data=json.dumps(data))

    if res.status_code == 200:
        return res.json()
    return None
