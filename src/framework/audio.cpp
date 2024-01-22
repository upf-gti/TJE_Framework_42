#include "audio.h"
//
//void _audio_loaded_callback(void* userdata, uint8_t* stream, int32_t len) {
//	AudioTrack* current_track = (AudioTrack*)userdata;
//	//SDL_memcpy (stream, audio_pos, len); 					// simply copy from one buffer into the other
//	current_track->track_len = len;
//	SDL_MixAudio(stream, current_track->track_buffer, len, SDL_MIX_MAXVOLUME);
//}
//
//inline uint32_t Audio::play_audio(const char* audio_dir) {
//	AudioTrack *new_track = new AudioTrack();
//	SDL_AudioSpec audio_spec;
//	//audio_spec.callback = _audio_loaded_callback;
//	audio_spec.userdata = new_track;
//
//	// TODO: assert so it it not null
//	//SDL_LoadWAV(audio_dir, &audio_spec, &(new_track->track_buffer), &(new_track->track_len));
//
//	return 0;
//}
//
//inline void	Audio::stop_audio(const uint32_t track_id) {}