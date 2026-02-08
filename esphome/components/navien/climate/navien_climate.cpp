

#include <array>
#include "esphome.h"
#include "esphome/components/climate/climate_mode.h"
//#include "esphome/components/climate/climate_traits.h"
#include "navien_climate.h"

namespace esphome {
namespace navien {
  
static const char *TAG = "navien.climate";

//std::set<climate::ClimateMode> supported_modes = {climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_HEAT};
static constexpr climate::ClimateModeMask supported_modes = {climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_HEAT}; 

void NavienClimate::setup(){
}
  
void NavienClimate::dump_config(){
  ESP_LOGCONFIG(TAG, "DHW mode: %s", this->use_dhw_ ? "true" : "false");
}

climate::ClimateTraits NavienClimate::traits(){
    auto traits = climate::ClimateTraits();
    
    traits.add_feature_flags(esphome::climate::CLIMATE_SUPPORTS_CURRENT_TEMPERATURE);

    //traits.add_feature_flags(supported_features);
    traits.set_supported_modes(supported_modes);
    return traits;
}
  
void NavienClimate::control(const climate::ClimateCall &call){
  if (call.get_target_temperature().has_value()){
    esphome::optional<float> f = call.get_target_temperature();
    float target = *f;
    switch (use_dhw_) {
    case true:
      ESP_LOGD(TAG, "Setting DHW target temperature to %f", target);
      parent->send_set_dhw_temp_cmd(target);
      break;
    case false:
      ESP_LOGD(TAG, "Setting SH target temperature to %f", target);
      parent->send_set_sh_temp_cmd(target);
      break;
    }
  }
    
  if (call.get_mode().has_value()) {
    esphome::climate::ClimateMode mode = call.get_mode().value();
    ESP_LOGD(TAG, "Setting mode to %s", climate::climate_mode_to_string(mode));
    switch(mode){
    case climate::ClimateMode::CLIMATE_MODE_OFF:
      parent->send_turn_off_cmd();
      break;
    case climate::ClimateMode::CLIMATE_MODE_HEAT:
      parent->send_turn_on_cmd();
      break;
    default:
      ESP_LOGD(TAG, "Unsupported Mode (%s)", climate::climate_mode_to_string(mode));
    }
  }
}

  void NavienClimate::set_parent(Navien * parent_){
    this->parent = parent_;
    parent->set_climate(this);
  }
}


void NavienClimate::set_use_dhw(bool dhw){
  this->use_dhw_ = dhw;
}
}
