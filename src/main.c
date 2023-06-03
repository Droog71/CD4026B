#include "wokwi-api.h"
#include <stdlib.h>

// Segment values.
const uint8_t digits [10] = {
  0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
  0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111
};

// Chip data.
typedef struct {
  pin_t MR;
  pin_t CLK;
  pin_t INH;
  pin_t DEI;
  pin_t DEO;
  pin_t CO;
  pin_t VSS;
  pin_t VDD;
  pin_t UCS;
  pin_t SEG[7];
  uint8_t digit;
} chip_data_t;

// Returns true if the power source is connected correctly.
bool power_connected(void *data)
{
  chip_data_t *chip = (chip_data_t*)data;
  return pin_read(chip->VDD) && !pin_read(chip->VSS);
}

// Resets the digit to zero. Active high on CO.
void reset(void *user_data)
{
  chip_data_t *chip = (chip_data_t*)user_data;
  chip->digit = 0;
  pin_write(chip->CO, 1);

  if (pin_read(chip->DEI))
  {
    for (int i = 0; i < 7; ++i)
    {
      pin_write(chip->SEG[i], digits[chip->digit] >> i & 1);
    }
  }
}

// Called on rising edge of clock pin.
void clock(void *user_data)
{
  chip_data_t *chip = (chip_data_t*)user_data;

  pin_write(chip->UCS, chip->digit == 2);

  if (chip->digit < 9)
  {
    chip->digit++;
    pin_write(chip->CO, 0);
  }
  else
  {
    reset(user_data);
  }

  if (pin_read(chip->DEI))
  {
    for (int i = 0; i < 7; ++i)
    {
      pin_write(chip->SEG[i], digits[chip->digit] >> i & 1);
    }
  }
}

// Enables or disables output pins A-F.
void dei(void* user_data, uint32_t value)
{
  chip_data_t *chip = (chip_data_t*)user_data;
  pin_write(chip->DEO, value);

  for (int i = 0; i < 7; ++i)
  {
    uint8_t digit = value ? digits[chip->digit] >> i & 1 : 0;
    pin_write(chip->SEG[i], digit);
  }
}

// Called whenever the clock pin changes state.
void chip_pin_change(void *user_data, pin_t pin, uint32_t value)
{
  chip_data_t *chip = (chip_data_t*)user_data;

  if (!power_connected(chip))
  {
    return;
  }

  if (pin == chip->DEI)
  {
    dei(user_data, value);
  }

  if (pin == chip->CLK && value)
  {
    clock(user_data);
  }

  if (pin == chip->MR && value)
  {
    reset(user_data);
  }
}

// Initializes the chip.
void chip_init() 
{
  chip_data_t *chip = (chip_data_t*)malloc(sizeof(chip_data_t));

  chip->MR = pin_init("MR", INPUT);
  chip->VSS = pin_init("VSS", INPUT);
  chip->VDD = pin_init("VDD", INPUT);
  chip->CLK = pin_init("CLK", INPUT);
  chip->INH = pin_init("INH", INPUT);
  chip->DEI = pin_init("DEI", INPUT);

  chip->CO = pin_init("CO", OUTPUT);
  chip->DEO = pin_init("DEO", OUTPUT);
  chip->UCS = pin_init("UCS", OUTPUT);
  chip->SEG[0] = pin_init("A", OUTPUT);
  chip->SEG[1] = pin_init("B", OUTPUT);
  chip->SEG[2] = pin_init("C", OUTPUT);
  chip->SEG[3] = pin_init("D", OUTPUT);
  chip->SEG[4] = pin_init("E", OUTPUT);
  chip->SEG[5] = pin_init("F", OUTPUT);
  chip->SEG[6] = pin_init("G", OUTPUT);

  const pin_watch_config_t watch_config = {
    .edge = BOTH,
    .pin_change = chip_pin_change,
    .user_data = chip,
  };

  pin_watch(chip->CLK, &watch_config);
  pin_watch(chip->DEI, &watch_config);
  pin_watch(chip->MR, &watch_config);
}