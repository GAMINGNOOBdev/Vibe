#ifndef __GAMING_H_
#define __GAMING_H_ 1

void switch_to_gaming(const char* beatmap_folder, const char* beatmap_path);

void gaming_init(void);
void gaming_dispose(void);
void gaming_update(float delta);
void gaming_render(void);

#endif
