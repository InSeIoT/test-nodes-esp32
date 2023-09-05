#include "WiMoUtils.h"

WiMoUtils::WiMoUtils()
{

}

WiMo_Config WiMoUtils::loadConfig()
{
  EEPROM.begin(200); /**< Reserva un espacio de memoria FLASH como EEPROM*/
  // Valores por defecto
  config.SWLPF = 1;
  config.ACC_SCALE = 1;
  config.GYRO_SCALE = 1;
  config.MAG_SCALE = 1;
  config.offset = false;
  config.gain = false;
  config.thermal = false;
  for (int i = 0; i < 9; i++)
  {
    config.ALPHA[i] = 0;
    config.OFFSETS[i] = 0;
    config.GAIN[i] = 1;
  }

  uint8_t sig = 0;               /**< Variable temporal para verificar la firma de integridad EEPROM*/
  EEPROM_read(EEADDR_SIGN, sig); /**< Lee firma de integridad*/

  if (sig != VERIFY_SIGN)
  {
    saveConfig(config);
  }
  else
  {
    //Carga parámetros de corrección térmica de EEPROM y los muestra por UART
    for (int i = 0; i < 9; i++)
    {
      EEPROM_readdouble(ALPHA_EEPROM_LOC + (i * 8), config.ALPHA[i]);
      Serial.printf("ALPHA[%d]:\t\t", i);
      Serial.println(config.ALPHA[i]);
    }

    //Carga parámetros de corrección de corrimiento de EEPROM y los muestra por UART
    for (int i = 0; i < 9; i++)
    {
      EEPROM_read(OFFSET_EEPROM_LOC + (i * 2), config.OFFSETS[i]);
      Serial.printf("OFFSETS[%d]:\t%x\r\n", i, config.OFFSETS[i]);
    }

    //Carga parámetros de corrección de ganancia de EEPROM y los muestra por UART
    for (int i = 0; i < 9; i++)
    {
      EEPROM_readdouble(GAIN_EEPROM_LOC + (i * 8), config.GAIN[i]);
      Serial.printf("GAIN[%d]:\t\t", i);
      Serial.println(config.GAIN[i]);
    }

    //Carga parámetros de configuración de EEPROM y los muestra por UART
    EEPROM_read(CONFIG_EEPROM_LOC + EEADDR_SWLPF, config.SWLPF);
    EEPROM_read(CONFIG_EEPROM_LOC + EEADDR_ACC_SCALE, config.ACC_SCALE);
    EEPROM_read(CONFIG_EEPROM_LOC + EEADDR_GYRO_SCALE, config.GYRO_SCALE);
    EEPROM_read(CONFIG_EEPROM_LOC + EEADDR_MAG_SCALE, config.MAG_SCALE);
    EEPROM_read(CONFIG_EEPROM_LOC + EEADDR_THERMAL, config.thermal);
    EEPROM_read(CONFIG_EEPROM_LOC + EEADDR_GAIN, config.gain);
    EEPROM_read(CONFIG_EEPROM_LOC + EEADDR_OFFSET, config.offset);
  }
  return config;
}

void WiMoUtils::setLPF(uint8_t lpf_size)
{
  _lpf_size = (lpf_size > 0) ? lpf_size : 1;
}

/**
 * @brief Calcula el promedio de un vector
 * 
 * @param vec Vector a leer
 * @return float Promedio calculado
 */
float WiMoUtils::average(int16_t *vec)
{
  long double sum = 0;
  for (uint8_t i = 0; i < config.SWLPF; i++)
  {
    sum += vec[i];
  }
  return sum / config.SWLPF;
}

/*********************************  FUNCIONES SOBRE EEPROM  ****************************/

/**
 * @defgroup EEPROM_OP
 * @brief Funciones de operación sobre EEPROM
 * @{
 */

/**
 * @brief Lee un valor entero de cualquier tamaño de bit en EEPROM
 * 
 * @tparam RData Plantilla de tipo de dato
 * @param addr Dirección de memoria EEPROM a leer
 * @param arg  Valor a escribir en RAM
 */
template <typename RData>
void WiMoUtils::EEPROM_read(uint8_t addr, RData &arg)
{
  union
  {
    uint8_t b[8];
    uint64_t f;
  } data;
  data.f = 0;
  for (int i = 0; i < sizeof(arg); i++)
  {
    data.b[i] = EEPROM.read(addr + i);
  }
  arg = data.f;
}

/**
 * @brief Escribe un valor entero de cualquier tamaño de bit en EEPROM
 * 
 * @tparam WData Plantilla de tipo de dato
 * @param addr Dirección de memoria EEPROM
 * @param arg Valor a escribir en EEPROM
 */
template <typename WData>
void WiMoUtils::EEPROM_write(uint8_t addr, WData &arg)
{
  union
  {
    uint8_t b[8];
    uint64_t f;
  } data;
  data.f = arg;
  for (int i = 0; i < sizeof(arg); i++)
  {
    EEPROM.write(addr + i, data.b[i]);
  }
  EEPROM.commit();
}

/**
 * @brief Lee un valor de punto flotante double de EEPROM
 * 
 * @tparam RData Plantilla de tipo de dato
 * @param addr Dirección de memoria EEPROM a leer
 * @param arg Variable a escribir en RAM
 */
template <typename RData>
void WiMoUtils::EEPROM_readdouble(uint8_t addr, RData &arg)
{
  union
  {
    uint8_t b[8];
    double f;
  } data;
  data.f = 0;
  for (int i = 0; i < sizeof(arg); i++)
  {
    data.b[i] = EEPROM.read(addr + i);
  }
  arg = data.f;
}

/**
 * @brief Escribe un valor de punto flotante double en EEPROM
 * 
 * @tparam WData Plantilla de tipo de dato
 * @param addr Dirección de memoria EEPROM
 * @param arg Valor a escribir en EEPROM
 */
template <typename WData>
void WiMoUtils::EEPROM_writedouble(uint8_t addr, WData &arg)
{
  union
  {
    uint8_t b[8];
    double f;
  } data;
  data.f = arg;
  for (int i = 0; i < sizeof(arg); i++)
  {
    EEPROM.write(addr + i, data.b[i]);
  }
  EEPROM.commit();
}

void WiMoUtils::saveConfig(WiMo_Config cfg)
{
  uint8_t sig = 0xBB;
  EEPROM_write(EEADDR_SIGN, sig);
  saveOffsets(cfg);
  saveGain(cfg);
  saveAlpha(cfg);
  saveFlags(cfg);
  sig = VERIFY_SIGN;
  EEPROM_write(EEADDR_SIGN, sig);
}

/**
 * @brief Guarda en bloque los factores de corrimiento en EEPROM
 * 
 */
void WiMoUtils::saveOffsets(WiMo_Config cfg)
{
  config = cfg;
  for (int i = 0; i < 9; i++)
  {
    EEPROM_write(OFFSET_EEPROM_LOC + (i * 2), config.OFFSETS[i]);
  }
}

/**
 * @brief Guarda en bloque los factores de ganacia en EEPROM
 * 
 */
void WiMoUtils::saveGain(WiMo_Config cfg)
{
  config = cfg;
  for (int i = 0; i < 9; i++)
  {
    EEPROM_writedouble(GAIN_EEPROM_LOC + (i * 8), config.GAIN[i]);
  }
}

/**
 * @brief Guarda en bloque los factores de corrección térmica en EEPROM
 * 
 */
void WiMoUtils::saveAlpha(WiMo_Config cfg)
{
  config = cfg;
  for (int i = 0; i < 9; i++)
  {
    EEPROM_writedouble(ALPHA_EEPROM_LOC + (i * 8), config.ALPHA[i]);
  }
}

/**
 * @brief Guarda en bloque los valores de configuración del nodo en EEPROM
 * 
 */
void WiMoUtils::saveFlags(WiMo_Config cfg)
{
  config = cfg;
  EEPROM_write(CONFIG_EEPROM_LOC + EEADDR_SWLPF, config.SWLPF);
  EEPROM_write(CONFIG_EEPROM_LOC + EEADDR_ACC_SCALE, config.ACC_SCALE);
  EEPROM_write(CONFIG_EEPROM_LOC + EEADDR_GYRO_SCALE, config.GYRO_SCALE);
  EEPROM_write(CONFIG_EEPROM_LOC + EEADDR_MAG_SCALE, config.MAG_SCALE);
  EEPROM_write(CONFIG_EEPROM_LOC + EEADDR_THERMAL, config.thermal);
  EEPROM_write(CONFIG_EEPROM_LOC + EEADDR_GAIN, config.gain);
  EEPROM_write(CONFIG_EEPROM_LOC + EEADDR_OFFSET, config.offset);
}