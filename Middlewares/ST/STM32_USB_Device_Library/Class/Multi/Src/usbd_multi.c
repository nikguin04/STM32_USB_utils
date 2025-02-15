/*
 * usbd_multi.c
 *
 *  Created on: Feb 15, 2025
 *      Author: nikla
 */

#include "usbd_multi.h"
#include "usbd_ctlreq.h"
#include "usbd_cdc.h"
#include "usbd_hid.h"

USBD_ClassTypeDef  USBD_MULTI =
{
  NULL,//USBD_MULTI_Init,
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


static uint8_t USBD_MULTI_CfgDesc[USB_CDC_CONFIG_DESC_SIZE + USB_HID_CONFIG_DESC_SIZE];

uint16_t cfgIndex = 0;
memcpy(USBD_MULTI_CfgDesc[cfgIndex], USBD_CDC_CfgDesc, USB_CDC_CONFIG_DESC_SIZE);
cfgIndex = cfgIndex + USB_CDC_CONFIG_DESC_SIZE;
memcpy(USBD_MULTI_CfgDesc[cfgIndex], USBD_HID_CfgDesc, USB_HID_CONFIG_DESC_SIZE);
cfgIndex = cfgIndex + USB_HID_CONFIG_DESC_SIZE;

/**
  * @brief  USBD_MULTI_Init
  *         Initialize the MULTI interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t USBD_MULTI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  UNUSED(cfgidx);
  USBD_CDC_HandleTypeDef *hcdc;

  hcdc = (USBD_CDC_HandleTypeDef *)USBD_malloc_CDC(sizeof(USBD_CDC_HandleTypeDef));

  if (hcdc == NULL)
  {
    pdev->pClassDataCmsit[pdev->classId] = NULL;
    return (uint8_t)USBD_EMEM;
  }

  (void)USBD_memset(hcdc, 0, sizeof(USBD_CDC_HandleTypeDef));

  pdev->pClassDataCmsit[pdev->classId] = (void *)hcdc;
  pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];

#ifdef USE_USBD_COMPOSITE
  /* Get the Endpoints addresses allocated for this class instance */
  CDCInEpAdd  = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  CDCOutEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_OUT, USBD_EP_TYPE_BULK, (uint8_t)pdev->classId);
  CDCCmdEpAdd = USBD_CoreGetEPAdd(pdev, USBD_EP_IN, USBD_EP_TYPE_INTR, (uint8_t)pdev->classId);
#endif /* USE_USBD_COMPOSITE */

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Open EP IN */
    (void)USBD_LL_OpenEP(pdev, CDCInEpAdd, USBD_EP_TYPE_BULK,
                         CDC_DATA_HS_IN_PACKET_SIZE);

    pdev->ep_in[CDCInEpAdd & 0xFU].is_used = 1U;

    /* Open EP OUT */
    (void)USBD_LL_OpenEP(pdev, CDCOutEpAdd, USBD_EP_TYPE_BULK,
                         CDC_DATA_HS_OUT_PACKET_SIZE);

    pdev->ep_out[CDCOutEpAdd & 0xFU].is_used = 1U;

    /* Set bInterval for CDC CMD Endpoint */
    pdev->ep_in[CDCCmdEpAdd & 0xFU].bInterval = CDC_HS_BINTERVAL;
  }
  else
  {
    /* Open EP IN */
    (void)USBD_LL_OpenEP(pdev, CDCInEpAdd, USBD_EP_TYPE_BULK,
                         CDC_DATA_FS_IN_PACKET_SIZE);

    pdev->ep_in[CDCInEpAdd & 0xFU].is_used = 1U;

    /* Open EP OUT */
    (void)USBD_LL_OpenEP(pdev, CDCOutEpAdd, USBD_EP_TYPE_BULK,
                         CDC_DATA_FS_OUT_PACKET_SIZE);

    pdev->ep_out[CDCOutEpAdd & 0xFU].is_used = 1U;

    /* Set bInterval for CMD Endpoint */
    pdev->ep_in[CDCCmdEpAdd & 0xFU].bInterval = CDC_FS_BINTERVAL;
  }

  /* Open Command IN EP */
  (void)USBD_LL_OpenEP(pdev, CDCCmdEpAdd, USBD_EP_TYPE_INTR, CDC_CMD_PACKET_SIZE);
  pdev->ep_in[CDCCmdEpAdd & 0xFU].is_used = 1U;

  hcdc->RxBuffer = NULL;

  /* Init  physical Interface components */
  ((USBD_CDC_ItfTypeDef *)pdev->pUserData[pdev->classId])->Init();

  /* Init Xfer states */
  hcdc->TxState = 0U;
  hcdc->RxState = 0U;

  if (hcdc->RxBuffer == NULL)
  {
    return (uint8_t)USBD_EMEM;
  }

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Prepare Out endpoint to receive next packet */
    (void)USBD_LL_PrepareReceive(pdev, CDCOutEpAdd, hcdc->RxBuffer,
                                 CDC_DATA_HS_OUT_PACKET_SIZE);
  }
  else
  {
    /* Prepare Out endpoint to receive next packet */
    (void)USBD_LL_PrepareReceive(pdev, CDCOutEpAdd, hcdc->RxBuffer,
                                 CDC_DATA_FS_OUT_PACKET_SIZE);
  }

  return (uint8_t)USBD_OK;
}

/**
  * @brief  USBD_MULTI_GetFSCfgDesc
  *         Return configuration descriptor
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t *USBD_MULTI_GetFSCfgDesc(uint16_t *length)
{
  uint16_t dummy_len = 0;
  USBD_CDC_GetFSCfgDesc(&dummy_len);
  USBD_HID_GetFSCfgDesc(&dummy_len);

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
  uint16_t dummy_len = 0;
  USBD_CDC_GetHSCfgDesc(&dummy_len);
  USBD_HID_GetHSCfgDesc(&dummy_len);

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
  uint16_t dummy_len = 0;
  USBD_CDC_GetOtherSpeedCfgDesc(&dummy_len);
  USBD_HID_GetOtherSpeedCfgDesc(&dummy_len);

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
