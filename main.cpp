#include <iostream>
#include <SDL.h>

#define AUDIO_COMPLETE "assets/Uber_Everywhere.wav"
#define AUDIO_TRAVIS "assets/Uber_Everywhere_Travis.wav"
#define AUDIO_MADEINTYO "assets/Uber_Everywhere_MadeinTYO.wav"

using namespace std;

struct AudioData {
	Uint8 *pos;
	Uint32 length;
};

Uint8* ModifySamples(float pitch, Uint8 *stream, Uint8 *startPos, int streamLength) {
	//float *stream = (float*)stream;
	Uint32 length = (Uint32)streamLength;
	Sint16 *samples = (Sint16*)startPos;
	float sampleIndex = 0;

	int volume = 100;
	float factor = volume * 1.0f / 32768.0f;
	
	for (Uint32 i = 0; i < length; i++) {
		stream[i] = samples[(size_t)sampleIndex] * factor;
		sampleIndex += pitch;
	}

	return stream;
}

void AudioCallback(void *userdata, Uint8 *stream, int streamLength) {
	AudioData *audio = (AudioData*)userdata;
	
	if (audio->length == 0) return;

	Uint32 length = (Uint32)streamLength;
	length = length > audio->length ? audio->length : length;

	Uint8 *modifiedSamplesPos = ModifySamples(1.0f, stream, audio->pos, length);

	//SDL_memcpy(stream, audio->pos, length);
	audio->pos += length;
	audio->length -= length;
}

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_AUDIO);

	SDL_AudioSpec wavSpec;
	Uint8 *wavStart;
	Uint32 wavLength;

	if (SDL_LoadWAV(AUDIO_TRAVIS, &wavSpec, &wavStart, &wavLength) == NULL) {
		cerr << SDL_GetError() << endl;
		return -1;
	}

	//wavLength = ModifySamples(, wavStart, wavLength, 2.0);

	AudioData audio = { wavStart, wavLength };
	wavSpec.callback = AudioCallback;
	wavSpec.userdata = &audio;

	SDL_AudioDeviceID deviceID = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
	if (!deviceID) {
		cerr << SDL_GetError() << endl;;
		return -1;
	}
	else {
		cout << "Frequency: " << wavSpec.freq << endl
			 << "Format: " << wavSpec.format << endl
			 << "Samples: " << wavSpec.samples << endl
			 << "-------------------------------------" << endl;
	}

	SDL_PauseAudioDevice(deviceID, 0);

	while (audio.length > 0) SDL_Delay(100);

	SDL_CloseAudioDevice(deviceID);
	SDL_FreeWAV(wavStart);
	SDL_Quit();
	return 0;
}

