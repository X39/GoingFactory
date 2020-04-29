#pragma once
#include <deque>

extern int DISPLAY_WIDTH;
extern int DISPLAY_HEIGHT;

int main();
extern size_t render_fps;
extern size_t simulation_fps;

#define FPS_DEQUE_MAX 300
extern std::deque<size_t> render_fps_deque;
extern std::deque<size_t> simulation_fps_deque;