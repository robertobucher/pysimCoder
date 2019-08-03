typedef struct {
  short X;
  short Y;
  short Z;
}IMU_data;

int conf_reg(uint8_t reg, uint8_t val);
IMU_data imu_reg(uint8_t reg);
int spiOpen();
void spiClose();

