/* ScummVM - Scumm Interpreter
 * Dreamcast port
 * Copyright (C) 2002  Marcus Comstedt
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

#include "stdafx.h"
#include "scumm.h"
#include "gui.h"
#include "sound.h"
#include "dc.h"

Scumm scumm;
ScummDebugger debugger;
Gui gui;

SoundEngine sound;
SOUND_DRIVER_TYPE snd_driv;


void waitForTimer(Scumm *s, int time)
{
  if(time<0)
    return;
  unsigned int start = Timer();
  unsigned int devpoll = start+USEC_TO_TIMER(25000);
  unsigned int t;
  int oldmousex = s->mouse.x, oldmousey = s->mouse.y;
  time = (((unsigned int)time)*100000U)>>11;
  int mask = getimask();
  while(((int)((t = Timer())-start))<time)
    if(((int)(t-devpoll))>0) {
      setimask(15);
      checkSound(s);
      handleInput(locked_get_pads(), s->mouse.x, s->mouse.y,
		  s->_leftBtnPressed, s->_rightBtnPressed, s->_keyPressed);
      setimask(mask);
      devpoll += USEC_TO_TIMER(17000);
      if(s->mouse.x != oldmousex || s->mouse.y != oldmousey) {
	updateScreen(s);
	oldmousex = s->mouse.x;
	oldmousey = s->mouse.y;
      }
    }
}

static char *argv[] = { "scummvm", NULL, NULL };
static int argc = 2;

int main()
{
  int delta,tmp;
  int last_time, new_time;

#ifndef NOSERIAL
  serial_init(57600);
  usleep(2000000);
  printf("Serial OK\r\n");
#endif

  cdfs_init();
  maple_init();
  dc_setup_ta();
  init_arm();

  if(!selectGame(&scumm, argv[1]))
    exit(0);

  sound.initialize(&scumm, &snd_driv);
  printf("Sound initalized.\n");

  scumm._gui = &gui;
  scumm.scummMain(argc, argv);

  printf("scummMain called.\n");
  gui.init(&scumm);

  last_time = Timer();
  delta = 0;
  do {
    updateScreen(&scumm);

    new_time = Timer();
    waitForTimer(&scumm, delta * 15 - ((new_time - last_time)<<11)/100000);
    last_time = Timer();

    if (gui._active) {
      gui.loop();
      delta = 5;
    } else {
      delta = scumm.scummLoop(delta);
    }
  } while(1);
  
  printf("All done.  Returning to menu.\n");
  exit(0);
}
