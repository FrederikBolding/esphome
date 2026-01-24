#pragma once
#if defined(USE_ESP32_VARIANT_ESP32P4) || defined(USE_ESP32_VARIANT_ESP32S2) || defined(USE_ESP32_VARIANT_ESP32S3)
#include "esphome/core/component.h"

#include "tinyusb.h"
#include "tusb.h"

namespace esphome::tinyusb {

enum USBDStringDescriptor : uint8_t {
  LANGUAGE_ID = 0,
  MANUFACTURER = 1,
  PRODUCT = 2,
  SERIAL_NUMBER = 3,
  INTERFACE = 4,
  TERMINATOR = 5,
  SIZE = 6,
};

static const char *DEFAULT_USB_STR = "ESPHome";

class TinyUSB : public Component {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::BUS; }

  void set_usb_desc_product_id(uint16_t product_id) { this->usb_descriptor_.idProduct = product_id; }
  void set_usb_desc_vendor_id(uint16_t vendor_id) { this->usb_descriptor_.idVendor = vendor_id; }
  void set_usb_desc_lang_id(uint16_t lang_id) {
    this->usb_desc_lang_id_[0] = lang_id & 0xFF;
    this->usb_desc_lang_id_[1] = lang_id >> 8;
  }
  void set_usb_desc_manufacturer(const char *usb_desc_manufacturer) {
    this->string_descriptor_[MANUFACTURER] = usb_desc_manufacturer;
  }
  void set_usb_desc_product(const char *usb_desc_product) { this->string_descriptor_[PRODUCT] = usb_desc_product; }
  void set_usb_desc_serial(const char *usb_desc_serial) { this->string_descriptor_[SERIAL_NUMBER] = usb_desc_serial; }

 protected:
  char usb_desc_lang_id_[2] = {0x09, 0x04};  // defaults to english

  const char *string_descriptor_[SIZE] = {
      this->usb_desc_lang_id_,  // 0: supported language is English (0x0409)
      DEFAULT_USB_STR,          // 1: Manufacturer
      DEFAULT_USB_STR,          // 2: Product
      nullptr,                  // 3: Serial Number
      nullptr,                  // 4: Interface
      nullptr,                  // 5: Terminator
  };

  tinyusb_config_t tusb_cfg_{};
  tusb_desc_device_t usb_descriptor_{
      .bLength = sizeof(tusb_desc_device_t),
      .bDescriptorType = TUSB_DESC_DEVICE,
      .bcdUSB = 0x0200,
      .bDeviceClass = TUSB_CLASS_MISC,
      .bDeviceSubClass = MISC_SUBCLASS_COMMON,
      .bDeviceProtocol = MISC_PROTOCOL_IAD,
      .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
      .idVendor = 0x303A,
      .idProduct = 0x4001,
      .bcdDevice = CONFIG_TINYUSB_DESC_BCD_DEVICE,
      .iManufacturer = 1,
      .iProduct = 2,
      .iSerialNumber = 3,
      .bNumConfigurations = 1,
  };

  // Default HID report descriptor (keyboard). Used when no HID report descriptor
  // is provided by codegen. Defined inline so it can live in the header and be
  // referenced from multiple translation units without violating the ODR.
  static inline const uint8_t default_hid_report_desc[] = {
    0x05, 0x01,       // Usage Page (Generic Desktop)
    0x09, 0x06,       // Usage (Keyboard)
    0xA1, 0x01,       // Collection (Application)
    0x05, 0x07,       //   Usage Page (Key Codes)
    0x19, 0xE0,       //   Usage Minimum (224)
    0x29, 0xE7,       //   Usage Maximum (231)
    0x15, 0x00,       //   Logical Minimum (0)
    0x25, 0x01,       //   Logical Maximum (1)
    0x75, 0x01,       //   Report Size (1)
    0x95, 0x08,       //   Report Count (8)
    0x81, 0x02,       //   Input (Data,Var,Abs)
    0x95, 0x01,       //   Report Count (1)
    0x75, 0x08,       //   Report Size (8)
    0x81, 0x01,       //   Input (Cnst,Var,Abs)
    0x95, 0x05,       //   Report Count (5)
    0x75, 0x01,       //   Report Size (1)
    0x05, 0x08,       //   Usage Page (LEDs)
    0x19, 0x01,       //   Usage Minimum (1)
    0x29, 0x05,       //   Usage Maximum (5)
    0x91, 0x02,       //   Output (Data,Var,Abs)
    0x95, 0x01,       //   Report Count (1)
    0x75, 0x03,       //   Report Size (3)
    0x91, 0x01,       //   Output (Cnst,Var,Abs)
    0x95, 0x06,       //   Report Count (6)
    0x75, 0x08,       //   Report Size (8)
    0x15, 0x00,       //   Logical Minimum (0)
    0x25, 0x65,       //   Logical Maximum (101)
    0x05, 0x07,       //   Usage Page (Key Codes)
    0x19, 0x00,       //   Usage Minimum (0)
    0x29, 0x65,       //   Usage Maximum (101)
    0x81, 0x00,       //   Input (Data,Ary,Abs)
    0xC0              // End Collection
  };

  // Default USB configuration descriptor for a single HID keyboard interface.
  // wDescriptorLength in the HID descriptor is set to the report length.
  static inline const uint8_t default_config_descriptor[] = {
    // Configuration descriptor (9 bytes)
    0x09, TUSB_DESC_CONFIGURATION,
    0x22, 0x00,  // wTotalLength (will cover this simple config)
    0x01,        // bNumInterfaces
    0x01,        // bConfigurationValue
    0x00,        // iConfiguration
    0x80,        // bmAttributes (bus powered)
    50,          // bMaxPower (mA/2) -> 100 mA

    // Interface descriptor (9 bytes)
    0x09, TUSB_DESC_INTERFACE,
    0x00,        // bInterfaceNumber
    0x00,        // bAlternateSetting
    0x01,        // bNumEndpoints (interrupt IN)
    TUSB_CLASS_HID,    // bInterfaceClass
    HID_SUBCLASS_BOOT, // bInterfaceSubClass (boot)
    HID_ITF_PROTOCOL_KEYBOARD, // bInterfaceProtocol
    0x00,        // iInterface

    // HID descriptor (class-specific) — 9 bytes for HID
    0x09, HID_DESC_TYPE_HID,
    0x11, 0x01,  // bcdHID (1.11)
    0x00,        // bCountryCode
    0x01,        // bNumDescriptors
    HID_DESC_TYPE_REPORT, // bDescriptorType
    /* wDescriptorLength LSB/MSB */ (uint8_t)(sizeof(default_hid_report_desc) & 0xFF),
    (uint8_t)((sizeof(default_hid_report_desc) >> 8) & 0xFF),

    // Endpoint descriptor (7 bytes) — interrupt IN EP 1
    0x07, TUSB_DESC_ENDPOINT,
    0x81,        // bEndpointAddress (IN endpoint 1)
    0x03,        // bmAttributes (interrupt)
    0x08, 0x00,  // wMaxPacketSize (8 bytes)
    0x0A         // bInterval (10 ms)
  };
};

}  // namespace esphome::tinyusb
#endif  // USE_ESP32_VARIANT_ESP32P4 || USE_ESP32_VARIANT_ESP32S2 || USE_ESP32_VARIANT_ESP32S3
