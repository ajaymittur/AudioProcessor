#include <iostream>
#include <SDL.h>

#define AUDIO_FILE "assets/Uber_Everywhere.wav"

using namespace std;

struct AudioData {
	Uint8 *pos;
	Uint32 length;
};

void AudioCallback(void *userdata, Uint8 *stream, int streamLength) {
	AudioData *audio = (AudioData *)userdata;
	
	if (audio->length == 0) return;

	Uint32 length = (Uint32)streamLength;
	length = length > audio->length ? audio->length : length;

	SDL_memcpy(stream, audio->pos, length);
	audio->pos += length;
	audio->length -= length;
}

int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_AUDIO);

	SDL_AudioSpec wavSpec;
	Uint8 *wavStart;
	Uint32 wavLength;

	if (SDL_LoadWAV(AUDIO_FILE, &wavSpec, &wavStart, &wavLength) == NULL) {
		cerr << SDL_GetError() << endl;
		return -1;
	}

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
			 << "Samples: " << wavSpec.samples << endl;
	}

	SDL_PauseAudioDevice(deviceID, 0);

	while (audio.length > 0) SDL_Delay(100);

	SDL_CloseAudioDevice(deviceID);
	SDL_FreeWAV(wavStart);
	SDL_Quit();
	return 0;
}

