#pragma once

#include <cstdint>
#include "Screen.h"
#include <lvgl/src/lv_core/lv_style.h>
#include <lvgl/src/lv_core/lv_obj.h>

namespace Watch {
  namespace Applications {
    namespace Screens {

      class Meter : public Screen{
        public:
          Meter(DisplayApp* app);
          ~Meter() override;

          bool Refresh() override;
          bool OnButtonPushed() override;

        private:
          lv_style_t style_lmeter;
          lv_obj_t * lmeter;

          uint32_t value=0;
          bool running = true;

      };
    }
  }
}
