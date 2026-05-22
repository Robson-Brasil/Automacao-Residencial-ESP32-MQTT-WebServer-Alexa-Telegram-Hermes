# 📚 Índice de Documentação

## 📖 Guias Principal

| Arquivo | Descrição |
|---------|------------|
| [PROJETO_DETALHADO.md](./PROJETO_DETALHADO.md) | Guia completo com todos os detalhes técnicos, arquitetura, configurações e funcionalidades |
| [CHEATSHEET.md](./CHEATSHEET.md) | Referência rápida de configuração, comandos MQTT, Hermes, API REST e pinagem |

---

## 🔧 Seções do Projeto Detalhado

1. [Visão Geral do Projeto](#1-visão-geral-do-projeto)
2. [Hardware e Pinagem](#2-hardware-e-pinagem)
   - Mapeamento de GPIOs
   - Nomes dos Relés
   - Sensores Suportados
   - Módulo Relé
3. [Arquitetura Multi-Core](#3-arquitetura-multi-core)
   - Core 0 (TaskConexoes)
   - Core 1 (TaskSensores)
   - Comunicação Inter-Core (IPC)
   - Task Handles
4. [Conectividade e Rede](#4-conectividade-e-rede)
   - WiFiManager
   - Persistência MQTT via NVS
   - mDNS
   - Auto-Reconexão
   - Auto Restart
5. [Sensores](#5-sensores)
   - DHT22 (Temperatura, Umidade, Sensação Térmica, Ponto de Orvalho)
   - BMP180 (Pressão, Altitude)
6. [Relés e Atuação](#6-relés-e-atuação)
   - Controle
   - Métodos de Controle
   - Persistência
7. [Integrações](#7-integrações)
   - MQTT
   - **Hermes** (Comandos em Texto Natural)
   - Telegram Bot
   - SinricPro (Alexa/Google Home)
   - **Home Assistant Discovery**
   - Dashboard Web
8. [Atualização OTA](#8-atualização-ota)
9. [Remote Debug (Telnet)](#9-remote-debug-telnet)
10. [Persistência de Dados](#10-persistência-de-dados)
11. [Tabela de Tópicos MQTT](#11-tabela-de-tópicos-mqtt)
12. [API REST do Dashboard](#12-api-rest-do-dashboard)
13. [Estrutura de Arquivos](#13-estrutura-de-arquivos)
14. [Requisitos de Bibliotecas](#14-requisitos-de-bibliotecas)
15. [Intervalos de Execução](#15-intervalos-de-execução)
16. [Fluxo de Sincronização Cross-Platform](#16-fluxo-de-sincronização-cross-platform)

---

## ⚡ Referência Rápida (Cheatsheet)

Ver [CHEATSHEET.md](./CHEATSHEET.md) para:
- Configuração de credenciais
- Comandos MQTT rápidos
- **Comandos Hermes** (texto natural)
- Comandos Telegram
- **Home Assistant Discovery**
- Endpoints da API REST
- Pinagem dos GPIOs
- Mapeamento Relé ↔ Nome ↔ Hermes
- Intervalos de execução
- Troubleshooting

---

## 📋 Histórico de Versões

| Versão | Data | Alterações |
|--------|------|-------------|
| 1.0.0 | Maio/2026 | Versão inicial com todas as funcionalidades documentadas |
| 1.1.0 | Maio/2026 | Adicionado: Hermes, Home Assistant Discovery, Telnet Debug, WiFiManager MQTT Config, Auto Restart, Ponto de Orvalho, nomes dos relés, dashboard glassmorphism |

---

*Última atualização: Maio de 2026*
