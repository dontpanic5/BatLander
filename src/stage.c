#include "stage.h"

const double ROTATION_TICK = ROTATION_SPEED * GAME_LOOP_FRACTION;

const double GRAVITY = GRAVITY_METERS * M_TO_P;
const int FLAP_FORCE = FLAP_FORCE_METERS * M_TO_P;

// we work backwards to get the drag force instead of computing it. Too much work!
// this gives us a magic number that is our computed drag that gives us our desired terminal velocity.
double const MAGIC_DRAG_NUMBER = GRAVITY_METERS / DESIRED_TERMINAL_VELOCITY_METERS;

static void logic(void);
static void draw(void);
static void initPlayer();

static Entity* player;
static SDL_Texture* playerTexture;

void initStage(void)
{
	app.delegate.logic = logic;
	app.delegate.draw = draw;

	playerTexture = loadTexture("gfx/bat.png");

	initPlayer();
}

static void initPlayer()
{
	player = malloc(sizeof(Entity));
	memset(player, 0, sizeof(Entity));

	player->texture = playerTexture;
	player->pos.x = 100;
	player->pos.y = 100;
}

static void logic(void)
{
	// track player's acceleration this frame. Always start with gravity
	DoubleVector playerAcceleration = { 0, GRAVITY };
	double playerDrag = player->velocity.y * MAGIC_DRAG_NUMBER;
	playerAcceleration.y += playerDrag;

	if (app.right)
		player->rotation += ROTATION_TICK;
	if (app.left)
		player->rotation -= ROTATION_TICK;
	// TODO at this point I might want to bound the possible rotation values

	if (app.up)
	{
		double rot = player->rotation;
		if (rot > 90 && rot <= 180)
		{
			double diff = rot - 90;
			rot = 90 - diff;
		}
		// TODO I have the x accel working from 90 to 180 but the y is messed up in that range
		// TODO I also need to check negative rotations
		// subtract on the x axis
		playerAcceleration.x -= sin(rot / 180) * FLAP_FORCE;
		playerAcceleration.y += cos(rot / 180) * FLAP_FORCE;
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Accel: X Calc: %f", sin(rot / 180));
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Accel: X: %f Y: %f ROT: %f", playerAcceleration.x, playerAcceleration.y, rot);
	}

	player->velocity.x += playerAcceleration.x * GAME_LOOP_FRACTION;
	player->velocity.y += playerAcceleration.y * GAME_LOOP_FRACTION;

	player->pos.x += (int)round(player->velocity.x * GAME_LOOP_FRACTION);
	player->pos.y += (int)round(player->velocity.y * GAME_LOOP_FRACTION);

	// TODO check bounds of window

	
}

static void draw()
{
	blit(player->texture, player->pos.x, player->pos.y, player->rotation, .25);
}