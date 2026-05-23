# Design: Integração MQTT no WiFiManager

**Data:** 2026-05-14
**Projeto:** Automação Residencial ESP32

---

## 1. Visão Geral

Adicionar campos de configuração do MQTT como parâmetros customizados no portal WiFiManager. O usuário configura WiFi + MQTT em um único lugar. Todas as configurações salvas na memória Flash.

---

## 2. Objetivos

- Eliminar credenciais hardcoded do MQTT
- Permitir configuração do Broker sem recompilação
- Manter portabilidade do dispositivo
- Usar WiFiManager como ponto único de configuração

---

## 3. Custom Parameters

Campos a serem adicionados no portal WiFiManager:

| Campo | ID | Label | Tipo | Default |
|-------|-----|-------|------|---------|
| Broker MQTT | mqtt_broker | IP do Broker | text | 192.168.15.150 |
| Porta MQTT | mqtt_port | Porta MQTT | number | 1883 |
| Usuário MQTT | mqtt_user | Usuario MQTT | text | RobsonBrasil |
| Senha MQTT | mqtt_pass | Senha MQTT | password | S3nh@S3gur@ |
| Client ID | mqtt_client | Client ID | text | ESP32-Cliente |

---

## 4. Fluxo de Operação

### Primeira inicialização:
1. ESP32 inicia sem configurações salvas
2. WiFiManager abre portal de configuração
3. Usuário preenche WiFi + MQTT
4. Ao salvar, WiFiManager salva tudo na memória
5. Conecta ao WiFi e depois ao Broker MQTT

### Reinicializações:
1. Carrega configurações do WiFiManager
2. Conecta automaticamente ao WiFi
3. Conecta ao Broker MQTT com configurações salvas
4. Se perder conexão, tenta reconectar automaticamente (já implementado)

---

## 5. Arquitetura

### WiFiManagerConfig.h
- Adicionar `WiFiManagerParameter` para cada campo MQTT
- Função de callback para processar configurações
- Carregar valores salvos na inicialização
- Validar campos antes de salvar

### LoginsSenhas.h
- Converter `extern const char*` para variáveis globais reais
- Inicializar com valores padrão
- Receber valores do WiFiManager

### AutomacaoResidencial.ino
- Remover definições hardcoded de MQTT
- Ajustar `setupMQTT()` para usar variáveis globais
- Ajustar `reconnectMQTT()` para usar variáveis globais

---

## 6. Persistência

- WiFiManager usa `wm.saveConfig()` internamente
- Armazena em partição NVS do ESP32
- Sobrecreve os valores hardcoded atuais
- Mantido junto com o estado dos relés (Preferences)

---

## 7. Tratamento de Erros

- Se MQTT não conectar, tenta reconectar em loop (já implementado)
- Portal WiFi continua disponível se WiFi falhar
- Se configuração MQTT vazia, usa valores padrão

---

## 8. Validações

- Broker MQTT: não pode estar vazio
- Porta MQTT: deve ser número válido (1-65535)
- Client ID: não pode estar vazio
- Usuário/Senha: opcionais (para brokers sem autenticação)

---

## 9. Backward Compatibility

- Na primeira execução, usa valores default
- Usuários existentes podem reconfigurar via portal
- Funcionalidade anterior (IP fixo) foi removida anteriormente