#ifndef FRANK_PCM5122_H
#define FRANK_PCM5122_H

#include <stdbool.h>

/* Configure the PCM5122 for 16-bit, BCK-referenced slave-mode I2S. */
bool pcm5122_init(void);

#endif
