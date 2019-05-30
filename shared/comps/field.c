/*
* This file is part of the stmbl project.
*
* Copyright (C) 2013-2016 Rene Hopf <renehopf@mac.com>
* Copyright (C) 2013-2016 Nico Stute <crinq@crinq.de>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You shou l have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "commands.h"
#include "hal.h"
#include "math.h"
#include "defines.h"
#include "angle.h"


HAL_COMP(field);

//in
HAL_PIN(vel_n);
HAL_PIN(i_n);
HAL_PIN(t_n);
HAL_PIN(vel_boost);
HAL_PIN(t_boost);

HAL_PIN(vel);

//out
HAL_PIN(id);
HAL_PIN(iq_max);
HAL_PIN(iq_min);

HAL_PIN(t_max);
HAL_PIN(t_min);

static void nrt_init(void *ctx_ptr, hal_pin_inst_t *pin_ptr) {
  // struct field_ctx_t *ctx      = (struct field_ctx_t *)ctx_ptr;
  struct field_pin_ctx_t *pins = (struct field_pin_ctx_t *)pin_ptr;
  PIN(vel_n) = 50.0 * 2.0 * M_PI / 2.0;
  PIN(i_n) = 15.0;
  PIN(t_n) = 23.0;
  PIN(vel_boost) = 1.0;
  PIN(t_boost) = 1.5;
}

static void rt_func(float period, void *ctx_ptr, hal_pin_inst_t *pin_ptr) {
  // struct field_ctx_t *ctx      = (struct field_ctx_t *)ctx_ptr;
  struct field_pin_ctx_t *pins = (struct field_pin_ctx_t *)pin_ptr;

  float vel = MAX(ABS(PIN(vel)), 0.01);
  float i_n = PIN(i_n);
  float t_n = MAX(PIN(t_n), 0.01);
  float vel_n = PIN(vel_n);
  float t_boost = PIN(t_boost);
  float vel_boost = PIN(vel_boost);
  
  // float p_max = vel_n * t_n * vel_boost;
  // float t_max = MIN(t_n * t_boost, p_max / vel);

  float id_n = i_n / sqrtf(2.0);
  float id = MIN(id_n, id_n * vel_n * vel_boost / vel);

  float iq_max = 0;
  float iq_min = 0;
  
  if(PIN(vel) > 0.0){
    iq_max = MIN(id_n * t_boost, id_n * vel_n * vel_boost / vel);
    iq_min = -id_n * t_boost;
  }
  else{
    iq_max = id_n * t_boost;
    iq_min = -MIN(id_n * t_boost, id_n * vel_n * vel_boost / vel);
  }

  float t_max = iq_max * id / id_n / id_n * t_n;
  float t_min = iq_min * id / id_n / id_n * t_n;
  
  PIN(id) = id;
  PIN(iq_max) = iq_max;
  PIN(iq_min) = iq_min;
  PIN(t_max) = t_max;
  PIN(t_min) = t_min;
   
}

hal_comp_t field_comp_struct = {
    .name      = "field",
    .nrt       = 0,
    .rt        = rt_func,
    .frt       = 0,
    .nrt_init  = nrt_init,
    .rt_start  = 0,
    .frt_start = 0,
    .rt_stop   = 0,
    .frt_stop  = 0,
    .ctx_size  = 0,
    .pin_count = sizeof(struct field_pin_ctx_t) / sizeof(struct hal_pin_inst_t),
};
