/*
 * beatdetector.h
 *
 * Created: 25/03/2021 20:53:09
 *  Author: eduardo
 */ 


#ifndef BEATDETECTOR_H_
#define BEATDETECTOR_H_


#define BEATDETECTOR_STATE_INIT 1
#define BEATDETECTOR_STATE_WAITING 2
#define BEATDETECTOR_STATE_FOLLOWING_SLOPE 3
#define BEATDETECTOR_STATE_MAYBE_DETECTED 4
#define BEATDETECTOR_STATE_MASKING 5

#define BEATDETECTOR_INIT_HOLDOFF                2000    // in ms, how long to wait before counting
#define BEATDETECTOR_MASKING_HOLDOFF             200     // in ms, non-retriggerable window after beat detection
#define BEATDETECTOR_BPFILTER_ALPHA              0.6     // EMA factor for the beat period value
#define BEATDETECTOR_MIN_THRESHOLD               20      // minimum threshold (filtered) value
#define BEATDETECTOR_MAX_THRESHOLD               800     // maximum threshold (filtered) value
#define BEATDETECTOR_STEP_RESILIENCY             30      // maximum negative jump that triggers the beat edge
#define BEATDETECTOR_THRESHOLD_FALLOFF_TARGET    0.3     // thr chasing factor of the max value when beat
#define BEATDETECTOR_THRESHOLD_DECAY_FACTOR      0.99    // thr chasing factor when no beat
#define BEATDETECTOR_INVALID_READOUT_DELAY       2000    // in ms, no-beat time to cause a reset
#define BEATDETECTOR_SAMPLES_PERIOD              10      // in ms, 1/Fs


typedef struct {
	uint32_t state;
	float threshold;
	float beatPeriod;
	float lastMaxValue;
	uint32_t tsLastBeat;
} beatdetector;

void beatdetector_init(beatdetector* beat);
uint32_t beatdetector_check_for_beat(beatdetector* beat, float sample);
void beatdetector_decrease_threshold(beatdetector* beat);
uint32_t beatdetector_add_sample(beatdetector* beat, float sample);
float beatdetector_get_rate(beatdetector* beat);



#endif /* BEATDETECTOR_H_ */