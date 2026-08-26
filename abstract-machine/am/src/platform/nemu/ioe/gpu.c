#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

int w, h;
void __am_gpu_init() {
	w = inl(VGACTL_ADDR) >> 16;
	h = inl(VGACTL_ADDR) & 0xffff;
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
	uint32_t wh = inl(VGACTL_ADDR);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = wh >> 16, .height = wh & 0xffff,
    .vmemsz = 0
  };
	cfg->vmemsz = cfg->width * cfg->height;
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
	int i ,j;
	uint32_t *canvas = ctl->pixels;
	if (ctl->pixels) {
		for (i = 0; i < ctl->h; i++) {
			for (j = 0; j < ctl->w; j++) {
				outl(FB_ADDR + ((ctl->y + i) * w + ctl->x + j) * 4, canvas[i * ctl->w + j]);
			}
		}
	}
  outl(SYNC_ADDR, ctl->sync);
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
