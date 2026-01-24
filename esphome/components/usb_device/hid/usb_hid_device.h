#pragma once
#include "esphome/core/defines.h"
#include "esphome/components/hid/hid_device.h"
#if defined(USE_ESP32_VARIANT_ESP32S2) || defined(USE_ESP32_VARIANT_ESP32S3)
#include <cstdint>
#include <functional>
#include "tusb.h"
#include "esphome/core/component.h"
#ifdef USE_KEYBOARD
#include "usb_keyboard.h"
#endif

namespace esphome {
namespace usb_device {

class USBHIDDevice : public Component, public hid::HIDDevice {
 public:
  USBHIDDevice() = default;
  void setup() override;
  void loop() override;
  float get_setup_priority() const override;
  void dump_config();
  // Called by tinyusb C callbacks when an output report (SET_REPORT) is received.
  // Made public so extern "C" wrappers can forward reports here.
  static void hid_report_callback(uint8_t report_id, const uint8_t *buffer, uint16_t bufsize);
#ifdef USE_KEYBOARD
  void set_report(KeyboardReport *keyboard_report) { keyboard_report_ = keyboard_report; }
  void set_report(MediaKeysReport *media_keys_report) { media_keys_report_ = media_keys_report; }
  KeyboardReport *keyboard_control() { return keyboard_report_; }
  MediaKeysReport *media_keys_control() { return media_keys_report_; }
#endif
 protected:
#ifdef USE_KEYBOARD
  KeyboardReport *keyboard_report_{nullptr};
  MediaKeysReport *media_keys_report_{nullptr};
#endif
};

}  // namespace usb_device
}  // namespace esphome

#endif
