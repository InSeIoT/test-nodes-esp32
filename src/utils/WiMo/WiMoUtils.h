#include "Arduino.h"
#include <EEPROM.h>

/**
 * @defgroup VLR_MACRO
 * @brief Directivas con valores constantes
 * @{
 */

/**
 * @defgroup EEPROM_CONFIG_MAP
 * @brief Mapa de memoria EEPROM con registros de configuración
 * @note Valores tipos uint8_t
 * @{
 */
#define CONFIG_EEPROM_LOC 0 /**< Referencia de posición de registros de configuración*/
#define EEADDR_SWLPF 0
#define EEADDR_ACC_SCALE 1
#define EEADDR_GYRO_SCALE 2
#define EEADDR_MAG_SCALE 3
#define EEADDR_THERMAL 4
#define EEADDR_GAIN 5
#define EEADDR_OFFSET 6
#define EEADDR_SIGN 7
//@}

/**
 * @defgroup EEPROM_ALPHA_MAP
 * @brief Mapa de memoria EEPROM con parámetros de corrección térmica
 * @note Valores tipo double
 * @{
 */
#define ALPHA_EEPROM_LOC 8 /**< Referencia de posición con parámetros de corrección térmica*/
#define EEADDR_ALPHA_ACCX 0
#define EEADDR_ALPHA_ACCY 8
#define EEADDR_ALPHA_ACCZ 16
#define EEADDR_ALPHA_GYROX 24
#define EEADDR_ALPHA_GYROY 32
#define EEADDR_ALPHA_GYROZ 40
#define EEADDR_ALPHA_MAGX 48
#define EEADDR_ALPHA_MAGY 56
#define EEADDR_ALPHA_MAGZ 64
//@}

/**
 * @defgroup EEPROM_GAIN_MAP
 * @brief Mapa de memoria EEPROM con parámetros de corrección de ganancia
 * @note Valores tipo double
 * @{
 */
#define GAIN_EEPROM_LOC 80 /**< Referencia de posición con parámetros de ganancia*/
#define EEADDR_ALPHA_ACCX 0
#define EEADDR_ALPHA_ACCY 8
#define EEADDR_ALPHA_ACCZ 16
#define EEADDR_ALPHA_GYROX 24
#define EEADDR_ALPHA_GYROY 32
#define EEADDR_ALPHA_GYROZ 40
#define EEADDR_ALPHA_MAGX 48
#define EEADDR_ALPHA_MAGY 56
#define EEADDR_ALPHA_MAGZ 64
//@}

/**
 * @defgroup EEPROM_OFFSET_MAP
 * @brief Mapa de memoria EEPROM con parámetros de corrección de corrimiento
 * @note Valores tipo int16_t
 * @{
 */
#define OFFSET_EEPROM_LOC 160 /**< Referencia de posición con parámetros de corrimiento*/
#define EEADDR_OFF_ACCX 0
#define EEADDR_OFF_ACCY 2
#define EEADDR_OFF_ACCZ 4
#define EEADDR_OFF_GYROX 6
#define EEADDR_OFF_GYROY 8
#define EEADDR_OFF_GYROZ 10
#define EEADDR_OFF_MAGX 12
#define EEADDR_OFF_MAGY 14
#define EEADDR_OFF_MAGZ 16
//@}

#define VERIFY_SIGN 0xAB /**< Firma de integridad de EEPROM por defecto*/

/**
 * @} VLR_MACRO
 */

typedef struct
{
    bool offset;        /**< Compensar desplazamiento de valores */
    bool thermal;       /**< Compensar temperatura */
    bool gain;          /**< Compensar ganancia */
    uint8_t ACC_SCALE;  /**< Escala de ganancia de acelerómetro */
    uint8_t GYRO_SCALE; /**< Escala de ganancia de giroscopio */
    uint8_t MAG_SCALE;  /**< Escala de ganancia de magnetómetro */
    uint8_t SWLPF;      /**< Ajuste de filtro pasabajas de software */
    double ALPHA[9];    /**< Valores para compensación térmica*/
    int16_t OFFSETS[9]; /**< Valores de compensación de corrimiento*/
    double GAIN[9];     /**< Valores de ajuste de ganancia*/
} WiMo_Config;

class WiMoUtils
{
private:
    uint8_t _lpf_size;
    WiMo_Config config;

public:
    WiMoUtils();
    WiMo_Config loadConfig();
    float average(int16_t *vec);
    void setLPF(uint8_t lpf_size);
    template <typename RData>
    void EEPROM_read(uint8_t addr, RData &arg);
    template <typename WData>
    void EEPROM_write(uint8_t addr, WData &arg);
    template <typename RRData>
    void EEPROM_readdouble(uint8_t addr, RRData &arg);
    template <typename WWData>
    void EEPROM_writedouble(uint8_t addr, WWData &arg);
    void saveConfig(WiMo_Config cfg);
    void saveOffsets(WiMo_Config cfg);
    void saveGain(WiMo_Config cfg);
    void saveAlpha(WiMo_Config cfg);
    void saveFlags(WiMo_Config cfg);
};