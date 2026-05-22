#!/usr/bin/env python3
"""
Hermes MQTT Bridge - Unificado
Envia comandos e recebe dados dos sensores do ESP32

Uso:
 python3 mqtt_hermes.py LIGAR:Sala
 python3 mqtt_hermes.py DESLIGAR:TUDO
 python3 mqtt_hermes.py STATUS
 python3 mqtt_hermes.py SENSORES
"""
import sys
import time
import json
import paho.mqtt.client as mqtt

# Configurações do Broker
BROKER = "192.168.15.21"
PORTA = 1883
USUARIO = "RobsonBrasil"
SENHA = "S3nh@S3gur@"

# Tópicos
TOPIC_COMANDO = "ESP32/Hermes/Comando"
TOPIC_STATUS_RESPOSTA = "ESP32/Hermes/StatusResposta"

TOPIC_SENSORES = [
    "ESP32/Temperatura",
    "ESP32/Umidade",
    "ESP32/SensacaoTermica",
    "ESP32/PontoOrvalho",
    "ESP32/BMP180/Temperatura",
    "ESP32/BMP180/PressaoAtmosferica/Real",
    "ESP32/BMP180/PressaoAtmosferica/NivelMar",
    "ESP32/BMP180/AltitudeReal",
    "ESP32/BMP180/AltitudeNivelMar",
]

LABELS = {
    "Temperatura": "🌡️ Temperatura DHT22",
    "Umidade": "💧 Umidade",
    "SensacaoTermica": "🔥 Sensação Térmica",
    "PontoOrvalho": "💦 Ponto de Orvalho",
    "BMP180/Temperatura": "🌡️ Temperatura BMP180",
    "BMP180/PressaoAtmosferica/Real": "📊 Pressão Real (BMP180)",
    "BMP180/PressaoAtmosferica/NivelMar": "📊 Pressão Nível Mar (BMP180)",
    "BMP180/AltitudeReal": "📏 Altitude Real (BMP180)",
    "BMP180/AltitudeNivelMar": "📏 Altitude Nível Mar (BMP180)",
}

# Mapeamento de itens normalizados
ITEM_MAP = {
    "VARANDA": "Varanda",
    "BANCADA": "Bancada",
    "SALA": "Sala",
    "COZINHA": "Cozinha",
    "QUINTAL": "Quintal",
    "VAL": "Val",
    "VARAO": "Val",  # Normaliza "Varão" para "Val"
    "VARÃO": "Val",  # Mesmo com UTF-8
    "ROBSON": "Robson",
    "KINHA": "Kinha",
    "TUDO": "TUDO",
}

COMANDOS_SENSORES = {"SENSORES", "SENSOR"}
COMANDOS_STATUS = {"STATUS"}

received_data = {}
status_data = {}


def normalizar_comando(comando):
    """Normaliza comando para LIGAR:ITEM em maiúsculas e mapeia itens."""
    cmd_parts = comando.upper().split(':', 1)
    if len(cmd_parts) != 2:
        return comando.upper()  # STATUS ou SENSORES
    
    cmd, item = cmd_parts
    item = ITEM_MAP.get(item, item)  # Aplica mapeamento
    return f"{cmd}:{item}"


def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        for topic in TOPIC_SENSORES:
            client.subscribe(topic)
        client.subscribe(TOPIC_STATUS_RESPOSTA)  # Novo tópico pra STATUS
    else:
        print(f"❌ Falha na conexão, código: {rc}")


def on_message(client, userdata, msg):
    global received_data, status_data
    try:
        payload = msg.payload.decode()
        if msg.topic == TOPIC_STATUS_RESPOSTA:
            # STATUS resposta vem em JSON: {"relay": "Varanda", "state": "ON"}
            status_data = json.loads(payload)
            return
        
        topic_key = msg.topic.split("/", 1)[1]
        label = LABELS.get(topic_key, topic_key)
        received_data[label] = payload
    except Exception as e:
        print(f"❌ Erro ao decodificar {msg.topic}: {e}")


def criar_cliente():
    client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
    client.username_pw_set(USUARIO, SENHA)
    return client


def enviar_comando(comando):
    """Envia comando e retorna True se sucesso"""
    comando = normalizar_comando(comando)  # Normaliza item e comando
    client = criar_cliente()
    try:
        client.connect(BROKER, PORTA, 60)
        client.loop_start()
        infot = client.publish(TOPIC_COMANDO, comando, qos=1)
        infot.wait_for_publish(timeout=3)
        print(f"✅ Comando enviado: {comando}")
        print(f" Topic: {TOPIC_COMANDO}")
        return True
    except Exception as e:
        print(f"❌ Erro na comunicação: {e}")
        return False
    finally:
        client.loop_stop()
        client.disconnect()


def ler_sensores():
    """Envia SENSORES e aguarda resposta do ESP32"""
    global received_data
    received_data = {}  # Limpa dados anteriores
    client = criar_cliente()
    client.on_connect = on_connect
    client.on_message = on_message

    try:
        client.connect(BROKER, PORTA, 60)
        client.loop_start()
        time.sleep(1)  # Aguarda conexão

        print("📤 Enviando comando SENSORES ao ESP32...")
        infot = client.publish(TOPIC_COMANDO, "SENSORES", qos=1)
        infot.wait_for_publish(timeout=3)

        print("⏳ Aguardando dados do ESP32 (10s)...")
        time.sleep(10)  # Timeout maior

        client.loop_stop()
        client.disconnect()

        return received_data
    except Exception as e:
        print(f"❌ Erro: {e}")
        return None


def ler_status():
    """Envia STATUS e aguarda resposta do ESP32"""
    global status_data
    status_data = {}  # Limpa dados anteriores
    client = criar_cliente()
    client.on_connect = on_connect
    client.on_message = on_message

    try:
        client.connect(BROKER, PORTA, 60)
        client.loop_start()
        time.sleep(1)

        print("📤 Enviando comando STATUS ao ESP32...")
        infot = client.publish(TOPIC_COMANDO, "STATUS", qos=1)
        infot.wait_for_publish(timeout=3)

        print("⏳ Aguardando status do ESP32 (5s)...")
        time.sleep(5)  # Aguarda resposta STATUS

        client.loop_stop()
        client.disconnect()

        return status_data
    except Exception as e:
        print(f"❌ Erro: {e}")
        return None


def exibir_sensores(dados):
    if not dados:
        print("\n⏰ Nenhum dado recebido.")
        print("💡 Verifique:")
        print(" 1. ESP32 conectado à mesma rede WiFi")
        print(" 2. ESP32 publicando dados após comando SENSORES")
        print(" 3. Broker MQTT acessível em 192.168.15.21:1883")
        return

    print(f"\n📊 Dados recebidos ({len(dados)} sensores):")
    for label, value in dados.items():
        print(f" {label}: {value}")


def exibir_status(dados):
    if not dados:
        print("\n⏰ Nenhum status recebido.")
        return

    print("\n📋 Status dos Relés:")
    estados = dados.get("estados", {})
    for item, estado in estados.items():
        emoji = "🟢" if estado == "ON" else "🔴"
        print(f" {emoji} {item}: {estado}")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Uso:")
        print(" python3 mqtt_hermes.py LIGAR:Sala")
        print(" python3 mqtt_hermes.py DESLIGAR:TUDO")
        print(" python3 mqtt_hermes.py STATUS")
        print(" python3 mqtt_hermes.py SENSORES")
        sys.exit(1)

    raw = sys.argv[1].upper()

    # Aceita tanto "ligar:bancada" quanto "ligar bancada"
    if raw in COMANDOS_SENSORES:
        dados = ler_sensores()
        exibir_sensores(dados)
        sys.exit(0 if dados else 1)
    if raw in COMANDOS_STATUS:
        dados = ler_status()
        exibir_status(dados)
        sys.exit(0 if dados else 1)

    if len(sys.argv) >= 3 and ":" not in raw:
        raw = f"{raw}:{sys.argv[2].upper()}"

    sucesso = enviar_comando(raw)
    sys.exit(0 if sucesso else 1)