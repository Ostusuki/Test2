#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <list>

using namespace sf;
const int W = 1200;
const int H = 800;

float DEGTORAD = 0.017453f;
int scorerank[6];

//Start
int turnback = 0, fakescore = 0;
int score = 0, buff = 0;
int life = 3;
int x_background = 0, sound_menu = 0, sound_play = 0;
bool menu_on = true, menu_score_on = false, menu_high_score_on = false;
int menu_start = 100;
Time choose_delaytime = sf::milliseconds(150), score_delaytime = milliseconds(1000);

void restart(bool start) {
	if (start) {
		score = 0;
		buff = 0;
		life = 3;
		x_background = 0, sound_menu = 0, sound_play = 0;
		menu_start = 1;
	}
}


class Animation
{
public:
	float Frame, speed;
	Sprite sprite;
	std::vector<IntRect> frames;

	Animation() {}

	Animation(Texture& t, int x, int y, int w, int h, int count, float Speed);

	void update();

	bool isEnd();
};

Animation::Animation(Texture& t, int x, int y, int w, int h, int count, float Speed)
{
	Frame = 0;
	speed = Speed;

	for (int i = 0; i < count; i++)
		frames.push_back(IntRect(x + i * w, y, w, h));

	sprite.setTexture(t);
	sprite.setOrigin(w / 2, h / 2);
	sprite.setTextureRect(frames[0]);
}

void Animation::update()
{
	Frame += speed;
	int n = frames.size();
	if (Frame >= n) Frame -= n;
	if (n > 0) sprite.setTextureRect(frames[int(Frame)]);
}

bool Animation::isEnd()
{
	return Frame + speed >= frames.size();
}


class Entity
{
public:
	float x, y, dx, dy, R, angle;
	bool life;
	std::string name;
	Animation anim;

	Entity()
	{
		life = 1;
	}

	void settings(Animation& a, int X, int Y, float Angle = 0, int radius = 1)
	{
		anim = a;
		x = X; y = Y;
		angle = Angle;
		R = radius;
	}

	virtual void update() {};

	void draw(RenderWindow& app)
	{
		anim.sprite.setPosition(x, y);
		anim.sprite.setRotation(angle + 90);
		app.draw(anim.sprite);

		CircleShape circle(R);
		circle.setFillColor(Color(255, 0, 0, 170));
		circle.setPosition(x, y);
		circle.setOrigin(R, R);
		//app.draw(circle);
	}

	virtual ~Entity() {};
};

class asteroid : public Entity
{
public:
	asteroid()
	{
		dx = rand() % 8 - 4;
		dy = rand() % 8 - 4;
		name = "asteroid";
	}

	void update()
	{
		x += dx;
		y += dy;

		/* if (x > W) x = 0;  if (x < 0) x = W;
		 if (y > H) y = 0;  if (y < 0) y = H;*/
		if (x > 1200) /* x = 0 */life = false;
		if (x < 0) x = W;
		if (y > 800) y = 0;
		if (y < 0) y = H;
	}

};


class bullet : public Entity
{
public:
	bullet()
	{
		name = "bullet";
	}

	void  update()
	{
		dx = cos(angle * DEGTORAD) * 6;
		dy = sin(angle * DEGTORAD) * 6;
		//angle+=rand()%7-3;  /*try this*/
		x += dx;
		y += dy;

		if (x > W || x<0 || y>H || y < 0) life = 0;
	}

};



class player : public Entity
{
public:
	bool thrust;

	player()
	{
		name = "player";
	}

	void update()
	{
		if (thrust)
		{
			dx += cos(angle * DEGTORAD) * 0.2;
			dy += sin(angle * DEGTORAD) * 0.2;
		}
		else
		{
			dx *= 0.99;
			dy *= 0.99;
		}

		int maxSpeed = 15;
		float speed = sqrt(dx * dx + dy * dy);
		if (speed > maxSpeed)
		{
			dx *= maxSpeed / speed;
			dy *= maxSpeed / speed;
		}

		x += dx;
		y += dy;

		if (x > W - 25) x = W - 25; if (x < 25) x = 25;
		if (y > H - 25) y = H - 25; if (y < 25) y = 25;
	}

};


bool isCollide(Entity* a, Entity* b)
{
	return (b->x - a->x) * (b->x - a->x) +
		(b->y - a->y) * (b->y - a->y) <
		(a->R + b->R) * (a->R + b->R);
}



int main()
{
	srand(time(0));

	RenderWindow app(VideoMode(W, H), "Space shoot");
	app.setFramerateLimit(60);

	Texture t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13;
	t1.loadFromFile("images/spaceship.png");
	t2.loadFromFile("images/background(7)(fix_2).jpg");
	t3.loadFromFile("images/explosions/type_C.png");
	t4.loadFromFile("images/rock.png");
	t5.loadFromFile("images/fire_blue.png");
	t6.loadFromFile("images/rock_small.png");
	t7.loadFromFile("images/explosions/type_B.png");
	t8.loadFromFile("images/Menu(fix_3).png");
	t9.loadFromFile("images/Menu(fix_1).png");
	t11.loadFromFile("images/high_score(fix_2).png");
	t12.loadFromFile("images/menu_high_score.png");
	t13.loadFromFile("images/Menu(fix_2_1).png");



	t1.setSmooth(true);
	t2.setSmooth(true);
	t10.setSmooth(true);


	//Font
	sf::Font f, font_space, arial;
	f.loadFromFile("arialbi.ttf");
	font_space.loadFromFile("AquireBold-8Ma60.otf");
	arial.loadFromFile("arial.ttf");


	//Text
	sf::Text mytext, scores_text, top1, top2, top3, top4, top5;



	//Music
	sf::Music music_play, music_menu;
	music_play.openFromFile("audio/Music_play_game.ogg");
	music_menu.openFromFile("audio/Music_menu.ogg");



	//Sound
	sf::SoundBuffer buffer, choose, explosition;
	choose.loadFromFile("audio/Choose_sound(3).wav");
	buffer.loadFromFile("audio/Laze_gun_sound.ogg");
	explosition.loadFromFile("audio/Explosition.ogg");
	sf::Sound bullet_sound, choose_sound, explosition_sound;
	explosition_sound.setBuffer(explosition);
	bullet_sound.setBuffer(buffer);
	choose_sound.setBuffer(choose);
	choose_sound.setVolume(100);
	explosition_sound.setVolume(10000);



	Sprite background(t2), Menu(t8), Menu_exit(t9), sLife(t10), menu_score(t11), menu_high_score(t12), Menu_highscore(t13);
	menu_score.setScale(0.4, 0.4);
	menu_score.setPosition(360, -100);
	menu_high_score.setScale(0.4, 0.4);
	menu_high_score.setPosition(360, -110);


	Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
	Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
	Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
	Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
	Animation sPlayer(t1, 40, 40, 40, 45, 1, 0);
	Animation sPlayer_down(t1, 80, 40, 40, 45, 1, 0);
	Animation sPlayer_up(t1, 0, 40, 40, 45, 1, 0);
	/*Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);*/
	Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);


	//Entity
	std::list<Entity*> entities;

	//File high score
	std::ifstream tep("highscore.txt");
	for (int i = 1; i <= 5; i++) {
		tep >> scorerank[i];
	}
	score = 0;


	//Creat Rocks
	for (int i = 0; i < 10; i++)
	{
		asteroid* a = new asteroid();
		a->settings(sRock, rand() % 100 + 1100, rand() % 800, rand() % 90 + 225, 25);
		entities.push_back(a);
	}



	player* p = new player();
	p->settings(sPlayer, 200, 200, 0, 20);
	entities.push_back(p);



	///main loop///
	while (app.isOpen())
	{
		Event event;
		while (app.pollEvent(event))
		{
			///Close///
			if (event.type == Event::Closed)
				app.close();
			///Bullet///
			if (event.type == Event::KeyPressed && menu_on == false && menu_score_on == false && menu_high_score_on == false)
				if (event.key.code == Keyboard::Space)
				{
					bullet_sound.play();
					bullet_sound.setVolume(50);
					bullet* b = new bullet();
					b->settings(sBullet, p->x, p->y, p->angle, 10);
					entities.push_back(b);
				}
		}

		if (score != 0) fakescore = score;
		//Menu highscore
		if (menu_high_score_on) {
			//Top1
			top1.setFont(arial);
			top1.setCharacterSize(40);
			top1.setFillColor(sf::Color::Yellow);
			std::stringstream ss1;
			ss1 << scorerank[1];
			top1.setPosition(530, 200);
			top1.setString(ss1.str().c_str());

			//Top2
			top2.setFont(arial);
			top2.setCharacterSize(40);
			top2.setFillColor(sf::Color::Yellow);
			std::stringstream ss2;
			ss2 << scorerank[2];
			top2.setPosition(530, 280);
			top2.setString(ss2.str().c_str());

			//Top3
			top3.setFont(arial);
			top3.setCharacterSize(40);
			top3.setFillColor(sf::Color::Yellow);
			std::stringstream ss3;
			ss3 << scorerank[3];
			top3.setPosition(530, 360);
			top3.setString(ss3.str().c_str());

			//Top4
			top4.setFont(arial);
			top4.setCharacterSize(40);
			top4.setFillColor(sf::Color::Yellow);
			std::stringstream ss4;
			ss4 << scorerank[4];
			top4.setPosition(530, 430);
			top4.setString(ss4.str().c_str());

			//Top5
			top5.setFont(arial);
			top5.setCharacterSize(40);
			top5.setFillColor(sf::Color::Yellow);
			std::stringstream ss5;
			ss5 << scorerank[5];
			top5.setPosition(530, 500);
			top5.setString(ss5.str().c_str());

			//Mouse event
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left) {
					if (event.mouseButton.x > 417 && event.mouseButton.x < 508 && event.mouseButton.y > 616 && event.mouseButton.y < 642) {
						choose_sound.play();
						if (turnback == 0) {
							menu_high_score_on = false;
							menu_score_on = false;
							menu_on = true;
						}
						else {
							menu_high_score_on = false;
							menu_score_on = true;
							restart(true);
						}
					}
					if (event.mouseButton.x > 532 && event.mouseButton.x < 622 && event.mouseButton.y > 617 && event.mouseButton.y < 642) {
						choose_sound.play();
						menu_high_score_on = false;
						menu_score_on = false;
						menu_on = true;
						restart(true);
					}
					if (event.mouseButton.x > 646 && event.mouseButton.x < 735 && event.mouseButton.y > 618 && event.mouseButton.y < 642) {
						choose_sound.play();
						restart(true);
						app.close();
					}
				}
			}
			//sf::Keyboard::isKeyPressed(sf::Keyboard::Right)

			//draw
			app.draw(menu_high_score);
			app.draw(top1);
			app.draw(top2);
			app.draw(top3);
			app.draw(top4);
			app.draw(top5);
			app.display();
			continue;
		}

		//Menu score
		if (menu_score_on) {
			turnback = 1;
			music_play.pause();
			sound_menu = 0, sound_play = 0;
			///Score
			mytext.setFont(arial);
			mytext.setCharacterSize(50);
			mytext.setFillColor(sf::Color::White);
			std::stringstream ss;
			ss << fakescore;
			mytext.setPosition(570 - ss.str().length() * 11, 250);
			mytext.setString(ss.str().c_str());

			///Highest score
			top1.setFont(arial);
			top1.setCharacterSize(45);
			top1.setFillColor(sf::Color::Red);
			std::stringstream sTop1;
			sTop1 << scorerank[1];
			top1.setPosition(560 - sTop1.str().length() * 10, 420);
			top1.setString(sTop1.str().c_str());

			//Mouse event
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left) {
					if (event.mouseButton.x > 438 && event.mouseButton.x < 528 && event.mouseButton.y > 489 && event.mouseButton.y < 517) {
						choose_sound.play();
						menu_score_on = false;
						menu_high_score_on = true;
						restart(true);
					}
					if (event.mouseButton.x > 551 && event.mouseButton.x < 642 && event.mouseButton.y > 489 && event.mouseButton.y < 517) {
						choose_sound.play();
						menu_high_score_on = false;
						menu_score_on = false;
						menu_on = true;
						restart(true);
					}
					if (event.mouseButton.x > 665 && event.mouseButton.x < 755 && event.mouseButton.y > 489 && event.mouseButton.y < 517) {
						choose_sound.play();
						restart(true);
						app.close();
					}
				}
			}

			//draw
			app.clear(Color::Black);
			app.draw(menu_score);
			app.draw(mytext);
			app.draw(top1);
			app.display();
			continue;
		}

		///Menu///
		if (menu_on) {
			turnback = 0;
			sound_menu++;
			if (sound_menu == 1) {
				music_menu.play();
				music_menu.setLoop(true);
			}
			if (menu_start % 3 == 0 && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
				choose_sound.setVolume(100);
				menu_high_score_on = true;
				choose_sound.play();
			}
			if (menu_start % 3 == 1 && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
				menu_on = false;
				choose_sound.play();
			}
			if (menu_start % 3 == 2 && sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))  app.close();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
				menu_start++;
				choose_sound.setVolume(100);
				choose_sound.play();
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				menu_start--;
				choose_sound.setVolume(100);
				choose_sound.play();
			}
			if (menu_start % 3 == 0) app.draw(Menu_highscore);
			if (menu_start % 3 == 1) app.draw(Menu);
			if (menu_start % 3 == 2) app.draw(Menu_exit);

			app.display();
			sleep(choose_delaytime);
			continue;
		}

		//Sound play
		sound_play++;
		if (sound_play == 1) {
			music_menu.pause();
			music_play.play();
			music_play.setLoop(true);
		}


		//Space ship movement
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			p->x -= 5;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			p->x += 5;
			/*p->thrust = true;*/
		}
		else p->thrust = false;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			p->y -= 5;
			/*p->anim = sPlayer_up;*/
		}
		/*else p->anim = sPlayer;*/

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			p->y += 5;
			/*p->anim = sPlayer_down;*/
		}
		/*else p->anim = sPlayer;*/


		///Explosion///
		for (auto a : entities)
			for (auto b : entities)
			{
				if (a->name == "asteroid" && b->name == "bullet")
					if (isCollide(a, b))
					{
						score += 100;
						a->life = false;
						b->life = false;

						Entity* e = new Entity();
						e->settings(sExplosion, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);


						for (int i = 0; i < 5; i++)
						{
							if (a->R == 15) continue;
							Entity* e = new asteroid();
							e->settings(sRock_small, a->x, a->y, rand() % 360, 15);
							entities.push_back(e);
						}

					}
				if (a->name == "player" && b->name == "asteroid")
					if (isCollide(a, b))
					{
						explosition_sound.setVolume(10000);
						explosition_sound.play();
						b->life = false;

						Entity* e = new Entity();
						e->settings(sExplosion_ship, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);

						p->settings(sPlayer, 30, 300, 0, 20);
						p->dx = 0; p->dy = 0;
						life--;
					}
			}
		/*
		if (p->thrust)  p->anim = sPlayer_go;
		else   p->anim = sPlayer;
		*/
		for (auto e : entities)
			if (e->name == "explosion")
				if (e->anim.isEnd()) e->life = 0;

		if (rand() % 150 == 0)
		{
			for (int i = 0; i < 2; i++)
			{
				asteroid* a = new asteroid();
				a->settings(sRock, rand() % 100 + 1100, rand() % 800, rand() % 90 + 225, 25);
				entities.push_back(a);
			}
		}


		//Entity movement
		for (auto i = entities.begin(); i != entities.end();)
		{
			Entity* e = *i;

			e->update();
			e->anim.update();

			if (e->life == false) { i = entities.erase(i); delete e; }
			else i++;
		}



		//Score_Text
		buff++;
		if (buff % 100 == 0) score++;

		scores_text.setFont(font_space);
		scores_text.setString("SCORE");
		scores_text.setCharacterSize(24);
		scores_text.setFillColor(sf::Color::Red);
		scores_text.setStyle(sf::Text::Bold);
		scores_text.setPosition(700, 3);

		mytext.setFont(font_space);
		mytext.setString("Space Shoot");
		mytext.setCharacterSize(23);
		mytext.setFillColor(sf::Color::White);
		mytext.setStyle(sf::Text::Bold);
		mytext.setPosition(820, 2);
		std::stringstream ss;     /*#include <sstream>*/
		ss << score;
		mytext.setString(ss.str().c_str());


		//Background scrolling 
		background.setTextureRect(IntRect(x_background, 0, 1200, 800));
		x_background += 2;
		if (x_background == 1426) x_background = 0;


		//Life 
		if (life == 0) {
			t10.loadFromFile("images/Menu_scores(fix_1_4).png");
			for (int i = 1; i <= 5; i++) {
				if (score >= scorerank[i]) {
					for (int j = 6; j > i; j--) {
						if (scorerank[j - 1] != 0)
							scorerank[j] = scorerank[j - 1];
					}
					scorerank[i] = score;
					break;
				}

			}
			//Ghi file highscore
			std::ofstream file_score("highscore.txt");
			for (int i = 1; i <= 5; i++) {
				file_score << scorerank[i] << std::endl;
			}
			menu_score_on = true;
			//sleep(score_delaytime);
		}
		if (life == 1)  t10.loadFromFile("images/Menu_scores(fix_1_1).png");
		if (life == 2)  t10.loadFromFile("images/Menu_scores(fix_1_3).png");
		if (life == 3)  t10.loadFromFile("images/Menu_scores(fix_1_2).png");
		sLife.setTextureRect(IntRect(0, 0, 1204, 50));
		sLife.setPosition(-342, -15);
		sLife.setScale(1.7, 1.5);

		//draw
		app.clear(sf::Color::Black);
		app.draw(background);
		for (auto i : entities) i->draw(app);
		app.draw(sLife);
		app.draw(mytext);
		app.draw(scores_text);
		app.display();
	}

	return 0;
}