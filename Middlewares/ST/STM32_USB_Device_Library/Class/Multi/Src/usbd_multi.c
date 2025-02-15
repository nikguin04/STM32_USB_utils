/*
 * usbd_multi.c
 *
 *  Created on: Feb 15, 2025
 *      Author: nikla
 */

#include "usbd_multi.h"
#include "usbd_ctlreq.h"


static uint8_t USBD_MULTI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_MULTI_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_MULTI_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_MULTI_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_MULTI_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_MULTI_EP0_RxReady(USBD_HandleTypeDef *pdev);
#ifndef USE_USBD_COMPOSITE
static uint8_t *USBD_MULTI_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_MULTI_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_MULTI_GetOtherSpeedCfgDesc(uint16_t *length);
uint8_t *USBD_MULTI_GetDeviceQualifierDescriptor(uint16_t *length);
#endif /* USE_USBD_COMPOSITE  */


USBD_ClassTypeDef  USBD_MULTI =
{
  USBD_MULTI_Init,
  NULL,//USBD_MULTI_DeInit,
  NULL,//USBD_MULTI_Setup,
  NULL,                 // EP0_TxSent
  NULL,//USBD_MULTI_EP0_RxReady,
  NULL,//USBD_MULTI_DataIn,
  NULL,//USBD_MULTI_DataOut,
  NULL,
  NULL,
  NULL,
#ifdef USE_USBD_COMPOSITE
  NULL,
  NULL,
  NULL,
  NULL,
#else
  USBD_MULTI_GetHSCfgDesc,
  USBD_MULTI_GetFSCfgDesc,
  USBD_MULTI_GetOtherSpeedCfgDesc,
  USBD_MULTI_GetDeviceQualifierDescriptor,
#endif // USE_USBD_COMPOSITE
};

static uint8_t USBD_MULTI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  pdev->classId = USB_CDC_CLASSID; // CDC INIT
  USBD_CDC.Init(pdev, cfgidx);
  pdev->classId = USB_HID_CLASSID; // HID INIT
  USBD_HID.Init(pdev, cfgidx);

}

static uint8_t USBD_MULTI_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00, // usb release
  0x02, // usb release
  0x00, // class
  0x00, // subclass
  0x00, // protocol
  0x40, // Max packet size for other config
  0x01, // Number of other speed configs
  0x00, // Reserved
};


uint8_t USBD_MULTI_CfgDesc[USB_CDC_CONFIG_DESC_SIZ + USB_HID_CONFIG_DESC_SIZ];

uint16_t cfgIndex = 0;
static void initCfgDesc() {
	uint16_t dummy_len = 0;
	memcpy(USBD_MULTI_CfgDesc + cfgIndex, USBD_CDC.GetFSConfigDescriptor(&dummy_len), USB_CDC_CONFIG_DESC_SIZ);
	cfgIndex = cfgIndex + USB_CDC_CONFIG_DESC_SIZ;
	memcpy(USBD_MULTI_CfgDesc + cfgIndex, USBD_HID.GetFSConfigDescriptor(&dummy_len), USB_HID_CONFIG_DESC_SIZ);
	cfgIndex = cfgIndex + USB_HID_CONFIG_DESC_SIZ;
}


/**
  * @brief  USBD_MULTI_GetFSCfgDesc
  *         Return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_MULTI_GetFSCfgDesc(uint16_t *length)
{
  initCfgDesc();
  uint16_t dummy_len = 0;
  USBD_CDC.GetFSConfigDescriptor(&dummy_len);
  USBD_HID.GetFSConfigDescriptor(&dummy_len);

  *length = cfgIndex;
  return USBD_MULTI_CfgDesc;
}

/**
  * @brief  USBD_MULTI_GetHSCfgDesc
  *         Return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_MULTI_GetHSCfgDesc(uint16_t *length)
{
  initCfgDesc();
  uint16_t dummy_len = 0;
  USBD_CDC.GetFSConfigDescriptor(&dummy_len);
  USBD_HID.GetFSConfigDescriptor(&dummy_len);

  *length = cfgIndex;
  return USBD_MULTI_CfgDesc;
}

/**
  * @brief  USBD_MULTI_GetOtherSpeedCfgDesc
  *         Return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_MULTI_GetOtherSpeedCfgDesc(uint16_t *length)
{
  initCfgDesc();
  uint16_t dummy_len = 0;
  USBD_CDC.GetFSConfigDescriptor(&dummy_len);
  USBD_HID.GetFSConfigDescriptor(&dummy_len);

  *length = cfgIndex;
  return USBD_MULTI_CfgDesc;
}

/**
  * @brief  USBD_CDC_GetDeviceQualifierDescriptor
  *         return Device Qualifier descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
uint8_t *USBD_MULTI_GetDeviceQualifierDescriptor(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_MULTI_DeviceQualifierDesc);

  return USBD_MULTI_DeviceQualifierDesc;
}
