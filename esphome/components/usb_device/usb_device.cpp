#if defined(USE_ESP32_VARIANT_ESP32S2) || defined(USE_ESP32_VARIANT_ESP32S3)
#include "usb_device.h"
#include "esphome/core/log.h"
#include "tusb.h"

namespace esphome {
namespace usb_device {

static const char *const TAG = "usb_device";

void UsbDevice::setup() {
  // With ESP-IDF we use the TinyUSB C stack initialized by the tinyusb component.
  // The USB descriptors (VID/PID, strings) are configured via the tinyusb component
  // and the build-time defines. Store the values locally so they can be applied by
  // the tinyusb component if needed.
}

void UsbDevice::update() {
#ifdef USE_BINARY_SENSOR
  if (mounted_ != nullptr) {
    mounted_->publish_state(tud_mounted());
  }
  if (ready_ != nullptr) {
    ready_->publish_state(tud_ready());
  }
  if (suspended_ != nullptr) {
    suspended_->publish_state(tud_suspended());
  }
#endif
}

float UsbDevice::get_setup_priority() const {
  // it should be registered after all USB components
  return setup_priority::HARDWARE - 100;
}

void UsbDevice::dump_config() {
  ESP_LOGCONFIG(TAG, "USB device - mounted: %s, suspended: %s, ready: %s", YESNO(tud_mounted()),
                YESNO(tud_suspended()), YESNO(tud_ready()));
}

void UsbDevice::set_vendor_id(const uint16_t vid) { this->vendor_id_ = vid; }
void UsbDevice::set_product_id(const uint16_t pid) { this->product_id_ = pid; }
void UsbDevice::set_manufacturer_name(const std::string &manufacturer_name) { this->manufacturer_name_ = manufacturer_name; }
void UsbDevice::set_product_name(const std::string &product_name) { this->product_name_ = product_name; }

#ifdef USE_BINARY_SENSOR
void UsbDevice::set_mounted_binary_sensor(binary_sensor::BinarySensor *sensor) { mounted_ = sensor; };
void UsbDevice::set_ready_binary_sensor(binary_sensor::BinarySensor *sensor) { ready_ = sensor; };
void UsbDevice::set_suspended_binary_sensor(binary_sensor::BinarySensor *sensor) { suspended_ = sensor; };
#endif

}  // namespace usb_device
}  // namespace esphome
#endif
