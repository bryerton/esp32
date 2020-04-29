/*
Copyright 2020 TRIUMF

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see www.gnu.org/licenses/.
*/

#include <control.h>
#include <board.h>
#include <stdlib.h>

void control_init(control_t* control) {
  assert(control);

  if (control != NULL) {
    control->state = CONTROL_STATE_RESET;
    control->settings.mode = CONTROL_MODE_OFF;
  }
}

void control_update(control_t* control) {
  assert(control);

  if (control != NULL) {
    switch (control->state) {
      case CONTROL_STATE_RESET:
        break;
      case CONTROL_STATE_INIT:
        break;
      case CONTROL_STATE_START:
        break;
      case CONTROL_STATE_RUN:
        break;
      case CONTROL_STATE_STOP:
        break;
      case CONTROL_STATE_SHUTDOWN:
        break;
      default:
        break;
    }
  }
}
