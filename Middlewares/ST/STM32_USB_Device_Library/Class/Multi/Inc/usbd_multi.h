/*
 * usbd_multi.h
 *
 *  Created on: Feb 15, 2025
 *      Author: nikla
 */

#ifndef ST_STM32_USB_DEVICE_LIBRARY_CLASS_MULTI_INC_USBD_MULTI_H_
#define ST_STM32_USB_DEVICE_LIBRARY_CLASS_MULTI_INC_USBD_MULTI_H_

#include  "usbd_ioreq.h"
#include "usbd_cdc.h"
#include "usbd_hid.h"

#define USB_CDC_CLASSID 1
#define USB_HID_CLASSID 2

#define USBD_MULTI_CfgDesc_BaseLength 9U

extern USBD_ClassTypeDef USBD_MULTI;

#endif /* ST_STM32_USB_DEVICE_LIBRARY_CLASS_MULTI_INC_USBD_MULTI_H_ */
