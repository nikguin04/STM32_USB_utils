/*
 * usbd_multi.h
 *
 *  Created on: Feb 21, 2025
 *      Author: nikla
 */

#ifndef ST_STM32_USB_DEVICE_LIBRARY_CLASS_MULTI_USBD_MULTI_H_
#define ST_STM32_USB_DEVICE_LIBRARY_CLASS_MULTI_USBD_MULTI_H_

#include "usbd_def.h"

#include "usbd_hid.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

#define USB_MULTI_CONFIG_DESC_SIZ (67U+34U-9U+8U)

extern USBD_ClassTypeDef USBD_MULTI;
#define USBD_MULTI_CLASS &USBD_MULTI;

#define MULTI_CLASSID 0x00


typedef struct
{
  uint32_t dummydata;
} USBD_MULTI_HandleTypeDef;

#endif /* ST_STM32_USB_DEVICE_LIBRARY_CLASS_MULTI_USBD_MULTI_H_ */
