from flask import Flask, render_template, request
from flask_socketio import SocketIO, emit, send
from socket import *

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret_key'
socketio = SocketIO(app)

client_socket =socket(AF_INET, SOCK_DGRAM) #Set up the Socket

clients = []
address = ('192.168.0.243', 80)

@app.route('/')
def index():
    return render_template('index.html')

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
    socketio.run(app)
