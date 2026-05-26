import paho.mqtt.client as mqtt
import time

BROKER = "192.168.15.21"
PORTA = 1883
USUARIO = "RobsonBrasil"
SENHA = "S3nh@S3gur@"
TOPIC_COMANDO = "ESP32/Hermes/Comando"
TOPIC_STATUS_RESPOSTA = "ESP32/Hermes/StatusResposta"

status_data = {}
connected = False
t0 = time.time()

def on_connect(client, userdata, flags, rc, properties=None):
    global connected
    connected = True
    print(f"[on_connect T+{time.time()-t0:.1f}s] rc={rc}")
    client.subscribe(TOPIC_STATUS_RESPOSTA)

def on_message(client, userdata, msg):
    print(f"[MSG T+{time.time()-t0:.1f}s] [{msg.topic}] {msg.payload.decode()[:80]}")
    if msg.topic == TOPIC_STATUS_RESPOSTA:
        status_data["r"] = msg.payload.decode()

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(USUARIO, SENHA)
client.connect(BROKER, PORTA, 60)
client.loop_start()

time.sleep(1)
print(f"[PUB T+{time.time()-t0:.1f}s] connected={connected}")
client.publish(TOPIC_COMANDO, "STATUS", qos=1)

time.sleep(5)
client.loop_stop()
client.disconnect()
tem_resp = "r" in status_data
print(f"\nResposta: {tem_resp} (connected={connected})")
if tem_resp:
    print("Dados: " + status_data["r"])
