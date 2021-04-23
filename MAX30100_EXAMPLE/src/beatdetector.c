/*
 * beatdetector.c
 *
 * Created: 25/03/2021 20:52:48
 *  Author: eduardo
 */ 
#include <asf.h>
#include "beatdetector.h"
#include "time_utils.h"

void beatdetector_init(beatdetector* beat) {
	beat->state = BEATDETECTOR_STATE_INIT;
	beat->threshold = BEATDETECTOR_MIN_THRESHOLD * 1.0f;
	beat->beatPeriod = 0.0f;
	beat->lastMaxValue = 0.0f;
	beat->tsLastBeat = 0;
}

void beatdetector_decrease_threshold(beatdetector* beat)
{
	// When a valid beat rate readout is present, target the
	if (beat->lastMaxValue > 0 && beat->beatPeriod > 0) {
		beat->threshold -= beat->lastMaxValue * (1.0f - BEATDETECTOR_THRESHOLD_FALLOFF_TARGET) /
		(beat->beatPeriod / (BEATDETECTOR_SAMPLES_PERIOD * 1.0f));
		} else {
		// Asymptotic decay
		beat->threshold *= BEATDETECTOR_THRESHOLD_DECAY_FACTOR;
	}

	if (beat->threshold < BEATDETECTOR_MIN_THRESHOLD) {
		beat->threshold = BEATDETECTOR_MIN_THRESHOLD * 1.0f;
	}
}

uint32_t beatdetector_check_for_beat(beatdetector* beat, float sample)
{
	uint32_t beatDetected = 0;

	switch (beat->state) {
		case BEATDETECTOR_STATE_INIT:
		if (systick_get_counter() > BEATDETECTOR_INIT_HOLDOFF) {
			beat->state = BEATDETECTOR_STATE_WAITING;
		}
		break;

		case BEATDETECTOR_STATE_WAITING:
		if (sample > beat->threshold) {
			beat->threshold = min(sample, BEATDETECTOR_MAX_THRESHOLD);
			beat->state = BEATDETECTOR_STATE_FOLLOWING_SLOPE;
		}

		// Tracking lost, resetting
		if (systick_get_counter() - beat->tsLastBeat > BEATDETECTOR_INVALID_READOUT_DELAY) {
			beat->beatPeriod = 0;
			beat->lastMaxValue = 0;
		}

		beatdetector_decrease_threshold(beat);
		break;

		case BEATDETECTOR_STATE_FOLLOWING_SLOPE:
		if (sample < beat->threshold) {
			beat->state = BEATDETECTOR_STATE_MAYBE_DETECTED;
			} else {
			beat->threshold = min(sample, BEATDETECTOR_MAX_THRESHOLD);
		}
		break;

		case BEATDETECTOR_STATE_MAYBE_DETECTED:
		if (sample + BEATDETECTOR_STEP_RESILIENCY < beat->threshold) {
			// Found a beat
			beatDetected = true;
			beat->lastMaxValue = sample;
			beat->state = BEATDETECTOR_STATE_MASKING;
			float delta = systick_get_counter() - beat->tsLastBeat;
			if (delta) {
				beat->beatPeriod = BEATDETECTOR_BPFILTER_ALPHA * delta +
				(1.0f - BEATDETECTOR_BPFILTER_ALPHA) * beat->beatPeriod;
			}

			beat->tsLastBeat = systick_get_counter();
			} else {
			beat->state = BEATDETECTOR_STATE_FOLLOWING_SLOPE;
		}
		break;

		case BEATDETECTOR_STATE_MASKING:
		if (systick_get_counter() - beat->tsLastBeat > BEATDETECTOR_MASKING_HOLDOFF) {
			beat->state = BEATDETECTOR_STATE_WAITING;
		}
		beatdetector_decrease_threshold(beat);
		break;
	}

	return beatDetected;
}

uint32_t beatdetector_add_sample(beatdetector* beat, float sample) {
	return beatdetector_check_for_beat(beat, sample);
}

float beatdetector_get_rate(beatdetector* beat) {
	if (beat->beatPeriod != 0) {
		return 1.0 / beat->beatPeriod * 1000.0 * 60.0;
	} else {
		return 0;
	}
}
