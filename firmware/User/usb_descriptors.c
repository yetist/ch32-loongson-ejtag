// SPDX-License-Identifier: GPL-3.0-only

#include "usb_descriptors.h"
#include <usbd_core.h>
#include <assert.h>

//
// USB Descriptors =============================================================
//

// Loongson EJTAG USB PID/VID
#define USB_VID   0x2961
#define USB_PID   0x6688
#define USB_BCD   0x0200

#define USB_SIZEOF_VENDOR_DESC (USB_SIZEOF_INTERFACE_DESC+USB_SIZEOF_ENDPOINT_DESC*2)
#define USB_SIZEOF_CONF_DESC (USB_SIZEOF_CONFIG_DESC + USB_SIZEOF_VENDOR_DESC)// + CDC_ACM_DESCRIPTOR_LEN)

static const uint8_t
    desc_device[] = {
        USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xFF, 0x00, 0x00, USB_VID, USB_PID, USB_2_0, 1)
    },

    desc_configuration_hs[] = {
        USB_CONFIG_DESCRIPTOR_INIT(
            USB_SIZEOF_CONF_DESC,
            ITF_NUM_TOTAL,
            1,
            USB_CONFIG_REMOTE_WAKEUP,
            500),
        
        // EJTAG Vendor Class
        USB_INTERFACE_DESCRIPTOR_INIT(
            ITF_NUM_EJTAG,
            0,
            2,
            USB_DEVICE_CLASS_VEND_SPECIFIC,
            0,
            0,
            0),
        USB_ENDPOINT_DESCRIPTOR_INIT(
            USB_ENDPOINT_OUT(EPNUM_EJTAG_OUT),
            USB_ENDPOINT_TYPE_BULK,
            USB_BULK_EP_MPS_HS,
            0),
        USB_ENDPOINT_DESCRIPTOR_INIT(
            USB_ENDPOINT_IN(EPNUM_EJTAG_IN),
            USB_ENDPOINT_TYPE_BULK,
            USB_BULK_EP_MPS_HS,
            0),

        // CDC ACM
        // CDC_ACM_DESCRIPTOR_INIT(
        //     ITF_NUM_CDC,
        //     USB_ENDPOINT_IN(EPNUM_CDC_NOTIF),
        //     USB_ENDPOINT_OUT(EPNUM_CDC_OUT),
        //     USB_ENDPOINT_IN(EPNUM_CDC_IN),
        //     USB_BULK_EP_MPS_HS,
        //     4)
    },

    desc_configuration_fs[] = {
        USB_CONFIG_DESCRIPTOR_INIT(
            USB_SIZEOF_CONF_DESC,
            ITF_NUM_TOTAL,
            1,
            USB_CONFIG_REMOTE_WAKEUP,
            500),
        
        // EJTAG Vendor Class
        USB_INTERFACE_DESCRIPTOR_INIT(
            ITF_NUM_EJTAG,
            0,
            2,
            USB_DEVICE_CLASS_VEND_SPECIFIC,
            0,
            0,
            0),
        USB_ENDPOINT_DESCRIPTOR_INIT(
            USB_ENDPOINT_OUT(EPNUM_EJTAG_OUT),
            USB_ENDPOINT_TYPE_BULK,
            USB_BULK_EP_MPS_FS,
            0),
        USB_ENDPOINT_DESCRIPTOR_INIT(
            USB_ENDPOINT_IN(EPNUM_EJTAG_IN),
            USB_ENDPOINT_TYPE_BULK,
            USB_BULK_EP_MPS_FS,
            0),

        // CDC ACM
        // CDC_ACM_DESCRIPTOR_INIT(
        //     ITF_NUM_CDC,
        //     USB_ENDPOINT_IN(EPNUM_CDC_NOTIF),
        //     USB_ENDPOINT_OUT(EPNUM_CDC_OUT),
        //     USB_ENDPOINT_IN(EPNUM_CDC_IN),
        //     USB_BULK_EP_MPS_FS,
        //     4)
    },

    desc_quality[] = {
        USB_DEVICE_QUALIFIER_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, 0x01)
    },

    desc_other_speed_hs[] = {
        USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(
            USB_SIZEOF_CONF_DESC,
            ITF_NUM_TOTAL,
            1,
            USB_CONFIG_REMOTE_WAKEUP,
            500),
        
        // EJTAG Vendor Class
        USB_INTERFACE_DESCRIPTOR_INIT(
            ITF_NUM_EJTAG,
            0,
            2,
            USB_DEVICE_CLASS_VEND_SPECIFIC,
            0,
            0,
            0),
        USB_ENDPOINT_DESCRIPTOR_INIT(
            USB_ENDPOINT_OUT(EPNUM_EJTAG_OUT),
            USB_ENDPOINT_TYPE_BULK,
            USB_BULK_EP_MPS_FS,
            0),
        USB_ENDPOINT_DESCRIPTOR_INIT(
            USB_ENDPOINT_IN(EPNUM_EJTAG_IN),
            USB_ENDPOINT_TYPE_BULK,
            USB_BULK_EP_MPS_FS,
            0),

        // CDC ACM
        // CDC_ACM_DESCRIPTOR_INIT(
        //     ITF_NUM_CDC,
        //     USB_ENDPOINT_IN(EPNUM_CDC_NOTIF),
        //     USB_ENDPOINT_OUT(EPNUM_CDC_OUT),
        //     USB_ENDPOINT_IN(EPNUM_CDC_IN),
        //     USB_BULK_EP_MPS_FS,
        //     4)
    },

    desc_other_speed_fs[] = {
        USB_OTHER_SPEED_CONFIG_DESCRIPTOR_INIT(
            USB_SIZEOF_CONF_DESC,
            ITF_NUM_TOTAL,
            1,
            USB_CONFIG_REMOTE_WAKEUP,
            500),
        
        // EJTAG Vendor Class
        USB_INTERFACE_DESCRIPTOR_INIT(
            ITF_NUM_EJTAG,
            0,
            2,
            USB_DEVICE_CLASS_VEND_SPECIFIC,
            0,
            0,
            0),
        USB_ENDPOINT_DESCRIPTOR_INIT(
            USB_ENDPOINT_OUT(EPNUM_EJTAG_OUT),
            USB_ENDPOINT_TYPE_BULK,
            USB_BULK_EP_MPS_FS,
            0),
        USB_ENDPOINT_DESCRIPTOR_INIT(
            USB_ENDPOINT_IN(EPNUM_EJTAG_IN),
            USB_ENDPOINT_TYPE_BULK,
            USB_BULK_EP_MPS_FS,
            0),

        // CDC ACM
        // CDC_ACM_DESCRIPTOR_INIT(
        //     ITF_NUM_CDC,
        //     USB_ENDPOINT_IN(EPNUM_CDC_NOTIF),
        //     USB_ENDPOINT_OUT(EPNUM_CDC_OUT),
        //     USB_ENDPOINT_IN(EPNUM_CDC_IN),
        //     USB_BULK_EP_MPS_FS,
        //     4)
    }
;

char const *desc_string[] = {
    (const char[]){0x09, 0x04}, // 0: LangID = 0x0409: U.S. English
    "RigoLigo Creations",       // 1: Manufacturer
    "CH32 Loongson EJTAG",      // 2: Product
    NULL,                       // 3: Serial
    // "CH32 LS-EJTAG Debug",      // 4: CDC-ACM Interface
};

//
// USB Buffers =================================================================
//

// USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t cdc_read_buffer[512];
// USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t cdc_write_buffer[2048];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t ejtag_read_buffer[64 * 4];
USB_NOCACHE_RAM_SECTION USB_MEM_ALIGNX uint8_t ejtag_write_buffer[64 * 4];
chry_ringbuffer_t rb_ejtag_read;
chry_ringbuffer_t rb_ejtag_write;
// volatile bool dtr_enable;
volatile bool ep_tx_busy_flag;
volatile bool ep_rx_busy_flag;

//
// CherryUSB Callbacks =========================================================
//

static const uint8_t *device_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return desc_device;
}

static const uint8_t *config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return desc_configuration_hs;
    } else if (speed == USB_SPEED_FULL) {
        return desc_configuration_fs;
    } else {
        return NULL;
    }
}

static const uint8_t *device_quality_descriptor_callback(uint8_t speed)
{
    (void)speed;

    return desc_quality;
}

static const uint8_t *other_speed_config_descriptor_callback(uint8_t speed)
{
    if (speed == USB_SPEED_HIGH) {
        return desc_other_speed_hs;
    } else if (speed == USB_SPEED_FULL) {
        return desc_other_speed_fs;
    } else {
        return NULL;
    }
}

static const char *string_descriptor_callback(uint8_t speed, uint8_t index)
{
    (void)speed;

    if (index >= (sizeof(desc_string) / sizeof(char *))) {
        return NULL;
    }

    if (index == 3) {
        // Generate ID string
        // Ugh... WCH didn't even provide an address for these registers
        static char idstr[26] = {0};
        if (idstr[0] == 0) {
            sprintf(idstr,
                    "%08X%08X%08X",
                    *((uint32_t*)0x1FFFF7E8),
                    *((uint32_t*)0x1FFFF7EC),
                    *((uint32_t*)0x1FFFF7F0));
            idstr[0] = 1;
        }
        return idstr + 1;
    }
    
    return desc_string[index];
}

// static void usbd_cdc_acm_bulk_out(uint8_t busid, uint8_t ep, uint32_t nbytes)
// {
//     // USB_LOG_RAW("actual out len:%d\r\n", nbytes);

//     usbd_ep_start_read(busid, ep, &cdc_read_buffer[0], usbd_get_ep_mps(busid, ep));
//     usbd_ep_start_write(busid, USB_ENDPOINT_IN(EPNUM_CDC_IN), &cdc_read_buffer[0], nbytes);
// }

// static void usbd_cdc_acm_bulk_in(uint8_t busid, uint8_t ep, uint32_t nbytes)
// {
//     // USB_LOG_RAW("actual in len:%d\r\n", nbytes);

//     if ((nbytes % usbd_get_ep_mps(busid, ep)) == 0 && nbytes) {
//         /* send zlp */
//         usbd_ep_start_write(busid, ep, NULL, 0);
//     } else {
//         ep_tx_busy_flag = false;
//     }
// }

static void usbd_ejtag_bulk_out_complete(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    // Complete last USB transfer
    if (nbytes) {
        chry_ringbuffer_linear_write_done(&rb_ejtag_read, nbytes);
    }

    // Fetch the write pointer and linear read size
    uint32_t avail_len;
    void* dest_ptr = chry_ringbuffer_linear_write_setup(&rb_ejtag_read, &avail_len);

    // Only start new transaction if there's more linear space than MPS.
    // Otherwise, unset RX busy flag. Wait till the ring buffer is completely cleared by consumer
    // and the consumer will restart the RX loop
    uint16_t mps = usbd_get_ep_mps(busid, ep);
    if (avail_len >= mps) {
        // Setup next USB transfer
        usbd_ep_start_read(busid, ep, dest_ptr, mps);
    } else {
        ep_rx_busy_flag = false;
    }
}

static void usbd_ejtag_bulk_in_complete(uint8_t busid, uint8_t ep, uint32_t nbytes)
{
    chry_ringbuffer_linear_read_done(&rb_ejtag_write, nbytes);
    // printf("USB_IN_CPLT: % 2d bytes sent; % 2d bytes in buffer; ", nbytes, chry_ringbuffer_get_used(&rb_ejtag_write));
    if ((nbytes % usbd_get_ep_mps(busid, ep)) == 0 && nbytes) {
        /* send zlp */
        // printf("Send ZLP\r\n");
        usbd_ep_start_write(busid, ep, NULL, 0);
        return;
    } else if (!chry_ringbuffer_check_empty(&rb_ejtag_write)) {
        // If the TX buffer still has data, send them out
        uint32_t avail_len;
        void *read_addr = chry_ringbuffer_linear_read_setup(&rb_ejtag_write, &avail_len);
        avail_len = MIN(usbd_get_ep_mps(busid, ep), avail_len);
        // printf("Issue new write: avail_len % 2d\r\n", avail_len);
        usbd_ep_start_write(busid, ep, read_addr, avail_len);
        return;
    } else {
        // Marks the end of a consecutive transmission
        // printf("TX Busy Deasserted\r\n");
        chry_ringbuffer_reset(&rb_ejtag_write);
        ep_tx_busy_flag = false;
        return;
    }

    while (1);
}

static int ejtag_vendor_request_handler(uint8_t busid, struct usb_setup_packet *setup, uint8_t **data,
                                        uint32_t *len) {
    // Loongson EJTAG doesn't really do anything with setup endpoint
    return 0;
}

static void usbd_event_handler(uint8_t busid, uint8_t event)
{
    switch (event) {
    case USBD_EVENT_RESET:
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        break;
    case USBD_EVENT_SUSPEND:
        break;
    case USBD_EVENT_CONFIGURED:
    {
        // Fetch the write pointer and linear read size
        uint32_t avail_len;
        void* dest_ptr = chry_ringbuffer_linear_write_setup(&rb_ejtag_read, &avail_len);

        // Clamp read size down to MPS
        uint16_t mps = usbd_get_ep_mps(busid, USB_ENDPOINT_OUT(EPNUM_EJTAG_OUT));
        assert(avail_len >= mps);

        // Setup initial USB transfer
        usbd_ep_start_read(busid, USB_ENDPOINT_OUT(EPNUM_EJTAG_OUT), dest_ptr, avail_len);
        break;
    }
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

//
// CherryUSB Structures ========================================================
//

static const struct usb_descriptor cherryusb_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .other_speed_descriptor_callback = other_speed_config_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
    .msosv1_descriptor = NULL,
    .msosv2_descriptor = NULL,
    .webusb_url_descriptor = NULL,
    .bos_descriptor = NULL
};

static struct usbd_interface ejtag_intf; //, cdc_intf0, cdc_intf1;

static struct usbd_endpoint
    // cdc_in_ep = {
    //     .ep_addr = EPNUM_CDC_IN,
    //     .ep_cb = usbd_cdc_acm_bulk_in
    // },
    // cdc_out_ep = {
    //     .ep_addr = EPNUM_CDC_OUT,
    //     .ep_cb = usbd_cdc_acm_bulk_out
    // },

    ejtag_in_ep = {
        .ep_addr = USB_ENDPOINT_IN(EPNUM_EJTAG_IN),
        .ep_cb = usbd_ejtag_bulk_in_complete
    },
    ejtag_out_ep = {
        .ep_addr = USB_ENDPOINT_OUT(EPNUM_EJTAG_OUT),
        .ep_cb = usbd_ejtag_bulk_out_complete
    }
;


//
// Functions ===================================================================
//

static struct usbd_interface *usbd_ejtag_init_intf(uint8_t busid, struct usbd_interface *intf) {
    (void)busid;

    intf->class_interface_handler = NULL;
    intf->class_endpoint_handler = NULL;
    intf->vendor_handler = ejtag_vendor_request_handler;
    intf->notify_handler = NULL;

    return intf;
}

void init_cherryusb(uint8_t busid, uint32_t reg_base) {
    usbd_desc_register(busid, &cherryusb_descriptor);
    // CDC-ACM
    // usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &cdc_intf0));
    // usbd_add_interface(busid, usbd_cdc_acm_init_intf(busid, &cdc_intf1));
    // usbd_add_endpoint(busid, &cdc_in_ep);
    // usbd_add_endpoint(busid, &cdc_out_ep);
    // EJTAG
    usbd_add_interface(busid, usbd_ejtag_init_intf(busid, &ejtag_intf));
    usbd_add_endpoint(busid, &ejtag_in_ep);
    usbd_add_endpoint(busid, &ejtag_out_ep);
    usbd_initialize(busid, reg_base, usbd_event_handler);
    // Buffers and flags
    chry_ringbuffer_init(&rb_ejtag_read, ejtag_read_buffer, sizeof(ejtag_read_buffer));
    chry_ringbuffer_init(&rb_ejtag_write, ejtag_write_buffer, sizeof(ejtag_write_buffer));
    ep_tx_busy_flag = false;
    ep_rx_busy_flag = false;
}
