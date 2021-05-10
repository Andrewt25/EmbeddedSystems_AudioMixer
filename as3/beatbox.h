#ifndef BEATBOX_CONTROL
#define BEATBOX_CONTROL

enum eInstruments {
    BASE = 0,
    HIHAT,
    SNARE,
};
void Beatbox_init(void);
void Beatbox_cleanup(void);

void Beatbox_cycleBeat(void);
void Beatbox_setMode(int mode);
int Beatbox_getMode(void);

int Beatbox_increaseVolume(void);
int Beatbox_decreaseVolume(void);

int Beatbox_increaseBpm(void);
int Beatbox_decreaseBpm(void);
int Beatbox_getBpm(void);

void Beatbox_playSound(enum eInstruments drum);

#endif