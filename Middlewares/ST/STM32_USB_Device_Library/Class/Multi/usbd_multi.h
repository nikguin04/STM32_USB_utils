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

#define USB_MULTI_CONFIG_DESC_SIZ (67U+25U)

extern USBD_ClassTypeDef USBD_MULTI;
#define USBD_MULTI_CLASS &USBD_MULTI;

#endif /* ST_STM32_USB_DEVICE_LIBRARY_CLASS_MULTI_USBD_MULTI_H_ */
