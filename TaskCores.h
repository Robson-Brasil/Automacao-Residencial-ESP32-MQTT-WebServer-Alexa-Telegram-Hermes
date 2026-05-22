/**
 * @file TaskCores.h
 * @brief Gerenciamento de Tasks FreeRTOS e distribuição entre núcleos do ESP32.
 * Define as funções de loop para Conexões (Core 0) e Sensores (Core 1).
 */
#ifndef TASKCORES_H
#define TASKCORES_H

void TaskConexoes(void *pvParameters);
void TaskSensores(void *pvParameters);

#endif // TASKCORES_H
