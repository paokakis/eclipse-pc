#ifndef FOLDERS_H
#define FOLDERS_H

#pragma comment(lib, "winmm.lib")

// Shaders
#define STANDARD_VERTEX			("Assets/droidblaster/Shaders/standard.vs")
#define STANDARD_FRAGMENT		("Assets/droidblaster/Shaders/standard.frag")
#define BILLBOARD_VERTEX		("Assets/droidblaster/Shaders/Billboard.vs")
#define BILLBOARD_FRAGMENT		("Assets/droidblaster/Shaders/Billboard.frag")
// Fonts path
#define DEFAULT_FONT_PATH		("Assets/droidblaster/fonts/BlackWolf.ttf")

// Objects paths
#define CUBE_OBJ_PATH			("Assets/droidblaster/obj/cube.obj")

#define ASTEROID1_OBJ_PATH		("Assets/droidblaster/obj/Asteroid.obj")
#define ASTEROID2_OBJ_PATH		("Assets/droidblaster/obj/asteroid_OBJ.obj")
#define ASTEROID3_OBJ_PATH		("Assets/droidblaster/obj/rock_d_06.obj")
#define ASTEROID4_OBJ_PATH		("Assets/droidblaster/obj/Meteor/Meteor.obj")

#define SUN_PATH				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Sun/Sun.3ds")
#define MOON_PATH				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/moon.3DS")
#define MARS_PATH				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Mars/Mars.3ds")
#define JUPITER_PATH			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Jupiter/Jupiter.3ds")
#define SATURN_PATH				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Saturn/Saturn.3ds")
#define EARTH_PATH				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Earth/3ds/earth.3ds")
#define MERCURY_PATH			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Mercury/mercury.3ds")
#define VENUS_PATH				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Venus/venus.3ds")
#define URANUS_PATH				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Uranus/uranus.3ds")
#define NEPTUNE_PATH			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Neptune/neptune.3ds")
#define LAND_PATH				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Landscape1/Desert_01_LOW.obj")
#define HEIGHTMAP_PATH			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/twitch/Assets/droidblaster/HeightMap.bmp")

#define SHIP_OBJ_PATH			("Assets/droidblaster/obj/SpaceShip/obj/s101-c_one_material.obj")

#define THRUSTER_A_OBJ_PATH		CUBE_OBJ_PATH

#define BULLET_PATH				("Assets/droidblaster/obj/Earth/3ds/earth.3ds")
#define ROCKET_PATH				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/twitch/Assets/droidblaster/rocket/bombs_01.3DS")

// Texture paths
#define STAR_TEXTURE			("Assets/droidblaster/star.png")
#define STARS_TEXTURE			("Assets/droidblaster/backGround/Stars.png")

#define BILLBOARD_TEXTURE		("Assets/droidblaster/ExampleBillboard.DDS")

#define CORSHAIR_TEXTURE		("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/corsHair.png")//("Assets/droidblaster/corsHair.png")

#define SKYBOX_BACK				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/SkyBox/bkg1_back.png")
#define SKYBOX_FRONT			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/SkyBox/bkg1_front.png")
#define SKYBOX_BOT				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/SkyBox/bkg1_bot.png")
#define SKYBOX_LEFT				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/SkyBox/bkg1_left.png")
#define SKYBOX_RIGHT			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/SkyBox/bkg1_right.png")
#define SKYBOX_TOP				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/SkyBox/bkg1_top.png")

#define SKYBOX1_BACK			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/SkyBox/1.png")
#define SKYBOX1_FRONT			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/SkyBox/6.png") //stays
#define SKYBOX1_BOT				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/SkyBox/3.png")
#define SKYBOX1_LEFT			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/SkyBox/4.png")
#define SKYBOX1_RIGHT			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/SkyBox/5.png")
#define SKYBOX1_TOP				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/SkyBox/2.png") //stays

#define CUBE_TEXTURE_PATH		("Assets/droidblaster/ship.png")

#define EXPLOSION_CLOUD			("Assets/droidblaster/effects/explosion/CloudPart.png")
#define EXPLOSION_PATH0			("Assets/droidblaster/effects/explosion/explosion00.png")
#define EXPLOSION_PATH1			("Assets/droidblaster/effects/explosion/explosion01.png")
#define EXPLOSION_PATH2			("Assets/droidblaster/effects/explosion/explosion02.png")
#define EXPLOSION_PATH3			("Assets/droidblaster/effects/explosion/explosion03.png")
#define EXPLOSION_PATH4			("Assets/droidblaster/effects/explosion/explosion04.png")
#define EXPLOSION_PATH5			("Assets/droidblaster/effects/explosion/explosion05.png")
#define EXPLOSION_PATH6			("Assets/droidblaster/effects/explosion/explosion06.png")
#define EXPLOSION_PATH7			("Assets/droidblaster/effects/explosion/explosion07.png")
#define EXPLOSION_PATH8			("Assets/droidblaster/effects/explosion/explosion08.png")

#define EARTH_TEXTURE			("Assets/droidblaster/obj/Earth/commonMaps/EarthMapAtmos.jpg")
#define EARTH_NORMAL			("Assets/droidblaster/obj/Earth/commonMaps/EarthNormal.jpg")
#define EARTH_NIGHT				("Assets/droidblaster/obj/Earth/commonMaps/EarthNightLights.jpg")
#define EARTH_CLOUDS			("Assets/droidblaster/obj/Earth/commonMaps/EarthClouds.jpg")

#define MOON_TEXTURE			("Assets/droidblaster/moonmap2k.jpg")
#define MOON_NORMAL				("Assets/droidblaster/obj/moon-normal.jpg")
#define MARS_TEXTURE			("Assets/droidblaster/obj/Mars/MarsMap_2500x1250.jpg")
#define MARS_NORMAL				("Assets/droidblaster/obj/Mars/MarsElevation_2500x1250.jpg")
#define JUPITER_TEXTURE			("Assets/droidblaster/obj/Jupiter/Jupiter.jpg")
//#define JUPITER_NORMAL		("Assets/droidblaster/obj/Mars/MarsElevation_2500x1250.jpg")
#define SATURN_TEXTURE			("Assets/droidblaster/obj/Saturn/Saturn.jpg")
//#define SATURN_NORMAL			("Assets/droidblaster/obj/Mars/MarsElevation_2500x1250.jpg")
#define SUN_TEXTURE				("Assets/droidblaster/obj/Sun/sun.jpg")
#define SUN_TEXTURE1			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/twitch/Assets/droidblaster/obj/Sun/sun1.png")
#define SUN_TEXTURE2			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/twitch/Assets/droidblaster/obj/Sun/sun2.png")
//#define SUN_NORMAL			("Assets/droidblaster/obj/Mars/MarsElevation_2500x1250.jpg")

#define MERCURY_TEXTURE			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Mercury/mercury.png")
#define VENUS_TEXTURE			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Venus/venus.png")
#define URANUS_TEXTURE			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Uranus/uranus.png")
#define NEPTUNE_TEXTURE			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Neptune/neptune.png")
#define LAND_TEXTURE			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded - alt/Debug/Assets/droidblaster/obj/Landscape1/Desert_01_diffuse.png")

#define ASTEROID_TEXTURE_PATH	("Assets/droidblaster/obj/Map__4_Mix.tga")
#define ASTEROID_TEXTURE_PATH3	("Assets/droidblaster/obj/Map__15_Noise.tga")
#define ASTEROID_NORMAL_PATH3	("Assets/droidblaster/obj/rock_d_06_normal.tga")
#define ASTEROID_TEXTURE_PATH4	("Assets/droidblaster/obj/Meteor/Meteor_d.tga")
#define ASTEROID_NORMAL_PATH4	("Assets/droidblaster/obj/Meteor/Meteor_n.tga")

#define SHIP_TEXTURE_PATH		("Assets/droidblaster/obj/SpaceShip/commonMaps/camo_e.tga")
#define SHIP_NORMAL_PATH		("Assets/droidblaster/obj/SpaceShip/commonMaps/s101bump.tga")

#define BULLET_TEXTURE			("Assets/droidblaster/obj/Sun/star.png")

#define GUI_BB_BOT_LEFT			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/gui/Blue/corner2.png")
#define GUI_BB_TOP_LEFT			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/gui/Blue/corner3.png")
#define GUI_BB_FILL				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/gui/Blue/fill.png")
#define GUI_BB_BOT_RIGHT		("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/gui/Blue/corner1.png")
#define GUI_BB_TOP_RIGHT		("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/gui/Blue/corner0.png")
#define GUI_BB_BOT				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/gui/Blue/border-down.png")
#define GUI_BB_TOP				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/Debug/Assets/droidblaster/gui/Blue/border-up.png")

#define BG_GALAXY				("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/twitch/Assets/droidblaster/galaxy-bg.png")

#define ROCKET_TEXTURE			("F:/vs_workspace/game_development_workspace/game_1 - backup_threaded/twitch/Assets/droidblaster/rocket/Steelplt.jpg")

// Music paths
#define BGM_MUSIC				("Assets/droidblaster/Earth_Is_All_We_Have.ogg")
#define ASTEROID_EXPL_PATH		("Assets/droidblaster/asteroid_explosion.wav")

#define SPACESHIP_COLLISION		("Assets/droidblaster/spaceship_collision.wav")
#define SPACESHIP_LASER			("Assets/droidblaster/spaceship_laser.wav")
#define SPACESHIP_EXPLOSION		("Assets/droidblaster/spaceship_explosion.wav")

#endif