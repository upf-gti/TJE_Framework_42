#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <string>

#include "framework.h"

#include <iostream>

struct AudioTrack {
	uint8_t* track_buffer = nullptr;
	uint32_t track_lenght = 0u;
	uint32_t track_position = 0u;
	bool repeat = false;
};

static void _audio_play_callback(void* userdata, uint8_t* stream, int32_t len);

class Audio {
	// Store all the loaded tracks
	std::unordered_map<std::string, AudioTrack*> loaded_tracks;

	bool intialized_audio_device = false;

	uint32_t track_id_counter = 0u;

public:
	// Needs to be public in order for it to be read on the callback
	std::unordered_map<uint32_t, AudioTrack> playing_tracks;

	uint32_t play(const char* audio_dir, const bool repeat_sound = false);
	
	void stop(const uint32_t track_id);

	void clean();
};