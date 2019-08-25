#include <iostream>
#include <SDL.h>

using namespace std;

struct AudioData {
	Uint8 *pos;
	Uint8 *end;
	Uint32 length;
	float pitch;
};

void ChangePitch(void *userdata, Uint8 *stream, Uint32 length) {
	AudioData *audio = (AudioData *)userdata;

	Uint32 remainingLength = (Uint32)((audio->end - audio->pos)/audio->pitch);
	length = length > remainingLength ? remainingLength : length;

	Sint16 *samples = (Sint16*)audio->pos;
	float sampleIndex = 0;

	int volume = 100;
	float factor = volume / 32768.0f; // Normalize range to [-1, 1]

	for (Uint32 i = 0; i < length; i++) {
		stream[i] = samples[(size_t)sampleIndex] * factor;
		sampleIndex += audio->pitch;
	}
}

void AudioCallback(void *userdata, Uint8 *stream, int streamLength) {
	AudioData *audio = (AudioData*)userdata;
	Uint32 length = (Uint32)streamLength;
	
	if (audio->length == 0) return;

	ChangePitch(userdata, stream, length);
	
	audio->pos += length;
	audio->length -= length;
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Usage: AudioProcessor [absolute filepath] [x pitch] \n\n");
		return -1;
	}

	SDL_Init(SDL_INIT_AUDIO);
	char *filename = argv[1];
	float pitch = atof(argv[2]) * 0.5f; // 0.5 is the default pitch for the audio file

	SDL_AudioSpec wavSpec;
	Uint8 *wavStart;
	Uint32 wavLength;

	if (SDL_LoadWAV(filename, &wavSpec, &wavStart, &wavLength) == NULL) {
		cerr << SDL_GetError() << endl;
		return -1;
	}

	AudioData audio = { wavStart,  wavStart + wavLength, wavLength, pitch };
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

