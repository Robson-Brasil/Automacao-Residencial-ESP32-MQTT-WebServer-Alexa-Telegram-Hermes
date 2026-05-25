/**
 * @file ConstantesTempo.h
 * @brief Definição de intervalos de tempo para tasks e reconexões.
 * Centraliza os tempos de leitura de sensores e retry de rede.
 */
#ifndef CONSTANTESTEMPO_H
#define CONSTANTESTEMPO_H

/**
 * @brief Intervalo de tempo em milissegundos para tentativas de reconexão Wi-Fi.
 */
const unsigned long WIFI_RETRY_INTERVAL = 1000;

/**
 * @brief Intervalo de piscar do LED de status durante a conexão Wi-Fi.
 */
const unsigned long WIFI_BLINK_INTERVAL = 500;

/**
 * @brief Intervalo de tempo para verificar a conexão com o servidor/broker.
 */
const unsigned long RECONNECT_INTERVAL = 5000;

/**
 * @brief Intervalo de leitura do sensor de temperatura e umidade DHT.
 */
const unsigned long DHT_READ_INTERVAL = 60000;

/**
 * @brief Intervalo de leitura do sensor de pressão barométrica BMP.
 */
const unsigned long BMP_READ_INTERVAL = 120000;

#endif // CONSTANTESTEMPO_H
