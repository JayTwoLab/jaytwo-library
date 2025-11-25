import requests
import json
from flask import Flask, request, jsonify
 
app = Flask(__name__)

@app.route('/post', methods=['POST'])
def post_endpoint():
    # Try to parse JSON body 
    try:
        data = request.get_json(force=True)
    except Exception:
        return jsonify({"error": "Invalid JSON"}), 400

    # Echo back the received data with a status message
    response = {
        "status": "success",
        "received": data
    }
    return jsonify(response), 200

if __name__ == "__main__":
    # Run the server on localhost:50010
    app.run(host="127.0.0.1", port=50010, debug=True)
