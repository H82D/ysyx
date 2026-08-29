/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <common.h>
#include <device/map.h>
#include <SDL2/SDL.h>

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
	reg_start,
	reg_end,
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;
static SDL_AudioSpec s = {};

void audio_callback(void *userdata, Uint8 *stream, int len) {
	SDL_memset(stream, 0, len);
	if (!audio_base[reg_count]) return;
	len = (len > audio_base[reg_count]) ? audio_base[reg_count] : len;
	SDL_MixAudio(stream, sbuf + audio_base[reg_start], len, SDL_MIX_MAXVOLUME);
	audio_base[reg_start] += len;
	if (audio_base[reg_start] >= CONFIG_SB_SIZE) audio_base[reg_start] -= CONFIG_SB_SIZE;
	audio_base[reg_count] -= len;
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
	if (audio_base[reg_init] && is_write) {
		s.format = AUDIO_S16SYS;
		s.userdata = NULL;
		s.freq = audio_base[reg_freq];
		s.channels = audio_base[reg_channels];
		s.samples = audio_base[reg_samples];
		s.callback = audio_callback;
		SDL_InitSubSystem(SDL_INIT_AUDIO);
		SDL_OpenAudio(&s, NULL);
		SDL_PauseAudio(0);
	}
}

static void audio_play_handler(uint32_t offset, int len, bool is_write) {
	if (is_write) {
		audio_base[reg_end] += len;
		if (audio_base[reg_end] >= CONFIG_SB_SIZE) audio_base[reg_end] -= CONFIG_SB_SIZE;
		audio_base[reg_count] += len;
	}
}

void init_audio() {
	SDL_Init(SDL_INIT_AUDIO);
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (uint32_t *)new_space(space_size);
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif
  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, audio_play_handler);
	audio_base[reg_sbuf_size] = CONFIG_SB_SIZE;
	audio_base[reg_start] = 0;
	audio_base[reg_end] = 0;
}
