#include "usb_keyboard.h"
#if defined(USE_ESP32_VARIANT_ESP32S2) || defined(USE_ESP32_VARIANT_ESP32S3)
#ifdef USE_KEYBOARD
#include "esphome/core/log.h"
#include "tusb.h"

namespace esphome {
namespace usb_device {

static const char *const TAG = "usb_device";

template<class T> void Report<T>::loop() {
  if (pending_) {
    report();
  }
}

template<class T> void Report<T>::report() {
  if (!tud_ready()) {
    if (!pending_) {
      ESP_LOGD(TAG, "HID device is not ready");
    }
    pending_ = true;
    return;
  }
  if (report_()) {
    pending_ = false;
  } else {
    pending_ = true;
    ESP_LOGW(TAG, "HID device report was not sent");
  }
}

KeyboardReport::KeyboardReport(uint8_t report_id)
  : Report([this, report_id]() -> bool {
        ESP_LOGV(TAG, "keyboard report id: %d - modifier: %d, code %d, %d, %d, %d, %d, %d", report_id, modifier_,
                 hidcode_[0], hidcode_[1], hidcode_[2], hidcode_[3], hidcode_[4], hidcode_[5]);
        uint8_t buf[8] = {0};
        buf[0] = modifier_;
        buf[1] = 0;  // reserved
        buf[2] = hidcode_[0];
        buf[3] = hidcode_[1];
        buf[4] = hidcode_[2];
        buf[5] = hidcode_[3];
        buf[6] = hidcode_[4];
        buf[7] = hidcode_[5];
        return tud_hid_report(report_id, buf, sizeof(buf));
      }) {}

void KeyboardReport::loop() {
  Report::loop();
#ifdef USE_BINARY_SENSOR
  // those are updated from 2 different threads but it does not matter much
  if (led_control_ && update_led_) {
    update_led_ = false;
    led_control_->publish_capslock(led_indicator_ & KEYBOARD_LED_CAPSLOCK);
    led_control_->publish_numlock(led_indicator_ & KEYBOARD_LED_NUMLOCK);
    led_control_->publish_scrollock(led_indicator_ & KEYBOARD_LED_SCROLLLOCK);
  }
#endif
}

MediaKeysReport::MediaKeysReport(uint8_t report_id)
  : Report([this, report_id]() -> bool {
        ESP_LOGV(TAG, "media keys report id: %d - %d", report_id, media_keys_);
        uint16_t val = media_keys_;
        return tud_hid_report(report_id, &val, sizeof(val));
      }) {}

template class Report<hid::Keyboard>;
template class Report<hid::MediaKeys>;

}  // namespace usb_device
}  // namespace esphome
#endif
#endif
