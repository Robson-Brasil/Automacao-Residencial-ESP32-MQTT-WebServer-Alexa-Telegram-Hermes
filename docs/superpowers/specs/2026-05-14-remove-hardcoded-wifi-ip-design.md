# Design: Remoção de Configurações Fixas de Wi-Fi e IP

## 1. Contexto e Motivação
Atualmente, o projeto utiliza um sistema híbrido de conexão: primeiro tenta se conectar a um SSID e senha hardcoded com um IP estático; se falhar por 15 segundos, ativa o `WiFiManager`. Para produção e portabilidade, é necessário remover as credenciais fixas e a configuração de IP estático, confiando totalmente no `WiFiManager` e DHCP.

## 2. Mudanças Arquiteturais

### 2.1. Eliminação de Hardcoded Values
- **SSID/Password:** Removidos do arquivo principal e de `LoginsSenhas.h`.
- **IP Estático:** Removido o uso de `WiFi.config()`. O dispositivo agora obterá IP via DHCP.
- **Parâmetros de Rede:** `local_IP`, `gateway`, `subnet`, `primaryDNS`, `secondaryDNS` serão removidos.

### 2.2. Fluxo de Conectividade Simplificado
O `WiFiManager` será o único responsável por:
1. Tentar conectar a redes previamente salvas (se existirem).
2. Abrir o portal cativo (Access Point) caso não consiga conectar, permitindo a configuração manual pelo usuário.

## 3. Detalhamento das Alterações por Arquivo

### 3.1. `Automacao-Residencial-ESP32-MQTT-WebServer-Alexa-Telegram.ino`
- **Remover:** Variáveis globais `ssid`, `password`, `local_IP`, `gateway`, `subnet`, `primaryDNS`, `secondaryDNS`.
- **Refatorar `setupWiFi()`:**
  - Remover bloco `if (!wifiConfigDone)` que configurava IP fixo e chamava `WiFi.begin(ssid, password)`.
  - Remover o timer de fallback de 15 segundos (`currentMillis > 15000`).
  - Chamar `WiFiManagerSetup()` imediatamente se o Wi-Fi não estiver conectado.

### 3.2. `WiFiManagerConfig.h`
- **Remover:** Declarações `extern IPAddress` para parâmetros de rede estática.
- **Refatorar `WiFiManagerSetup()`:**
  - Remover a adição do parâmetro customizado `custom_ip` ("IP Fixo").

### 3.3. `LoginsSenhas.h`
- **Remover:** `extern const char* ssid` e `extern const char* password`.

### 3.4. `OTAConfig.cpp`
- **Remover:** `extern const char* ssid` e `extern const char* password`.

## 4. Validação
- O ESP32 deve iniciar e, caso não tenha rede salva, abrir um AP com o hostname definido.
- Ao configurar via portal, o dispositivo deve conectar e obter um IP via DHCP (visível no Serial Monitor).
- Re-conexão automática deve funcionar após reboot.
