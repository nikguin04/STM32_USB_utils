/*
 * usbd_multi.c
 *
 *  Created on: Feb 21, 2025
 *      Author: nikla
 */


#include "usbd_multi.h"


static uint8_t USBD_MULTI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_MULTI_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_MULTI_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_MULTI_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_MULTI_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_MULTI_EP0_RxReady(USBD_HandleTypeDef *pdev);

static uint8_t *USBD_MULTI_GetFSCfgDesc(uint16_t *length);
static uint8_t *USBD_MULTI_GetHSCfgDesc(uint16_t *length);
static uint8_t *USBD_MULTI_GetOtherSpeedCfgDesc(uint16_t *length);
uint8_t *USBD_MULTI_GetDeviceQualifierDescriptor(uint16_t *length);


USBD_ClassTypeDef  USBD_MULTI =
{
  USBD_MULTI_Init,
  USBD_MULTI_DeInit,
  USBD_MULTI_Setup,
  NULL,                 /* EP0_TxSent */
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
#endif /* USE_USBD_COMPOSITE  */
};

static uint8_t USBD_MULTI_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00, // bcdUSB
  0x02,
  0x00, // Device class
  0x00, // Device subclass
  0x00, // Device protocol
  0x40, // Packet size
  0x01, // Num of configs
  0x00, // Reserved
};

__ALIGN_BEGIN static uint8_t USBD_MULTI_CfgDesc[USB_MULTI_CONFIG_DESC_SIZ] __ALIGN_END =
{
  /* Configuration Descriptor */
  0x09,                                       /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,                /* bDescriptorType: Configuration */
  USB_MULTI_CONFIG_DESC_SIZ,                  /* wTotalLength */
  0x00,
  0x03,                                       /* bNumInterfaces: 2 interfaces */
  0x01,                                       /* bConfigurationValue: Configuration value */
  0x00,                                       /* iConfiguration: Index of string descriptor
                                                 describing the configuration */
#if (USBD_SELF_POWERED == 1U)
  0xC0,                                       /* bmAttributes: Bus Powered according to user configuration */
#else
  0x80,                                       /* bmAttributes: Bus Powered according to user configuration */
#endif /* USBD_SELF_POWERED */
  USBD_MAX_POWER,                             /* MaxPower (mA) */



  	  // START OF KEYBOARD DESCRIPTOR!! -----------------------------------------------
	/************** Descriptor of Joystick Mouse interface ****************/
	/* 09 */
	0x09,                                               /* bLength: Interface Descriptor size */
	USB_DESC_TYPE_INTERFACE,                            /* bDescriptorType: Interface descriptor type */
	HID_INTERFACE,                                      /* bInterfaceNumber: Number of Interface */
	0x00,                                               /* bAlternateSetting: Alternate setting */
	0x01,                                               /* bNumEndpoints */
	0x03,                                               /* bInterfaceClass: HID */
	0x01,                                               /* bInterfaceSubClass : 1=BOOT, 0=no boot */
	0x01,                                               /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
	0x00,                                               /* iInterface: Index of string descriptor */
	/******************** Descriptor of Joystick Mouse HID ********************/
	/* 18 */
	0x09,                                               /* bLength: HID Descriptor size */
	HID_DESCRIPTOR_TYPE,                                /* bDescriptorType: HID */
	0x11,                                               /* bcdHID: HID Class Spec release number */
	0x01,
	0x00,                                               /* bCountryCode: Hardware target country */
	0x01,                                               /* bNumDescriptors: Number of HID class descriptors to follow */
	0x22,                                               /* bDescriptorType */
	HID_KEYBOARD_REPORT_DESC_SIZE,                         /* wItemLength: Total length of Report descriptor */
	0x00,
	/******************** Descriptor of Mouse endpoint ********************/
	/* 27 */
	0x07,                                               /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType:*/

	HID_EPIN_ADDR,                                      /* bEndpointAddress: Endpoint Address (IN) */
	0x03,                                               /* bmAttributes: Interrupt endpoint */
	LOBYTE(HID_EPIN_SIZE),                                      /* wMaxPacketSize: 4 Bytes max */
	HIBYTE(HID_EPIN_SIZE),
	HID_FS_BINTERVAL,                                   /* bInterval: Polling Interval */
	/* 34 */
  /*---------------------------------------------------------------------------*/

	// START OF CDC DESCRIPTOR!! -----------------------------------------------
  // Interface association descriptor
  0x08,                                       /* bLength */
  0x0B,                                       /* bDescriptorType */
  CDC_CIC_INTERFACE,                          /* bFirstInterfaceNumber */
  0x02,                                       /* bInterfaceCount */
  0x02,                                       /* bFunctionClass: CDC */
  0x02,                                       /* bFunctionSubClass */
  0x01,                                       /* bFunctionProtocol */
  0x00,                                       /* iFunction */


  /* Interface Descriptor */
  0x09,                                       /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,                    /* bDescriptorType: Interface */
  /* Interface descriptor type */
  CDC_CIC_INTERFACE,                          /* bInterfaceNumber: Number of Interface */
  0x00,                                       /* bAlternateSetting: Alternate setting */
  0x01,                                       /* bNumEndpoints: One endpoint used */
  0x02,                                       /* bInterfaceClass: Communication Interface Class */
  0x02,                                       /* bInterfaceSubClass: Abstract Control Model */
  0x01,                                       /* bInterfaceProtocol: Common AT commands */
  0x00,                                       /* iInterface */

  /* Header Functional Descriptor */
  0x05,                                       /* bLength: Endpoint Descriptor size */
  0x24,                                       /* bDescriptorType: CS_INTERFACE */
  0x00,                                       /* bDescriptorSubtype: Header Func Desc */
  0x10,                                       /* bcdCDC: spec release number */
  0x01,

  /* Call Management Functional Descriptor */
  0x05,                                       /* bFunctionLength */
  0x24,                                       /* bDescriptorType: CS_INTERFACE */
  0x01,                                       /* bDescriptorSubtype: Call Management Func Desc */
  0x00,                                       /* bmCapabilities: D0+D1 */
  0x01,                                       /* bDataInterface */

  /* ACM Functional Descriptor */
  0x04,                                       /* bFunctionLength */
  0x24,                                       /* bDescriptorType: CS_INTERFACE */
  0x02,                                       /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,                                       /* bmCapabilities */

  /* Union Functional Descriptor */
  0x05,                                       /* bFunctionLength */
  0x24,                                       /* bDescriptorType: CS_INTERFACE */
  0x06,                                       /* bDescriptorSubtype: Union func desc */
  0x00,                                       /* bMasterInterface: Communication class interface */
  0x01,                                       /* bSlaveInterface0: Data Class Interface */

  /* Endpoint 2 Descriptor */
  0x07,                                       /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,                     /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                                 /* bEndpointAddress */
  0x03,                                       /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SIZE),                /* wMaxPacketSize */
  HIBYTE(CDC_CMD_PACKET_SIZE),
  CDC_FS_BINTERVAL,                           /* bInterval */
  /*---------------------------------------------------------------------------*/

  /* Data class interface descriptor */
  0x09,                                       /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_INTERFACE,                    /* bDescriptorType: */
  CDC_INTERFACE,                              /* bInterfaceNumber: Number of Interface */
  0x00,                                       /* bAlternateSetting: Alternate setting */
  0x02,                                       /* bNumEndpoints: Two endpoints used */
  0x0A,                                       /* bInterfaceClass: CDC */
  0x00,                                       /* bInterfaceSubClass */
  0x00,                                       /* bInterfaceProtocol */
  0x00,                                       /* iInterface */

  /* Endpoint OUT Descriptor */
  0x07,                                       /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,                     /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                                 /* bEndpointAddress */
  0x02,                                       /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),        /* wMaxPacketSize */
  HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
  0x00,                                       /* bInterval */

  /* Endpoint IN Descriptor */
  0x07,                                       /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,                     /* bDescriptorType: Endpoint */
  CDC_IN_EP,                                  /* bEndpointAddress */
  0x02,                                       /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),        /* wMaxPacketSize */
  HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
  0x00,                                        /* bInterval */



};


static uint8_t USBD_MULTI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {

	pdev->classId = HID_CLASSID;
	USBD_HID.Init(pdev, cfgidx);

	pdev->classId = CDC_CLASSID;
	if (USBD_CDC_RegisterInterface(pdev, &USBD_Interface_fops_FS) != USBD_OK) {
	  Error_Handler();
	}
	USBD_CDC.Init(pdev, cfgidx);


	pdev->classId = MULTI_CLASSID;
	USBD_MULTI_HandleTypeDef *h_multi;
	h_multi = (USBD_MULTI_HandleTypeDef *)USBD_malloc_MULTI(sizeof(USBD_MULTI_HandleTypeDef));
	if (h_multi == NULL)
	{
		pdev->pClassDataCmsit[pdev->classId] = NULL;
		return (uint8_t)USBD_EMEM;
	}
	(void)USBD_memset(h_multi, 0, sizeof(USBD_MULTI_HandleTypeDef));


	pdev->pClassDataCmsit[pdev->classId] = (void *)h_multi;
	//pdev->pClassData = pdev->pClassDataCmsit[pdev->classId];
	return (uint8_t)USBD_OK;
}

static uint8_t USBD_MULTI_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx) {

	pdev->classId = HID_CLASSID;
	USBD_HID.DeInit(pdev, cfgidx);

	pdev->classId = CDC_CLASSID;
	USBD_CDC.DeInit(pdev, cfgidx);

	pdev->classId = MULTI_CLASSID;
	if (pdev->pClassDataCmsit[pdev->classId] != NULL)
	{
		(void)USBD_free(pdev->pClassDataCmsit[pdev->classId]);
		pdev->pClassDataCmsit[pdev->classId] = NULL;
	}

	return (uint8_t)USBD_OK;
}

static uint8_t USBD_MULTI_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
	// Please note that this might require passing based on interfaces and endpoints in bmRequestType (see 9.3 USB Device Requests of "Universal Serial Bus Specification Revision 1.1")

	if ((req->bmRequest & 0b00011111) == 0b00001) { // Recipient == Interface
		switch (req->wIndex) {
			case HID_INTERFACE:
				pdev->classId = HID_CLASSID;
				return USBD_HID.Setup(pdev, req);

			case CDC_CIC_INTERFACE:
			case CDC_INTERFACE:
				pdev->classId = CDC_CLASSID;
				return USBD_CDC.Setup(pdev, req);

			default:
				return USBD_FAIL;

		}
	} else
	if ((req->bmRequest & 0b00011111) == 0b00010) { // Recipient == Endpoint
		switch (req->wIndex) {
			case CDC_IN_EP:
			case CDC_CMD_EP:
			case CDC_OUT_EP:
				pdev->classId = CDC_CLASSID;
				return USBD_CDC.Setup(pdev, req);

			case HID_EPIN_ADDR:
				pdev->classId = HID_CLASSID;
				return USBD_HID.Setup(pdev, req);

			default:
				return USBD_FAIL;
		}
	} else {
		printf("ERROR MULTI SETUP");
		return USBD_FAIL;
	}
}

static uint8_t USBD_MULTI_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum) {
	int i = 0;
	i++;
	switch (epnum+0x80) {
		case CDC_IN_EP:
		case CDC_CMD_EP:
			pdev->classId = CDC_CLASSID;
			return USBD_CDC.DataIn(pdev, epnum);
		case HID_EPIN_ADDR:
			pdev->classId = HID_CLASSID;
			return USBD_HID.DataIn(pdev, epnum);

		default:
			return USBD_FAIL;
	}
}

static uint8_t USBD_MULTI_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum) {
	switch (epnum) {
			case CDC_OUT_EP:
				pdev->classId = CDC_CLASSID;
				return USBD_CDC.DataOut(pdev, epnum);
			default:
				return USBD_FAIL;
		}
}


static uint8_t USBD_MULTI_EP0_RxReady(USBD_HandleTypeDef *pdev) {

	// Only CDC supports the RX ready
	//pdev->classId = CDC_CLASSID;
	if (pdev->classId == CDC_CLASSID) {
		return USBD_CDC.EP0_RxReady(pdev);
	} else {
		return 0;
	}
}

static uint8_t *USBD_MULTI_GetFSCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pEpDesc = USBD_GetEpDesc(USBD_MULTI_CfgDesc, HID_EPIN_ADDR);

  if (pEpDesc != NULL)
  {
    pEpDesc->bInterval = HID_FS_BINTERVAL;
  }

	USBD_EpDescTypeDef *pEpCmdDesc = USBD_GetEpDesc(USBD_MULTI_CfgDesc, CDC_CMD_EP);
	USBD_EpDescTypeDef *pEpOutDesc = USBD_GetEpDesc(USBD_MULTI_CfgDesc, CDC_OUT_EP);
	USBD_EpDescTypeDef *pEpInDesc = USBD_GetEpDesc(USBD_MULTI_CfgDesc, CDC_IN_EP);

	if (pEpCmdDesc != NULL)
	{
	pEpCmdDesc->bInterval = CDC_FS_BINTERVAL;
	}

	if (pEpOutDesc != NULL)
	{
	pEpOutDesc->wMaxPacketSize = CDC_DATA_FS_MAX_PACKET_SIZE;
	}

	if (pEpInDesc != NULL)
	{
	pEpInDesc->wMaxPacketSize = CDC_DATA_FS_MAX_PACKET_SIZE;
	}


  *length = (uint16_t)sizeof(USBD_MULTI_CfgDesc);
  return USBD_MULTI_CfgDesc;
}



static uint8_t *USBD_MULTI_GetHSCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pEpDesc = USBD_GetEpDesc(USBD_MULTI_CfgDesc, HID_EPIN_ADDR);

  if (pEpDesc != NULL)
  {
    pEpDesc->bInterval = HID_HS_BINTERVAL;
  }

    USBD_EpDescTypeDef *pEpCmdDesc = USBD_GetEpDesc(USBD_MULTI_CfgDesc, CDC_CMD_EP);
    USBD_EpDescTypeDef *pEpOutDesc = USBD_GetEpDesc(USBD_MULTI_CfgDesc, CDC_OUT_EP);
    USBD_EpDescTypeDef *pEpInDesc = USBD_GetEpDesc(USBD_MULTI_CfgDesc, CDC_IN_EP);

    if (pEpCmdDesc != NULL)
    {
      pEpCmdDesc->bInterval = CDC_HS_BINTERVAL;
    }

    if (pEpOutDesc != NULL)
    {
      pEpOutDesc->wMaxPacketSize = CDC_DATA_HS_MAX_PACKET_SIZE;
    }

    if (pEpInDesc != NULL)
    {
      pEpInDesc->wMaxPacketSize = CDC_DATA_HS_MAX_PACKET_SIZE;
    }

  *length = (uint16_t)sizeof(USBD_MULTI_CfgDesc);
  return USBD_MULTI_CfgDesc;
}

static uint8_t *USBD_MULTI_GetOtherSpeedCfgDesc(uint16_t *length)
{
  USBD_EpDescTypeDef *pEpDesc = USBD_GetEpDesc(USBD_MULTI_CfgDesc, HID_EPIN_ADDR);

  if (pEpDesc != NULL)
  {
    pEpDesc->bInterval = HID_FS_BINTERVAL;
  }

    USBD_EpDescTypeDef *pEpCmdDesc = USBD_GetEpDesc(USBD_MULTI_CfgDesc, CDC_CMD_EP);
	USBD_EpDescTypeDef *pEpOutDesc = USBD_GetEpDesc(USBD_MULTI_CfgDesc, CDC_OUT_EP);
	USBD_EpDescTypeDef *pEpInDesc = USBD_GetEpDesc(USBD_MULTI_CfgDesc, CDC_IN_EP);

	if (pEpCmdDesc != NULL)
	{
	  pEpCmdDesc->bInterval = CDC_FS_BINTERVAL;
	}

	if (pEpOutDesc != NULL)
	{
	  pEpOutDesc->wMaxPacketSize = CDC_DATA_FS_MAX_PACKET_SIZE;
	}

	if (pEpInDesc != NULL)
	{
	  pEpInDesc->wMaxPacketSize = CDC_DATA_FS_MAX_PACKET_SIZE;
	}

  *length = (uint16_t)sizeof(USBD_MULTI_CfgDesc);
  return USBD_MULTI_CfgDesc;
}

uint8_t *USBD_MULTI_GetDeviceQualifierDescriptor(uint16_t *length)
{
  *length = (uint16_t)sizeof(USBD_MULTI_DeviceQualifierDesc);

  return USBD_MULTI_DeviceQualifierDesc;
}
