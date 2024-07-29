#pragma once

typedef void Sound;

Sound* sound_create(const char* path);

void sound_destroy(Sound* sound);

int sound_is_playing(Sound* sound);

double sound_get_volume(Sound* sound);

double sound_get_position(Sound* sound);

void sound_set_volume(Sound* sound, double volume);

void sound_set_position(Sound* sound, double position);

void sound_play(Sound* sound);

void sound_stop(Sound* sound);

void sound_pause(Sound* sound);

void sound_resume(Sound* sound);
