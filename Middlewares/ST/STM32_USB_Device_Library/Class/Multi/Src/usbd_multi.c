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
  USBD_MULTI_DeInit,
  USBD_MULTI_Setup,
  NULL,                 // EP0_TxSent
  USBD_MULTI_EP0_RxReady,
  USBD_MULTI_DataIn,
  USBD_MULTI_DataOut,
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
  pdev->classId = USB_HID_CLASSID; // HID INIT
  USBD_HID.Init(pdev, cfgidx);
  pdev->classId = USB_CDC_CLASSID; // CDC INIT
  USBD_CDC.Init(pdev, cfgidx);

  uint32_t *hhid;
  hhid = (uint32_t *)USBD_malloc_MULTI(sizeof(uint32_t));
  pdev->pClassDataCmsit[pdev->classId] = (void *)hhid;
  pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];
  *hhid = 1;

  return USBD_OK;
}
static uint8_t USBD_MULTI_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {
	pdev->classId = USB_CDC_CLASSID; // CDC DEINIT
	USBD_CDC.DeInit(pdev, cfgidx);
	pdev->classId = USB_HID_CLASSID; // HID DEINIT
	USBD_HID.DeInit(pdev, cfgidx);
	return USBD_OK;
}
static uint8_t USBD_MULTI_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
	pdev->classId = USB_CDC_CLASSID; // CDC INIT
	USBD_CDC.Setup(pdev, req);
	pdev->classId = USB_HID_CLASSID; // HID INIT
	USBD_HID.Setup(pdev, req);
	return USBD_OK;
}
static uint8_t USBD_MULTI_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum) {
	// Poorly hardcoded stub
	if (epnum == HID_EPIN_ADDR) {
		pdev->classId = USB_HID_CLASSID;
		USBD_HID.DataIn(pdev, epnum);
	} else {
		pdev->classId = USB_CDC_CLASSID;
		USBD_CDC.DataIn(pdev, epnum);
	}

	return USBD_OK;
}
static uint8_t USBD_MULTI_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum) {
	// No data out for HID, so again, poorly hardcoded
	pdev->classId = USB_CDC_CLASSID;
	USBD_CDC.DataOut(pdev, epnum);
	return USBD_OK;
}
static uint8_t USBD_MULTI_EP0_RxReady(USBD_HandleTypeDef *pdev) {
	// No data out for HID, so again, poorly hardcoded
	pdev->classId = USB_CDC_CLASSID;
	USBD_CDC.EP0_RxReady(pdev);
	return USBD_OK;
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


uint8_t USBD_MULTI_CfgDesc[USBD_MULTI_CfgDesc_BaseLength + USB_CDC_CONFIG_DESC_SIZ + USB_HID_CONFIG_DESC_SIZ] =
{
	0x09,                              /* bLength: Configuration Descriptor size */
	USB_DESC_TYPE_CONFIGURATION,       /* bDescriptorType: Configuration */
	0x00,                              /* wTotalLength: Bytes returned (placeholder) */
	0x00,
	0x03,                              /* bNumInterfaces: 3 interfaces */
	0x01,                              /* bConfigurationValue: Configuration value */
	0x00,                              /* iConfiguration: Index of string descriptor
										describing the configuration */
	#if (USBD_SELF_POWERED == 1U)
		0xE0,                          /* bmAttributes: Bus Powered according to user configuration */
	#else
		0xA0,                          /* bmAttributes: Bus Powered according to user configuration */
	#endif /* USBD_SELF_POWERED */
		USBD_MAX_POWER,                /* MaxPower (mA) */
};

uint16_t cfgIndex = USBD_MULTI_CfgDesc_BaseLength;
static void initCfgDesc() {
	uint16_t dummy_len = 0;
	memcpy(USBD_MULTI_CfgDesc + cfgIndex, USBD_CDC.GetFSConfigDescriptor(&dummy_len), USB_CDC_CONFIG_DESC_SIZ);
	cfgIndex = cfgIndex + USB_CDC_CONFIG_DESC_SIZ;
	memcpy(USBD_MULTI_CfgDesc + cfgIndex, USBD_HID.GetFSConfigDescriptor(&dummy_len), USB_HID_CONFIG_DESC_SIZ);
	cfgIndex = cfgIndex + USB_HID_CONFIG_DESC_SIZ;

	USBD_MULTI_CfgDesc[2] = cfgIndex;// edit length from index
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
