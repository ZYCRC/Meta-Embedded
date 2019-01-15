#include "mpu6500.h"

#define PI 3.14159265358979323846f
#define GRAV 9.80665f
#define MPU6500_RX_BUF_SIZE 0x0E
#define TEMP_OFFSET 0.0f

static uint8_t mpu6500_RXData[MPU6500_RX_BUF_SIZE];

static const SPIConfig SPI5_cfg =
        {
                false,
                nullptr,
                GPIOF,
                GPIOF_SPI5_NSS,
                SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_MSTR |
                SPI_CR1_CPHA | SPI_CR1_CPOL //Set CPHA and CPOL to be 1
        };

void MPU6500Controller::mpu6500_write_reg(uint8_t reg_addr, uint8_t value) {
    uint8_t tx_data[2] = {reg_addr, value};
    spiAcquireBus(spi_driver);
    spiSelect(spi_driver);
    spiSend(spi_driver, 2, tx_data);
    spiUnselect(spi_driver);
    spiReleaseBus(spi_driver);
}

bool MPU6500Controller::start() {

    spiStart(spi_driver, &SPI5_cfg);
    mpu6500_write_reg(MPU6500_PWR_MGMT_1, MPU6500_RESET);
    chThdSleepMilliseconds(100);  // wait for MPU6500 to reset

    uint8_t init_reg[5][2] = {
            {MPU6500_PWR_MGMT_1, MPU6500_AUTO_SELECT_CLK}, // set auto clock
            {MPU6500_CONFIG, config._dlpf_config},
            {MPU6500_GYRO_CONFIG, config._gyro_scale << 3U},
            {MPU6500_ACCEL_CONFIG, config._accel_scale << 3U},
            {MPU6500_ACCEL_CONFIG_2, config._acc_dlpf_config}
    };

    for (int i = 0; i < 5; i++) {
        mpu6500_write_reg(init_reg[i][0], init_reg[i][1]);
    }

    dt = 0;
    _gyro_bias = 0;
    temperature = 0;
    _accel_bias[0][0] = 1.0f; _accel_bias[0][1] = 0.0f; _accel_bias[0][2] = 0.0f;
    _accel_bias[1][0] = 0.0f; _accel_bias[1][1] = 1.0f; _accel_bias[1][2] = 0.0f;
    _accel_bias[2][0] = 0.0f; _accel_bias[2][1] = 0.0f; _accel_bias[2][2] = 1.0f;

    // get the coefficient converting the raw data to degree or gravity
    switch(config._gyro_scale)
    {
        case MPU6500_GYRO_SCALE_250:
            _gyro_psc = (1.0f / 131.0f) * PI/180.0f;
            break;
        case MPU6500_GYRO_SCALE_500:
            _gyro_psc = (1.0f /  65.5f) * PI/180.0f;
            break;
        case MPU6500_GYRO_SCALE_1000:
            _gyro_psc = (1.0f /  32.8f) * PI/180.0f;
            break;
        case MPU6500_GYRO_SCALE_2000:
            _gyro_psc = (1.0f /  16.4f) * PI/180.0f;
            break;
        default:
            _gyro_psc = 0.0f;
            break;
    }

    switch(config._accel_scale)
    {
        case MPU6500_ACCEL_SCALE_2G:
            _accel_psc = (GRAV / 16384.0f);
            break;
        case MPU6500_ACCEL_SCALE_4G:
            _accel_psc = (GRAV /  8192.0f);
            break;
        case MPU6500_ACCEL_SCALE_8G:
            _accel_psc = (GRAV /  4096.0f);
            break;
        case MPU6500_ACCEL_SCALE_16G:
            _accel_psc = (GRAV /  2048.0f);
            break;
        default:
            _accel_psc = 0.0f;
            break;
    }

    return true;
}

void MPU6500Controller::getData() {
    uint32_t current_time =  chVTGetSystemTimeX();
    dt = TIME_I2S(current_time - prev_t);
    prev_t = current_time;

    // TODO: Here to read data

    float accel_x = _accel_psc * (int16_t)((mpu6500_RXData[ 0]<<8) | mpu6500_RXData[ 1]); // Accel X
    float accel_y = _accel_psc * (int16_t)((mpu6500_RXData[ 2]<<8) | mpu6500_RXData[ 3]); // Accel Y
    float accel_z = _accel_psc * (int16_t)((mpu6500_RXData[ 4]<<8) | mpu6500_RXData[ 5]); // Accel Z
    float gyro_x = _gyro_psc * (int16_t)((mpu6500_RXData[ 8]<<8) | mpu6500_RXData[ 9]);  // Gyro X
    float gyro_y = _gyro_psc * (int16_t)((mpu6500_RXData[10]<<8) | mpu6500_RXData[11]);  // Gyro Y
    float gyro_z = _gyro_psc * (int16_t)((mpu6500_RXData[12]<<8) | mpu6500_RXData[13]);  // Gyro Z
    auto temper = (int16_t)((mpu6500_RXData[6]<<8) | mpu6500_RXData[7]); // Temperature

    temperature = (((float)temper - TEMP_OFFSET) / 333.87f) + 21.0f;

    angel_speed.x = (gyro_x + _gyro_psc * _gyro_bias) / dt;
    angel_speed.y = (gyro_y + _gyro_psc * _gyro_bias) / dt;
    angel_speed.z = (gyro_z + _gyro_psc * _gyro_bias) / dt;

    a_component = Vector3D(accel_x, accel_y, accel_z) * _accel_bias;
}