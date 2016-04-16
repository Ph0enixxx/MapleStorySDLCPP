#pragma once
enum PLAYER_STATE {
	walk_left, walk_right,
	walk_front, walk_back,

	idle_left, idle_right,
	idle_front, idle_back
};

class PLAYER_ANIMATION_DATA {
public:
	string anim_play_name;
	string bmp_path;
};

class PLAYER_STATES {
public:
	bool walking = false;
};

class PLAYER : public SPRITE_IMAGE {


public:
	bool moving = false;
	bool moveFix = false;
	SDL_Rect player_pos[10];
	SDL_Rect ppos;
	SDL_Rect sprites_idle_left[10];
	SDL_Rect sprites_idle_right[10];
	SDL_Rect sprites_idle_front[10];
	SDL_Rect sprites_idle_back[10];

	SDL_Rect sprites_walk_front[10];
	SDL_Rect sprites_walk_back[10];
	SDL_Rect sprites_walk_left[10];
	SDL_Rect sprites_walk_right[10];


	PLAYER_STATES states;

	float animation_scrub = 1.0f;

	void addAnimation(SDL_Rect* clip, int row, int cnt, int w, int h)
	{
		for (int i = 0; i < cnt; i += 1) {
			clip[i].x = 0 + i * w;
			clip[i].y = row * h;
			clip[i].w = w;
			clip[i].h = h;
		}
	}

	void walkLeft() {
		this->ppos.x--;
	}

	void walkRight() {
		this->ppos.x++;
	}

	void walkFront() {
		this->ppos.y--;
	}

	void walkBack() {
		this->ppos.y++;
	}



	PLAYER_STATE player_state = idle_front;

	PLAYER(Uint32 color, int x, int y, int sprite_width = 46, int sprite_height = 78) : SPRITE_IMAGE(color, x, y, sprite_width, sprite_height) {
		//update_properties();
		for (int i = 0; i < 9; i += 1) {
			player_pos[i].h = 64;
			player_pos[i].w = 64;
			player_pos[i].x = 10;
			player_pos[i].y = 10;
		}
		

		addAnimation(sprites_idle_left, 1, 9, 64, 64);
		addAnimation(sprites_idle_right, 3, 6, 64, 64);
		addAnimation(sprites_idle_front, 0, 6, 64, 64);
		addAnimation(sprites_idle_back, 2, 6, 64, 64);

		addAnimation(sprites_walk_right, 11, 9, 64, 64);
		addAnimation(sprites_walk_left, 9, 9, 64, 64);
		addAnimation(sprites_walk_front, 8, 9, 64, 64);
		addAnimation(sprites_walk_back, 10, 9, 64, 64);
	}

	void displayPlayer(float frame, SDL_Surface*surface) {
		switch (player_state)
		{

		default:
		case idle_front:
			SDL_BlitSurface(sprite_image, &sprites_idle_front[static_cast<int>(frame)], surface, &ppos);
			break;

		case idle_left:
			SDL_BlitSurface(sprite_image, &sprites_idle_left[static_cast<int>(frame)], surface, &ppos);
			break;

		case idle_right:
			SDL_BlitSurface(sprite_image, &sprites_idle_right[static_cast<int>(frame)], surface, &ppos);
			break;

		case idle_back:
			SDL_BlitSurface(sprite_image, &sprites_idle_back[static_cast<int>(frame)], surface, &ppos);
			break;

		case walk_front:
			SDL_BlitSurface(sprite_image, &sprites_walk_front[static_cast<int>(frame)], surface, &ppos);
			break;

		case walk_left:
			SDL_BlitSurface(sprite_image, &sprites_walk_left[static_cast<int>(frame)], surface, &ppos);
			break;

		case walk_right:
			SDL_BlitSurface(sprite_image, &sprites_walk_right[static_cast<int>(frame)], surface, &ppos);
			break;

		case walk_back:
			SDL_BlitSurface(sprite_image, &sprites_walk_back[static_cast<int>(frame)], surface, &ppos);
			break;
		}
	}

};