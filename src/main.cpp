#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <legacy/nrf_drv_clock.h>
#include <hal/nrf_rtc.h>
#include <hal/nrf_wdt.h>
#include <os/os_cputime.h>
#include <libraries/timer/app_timer.h>
#include <libraries/gpiote/app_gpiote.h>
#include "displayapp/DisplayApp.h"
#include <softdevice/common/nrf_sdh.h>
#include "components/datetime/DateTimeController.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include <drivers/St7789.h>
#include <drivers/SpiMaster.h>
#include <drivers/Spi.h>
#include "displayapp/LittleVgl.h"
#include <systemtask/SystemTask.h>
#include <nimble/nimble_port_freertos.h>
#include <nimble/npl_freertos.h>
#include <nimble/nimble_port.h>
#include <host/ble_hs.h>
#include <controller/ble_ll.h>
#include <transport/ram/ble_hci_ram.h>
#include <host/util/util.h>
#include <services/gap/ble_svc_gap.h>
#include <components/motion/MotionController.h>
#include <drivers/Acnt101.h>
#include <drivers/Kx126.h>
#include <drivers/Kx022.h>


static constexpr uint8_t pinSpiSck = 16;
static constexpr uint8_t pinSpiMosi = 14;
static constexpr uint8_t pinSpiMiso = 15;
static constexpr uint8_t pinSpiFlashCsn = 23;
static constexpr uint8_t pinLcdCsn = 22;
static constexpr uint8_t pinLcdDataCommand = 11;
static constexpr uint8_t pinTwiScl = 6;
static constexpr uint8_t pinTwiSda = 7;
static constexpr uint8_t touchPanelTwiAddress = 0x15;
static constexpr uint8_t motionSensorTwiAddress = 0x1e;
static constexpr uint8_t pinSpiAccCsn = 8;
Watch::Drivers::SpiMaster spi{Watch::Drivers::SpiMaster::SpiModule::SPI0, {
        Watch::Drivers::SpiMaster::BitOrder::Msb_Lsb,
        Watch::Drivers::SpiMaster::Modes::Mode3,
        Watch::Drivers::SpiMaster::Frequencies::Freq8Mhz,
        pinSpiSck,
        pinSpiMosi,
        pinSpiMiso
  }
};

Watch::Drivers::Spi lcdSpi {spi, pinLcdCsn};
Watch::Drivers::St7789 lcd {lcdSpi, pinLcdDataCommand};

Watch::Drivers::Spi flashSpi {spi, pinSpiFlashCsn};
Watch::Drivers::SpiNorFlash spiNorFlash {flashSpi};

Watch::Drivers::Spi accSpi {spi, pinSpiAccCsn};
Watch::Drivers::Kx126 motionSensor {accSpi};


// The TWI device should work @ up to 400Khz but there is a HW bug which prevent it from
// respecting correct timings. According to erratas heet, this magic value makes it run
// at ~390Khz with correct timings.

static constexpr uint32_t MaxTwiFrequencyWithoutHardwareBug{0x06200000};
Watch::Drivers::TwiMaster twiMaster{Watch::Drivers::TwiMaster::Modules::TWIM1,
                                       Watch::Drivers::TwiMaster::Parameters {
                                               MaxTwiFrequencyWithoutHardwareBug, pinTwiSda, pinTwiScl}};                                   
Watch::Drivers::Cst816S touchPanel {twiMaster, touchPanelTwiAddress};
//Watch::Drivers::Kx022 motionSensor{twiMaster, motionSensorTwiAddress};

Watch::Components::LittleVgl lvgl {lcd, touchPanel};

TimerHandle_t debounceTimer;
Watch::Controllers::Battery batteryController;
Watch::Controllers::Ble bleController;
Watch::Controllers::DateTime dateTimeController;
void ble_manager_set_ble_connection_callback(void (*connection)());
void ble_manager_set_ble_disconnection_callback(void (*disconnection)());
static constexpr uint8_t pinTouchIrq = 25;
std::unique_ptr<Watch::System::SystemTask> systemTask;

Watch::Controllers::NotificationManager notificationManager;
Watch::Drivers::Acnt101 acnt101;

void nrfx_gpiote_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
  if(pin == pinTouchIrq) {
    systemTask->OnTouchEvent();
    return ;
  }
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xTimerStartFromISR(debounceTimer, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


void Time_IRQHandler(void * p_context){
 systemTask->ResetSensor(); 
}

void nrfx_gpiote_temp_evt_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {  
systemTask->ReadTempSensor();
}


extern "C" {
  void vApplicationIdleHook(void) {
    lv_tick_inc(1);
  }
}

void DebounceTimerCallback(TimerHandle_t xTimer) {
  xTimerStop(xTimer, 0);
  systemTask->OnButtonPushed();
}

extern "C" {
void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void) {
  if(((NRF_SPIM0->INTENSET & (1<<6)) != 0) && NRF_SPIM0->EVENTS_END == 1) {
    NRF_SPIM0->EVENTS_END = 0;
    spi.OnEndEvent();
  }

  if(((NRF_SPIM0->INTENSET & (1<<19)) != 0) && NRF_SPIM0->EVENTS_STARTED == 1) {
    NRF_SPIM0->EVENTS_STARTED = 0;
    spi.OnStartedEvent();
  }

  if(((NRF_SPIM0->INTENSET & (1<<1)) != 0) && NRF_SPIM0->EVENTS_STOPPED == 1) {
    NRF_SPIM0->EVENTS_STOPPED = 0;
  }
  }
}
static void (*radio_isr_addr)(void) ;
static void (*rng_isr_addr)(void) ;
static void (*rtc0_isr_addr)(void) ;


/* Some interrupt handlers required for NimBLE radio driver */
extern "C" {
void RADIO_IRQHandler(void) {
  ((void (*)(void)) radio_isr_addr)();
}

void RNG_IRQHandler(void) {
  ((void (*)(void)) rng_isr_addr)();
}

void RTC0_IRQHandler(void) {
  ((void (*)(void)) rtc0_isr_addr)();
}

void WDT_IRQHandler(void) {
  nrf_wdt_event_clear(NRF_WDT_EVENT_TIMEOUT);
}

void npl_freertos_hw_set_isr(int irqn, void (*addr)(void)) {
  switch (irqn) {
    case RADIO_IRQn:
      radio_isr_addr = addr;
      break;
    case RNG_IRQn:
      rng_isr_addr = addr;
      break;
    case RTC0_IRQn:
      rtc0_isr_addr = addr;
      break;
  }
}

uint32_t
npl_freertos_hw_enter_critical(void) {
  uint32_t ctx = __get_PRIMASK();
  __disable_irq();
  return (ctx & 0x01);
}

void npl_freertos_hw_exit_critical(uint32_t ctx) {
  if (!ctx) {
    __enable_irq();
  }
}


static struct ble_npl_eventq g_eventq_dflt;

struct ble_npl_eventq *
nimble_port_get_dflt_eventq(void) {
  return &g_eventq_dflt;
}

void nimble_port_run(void) {
  struct ble_npl_event *ev;

  while (1) {
    ev = ble_npl_eventq_get(&g_eventq_dflt, BLE_NPL_TIME_FOREVER);
    ble_npl_event_run(ev);
  }
}

void BleHost(void *) {
  nimble_port_run();
}

void nimble_port_init(void) {
  void os_msys_init(void);
  void ble_store_ram_init(void);
  ble_npl_eventq_init(&g_eventq_dflt);
  os_msys_init();
  ble_hs_init();
  ble_store_ram_init();

  int res;
  res = hal_timer_init(5, NULL);
  ASSERT(res == 0);
  res = os_cputime_init(32768);
  ASSERT(res == 0);
  ble_ll_init();
  ble_hci_ram_init();
  nimble_port_freertos_init(BleHost);
}

void nimble_port_ll_task_func(void *args) {
  extern void ble_ll_task(void *);
  ble_ll_task(args);
  }
}


int main(void) { 
  nrf_drv_clock_init();
  debounceTimer = xTimerCreate ("debounceTimer", 200, pdFALSE, (void *) 0, DebounceTimerCallback);
  systemTask.reset(new Watch::System::SystemTask(spi, lcd, spiNorFlash, twiMaster, touchPanel, motionSensor,
          lvgl, batteryController, bleController,dateTimeController, notificationManager,acnt101));
  systemTask->Start();
  nimble_port_init();
  vTaskStartScheduler();
  for (;;) {
    APP_ERROR_HANDLER(NRF_ERROR_FORBIDDEN);
     __WFE();
  }
}




