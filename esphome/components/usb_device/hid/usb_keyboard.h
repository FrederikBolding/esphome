#pragma once
#include "esphome/core/defines.h"
#if defined(USE_ESP32_VARIANT_ESP32S2) || defined(USE_ESP32_VARIANT_ESP32S3)
#ifdef USE_KEYBOARD

#include <functional>
#include "tusb.h"
#include <functional>
#if 0
class Adafruit_USBD_HID;
#else
class Adafruit_USBD_HID;
#endif
#include "esphome/components/hid/keyboard/hid_keyboard.h"
#include "esphome/components/hid/keyboard/hid_media_keys.h"

namespace esphome {
namespace usb_device {

template<class T> class Report : public T {
 public:
  explicit Report(const std::function<bool()> &report) : report_(report) {}
  virtual void loop();
  void report() override;

 protected:
  bool pending_;
  std::function<bool()> report_;
  // Adafruit wrapper removed; tinyusb C API is used instead.
};

class KeyboardReport : public Report<hid::Keyboard> {
 public:
  KeyboardReport(uint8_t report_id);
  void loop() override;
  void set_led_indicator(uint8_t led_indicator) {
    led_indicator_ = led_indicator;
    update_led_ = true;
  }

 protected:
  uint8_t led_indicator_{0};
  bool update_led_{false};
};

class MediaKeysReport : public Report<hid::MediaKeys> {
 public:
  MediaKeysReport(uint8_t report_id);
};

}  // namespace usb_device
}  // namespace esphome

#endif
#endif
