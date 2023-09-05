#include "WiMo2.h"

/**
 * @brief Este archivo contiene todas las funciones requeridas para
 * la operación y captura de datos de uno nodo de sensores con una
 * unidad de medición inercial de 16 bits
 * 
 * @file WiMo.cpp
 * @author Manuel Fernando Galindo
 * @date 2018-06-13
 * @par ©2018 - Copyright Manuel Fernando Galindo, Universidad del Cauca 
 */

/**
 * @brief Construye un nuevo objeto new Wi Mo:: Wi Mo
 */
WiMo::WiMo()
{
}

void WiMo::begin()
{
    Serial.println("WiMo Begin");
    config = utils.loadConfig();
}

IMU WiMo::correction(IMU_RAW imux)
{
    IMU imu;
    // Guardando valores en vector LPF
    uint8_t position = IMU_SAMPLES % config.SWLPF;
    acc_m[0][position] = imux.acc.x;
    acc_m[1][position] = imux.acc.y;
    acc_m[2][position] = imux.acc.z;
    gyro_m[0][position] = imux.gyro.x;
    gyro_m[1][position] = imux.gyro.y;
    gyro_m[2][position] = imux.gyro.z;
    mag_m[0][position] = imux.mag.x;
    mag_m[1][position] = imux.mag.y;
    mag_m[2][position] = imux.mag.z;
    temp_m[position] = imux.t;

    IMU_SAMPLES++;

    // Filtrado
    imu.t = utils.average(temp_m); /**< Obtiene el promedio del vector de temperatura*/

    imu.acc.x = utils.average(acc_m[0]); /**< Promedio del vector de acelerómetro en eje X*/
    imu.acc.y = utils.average(acc_m[1]); /**< Promedio del vector de acelerómetro en eje Y*/
    imu.acc.z = utils.average(acc_m[2]); /**< Promedio del vector de acelerómetro en eje Z*/

    imu.gyro.x = utils.average(gyro_m[0]); /**< Promedio del vector del giroscopio en eje X*/
    imu.gyro.y = utils.average(gyro_m[1]); /**< Promedio del vector del giroscopio en eje Y*/
    imu.gyro.z = utils.average(gyro_m[2]); /**< Promedio del vector del giroscopio en eje Z*/

    imu.mag.x = utils.average(mag_m[0]); /**< Promedio del vector del magnetómetro en eje X*/
    imu.mag.y = utils.average(mag_m[1]); /**< Promedio del vector del magnetómetro en eje Y*/
    imu.mag.z = utils.average(mag_m[2]); /**< Promedio del vector del magnetómetro en eje Z*/

    //Aplica el factor de compensación térmica en cada eje
    if (config.thermal)
    {
        imu.acc.x -= (imu.t * config.ALPHA[0]);
        imu.acc.y -= (imu.t * config.ALPHA[1]);
        imu.acc.z -= (imu.t * config.ALPHA[2]);
        imu.gyro.x -= (imu.t * config.ALPHA[3]);
        imu.gyro.y -= (imu.t * config.ALPHA[4]);
        imu.gyro.z -= (imu.t * config.ALPHA[5]);
        imu.mag.x -= (imu.t * config.ALPHA[6]);
        imu.mag.y -= (imu.t * config.ALPHA[7]);
        imu.mag.z -= (imu.t * config.ALPHA[8]);
    }

    //Aplica el factor de ganancia en cada eje
    if (config.gain)
    {
        imu.acc.x *= config.GAIN[0];
        imu.acc.y *= config.GAIN[1];
        imu.acc.z *= config.GAIN[2];
        imu.gyro.x *= config.GAIN[3];
        imu.gyro.y *= config.GAIN[4];
        imu.gyro.z *= config.GAIN[5];
        imu.mag.x *= config.GAIN[6];
        imu.mag.y *= config.GAIN[7];
        imu.mag.z *= config.GAIN[8];
    }

    //Resta el factor de compensación de corrimiento
    if (config.offset)
    {
        imu.acc.x -= config.OFFSETS[0];
        imu.acc.y -= config.OFFSETS[1];
        imu.acc.z -= config.OFFSETS[2];
        imu.gyro.x -= config.OFFSETS[3];
        imu.gyro.y -= config.OFFSETS[4];
        imu.gyro.z -= config.OFFSETS[5];
        imu.mag.x -= config.OFFSETS[6];
        imu.mag.y -= config.OFFSETS[7];
        imu.mag.z -= config.OFFSETS[8];
    }
    return imu;
}
