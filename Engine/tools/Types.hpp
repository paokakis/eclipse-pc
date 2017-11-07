#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
#include <cstdlib>

#define RANDOM(pMax) (float(pMax) * float(rand()) / float(RAND_MAX))
#define GAME_TITLE		"Eclipse 2147"

typedef int32_t status;

const status STATUS_OK = 0;
const status STATUS_KO = -1;
const status STATUS_EXIT = -2;

#endif