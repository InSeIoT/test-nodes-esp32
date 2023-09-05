#ifndef __WIMO_H__
#define __WIMO_H__

#include "Arduino.h"
#include "WiMoUtils.h"

/**
 * @defgroup VLR_MACRO
 * @brief Directivas con valores constantes
 * @{
 */

#define LPF_BUFFER_SIZE 255 /**< Tamaño de buffer para filtro pasabajas de software*/

/**
 * @} VLR_MACRO
 */

/**
 * @brief Crea una estructura de tres dimensiones tipo double
 */
typedef struct
{
    double x;
    double y;
    double z;
} Vector3DDouble;

/**
 * @brief Crea una estructura de tres dimensiones tipo Int16
 */
typedef struct
{
    int x;
    int y;
    int z;
} Vector3DInt;


/**
 * @brief Crea una estructura de vectores para contener datos de IMU
 */
typedef struct
{
    Vector3DDouble acc;
    Vector3DDouble gyro;
    Vector3DDouble mag;
    double t;
} IMU;

/**
 * @brief Crea una estructura de vectores para contener datos de IMU
 */
typedef struct
{
    Vector3DInt acc;
    Vector3DInt gyro;
    Vector3DInt mag;
    int t;
} IMU_RAW;

/**
 * @brief Definición de clase WiMo
 * 
 */
class WiMo
{
private:
    WiMo_Config config; /**< Configuración de los sensores */
    /**
     * @defgroup UTL_DEF
     * @brief Variables utilitarias de ejecución
     * @{
     */

    int16_t acc_m[3][LPF_BUFFER_SIZE] = {{0}};  /**< Matrix 3 x LPF_BUFFER_SIZE de buffer de acelerómetro*/
    int16_t gyro_m[3][LPF_BUFFER_SIZE] = {{0}}; /**< Matrix 3 x LPF_BUFFER_SIZE de buffer de giroscopio*/
    int16_t mag_m[3][LPF_BUFFER_SIZE] = {{0}};  /**< Matrix 3 x LPF_BUFFER_SIZE de buffer de magnetómetro*/
    int16_t temp_m[LPF_BUFFER_SIZE] = {0};      /**< Vector de tamaño LPF_BUFFER_SIZE de buffer de temperatura*/
    uint8_t IMU_SAMPLES;                        /**< Contador de muestras IMU */
    WiMoUtils utils;                            /**< Funciones utilitarias */
    /**
     * @} UTL_DEF
     */

public:
    WiMo();
    // void setNetAP(char *SSID, IPAddress ip_local, IPAddress gateway, IPAddress subnet);
    // void setNetClient(char *SSID, char *Pwd, IPAddress ip_local, IPAddress gateway, IPAddress subnet);
    // void setNetClient(char *SSID, IPAddress ip_local, IPAddress gateway, IPAddress subnet);
    // void setNetClient(char *SSID);
    // void setNetClient(char *SSID, char *Pwd);
    void begin();
    void updateConfig();
    IMU correction(IMU_RAW imu);
};

#endif