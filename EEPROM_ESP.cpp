/*
  EEPROM.h -ported by Paolo Becchi to Esp32 from esp8266 EEPROM
           -Modified by Elochukwu Ifediora <ifedioraelochukwuc@gmail.com>
           -Converted to nvs lbernstone@gmail.com
  Uses a nvs byte array to emulate EEPROM
  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifdef ESP32

#include "EEPROM_ESP.h"
#include <nvs.h>
#include <esp_log.h>

EEPROMClassESP::EEPROMClassESP(void)
    : _handle(NULL), _data(0), _size(0), _dirty(false), _name("eeprom"), _user_defined_size(0)
{
}

EEPROMClassESP::EEPROMClassESP(uint32_t sector)
    // Only for compatiility, no sectors in nvs!
    : _handle(NULL), _data(0), _size(0), _dirty(false), _name("eeprom"), _user_defined_size(0)
{
}

EEPROMClassESP::EEPROMClassESP(const char *name, uint32_t user_defined_size)
    : _handle(NULL), _data(0), _size(0), _dirty(false), _name(name), _user_defined_size(user_defined_size)
{
}

EEPROMClassESP::~EEPROMClassESP()
{
  // end();
}

bool EEPROMClassESP::begin(size_t size)
{
  if (!size)
  {
    return false;
  }

  esp_err_t res = nvs_open(_name, NVS_READWRITE, &_handle);
  if (res != ESP_OK)
  {
    log_e("Unable to open NVS namespace: %d", res);
    return false;
  }

  size_t key_size = 0;
  res = nvs_get_blob(_handle, _name, NULL, &key_size);
  if (res != ESP_OK && res != ESP_ERR_NVS_NOT_FOUND)
  {
    log_e("Unable to read NVS key: %d", res);
    return false;
  }
  if (size < key_size)
  { // truncate
    log_w("truncating EEPROM from %d to %d", key_size, size);
    uint8_t *key_data = (uint8_t *)malloc(key_size);
    if (!key_data)
    {
      log_e("Not enough memory to truncate EEPROM!");
      return false;
    }
    nvs_get_blob(_handle, _name, key_data, &key_size);
    nvs_set_blob(_handle, _name, key_data, size);
    nvs_commit(_handle);
    free(key_data);
  }
  else if (size > key_size)
  { // expand or new
    size_t expand_size = size - key_size;
    uint8_t *expand_key = (uint8_t *)malloc(expand_size);
    if (!expand_key)
    {
      log_e("Not enough memory to expand EEPROM!");
      return false;
    }
    // check for adequate free space
    if (nvs_set_blob(_handle, "expand", expand_key, expand_size))
    {
      log_e("Not enough space to expand EEPROM from %d to %d", key_size, size);
      free(expand_key);
      return false;
    }
    free(expand_key);
    nvs_erase_key(_handle, "expand");
    uint8_t *key_data = (uint8_t *)malloc(size);
    if (!key_data)
    {
      log_e("Not enough memory to expand EEPROM!");
      return false;
    }
    memset(key_data, 0, size);
    if (key_size)
    {
      log_i("Expanding EEPROM from %d to %d", key_size, size);
      // hold data while key is deleted
      nvs_get_blob(_handle, _name, key_data, &key_size);
      nvs_erase_key(_handle, _name);
    }
    else
    {
      log_i("New EEPROM of %d bytes", size);
    }
    nvs_commit(_handle);
    nvs_set_blob(_handle, _name, key_data, size);
    free(key_data);
    nvs_commit(_handle);
  }

  if (_data)
  {
    delete[] _data;
  }

  _data = (uint8_t *)malloc(size);
  if (!_data)
  {
    log_e("Not enough memory for %d bytes in EEPROM");
    return false;
  }
  _size = size;
  nvs_get_blob(_handle, _name, _data, &_size);
  return true;
}

void EEPROMClassESP::end()
{
  if (!_size)
  {
    return;
  }

  commit();
  if (_data)
  {
    delete[] _data;
  }
  _data = 0;
  _size = 0;
}

uint8_t EEPROMClassESP::read(int address)
{
  if (address < 0 || (size_t)address >= _size)
  {
    return 0;
  }
  if (!_data)
  {
    return 0;
  }

  return _data[address];
}

void EEPROMClassESP::write(int address, uint8_t value)
{
  if (address < 0 || (size_t)address >= _size)
    return;
  if (!_data)
    return;

  // Optimise _dirty. Only flagged if data written is different.
  uint8_t *pData = &_data[address];
  if (*pData != value)
  {
    *pData = value;
    _dirty = true;
  }
}

bool EEPROMClassESP::commit()
{
  bool ret = false;
  if (!_size)
  {
    return false;
  }
  if (!_data)
  {
    return false;
  }
  if (!_dirty)
  {
    return true;
  }

  if (ESP_OK != nvs_set_blob(_handle, _name, _data, _size))
  {
    log_e("error in write");
  }
  else
  {
    _dirty = false;
    ret = true;
  }

  return ret;
}

uint8_t *EEPROMClassESP::getDataPtr()
{
  _dirty = true;
  return &_data[0];
}

/*
   Get EEPROM total size in byte defined by the user
*/
uint16_t EEPROMClassESP::length()
{
  return _user_defined_size;
}

/*
   Read 'value' from 'address'
*/
uint8_t EEPROMClassESP::readByte(int address)
{
  uint8_t value = 0;
  return EEPROMClassESP::readAll(address, value);
}

int8_t EEPROMClassESP::readChar(int address)
{
  int8_t value = 0;
  return EEPROMClassESP::readAll(address, value);
}

uint8_t EEPROMClassESP::readUChar(int address)
{
  uint8_t value = 0;
  return EEPROMClassESP::readAll(address, value);
}

int16_t EEPROMClassESP::readShort(int address)
{
  int16_t value = 0;
  return EEPROMClassESP::readAll(address, value);
}

uint16_t EEPROMClassESP::readUShort(int address)
{
  uint16_t value = 0;
  return EEPROMClassESP::readAll(address, value);
}

int32_t EEPROMClassESP::readInt(int address)
{
  int32_t value = 0;
  return EEPROMClassESP::readAll(address, value);
}

uint32_t EEPROMClassESP::readUInt(int address)
{
  uint32_t value = 0;
  return EEPROMClassESP::readAll(address, value);
}

int32_t EEPROMClassESP::readLong(int address)
{
  int32_t value = 0;
  return EEPROMClassESP::readAll(address, value);
}

uint32_t EEPROMClassESP::readULong(int address)
{
  uint32_t value = 0;
  return EEPROMClassESP::readAll(address, value);
}

int64_t EEPROMClassESP::readLong64(int address)
{
  int64_t value = 0;
  return EEPROMClassESP::readAll(address, value);
}

uint64_t EEPROMClassESP::readULong64(int address)
{
  uint64_t value = 0;
  return EEPROMClassESP::readAll(address, value);
}

float_t EEPROMClassESP::readFloat(int address)
{
  float_t value = 0;
  return EEPROMClassESP::readAll(address, value);
}

double_t EEPROMClassESP::readDouble(int address)
{
  double_t value = 0;
  return EEPROMClassESP::readAll(address, value);
}

bool EEPROMClassESP::readBool(int address)
{
  int8_t value = 0;
  return EEPROMClassESP::readAll(address, value) ? 1 : 0;
}

size_t EEPROMClassESP::readString(int address, char *value, size_t maxLen)
{
  if (!value)
    return 0;

  if (address < 0 || address + maxLen > _size)
    return 0;

  uint16_t len;
  for (len = 0; len <= _size; len++)
    if (_data[address + len] == 0)
      break;

  if (address + len > _size)
    return 0;

  memcpy((uint8_t *)value, _data + address, len);
  value[len] = 0;
  return len;
}

String EEPROMClassESP::readString(int address)
{
  if (address < 0 || address > _size)
    return String();

  uint16_t len;
  for (len = 0; len <= _size; len++)
    if (_data[address + len] == 0)
      break;

  if (address + len > _size)
    return String();

  char value[len];
  memcpy((uint8_t *)value, _data + address, len);
  value[len] = 0;
  return String(value);
}

size_t EEPROMClassESP::readBytes(int address, void *value, size_t maxLen)
{
  if (!value || !maxLen)
    return 0;

  if (address < 0 || address + maxLen > _size)
    return 0;

  memcpy((void *)value, _data + address, maxLen);
  return maxLen;
}

template <class T>
T EEPROMClassESP::readAll(int address, T &value)
{
  if (address < 0 || address + sizeof(T) > _size)
    return value;

  memcpy((uint8_t *)&value, _data + address, sizeof(T));
  return value;
}

/*
   Write 'value' to 'address'
*/
size_t EEPROMClassESP::writeByte(int address, uint8_t value)
{
  return EEPROMClassESP::writeAll(address, value);
}

size_t EEPROMClassESP::writeChar(int address, int8_t value)
{
  return EEPROMClassESP::writeAll(address, value);
}

size_t EEPROMClassESP::writeUChar(int address, uint8_t value)
{
  return EEPROMClassESP::writeAll(address, value);
}

size_t EEPROMClassESP::writeShort(int address, int16_t value)
{
  return EEPROMClassESP::writeAll(address, value);
}

size_t EEPROMClassESP::writeUShort(int address, uint16_t value)
{
  return EEPROMClassESP::writeAll(address, value);
}

size_t EEPROMClassESP::writeInt(int address, int32_t value)
{
  return EEPROMClassESP::writeAll(address, value);
}

size_t EEPROMClassESP::writeUInt(int address, uint32_t value)
{
  return EEPROMClassESP::writeAll(address, value);
}

size_t EEPROMClassESP::writeLong(int address, int32_t value)
{
  return EEPROMClassESP::writeAll(address, value);
}

size_t EEPROMClassESP::writeULong(int address, uint32_t value)
{
  return EEPROMClassESP::writeAll(address, value);
}

size_t EEPROMClassESP::writeLong64(int address, int64_t value)
{
  return EEPROMClassESP::writeAll(address, value);
}

size_t EEPROMClassESP::writeULong64(int address, uint64_t value)
{
  return EEPROMClassESP::writeAll(address, value);
}

size_t EEPROMClassESP::writeFloat(int address, float_t value)
{
  return EEPROMClassESP::writeAll(address, value);
}

size_t EEPROMClassESP::writeDouble(int address, double_t value)
{
  return EEPROMClassESP::writeAll(address, value);
}

size_t EEPROMClassESP::writeBool(int address, bool value)
{
  int8_t Bool;
  value ? Bool = 1 : Bool = 0;
  return EEPROMClassESP::writeAll(address, Bool);
}

size_t EEPROMClassESP::writeString(int address, const char *value)
{
  if (!value)
    return 0;

  if (address < 0 || address > _size)
    return 0;

  uint16_t len;
  for (len = 0; len <= _size; len++)
    if (value[len] == 0)
      break;

  if (address + len > _size)
    return 0;

  memcpy(_data + address, (const uint8_t *)value, len + 1);
  _dirty = true;
  return strlen(value);
}

size_t EEPROMClassESP::writeString(int address, String value)
{
  return EEPROMClassESP::writeString(address, value.c_str());
}

size_t EEPROMClassESP::writeBytes(int address, const void *value, size_t len)
{
  if (!value || !len)
    return 0;

  if (address < 0 || address + len > _size)
    return 0;

  memcpy(_data + address, (const void *)value, len);
  _dirty = true;
  return len;
}

template <class T>
T EEPROMClassESP::writeAll(int address, const T &value)
{
  if (address < 0 || address + sizeof(T) > _size)
    return value;

  memcpy(_data + address, (const uint8_t *)&value, sizeof(T));
  _dirty = true;

  return sizeof(value);
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
EEPROMClassESP EEPROM_ESP;
#endif

#endif