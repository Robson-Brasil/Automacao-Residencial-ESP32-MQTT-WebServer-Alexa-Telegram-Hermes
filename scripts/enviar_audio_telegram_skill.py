#!/usr/bin/env python3
"""
Script: enviar_audio_telegram_skill.py
Descrição: Envia áudio pro Telegram usando Edge TTS e API do Telegram.
Salva o áudio em C:\\Users\\RobsonBrasil\\hermes_voice.mp3 (acessível pelo Telegram).
"""

import asyncio
import edge_tts
import requests
import sys
import os
import time

# Configurações
TELEGRAM_BOT_TOKEN = "8656762812:AAFkWy9reL7fUGTdigM9pFjN5amh-0NTh84"
TELEGRAM_CHAT_ID = "5270818980"
AUDIO_FILE = "/mnt/c/Users/RobsonBrasil/hermes_voice.mp3"  # Caminho WSL (acessível pelo Telegram)

async def gerar_audio(texto):
    """Gera áudio usando Edge TTS e salva em AUDIO_FILE."""
    communicate = edge_tts.Communicate(texto, voice="pt-BR-AntonioNeural")
    await communicate.save(AUDIO_FILE)
    print(f"🔊 Áudio gerado: {AUDIO_FILE}")
    return AUDIO_FILE

def enviar_telegram():
    """Envia áudio pro Telegram via API."""
    url = f"https://api.telegram.org/bot{TELEGRAM_BOT_TOKEN}/sendAudio"
    with open(AUDIO_FILE, "rb") as f:
        files = {"audio": f}
        data = {"chat_id": TELEGRAM_CHAT_ID}
        resposta = requests.post(url, files=files, data=data)
    return resposta.json()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("❌ Usa: python3 enviar_audio_telegram_skill.py \"Texto do áudio\"")
        sys.exit(1)

    texto = sys.argv[1]
    print(f"✨ Enviando áudio: '{texto}'")

    # 1. Gerar áudio
    loop = asyncio.get_event_loop()
    loop.run_until_complete(gerar_audio(texto))

    # 2. Enviar pro Telegram
    resposta = enviar_telegram()
    print("✅ Telegram respondeu:", resposta)

    # 3. Verificar se o envio deu certo
    if resposta.get("ok", False):
        print("🎤 Áudio confirmado no Telegram!")
        # Aguardar 2 segundos e remover
        time.sleep(2)
        os.remove(AUDIO_FILE)
        print("♻️ Áudio temporário removido.")
    else:
        print("❌ Erro no envio! Cheque o arquivo em:", AUDIO_FILE)