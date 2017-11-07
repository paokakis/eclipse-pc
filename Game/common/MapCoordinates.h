#pragma once
#include <glm.hpp>

const glm::vec3 WORLD_DIMENS = glm::vec3(1000000);

const glm::vec3 SUN_POS = glm::vec3(-70000, 5000, 400);
const glm::vec3 SUN_SCALE = glm::vec3(100);
const glm::vec3 SUN_ROTATION = glm::vec3(0, 1, 0);
const float SUN_ROT_SPEED = 10.f;

const glm::vec3 MERCURY_POS = glm::vec3(-22000, 5000.f, 400.f);
const glm::vec3 MERCURY_SCALE = glm::vec3(1);
const glm::vec3 MERCURY_ROTATION = glm::vec3(0, 1, 0);
const float MERCURY_ROT_SPEED = 10.f;

const glm::vec3 VENUS_POS = glm::vec3(-16000, 5000.f, 400.f);
const glm::vec3 VENUS_SCALE = glm::vec3(13);
const glm::vec3 VENUS_ROTATION = glm::vec3(0, 1, 0);
const float VENUS_ROT_SPEED = 10.f;

const glm::vec3 LAND_POS = glm::vec3(-16000, 5000.f, 4000.f);
const glm::vec3 LAND_SCALE = glm::vec3(4);
const glm::vec3 LAND_ROTATION = glm::vec3(0, 1, 0);
const float LAND_ROT_SPEED = 0.f;

const glm::vec3 EARTH_POS = glm::vec3(-7000, 5000.f, 400.f);
const glm::vec3 EARTH_SCALE = glm::vec3(10, 10, 10);
const glm::vec3 EARTH_ROTATION = glm::vec3(0, 1, 0);
const float EARTH_ROT_SPEED = 10.f;

const glm::vec3 MOON_POS = glm::vec3(-1000, 5000.f, 400.f);
const glm::vec3 MOON_SCALE = glm::vec3(0.4);
const glm::vec3 MOON_ROTATION = glm::vec3(0, 1, 0);
const float MOON_ROT_SPEED = 10.f;

const glm::vec3 MARS_POS = glm::vec3(3000, 5000.f, 400.f);
const glm::vec3 MARS_SCALE = glm::vec3(1);
const glm::vec3 MARS_ROTATION = glm::vec3(0, 1, 0);
const float MARS_ROT_SPEED = 10.f;

const glm::vec3 JUPITER_POS = glm::vec3(12000, 5000.f, 400.f);
const glm::vec3 JUPITER_SCALE = glm::vec3(15);
const glm::vec3 JUPITER_ROTATION = glm::vec3(0, 1, 0);
const float JUPITER_ROT_SPEED = 10.f;

const glm::vec3 SATURN_POS = glm::vec3(27000, 5000.f, 400.f);
const glm::vec3 SATURN_SCALE = glm::vec3(15);
const glm::vec3 SATURN_ROTATION = glm::vec3(0, 1, 0);
const float SATURN_ROT_SPEED = 10.f;

const glm::vec3 URANUS_POS = glm::vec3(38000, 5000.f, 400.f);
const glm::vec3 URANUS_SCALE = glm::vec3(12);
const glm::vec3 URANUS_ROTATION = glm::vec3(0, 1, 0);
const float URANUS_ROT_SPEED = 10.f;

const glm::vec3 NEPTUNE_POS = glm::vec3(50000, 5000.f, 400.f);
const glm::vec3 NEPTUNE_SCALE = glm::vec3(14);
const glm::vec3 NEPTUNE_ROTATION = glm::vec3(0, 1, 0);
const float NEPTUNE_ROT_SPEED = 10.f;

//const glm::vec3 SUN_POS = glm::vec3(0, 10000, 4000);
//const glm::vec3 SUN_SCALE = glm::vec3(109);
//const glm::vec3 SUN_ROTATION = glm::vec3(0, 1, 0);
//const float SUN_ROT_SPEED = 10.f;
//
//const glm::vec3 MERCURY_POS = glm::vec3(4000, 10000, 4000);
//const glm::vec3 MERCURY_SCALE = glm::vec3(0.38);
//const glm::vec3 MERCURY_ROTATION = glm::vec3(0, 1, 0);
//const float MERCURY_ROT_SPEED = 10.f;
//
//const glm::vec3 VENUS_POS = glm::vec3(7000, 10000, 4000);
//const glm::vec3 VENUS_SCALE = glm::vec3(0.95);
//const glm::vec3 VENUS_ROTATION = glm::vec3(0, 1, 0);
//const float VENUS_ROT_SPEED = 10.f;
//
//const glm::vec3 EARTH_POS = glm::vec3(10000, 10000, 4000.f);
//const glm::vec3 EARTH_SCALE = glm::vec3(1, 1, 1);
//const glm::vec3 EARTH_ROTATION = glm::vec3(0, 1, 0);
//const float EARTH_ROT_SPEED = 10.f;
//
//const glm::vec3 MOON_POS = glm::vec3(5280, 1500, 400.f);
//const glm::vec3 MOON_SCALE = glm::vec3(1, 1, 1);
//const glm::vec3 MOON_ROTATION = glm::vec3(0, 1, 0);
//const float MOON_ROT_SPEED = 10.f;
//
//const glm::vec3 MARS_POS = glm::vec3(25000, 5000.f, 400.f);
//const glm::vec3 MARS_SCALE = glm::vec3(8, 8, 8);
//const glm::vec3 MARS_ROTATION = glm::vec3(0, 1, 0);
//const float MARS_ROT_SPEED = 10.f;
//
//const glm::vec3 JUPITER_POS = glm::vec3(25000, 15000.f, 400.f);
//const glm::vec3 JUPITER_SCALE = glm::vec3(4, 4, 4);
//const glm::vec3 JUPITER_ROTATION = glm::vec3(0, 1, 0);
//const float JUPITER_ROT_SPEED = 10.f;
//
//const glm::vec3 SATURN_POS = glm::vec3(25000, 45000.f, 400.f);
//const glm::vec3 SATURN_SCALE = glm::vec3(2, 2, 2);
//const glm::vec3 SATURN_ROTATION = glm::vec3(0, 1, 0);
//const float SATURN_ROT_SPEED = 10.f;

const glm::vec3 STARFIELD_DIMENSIONS_LOW = glm::vec3(-5000, -5000, -5000);
const glm::vec3 STARFIELD_DIMENSIONS_HIGH = glm::vec3(50000, 50000, 50000);