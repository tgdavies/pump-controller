#define AVERAGE_COUNT (4)

#define MAX_SPEED UINT8_C(240)
#define STOP_SPEED UINT8_C(120)

typedef struct _esc {
	uint8_t port;
	uint8_t pin;
	uint8_t drive;
} ESC;

/**
 * Set up the output pins for the ESCs
 * This *must* be called first
 */
void setupEscs();


/**
 * Send the ESC a servo control signal, then delay for 20ms
 */
void serviceEscs();

/**
 * Send the ESCs the max forward, max backwards and center signals
 */
void calibrateEscs();

