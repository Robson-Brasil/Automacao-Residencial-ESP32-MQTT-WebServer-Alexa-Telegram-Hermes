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
extern const unsigned long WIFI_RETRY_INTERVAL;

/**
 * @brief Intervalo de piscar do LED de status durante a conexão Wi-Fi.
 */
extern const unsigned long WIFI_BLINK_INTERVAL;

/**
 * @brief Intervalo de tempo para verificar a conexão com o servidor/broker.
 */
extern const unsigned long RECONNECT_INTERVAL;

/**
 * @brief Intervalo de leitura do sensor de temperatura e umidade DHT.
 */
extern const unsigned long DHT_READ_INTERVAL;

/**
 * @brief Intervalo de leitura do sensor de pressão barométrica BMP.
 */
extern const unsigned long BMP_READ_INTERVAL;

#endif // CONSTANTESTEMPO_H
