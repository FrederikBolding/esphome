// This file provides the C callbacks tinyusb's HID class expects to find.
// We implement thin stubs that forward set-report (output report) to our
// USBHIDDevice C++ instance so LED indicators and other output reports are handled.

#include "tusb.h"
#include "usb_hid_device.h"

extern "C" {

// Called by tinyusb to get HID report descriptor for a specific interface.
// Return value: size of descriptor or 0 if not provided.
uint16_t tud_hid_descriptor_report_cb(uint8_t itf) {
  (void) itf;
  // Descriptor is provided via codegen/progmem array and the tinyusb component
  // handles descriptors; return 0 to indicate we don't provide an alternate.
  return 0;
}

// Called by tinyusb when host requests GET_REPORT (input reports).
// We don't implement dynamic input reports via this callback; return 0.
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, void *buffer, uint16_t reqlen) {
  (void) itf;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;
  return 0;
}

// Called by tinyusb when host sends a SET_REPORT (output report) to device.
// We'll forward the report payload to our USBHIDDevice instance for handling.
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, const void *buffer, uint16_t bufsize) {
  (void) itf;
  if (buffer == nullptr || bufsize == 0) return;

  // Forward to C++ handler if available
  extern esphome::usb_device::USBHIDDevice *global_usb_hid_device;  // declared in usb_hid_device.cpp
  if (global_usb_hid_device) {
    // tinyusb uses report_id == 0 for boot protocol keyboards sometimes; forward as-is
    esphome::usb_device::USBHIDDevice::hid_report_callback(report_id, static_cast<const uint8_t *>(buffer), bufsize);
  }
}

}  // extern "C"
