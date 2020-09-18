#ifndef LSM6DSL_LIBRARY_H
#define LSM6DSL_LIBRARY_H

#include <Wire.h>
#include <SPI.h>
#include <stdint.h>
#include "LSM6DSL_Constants.h"

/**
 * LSM6DSL operation mode enum
 *
 */
typedef enum {
    LSM6DSL_MODE_I2C,
    LSM6DSL_MODE_SPI
} lsm6dsl_mode_t;

/**
 * LSM6DSL status enum
 */
typedef enum {
    IMU_SUCCESS,
    IMU_HW_ERROR,
    IMU_NOT_SUPPORTED,
    IMU_GENERIC_ERROR,
    IMU_OUT_OF_BOUNDS,
    IMU_ALL_ONES_WARNING,
} lsm6dsl_status_t;

class LSM6DSLCore {
public:
    LSM6DSLCore(lsm6dsl_mode_t operationMode, uint8_t arg);
    LSM6DSLCore(uint8_t addr);
    ~LSM6DSLCore() = default;

    lsm6dsl_status_t beginCore();
    lsm6dsl_status_t readRegister(uint8_t* output, uint8_t offset);
    lsm6dsl_status_t readRegisterRegion(uint8_t* output, uint8_t offset, uint8_t length);
    lsm6dsl_status_t readRegisterInt16(int16_t* output, uint8_t offsetL, uint8_t offsetM);
    lsm6dsl_status_t readRegisterInt16(int16_t* output, uint8_t offset);
    lsm6dsl_status_t writeRegister(uint8_t offset, uint8_t data);
    lsm6dsl_status_t embeddedPage();
    lsm6dsl_status_t basePage();

private:
    lsm6dsl_mode_t opMode;
    uint8_t i2cAddress;
    uint8_t slaveSelect;
};

struct SensorSettings {
    //Gyro settings
    uint8_t gyroEnabled;
    uint16_t gyroRange;
    uint16_t gyroSampleRate;

    uint8_t gyroFifoEnabled;
    uint8_t gyroFifoDecimation;

    //Accelerometer settings
    uint8_t accelEnabled;
    uint8_t accelODROff;
    uint16_t accelRange;
    uint16_t accelSampleRate;
    uint16_t accelBandWidth;

    uint8_t accelFifoEnabled;
    uint8_t accelFifoDecimation;

    //Temperature settings
    uint8_t tempEnabled;

    //Non-basic mode settings
    uint8_t commMode;

    //FIFO control data
    uint16_t fifoThreshold;
    int16_t fifoSampleRate;
    uint8_t fifoModeWord;
};

class LSM6DSL: public LSM6DSLCore {
public:
    SensorSettings settings;

    LSM6DSL(uint8_t address = 0x6B);
    LSM6DSL(lsm6dsl_mode_t mode = LSM6DSL_MODE_I2C, uint8_t arg = 0x6B);
    ~LSM6DSL() = default;

    lsm6dsl_status_t begin();

    int16_t readRawTemperature();
    float readTemperatureC();
    float readTemperatureF();

    int16_t readRawAccelX();
    int16_t readRawAccelY();
    int16_t readRawAccelZ();

    int16_t readRawGyroX();
    int16_t readRawGyroY();
    int16_t readRawGyroZ();

    float readFloatAccelX();
    float readFloatAccelY();
    float readFloatAccelZ();

    float readFloatGyroX();
    float readFloatGyroY();
    float readFloatGyroZ();

    float convertAccel(int16_t axisValue);
    float convertGyro(int16_t axisValue);

private:
    void initSettings();
};

#endif
