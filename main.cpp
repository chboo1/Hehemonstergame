#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <iostream>
#include <stdlib.h>
#define STAND 0
#define WALK 1
#define RUN 2
#define WANDER 0 // Wander state for monster
#define INVESTIGATE 1 // Monster investigates footsteps
#define HUNT 2 // Monster chases player in view


class Main
{
public:
  // Class functions and stuff
  bool init();
  void gameloop();
  SDL_Surface* loadSurface(std::string path);
  bool loadGraphics();
  void close();
  bool isColliding(SDL_Rect rect1, SDL_Rect rect2);
  bool isNear1(SDL_Rect rect1, SDL_Rect rect2);
  // SDL stuff
  SDL_Window* window;
  SDL_Surface* screenSurface;
  SDL_Surface* player;
  SDL_Surface* steps;
  SDL_Surface* monster;
  SDL_Surface* win;
  SDL_Surface* lose;
  SDL_Surface* gift;
	SDL_Surface* alert;
  SDL_Rect p, ps, m, ms, g, pr, pr2, temp;
  SDL_Event e;
  // Class variables
  // Ints
  int exit = 0;
  const int basespeed = 4;
  double speed = basespeed;
  const int WIDTH = 1000;
  const int HEIGHT = 1000;
  int state = 0;
  int footstepspos[50][4]; // First list means 50 total footsteps on screen and list 2 means it stores x, y and angle.
  int footsteps = 0;
  int stepTimer = 10;
  int monsterState = 0;
  int monsterInvestigationPos[2] = {NULL,NULL};
  int giftProgress = 150;
	int alertTimer = 6;
	int monsterfstep[4] = {0, 0, 0, 0};
	int radarA = 0;
	int radarB = 46;
	int radarTimer = 12;
  // Bools
  bool quit = false;
  bool aPressed = false;
  bool dPressed = false;
  bool sPressed = false;
  bool wPressed = false;
	bool shift = false;
	bool radarDir = true;
  Main()
  {
    srand(time(0));
    p.x = WIDTH / 2;
    p.y = HEIGHT / 2;
    p.w = 50;
    p.y = 50;
    ps.w = 50;
    ps.h = 50;
    ps.y = 50;
    ps.x = 50;
    m.w = 50;
    m.h = 50;
    m.x = 0;
    m.y = 0;
    ms.x = 50;
    ms.y = 50;
    ms.w = 50;
    ms.h = 50;
    g.w = 100;
    g.h = 100;
    g.x = rand() % WIDTH - g.w;
		do {
			g.y = rand() % HEIGHT - g.h;
		} while (g.y < 100);
    if (!init())
    {
      exit = 1;
    }
    else
    {
      if (!loadGraphics())
      {
        exit = 1;
      }
      else
      {
        while (!quit)
        {
          gameloop();
        }
        close();
      }
    }
  }
};


bool Main::init()
{
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    std::cerr << "Failed to initialize SDL" << std::endl;
    return false;
  }
  else
  {
    window = SDL_CreateWindow("Hehe funny monster game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
      std::cerr << "Failed to create window" << std::endl;
      return false;
    }
    else
    {
      if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
      {
        std::cerr << "SDL_image failed to initialize" << std::endl;
        return false;
      }
      else
      {
        screenSurface = SDL_GetWindowSurface(window);
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
      }
    }
  }
  return true;
}


SDL_Surface* Main::loadSurface(std::string path)
{
  SDL_Surface* finalSurface = NULL;
  SDL_Surface* loadedSurface = IMG_Load(path.c_str());
  if (loadedSurface == NULL)
  {
    std::cerr << "Couldn't load image at " << path << "!" << std::endl;
    return NULL;
  }
  else
  {
    finalSurface = SDL_ConvertSurface(loadedSurface, screenSurface->format, 0);
    SDL_FreeSurface(loadedSurface);
    if (finalSurface == NULL)
    {
      std::cerr << "Couldn't optimize image!" << std::endl;
      return NULL;
    }
    else
    {
      return finalSurface;
    }
  }
}


bool Main::loadGraphics()
{
  player = loadSurface("./images/sprites/player.png");
  steps = loadSurface("./images/sprites/steps.png");
  monster = loadSurface("./images/sprites/monster.png");
  win = loadSurface("./images/sprites/ggs.png");
  lose = loadSurface("./images/sprites/gameover.png");
  gift = loadSurface("./images/sprites/gift.png");
	alert = loadSurface("./images/icons/alert.png");
  if (player == NULL)
  {
    std::cerr << "Couldn't load graphics!" << std::endl;
    return false;
  }
  else
  {
    return true;
  }
}


void Main::gameloop()
{
  while (SDL_PollEvent(&e) != 0)
  {
    if (e.type == SDL_QUIT)
    {
      quit = true;
    }
    else if (e.type == SDL_KEYDOWN)
    {
      switch (e.key.keysym.sym)
      {
        case SDLK_a:
          aPressed = true;
          break;
        case SDLK_d:
          dPressed = true;
          break;
        case SDLK_s:
          sPressed = true;
          break;
        case SDLK_w:
          wPressed = true;
          break;
        case SDLK_SPACE:
          if (p.x > g.x - 50 && p.x < g.x + 50 && p.y > g.y - 50 && p.y < g.y + 50)
          {
            giftProgress--;
						if (giftProgress % 30 == 0)
						{
							g.x = rand() % WIDTH - g.w;
							do {
								g.y = rand() % HEIGHT - g.h;
							} while (g.y < 100);
						}
          }
					break;
				case SDLK_LSHIFT:
					shift = true;
					break;

        }
      }
      else if (e.type == SDL_KEYUP)
      {
        switch (e.key.keysym.sym)
        {
          case SDLK_a:
            aPressed = false;
            break;
          case SDLK_d:
            dPressed = false;
            break;
          case SDLK_s:
            sPressed = false;
            break;
          case SDLK_w:
            wPressed = false;
            break;
					case SDLK_LSHIFT:
						shift = false;
        }
      }
  }
  if (aPressed || dPressed || wPressed || sPressed)
  {
    if (!shift) {state = WALK;} else {state = RUN;}
  }
  else
  {
    state = STAND;
  }
  if (state == WALK || state == RUN)
  {
    ps.x = 50;
    ps.y = 50;
		if (state == RUN) {speed = 0.5;} else {speed = 1;}
    if (aPressed)
    {
      p.x -= speed * basespeed;
      ps.x -= 50;
    }
    if (dPressed)
    {
      p.x += speed * basespeed;
      ps.x += 50;
    }
    if (sPressed)
    {
      p.y += speed * basespeed;
      ps.y += 50;
    }
    if (wPressed)
    {
      p.y-= speed * basespeed;
      ps.y -= 50;
    }
			stepTimer--;
	    if (stepTimer == 0)
	    {
	      stepTimer = 7;
				if (state == WALK)
				{
					if (footsteps < 50)
		      {
		        footstepspos[footsteps][0] = p.x + 25;
		        footstepspos[footsteps][1] = p.y + 50;
		        footstepspos[footsteps][2] = ps.x / 50;
		        footstepspos[footsteps][3] = ps.y / 50;
		        footsteps++;
		      }
		      else
		      {
		        footsteps = 0;
		        footstepspos[footsteps][0] = p.x;
		        footstepspos[footsteps][1] = p.y;
		        footstepspos[footsteps][2] = ps.x / 50;
		        footstepspos[footsteps][3] = ps.y / 50;
		        footsteps++;
		      }
				}
			}
  }
  if (monsterState == WANDER)
  {
		temp.w = 2; temp.h = 2;
		for (int a = radarA; a != radarB; a++)
		{
			for (int i = 0; i < 50; i++)
			{
				std::cout << a << ", " << i << ", " << radarB << std::endl;
				temp.x = m.x + (cos(radarA * (3.14159256 / 180)) * i * 20);
				temp.y = m.y + (sin(radarA * (3.14159256 / 180)) * i * 20);
				if (isColliding(temp, p))
				{
					monsterInvestigationPos[0] = p.x;
					monsterInvestigationPos[1] = p.y;
					monsterState = INVESTIGATE;
				}
			}
			a %= 360;
		}
    if (p.x > m.x - 100 && p.x < m.x + 100 && p.y > m.y - 10 && p.y < m.y + 100)
    {
      monsterState = HUNT;
      monsterInvestigationPos[0] = p.x;
      monsterInvestigationPos[1] = p.y;
    }
    if (((monsterInvestigationPos[0] > m.x - 10 && monsterInvestigationPos[0] < m.x + 10) && (monsterInvestigationPos[1] > m.y - 10 && monsterInvestigationPos[1] < m.y + 10)) || monsterInvestigationPos[0] == NULL || monsterInvestigationPos[1] == NULL)
    {
      // Monster reached its spot
			if (rand() % 10 == 0)
			{
				monsterInvestigationPos[0] = p.x;
				monsterInvestigationPos[1] = p.y;
			}
			else
	    {
				monsterInvestigationPos[0] = rand() % WIDTH - 10;
				monsterInvestigationPos[1] = rand() % HEIGHT - 10;
			}
    }
    else
    {
      ms.x = 50;
      ms.y = 50;
			if (monsterInvestigationPos[0] > m.x)
			{
				m.x += 2;
				ms.x += 50;
			}
			else if (monsterInvestigationPos[0] < m.x)
			{
				m.x -= 2;
				ms.x -= 50;
			}
			if (monsterInvestigationPos[1] > m.y)
			{
				m.y += 2;
				ms.y += 50;
			}
			else if (monsterInvestigationPos[1] < m.y)
			{
				m.y -= 2;
				ms.y -= 50;
			}
    }
    for (int i = 0; i < 50; i++)
    {
      if (footstepspos[i][0] > m.x - 65 && footstepspos[i][0] < m.x + 65 && footstepspos[i][1] > m.y - 65 && footstepspos[i][1] < m.y + 65)
      {
        monsterInvestigationPos[0] = footstepspos[i][0];
        monsterInvestigationPos[1] = footstepspos[i][1];
        footstepspos[i][0] = -100;
        footstepspos[i][1] = -100;
        footstepspos[i][2] = 0;
        footstepspos[i][3] = 0;
        monsterState = INVESTIGATE;
				monsterfstep[0] = footstepspos[i][0];
				monsterfstep[1] = footstepspos[i][1];
				monsterfstep[2] = footstepspos[i][2];
				monsterfstep[3] = footstepspos[i][3];
				alertTimer = 6;
      }
    }
  }
  else if (monsterState == INVESTIGATE)
  {
		if (alertTimer > 0)
		{
			alertTimer--;
			temp.x = m.x + 12;
			temp.y = m.y - 55;
			temp.w = 25;
			temp.h = 50;
			SDL_BlitSurface(alert, nullptr, screenSurface, &temp);
		}
		else
		{
			if (p.x > m.x - 100 && p.x < m.x + 100 && p.y > m.y - 10 && p.y < m.y + 100)
	    {
	      monsterState = HUNT;
	      monsterInvestigationPos[0] = p.x;
	      monsterInvestigationPos[1] = p.y;
	    }
	    else if (((monsterInvestigationPos[0] > m.x - 3 && monsterInvestigationPos[0] < m.x + 3) && (monsterInvestigationPos[1] > m.y - 3 && monsterInvestigationPos[1] < m.y + 3)) || monsterInvestigationPos[0] == NULL || monsterInvestigationPos[1] == NULL)
	    {
	      // Monster reached its spot
	      monsterState = WANDER;
	      for (int i = footsteps + 1; i < 50; i++)
	      {
	        if (footstepspos[i][0] > m.x - 65 && footstepspos[i][0] < m.x + 65 && footstepspos[i][1] > m.y - 65 && footstepspos[i][1] < m.y + 65)
	        {
	          monsterInvestigationPos[0] = footstepspos[i][0];
	          monsterInvestigationPos[1] = footstepspos[i][1];
	          footstepspos[i][0] = -100;
	          footstepspos[i][1] = -100;
	          footstepspos[i][2] = 0;
	          footstepspos[i][3] = 0;
	          monsterState = INVESTIGATE;
						monsterfstep[0] = footstepspos[i][0];
						monsterfstep[1] = footstepspos[i][1];
						monsterfstep[2] = footstepspos[i][2];
						monsterfstep[3] = footstepspos[i][3];
	        }
	      }
				for (int i = 0; i < footsteps + 1; i++)
	      {
	        if (footstepspos[i][0] > m.x - 65 && footstepspos[i][0] < m.x + 65 && footstepspos[i][1] > m.y - 65 && footstepspos[i][1] < m.y + 65)
	        {
	          monsterInvestigationPos[0] = footstepspos[i][0];
	          monsterInvestigationPos[1] = footstepspos[i][1];
	          footstepspos[i][0] = -100;
	          footstepspos[i][1] = -100;
	          footstepspos[i][2] = 0;
	          footstepspos[i][3] = 0;
	          monsterState = INVESTIGATE;
						monsterfstep[0] = footstepspos[i][0];
						monsterfstep[1] = footstepspos[i][1];
						monsterfstep[2] = footstepspos[i][2];
						monsterfstep[3] = footstepspos[i][3];
	        }
	      }
	    }
	    else
	    {
	      ms.x = 50;
	      ms.y = 50;
	      if (monsterInvestigationPos[0] > m.x)
	      {
	        m.x += 2;
	        ms.x += 50;
	      }
	      else if (monsterInvestigationPos[0] < m.x)
	      {
	        m.x -= 2;
	        ms.x -= 50;
	      }
	      if (monsterInvestigationPos[1] > m.y)
	      {
	        m.y += 2;
	        ms.y += 50;
	      }
	      else if (monsterInvestigationPos[1] < m.y)
	      {
	        m.y -= 2;
	        ms.y -= 50;
	      }
	    }
		}

  }
  else if (monsterState == HUNT)
  {
		SDL_BlitSurface(lose, NULL, screenSurface, NULL);
		SDL_UpdateWindowSurface(window);
		SDL_Delay(5000);
		quit = true;
  }
  if (giftProgress <= 0)
  {
    // U win
    SDL_BlitSurface(win, NULL, screenSurface, NULL);
    SDL_UpdateWindowSurface(window);
    SDL_Delay(5000);
    quit = true;
  }
  SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x80, 0x80, 0x80));
  for (int i = 0; i < 50; i++)
  {
    SDL_Rect stepRect, stepSRect;
    stepRect.w = 16;
    stepRect.h = 16;
    stepRect.x = footstepspos[i][0];
    stepRect.y = footstepspos[i][1];
    stepSRect.w = 16;
    stepSRect.h = 16;
    stepSRect.x = footstepspos[i][2] * 16;
    stepSRect.y = footstepspos[i][3] * 16;
    SDL_BlitSurface(steps, &stepSRect, screenSurface, &stepRect);
  }
	pr.x = g.x + g.w / 10;
	pr.y = g.y - 10;
	pr.w = g.w / 2;
	pr.h = 5;
	pr2.x = pr.x;
	pr2.y = pr.y;
	pr2.h = pr.h;
	pr2.w = (150 - giftProgress) / 3;
	if (alertTimer > 0)
	{
		temp.x = m.x + 12;
		temp.y = m.y - 55;
		temp.w = 25;
		temp.h = 50;
		SDL_BlitSurface(alert, nullptr, screenSurface, &temp);
	}
	if (monsterState == WANDER)
	{
		radarTimer--;
		if (radarTimer <= 0)
		{
			if (rand() % 2 == 0) {radarDir = true;} else {radarDir = false;}
			radarTimer = 12;
		}
		if (radarDir)
		{
			radarA += 3;
			radarB += 3;
			radarA %= 360;
			radarB %= 360;
		}
		else
		{
			radarA -= 3;
			radarB -= 3;
			radarA += 360;
			radarB += 360;
			radarA %= 360;
			radarB %= 360;
		}
		temp.w = 2; temp.h = 2;
		for (int i = 0; i < 50; i++)
		{
			temp.x = m.x + (cos(radarA * (3.14159256 / 180)) * i * 20);
			temp.y = m.y + (sin(radarA * (3.14159256 / 180)) * i * 20);
			SDL_FillRect(screenSurface, &temp, SDL_MapRGB(screenSurface->format, 0xff, 0, 0));
		}
		for (int i = 0; i < 50; i++)
		{
			temp.x = m.x + (cos(radarB * (3.14159256 / 180)) * i * 20);
			temp.y = m.y + (sin(radarB * (3.14159256 / 180)) * i * 20);
			SDL_FillRect(screenSurface, &temp, SDL_MapRGB(screenSurface->format, 0xff, 0, 0));
		}
	}
	SDL_FillRect(screenSurface, &pr, SDL_MapRGB(screenSurface->format, 0x10, 0x10, 0x10));
	SDL_FillRect(screenSurface, &pr2, SDL_MapRGB(screenSurface->format, 0, 0xFF, 0));
  SDL_BlitSurface(monster, &ms, screenSurface, &m);
  SDL_BlitSurface(player, &ps, screenSurface, &p);
  SDL_BlitSurface(gift, NULL, screenSurface, &g);
  SDL_UpdateWindowSurface(window);
	
}


bool Main::isColliding(SDL_Rect rect1, SDL_Rect rect2)
{
  if (rect1.x + rect1.w > rect2.x && rect1.x < rect2.x + rect2.w)
  {
    if (rect1.y + rect1.h > rect2.y && rect1.y < rect2.y + rect2.h)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}


void Main::close()
{
  SDL_DestroyWindow(window);
  SDL_FreeSurface(player);
  SDL_FreeSurface(monster);
  SDL_FreeSurface(steps);
  SDL_FreeSurface(win);
  SDL_FreeSurface(lose);
  SDL_FreeSurface(gift);
  SDL_Quit();
}


int main(int argc, char *args[])
{
  // Stuff
  Main mainclass;
  return mainclass.exit;
}
