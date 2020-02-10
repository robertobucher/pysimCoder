typedef struct {
  short X;
  short Y;
  short Z;
}IMU_data;

int conf_reg_IMU(uint8_t reg, uint8_t val);
IMU_data imu_reg_IMU(uint8_t reg);
int spiOpen_IMU();
void spiClose_IMU();

