#ifndef DOLPHIN_H
#define DOLPHIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "logconf.h"

struct dolphin* dolphin_init(const char token[]);
void dolphin_cleanup(struct dolphin *client);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DOLPHIN_H
