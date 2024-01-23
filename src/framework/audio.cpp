#include "audio.h"

// Called each frame in order to get the audio samples to play
static void _audio_play_callback(void* userdata, uint8_t* stream, int32_t len)
{
	std::unordered_map<uint32_t, AudioTrack>* playing_tracks = (std::unordered_map<uint32_t, AudioTrack>*) userdata;

	// First, clean the stream sample buffer (it will probably have teh prev sample inside)
	SDL_memset(stream, 0, len);

	std::vector<uint32_t> tracks_to_delete;

	// Iterate hrouight all the running tracks
	for (auto it = playing_tracks->begin(); it != playing_tracks->end(); ++it) {
		AudioTrack& current_track = it->second;
		if (current_track.track_position == current_track.track_lenght) {
			// If the tracks has ended and it is not a on repeat track, remove it from the playing track list
			if (current_track.repeat) {
				current_track.track_position = 0u;
			}
			else {
				tracks_to_delete.push_back(it->first);
				continue;
			}
		}
		// Compute the difference between the track lenght and the requested sample length,
		// in order to avoid a overflow
		const uint32_t diff = current_track.track_lenght - current_track.track_position;
		uint32_t increment = (diff < len) ? diff : len;
		// Mix the current sample on the buffer
		SDL_MixAudio(stream, current_track.track_buffer + current_track.track_position, increment, SDL_MIX_MAXVOLUME);
		// Advance the current track
		current_track.track_position += increment;
	}

	// Remove all the tracks after iteration throught all of them
	for (auto it = tracks_to_delete.begin(); it != tracks_to_delete.end(); ++it) {
		playing_tracks->erase(*it);
	}
}

uint32_t Audio::play(const char* audio_dir, const bool repeat_sound)
{
	std::string audio_string = std::string(audio_dir);

	uint32_t track_id = track_id_counter++;

	if (loaded_tracks.find(audio_string) != loaded_tracks.end()) {
		// If the track is already on memory, load it as is
		AudioTrack track = *loaded_tracks[audio_string];

		track.repeat = repeat_sound;
		track.track_position = 0u;

		playing_tracks[track_id] = track;
	}
	else {
		// Load and audio track
		AudioTrack* new_track = new AudioTrack();
		new_track->repeat = repeat_sound;

		SDL_AudioSpec audio_spec;

		// TODO: assert so it it not null
		SDL_LoadWAV(audio_dir, &audio_spec, &(new_track->track_buffer), &(new_track->track_lenght));

		audio_spec.callback = _audio_play_callback;
		audio_spec.userdata = (void*)&playing_tracks;

		// Initializes de audio player ofr the first time
		if (!intialized_audio_device) {
			SDL_OpenAudio(&audio_spec, NULL);

			SDL_PauseAudio(0);
			intialized_audio_device = true;
		}

		loaded_tracks[audio_string] = new_track;

		playing_tracks[track_id] = *new_track;
	}

	return track_id;
}


void Audio::stop(const uint32_t track_id)
{
	playing_tracks.erase(track_id);
}

void Audio::clean()
{
	SDL_CloseAudio();
	loaded_tracks.clear();
	playing_tracks.clear();
	track_id_counter = 0u;
	intialized_audio_device = false;
}