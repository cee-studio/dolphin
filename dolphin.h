#ifndef DOLPHIN_H
#define DOLPHIN_H

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define DOLPHIN_BASE_URL "https://discord.com/api/v9"

struct dolphin* dolphin_init(const char token[]);
void dolphin_cleanup(struct dolphin *client);

void dolphin_POST(struct dolphin *client, const char endpoint[], const char body[]);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DOLPHIN_H
