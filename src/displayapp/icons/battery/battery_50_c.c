#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif


#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

#ifndef LV_ATTRIBUTE_IMG_BATTERY_50_C
#define LV_ATTRIBUTE_IMG_BATTERY_50_C
#endif

const LV_ATTRIBUTE_MEM_ALIGN LV_ATTRIBUTE_LARGE_CONST LV_ATTRIBUTE_IMG_BATTERY_50_C uint8_t battery_50_c_map[] = {
  0x09, 0x0a, 0x0c, 0xff, 	/*Color of index 0*/
  0x55, 0x8c, 0x91, 0xff, 	/*Color of index 1*/
  0x94, 0xd6, 0xde, 0xff, 	/*Color of index 2*/
  0x26, 0x4e, 0x52, 0xff, 	/*Color of index 3*/
  0x93, 0xec, 0xf5, 0xff, 	/*Color of index 4*/
  0x37, 0x74, 0x7b, 0xff, 	/*Color of index 5*/
  0x26, 0x2a, 0x2a, 0xff, 	/*Color of index 6*/
  0xc5, 0xe9, 0xeb, 0xff, 	/*Color of index 7*/
  0x72, 0xae, 0xb4, 0xff, 	/*Color of index 8*/
  0x6c, 0x6a, 0x67, 0xff, 	/*Color of index 9*/
  0x85, 0x88, 0x89, 0xff, 	/*Color of index 10*/
  0xc9, 0xd4, 0xd7, 0xff, 	/*Color of index 11*/
  0x4f, 0x50, 0x4f, 0xff, 	/*Color of index 12*/
  0xa7, 0xb6, 0xb7, 0xff, 	/*Color of index 13*/
  0x7c, 0x7a, 0x7a, 0xff, 	/*Color of index 14*/
  0xe7, 0xfc, 0xfb, 0xff, 	/*Color of index 15*/

  0x06, 0xea, 0xaa, 0xae, 0xaa, 0xaa, 0xaa, 0xaa, 0xae, 0xaa, 0xee, 0xe0, 0x00, 0x00, 
  0x0a, 0xc6, 0x66, 0x06, 0x60, 0x06, 0x66, 0x00, 0x66, 0x66, 0x66, 0xce, 0x00, 0x00, 
  0x0e, 0x06, 0x88, 0x10, 0x38, 0x89, 0x09, 0x88, 0x30, 0x00, 0x00, 0x0e, 0x00, 0x00, 
  0x0e, 0x03, 0x44, 0x20, 0x14, 0x4f, 0xdb, 0x44, 0x10, 0x00, 0x00, 0x0a, 0xec, 0x00, 
  0x0e, 0x03, 0x44, 0x20, 0x54, 0x2f, 0xff, 0x77, 0xd6, 0x00, 0x00, 0x0a, 0x6a, 0x60, 
  0x0e, 0x03, 0x44, 0x20, 0x54, 0x2f, 0xff, 0xff, 0xff, 0x7d, 0xc0, 0x0e, 0x09, 0xc0, 
  0x0e, 0x69, 0x22, 0x20, 0x54, 0x2f, 0xff, 0x77, 0xb8, 0xdb, 0xbd, 0xae, 0x09, 0xc0, 
  0x0e, 0xed, 0x44, 0xf1, 0x84, 0x7f, 0xff, 0x22, 0x10, 0x00, 0x00, 0x69, 0x09, 0xc0, 
  0x0e, 0x03, 0x42, 0x7f, 0xff, 0xff, 0xff, 0x24, 0x10, 0x00, 0x00, 0x0a, 0x09, 0xc0, 
  0x0e, 0x03, 0x42, 0xd3, 0xd7, 0xff, 0xff, 0x82, 0x10, 0x00, 0x00, 0x0a, 0xca, 0x60, 
  0x0e, 0x03, 0x44, 0x20, 0x57, 0x77, 0x7f, 0xd7, 0x10, 0x00, 0x00, 0x0a, 0x9c, 0x00, 
  0x0e, 0x06, 0x11, 0x50, 0x31, 0x1c, 0x09, 0xe1, 0x30, 0x00, 0x00, 0x0e, 0x00, 0x00, 
  0x0e, 0xc6, 0x66, 0x66, 0x66, 0x06, 0x66, 0x66, 0x66, 0x66, 0x66, 0xce, 0x00, 0x00, 
  0x00, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0xee, 0x90, 0x00, 0x00,  
};


const lv_img_dsc_t battery_50_c = {
    {
      LV_IMG_CF_INDEXED_4BIT,
        0,
        0,
        27,
        14
    },
  260,
   battery_50_c_map
};
