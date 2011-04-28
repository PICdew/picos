#define TIME_PERIOD 25

unsigned char TIME_tickCounter;

typedef struct {
  char hours;
  char minutes;
  char seconds;
  char month;
  char day;
  char year; // since 2000
}TIME_t;

extern void TIME_init(void);
extern void TIME_tick(void);
