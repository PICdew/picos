#define TIME_PERIOD 25

volatile char TIME_tickCounter;

typedef struct {
  char hours;
  char minutes;
  char seconds;
  char month;
  char day;
  char year; // since 2000
}TIME_t;

/**
 * Initializes time structure.
 */
extern void TIME_init(void);

/**
 * Increments time one second.
 */
extern void TIME_tick(void);

/**
 * Sets time
 */
extern void TIME_set(TIME_t *t);

/**
 * Retrieves a pointer to the current time structure.
 */
extern const TIME_t* TIME_get();

/**
 * Prints the date and time to standard output
 * using the format (decimal):
 * 
 * MM/DD
 * hh:mm
 */
extern void TIME_stdout();

/**
 * Writes the date and time to a string using
 * the format (hex):
 *
 * MM/DD
 * hh:mm
 */
extern void strtime(char *str, const TIME_t *t);
