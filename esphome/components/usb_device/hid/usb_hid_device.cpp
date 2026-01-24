#if defined(USE_ESP32_VARIANT_ESP32S2) || defined(USE_ESP32_VARIANT_ESP32S3)
#include "usb_hid_device.h"
#include "esphome/core/log.h"

namespace esphome {
namespace usb_device {

static const char *const TAG = "usb_device";

USBHIDDevice *global_usb_hid_device;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

void USBHIDDevice::setup() {
  global_usb_hid_device = this;
  // For tinyusb (ESP-IDF) the HID stack is initialized by the tinyusb component.
  // Output reports will be delivered via the callback mechanism implemented here.
}

void USBHIDDevice::loop() {
#ifdef USE_KEYBOARD
  if (keyboard_report_) {
    keyboard_report_->loop();
  }
  if (media_keys_report_) {
    media_keys_report_->loop();
  }
#endif
}

float USBHIDDevice::get_setup_priority() const { return setup_priority::HARDWARE; }

void USBHIDDevice::dump_config() {
  ESP_LOGCONFIG(TAG, "HID is ready: %s", YESNO(tud_ready()));
  bool keyboard_exists = false;
  bool media_keys_exists = false;
#ifdef USE_KEYBOARD
  keyboard_exists = keyboard_report_ != nullptr;
  media_keys_exists = media_keys_report_ != nullptr;
#endif
  ESP_LOGCONFIG(TAG, "Keyboard exists: %s, media keys exists: %s", YESNO(keyboard_exists), YESNO(media_keys_exists));
}

void USBHIDDevice::hid_report_callback(uint8_t report_id, const uint8_t *buffer, uint16_t bufsize) {
  ESP_LOGV(TAG, "hid_report_callback - report_id %d, bufsize %d", report_id, bufsize);
  (void) report_id;
  (void) bufsize;
  // LED indicator is output report with only 1 byte length
  uint8_t led_indicator = buffer[0];
#ifdef USE_KEYBOARD
  if (global_usb_hid_device && global_usb_hid_device->keyboard_report_) {
    global_usb_hid_device->keyboard_report_->set_led_indicator(led_indicator);
  }
#endif
}

extern "C" {

// Provide tinyusb HID callbacks here so they are colocated with the USBHIDDevice
// implementation. These symbols must have C linkage and default visibility so
// the tinyusb HID class (compiled in the ESP-IDF managed component) can find
// them at link time.

uint8_t const * __attribute__((used, visibility("default"))) tud_hid_descriptor_report_cb(uint8_t instance) {
  (void) instance;
  // Descriptor is provided via codegen/progmem array and the tinyusb component
  // handles descriptors; return nullptr to indicate we don't provide an alternate.
  return nullptr;
}

uint16_t __attribute__((used, visibility("default"))) tud_hid_get_report_cb(uint8_t instance,
                                                                            uint8_t report_id,
                                                                            hid_report_type_t report_type,
                                                                            uint8_t *buffer,
                                                                            uint16_t reqlen) {
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;
  return 0;
}

void __attribute__((used, visibility("default"))) tud_hid_set_report_cb(uint8_t instance,
                                                                          uint8_t report_id,
                                                                          hid_report_type_t report_type,
                                                                          uint8_t const *buffer,
                                                                          uint16_t bufsize) {
  (void) instance;
  (void) report_type;
  if (buffer == nullptr || bufsize == 0) return;

  if (global_usb_hid_device) {
    // Forward to the C++ handler which will update LED indicators, etc.
    global_usb_hid_device->hid_report_callback(report_id, static_cast<const uint8_t *>(buffer), bufsize);
  }
}

}  // extern "C"

}  // namespace usb_device
}  // namespace esphome

#endif
