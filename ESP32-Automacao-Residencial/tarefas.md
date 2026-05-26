# Tarefas de Melhoria - ESP32 Automação Residencial

**Data de criação:** 25/05/2026  
**Última atualização:** 26/05/2026  
**Objetivo:** Corrigir problemas críticos de segurança, estabilidade e qualidade de código

---

## CRÍTICAS (Segurança e Estabilidade)

### ✅ 1. Race Conditions em variáveis compartilhadas entre cores
**Arquivos:** `VariaveisGlobais.h/cpp`, `.ino:209-386`, `.ino:738-773`  
**Problema:** Variáveis como `RelayState1-8`, `Todos`, `pendingMqttUpdate`, `pendingSinricProUpdate` são lidas/escritas por ambos os cores sem mutex ou semáforo. O Core 1 (TaskSensores/Telegram) modifica estados de relés enquanto o Core 0 (TaskConexoes) lê e publica via MQTT. Isso pode causar corrupção de dados, estados inconsistentes e crashes aleatórios.  
**Solução:** Usar `SemaphoreHandle_t` (mutex) para proteger acesso a variáveis compartilhadas, ou `std::atomic` para flags booleanos simples.  
**Status:** ✅ Concluído — `relayMutex` adicionado em `VariaveisGlobais.h/cpp` e inicializado no `setup()`.

### ✅ 2. `#define Serial DualSerial` no `RemoteDebug.h:62`
**Problema:** Esta macro redefinição global afeta todas as bibliotecas incluídas depois (PubSubClient, SinricPro, UniversalTelegramBot, etc). Qualquer biblioteca que use `Serial` internamente será redirecionada para o Telnet, podendo causar comportamento imprevisível, travamentos ou perda de dados.  
**Solução:** Remover a macro e usar `DualSerial` explicitamente apenas onde necessário, ou adotar uma abordagem de log centralizado.  
**Status:** ✅ Concluído — Macro substituída por macros `LOG_PRINT`/`LOG_PRINTLN` que não afetam bibliotecas externas.

### ✅ 3. Senha OTA hardcoded em `OTAConfig.cpp:12`
**Problema:** `ArduinoOTA.setPassword("S3nh@S3gur@")` está em texto claro no código-fonte. Se o repo for público ou acessado por terceiros, a senha fica exposta.  
**Solução:** Mover para `LoginsSenhas.h` ou armazenar em NVS.  
**Status:** ✅ Concluído — Senha movida para `LoginsSenhas.h` como `OTA_PASSWORD`.

### ✅ 4. `telegramClient.setInsecure()` em `TelegramBotESP32.h:89`
**Problema:** Desabilita verificação de certificado TLS, tornando a comunicação com o Telegram vulnerável a ataques MITM (Man-in-the-Middle).  
**Solução:** Usar certificado root CA para validação TLS (`telegramClient.setCACert()`).  
**Status:** ✅ Concluído — Certificado root CA do Telegram adicionado.

### ✅ 5. `LoginsSenhas.h` possivelmente committado no repo
**Problema:** O arquivo `LoginsSenhas.h` existe no repo junto com `LoginsSenhas_example.h`. Se o `.gitignore` não excluir `LoginsSenhas.h`, credenciais reais (WiFi, MQTT, Telegram, SinricPro) estão versionadas.  
**Solução:** Adicionar `LoginsSenhas.h` ao `.gitignore` e manter apenas o `_example.h`.  
**Status:** ✅ Concluído — `.gitignore` já continha `LoginsSenhas.h` na linha 67.

---

## ALTAS (Bugs e Corretude)

### ✅ 6. `pub17` (Ponto de Orvalho) nunca é publicado via MQTT
**Arquivos:** `TopicosMQTT.h:37`, `HomeAssistantDiscovery.h:92`, `.ino:510-535`  
**Problema:** O tópico `pub17` está definido e o HA Discovery o referencia, mas `publishSensorData()` não publica `str_dewpoint_data` em `pub17`. O sensor aparece no HA mas nunca recebe dados.  
**Solução:** Adicionar `mqttClient.publish(pub17, str_dewpoint_data, true);` em `publishSensorData()`.  
**Status:** ✅ Concluído — `pub17` agora é publicado em `publishSensorData()`.

### ✅ 7. `setupWiFi()` bloqueante por até 30 segundos
**Arquivo:** `.ino:124-128`  
**Problema:** Loop `while` com `delay(500)` x 60 tentativas = 30s de bloqueio total. Como é chamado dentro de `TaskConexoes`, bloqueia MQTT, Telegram, OTA e SinricPro durante toda a reconexão.  
**Solução:** Usar `WiFi.begin()` assíncrono e verificar conexão em iterações subsequentes do loop, sem `while` bloqueante.  
**Status:** ✅ Concluído — `setupWiFi()` agora usa `WiFi.begin()` assíncrono.

### ✅ 8. `delay(200)` bloqueante no HA Discovery
**Arquivo:** `HomeAssistantDiscovery.h`  
**Problema:** Cada `_publishHA` é seguido de `delay(200)`. Com 17 publicações = 3.4 segundos de bloqueio total no Core 0, paralisando todas as conexões.  
**Solução:** Usar timestamps (`millis()`) para espalhar as publicações ao longo do tempo, sem bloquear.  
**Status:** ✅ Concluído — HA Discovery agora usa máquina de estados com `millis()`.

### ✅ 9. Buffer overflow potencial no `mqttCallback`
**Arquivo:** `.ino:193-198`  
**Problema:** Não verifica `length` antes de construir `messageTemp`. Um payload malicioso ou corrompido muito grande pode causar overflow na `String`.  
**Solução:** Adicionar `if (length > 256) return;` no início do callback.  
**Status:** ✅ Concluído — Guard `if (length > 255) return;` adicionado antes da construção da string.

### ✅ 10. Inconsistência de nomes: "Varão" vs "Val"
**Arquivos:** `TelegramBotESP32.h:27` (define como "Varão"), `.ino:365` (aceita ambos), teclado usa "Val"  
**Problema:** `relayNames[5]` é definido como `"Varão"`, mas o teclado usa `"Val"`, e os comandos Hermes aceitam ambos. Isso causa confusão na UX.  
**Solução:** Padronizar um único nome em todos os pontos.  
**Status:** ✅ Concluído — Teclado do Telegram atualizado para "Varão".

---

## MÉDIAS (Qualidade de Código e Manutenção)

### ✅ 11. Código massivamente duplicado no `mqttCallback`
**Arquivo:** `.ino:209-300`  
**Problema:** 8 blocos `else if` quase idênticos para tratar cada relé. ~90 linhas que poderiam ser ~15 com arrays de lookup.  
**Solução:** Criar array de structs com `{sub_topic, pin, state_ptr, relay_num}` e iterar.  
**Status:** ✅ Concluído — 8 blocos `else if` substituídos por loop com arrays `relayTopics[]`, `relayPinNums[]`, `relayStatePtrs[]`.

### ✅ 12. Código massivamente duplicado nos comandos Hermes
**Arquivo:** `.ino:334-384`  
**Problema:** LIGAR/DESLIGAR individual repete 8 blocos idênticos em linhas longas e difíceis de ler.  
**Solução:** Criar função `setRelayByIndex(int index, bool state, const char* source)` e usar lookup de nome->índice.  
**Status:** ✅ Concluído — Funções `setRelayByIndex()`, `setAllRelays()`, `findRelayByName()` extraídas; 16 blocos eliminados.

### ✅ 13. Código duplicado no `handleTelegramMessages`
**Arquivo:** `TelegramBotESP32.h:134-141`  
**Problema:** Mesma repetição de 8 blocos para toggle de relés.  
**Solução:** Mesma abordagem de array de lookup.  
**Status:** ✅ Concluído — 8 blocos substituídos por loop + `setRelayByIndex()` + `setAllRelays()`.

### ❌ 14. `.ino` monolítico com 818 linhas
**Problema:** Toda a lógica principal (WiFi, MQTT, sensores, persistência, Hermes, tasks) está em um único arquivo.  
**Solução:** Mover implementações para arquivos `.cpp` separados (ex: `MQTTHandler.cpp`, `SensorManager.cpp`, `HermesHandler.cpp`).  
**Status:** ❌ Pendente — Refatoração estrutural extensa.

### ✅ 15. Stack size potencialmente insuficiente
**Arquivo:** `.ino:670-676`  
**Problema:** `TaskSensores` usa 4096 bytes. Operações I2C + DHT + `dtostrf` + `Serial.println` podem estourar esse limite, causando crash silencioso (stack overflow no ESP32 corrompe memória sem aviso).  
**Solução:** Aumentar para 8192 ou monitorar com `uxTaskGetStackHighWaterMark()`.  
**Status:** ✅ Concluído — Stack size aumentada para 8192.

---

## BAIXAS (Performance e Boas Práticas)

### ✅ 16. `String` do Arduino em vez de `char[]` no callback MQTT
**Arquivo:** `.ino:194`  
**Problema:** Concatenação caractere por caractere com `String` causa fragmentação de heap no ESP32.  
**Solução:** Usar `char messageTemp[256]; memcpy(messageTemp, payload, min(length, 255));`  
**Status:** ✅ Concluído — `char[256]` + `memcpy` substituiu concatenação caractere por caractere.

### ✅ 17. `readSensors()` - variável `buffer` não utilizada
**Arquivo:** `.ino:453`  
**Problema:** `char buffer[10]` recebe valores via `dtostrf` mas nunca é usado para publicação. Código morto.  
**Solução:** Remover ou documentar a intenção.  
**Status:** ✅ Concluído — Variável `buffer` e chamadas `dtostrf` mortas removidas.

### ❌ 18. Ausência de watchdog task
**Problema:** Não há monitoramento de saúde das tasks. Se `TaskConexoes` ou `TaskSensores` travar, o sistema fica inoperante sem recuperação.  
**Solução:** Implementar watchdog que monitora heartbeat de cada task e reinicia o ESP32 em caso de travamento.  
**Status:** ❌ Pendente — Requer implementação de watchdog com heartbeat.

### ✅ 19. `preferences` não é protegida contra acesso concorrente
**Problema:** `saveRelayState()` é chamada de ambos os cores (via Telegram no Core 0 e via Hermes/MQTT callback também no Core 0, mas `loadRelayState` é chamada no `setup`). Operações NVS não são thread-safe.  
**Solução:** Centralizar todas as operações NVS em um único core ou proteger com mutex.  
**Status:** ✅ Concluído — `saveRelayState()` e `loadRelayState()` protegidas com `xSemaphoreTake/Give(relayMutex)`.

### ✅ 20. `WiFiClientSecure` para Telnet sem autenticação
**Problema:** O servidor Telnet na porta 2323 aceita qualquer conexão sem autenticação. Qualquer pessoa na rede pode enviar comandos ao Serial.  
**Solução:** Adicionar autenticação básica ou restringir acesso por IP/MAC.  
**Status:** ✅ Concluído — Telnet exige senha (`TELNET_PASSWORD = OTA_PASSWORD`) antes de liberar bridge Serial.

---

## Resumo

| Prioridade | Total | ✅ Concluído | ❌ Pendente |
|------------|-------|-------------|------------|
| Críticas   | 5     | 5           | 0          |
| Altas      | 5     | 5           | 0          |
| Médias     | 5     | 4           | 1          |
| Baixas     | 5     | 4           | 1          |
| **Total**  | **20**| **18**      | **2**      |

**Próximas etapas recomendadas:**
1. Tarefa 18 — Implementar watchdog task com heartbeat
2. Tarefa 14 — Refatoração estrutural do `.ino` em múltiplos `.cpp`
