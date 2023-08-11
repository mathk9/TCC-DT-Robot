from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO, emit, send
from socket import *
import schedule
import time
import requests

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret_key'
socketio = SocketIO(app)

client_socket = socket(AF_INET, SOCK_DGRAM) #Set up the Socket

clients = []
address = ('192.168.0.153', 80)

helixPort = 80
helixAddress = ('52.149.214.24', helixPort)

# URL da API a ser monitorada
API_URL = "http://52.149.214.24:1026/v2/entities"

# Variável para armazenar os dados da última resposta da API
last_response_data = None

def monitor_api():
    global last_response_data
    try:
        headers = {
            'fiware-servicepath': '/',
            'Accept': '*/*',
            'fiware-service': 'openiot',
            'Content-Type': 'application/json',  # Define o tipo de conteúdo que estamos enviando (JSON neste caso)
            # Outras chaves e valores personalizados podem ser adicionados aqui, se necessário
        }

        response = requests.get(API_URL, headers=headers)
        response.raise_for_status()
        new_response_data = response.json()

        # print('new response: '+new_response_data)

        if new_response_data != last_response_data:
            last_response_data = new_response_data
            print("API monitored successfully.")
            # socketio.emit('api_response', last_response_data, broadcast=True)

    except requests.exceptions.RequestException as e:
        print(f"Failed to monitor API: {e}")

# Função para obter a última resposta da API
@app.route('/last_response', methods=['GET'])
def get_last_response():
    if last_response_data:
        return jsonify(last_response_data)
    return jsonify({"message": "No data available."}), 404

# Rota para a página inicial (com exemplo de uso de Socket.IO no frontend)
@app.route('/')
def index():
    return render_template('index.html')

# Agendar a execução da função de monitoramento a cada 1 minuto (pode ser ajustado conforme necessário)
schedule.every(0.05).minutes.do(monitor_api)

# Função para executar o agendador
def run_schedule():
    while True:
        schedule.run_pending()
        time.sleep(1)



@socketio.on('connect')
def handle_connect():
    print("client connected")
    clients.append(request.namespace)

@socketio.on('disconnect')
def handle_disconnect():
    print("client disconnected")
    clients.remove(request.namespace)

@socketio.on('update_servo')
def handle_update_servo(data):
    servo_id = data['servoId']
    angle = data['angle']

    # print('servo_id: '+str(servo_id)+' angle: '+str(angle))

    resp = 'servo_id='+str(servo_id)+'&angle='+str(angle)+' '

    client_socket.sendto( resp.encode(), address)

    rec_data, addr = client_socket.recvfrom(2048)
    print('servos: '+str(rec_data.decode()))

if __name__ == '__main__':
    # Iniciar a aplicação Flask com o Socket.IO em uma thread separada
    import threading
    threading.Thread(target=run_schedule).start()

    # Executar a aplicação Flask com o Socket.IO na thread principal
    socketio.run(app)
