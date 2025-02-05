#include "Scene.h"
#include <stdio.h>
#include "Globals.h"

Scene::Scene()
{
	player = nullptr;
	player2 = nullptr;
	level = nullptr;
	enemies = nullptr;
	shots = nullptr;
	particles = nullptr;
	P2in = false;
	P1in = true;

	camera.target = { 0, 0 };				//Center of the screen
	camera.offset = { 0, MARGIN_GUI_Y };	//Offset from the target (center of the screen)
	camera.rotation = 0.0f;					//No rotation
	camera.zoom = 1.0f;						//Default zoom
	eBubblingTime = 0;
	eTimeSpawnX = GetRandomValue(-1, 1);
	eTimeSpawnY = GetRandomValue(-1, 1);
	debug = DebugMode::OFF;
	AllObjects = 0;
	enemy = nullptr;

}
Scene::~Scene()
{
	if (player != nullptr)
	{
		player->Release();
		delete player;
		player = nullptr;
	}
	if (player2 != nullptr)
	{
		player2->Release();
		delete player2;
		player2 = nullptr;
	}
	if (level != nullptr)
	{
		level->Release();
		delete level;
		level = nullptr;
	}
	
	for (Entity* obj : objects)
	{
		delete obj;
		obj = nullptr;
	}
	for (Entity* bubl : bubbles)
	{
		delete bubl;
		bubl = nullptr;
	}
	for (BubbleFromPlayer* bubles : bubblesPlayer)
	{
		bubles->Release();
		delete bubles;
		bubles = nullptr;
		
	}
	if (enemies != nullptr)
	{
		enemies->Release();
		delete enemies;
		enemies = nullptr;
	}
	if (shots != nullptr)
	{
		delete shots;
		shots = nullptr;
	}
	if (particles != nullptr)
	{
		delete particles;
		particles = nullptr;
	}
	projectiles.clear();
	SDBOTTLES.clear();
	thunders.clear();
	objects.clear();
	thunds.clear();
	bubbles.clear();
	bubblesPlayer.clear();

}
AppStatus Scene::Init()
{
	player = new Player({ 0,0 }, State::IDLE, Look::RIGHT);
	if (player == nullptr)
	{
		LOG("Failed to allocate memory for Player");
		return AppStatus::ERROR;
	}
	player2 = new Player2({ 0,0 }, State2::IDLE, Look::LEFT);
	if (player == nullptr)
	{
		LOG("Failed to allocate memory for Player");
		return AppStatus::ERROR;
	}
	//Initialise player
	if (player->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Player");
		return AppStatus::ERROR;
	}
	if (player2->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Player");
		return AppStatus::ERROR;
	}

	//Create enemy manager
	enemies = new EnemyManager();
	if (enemies == nullptr)
	{
		LOG("Failed to allocate memory for Enemy Manager");
		return AppStatus::ERROR;
	}
	//Initialise enemy manager
	if (enemies->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Enemy Manager");
		return AppStatus::ERROR;
	}

	//Create shot manager 
	shots = new ShotManager();
	if (shots == nullptr)
	{
		LOG("Failed to allocate memory for Shot Manager");
		return AppStatus::ERROR;
	}
	//Initialise shot manager
	if (shots->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Shot Manager");
		return AppStatus::ERROR;
	}

	//Create particle manager 
	particles = new ParticleManager();
	if (particles == nullptr)
	{
		LOG("Failed to allocate memory for Particle Manager");
		return AppStatus::ERROR;
	}
	//Initialise particle manager
	if (particles->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Particle Manager");
		return AppStatus::ERROR;
	}

	//Create level 
	level = new TileMap();
	if (level == nullptr)
	{
		LOG("Failed to allocate memory for Level");
		return AppStatus::ERROR;
	}
	//Initialise level
	if (level->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Level");
		return AppStatus::ERROR;
	}
	enemies->SetTileMap(level);

	//Load level
	if (LoadLevel(1) != AppStatus::OK)
	{
		LOG("Failed to load Level 1");
		return AppStatus::ERROR;
	}

	//Assign the tile map reference to the player to check collisions while navigating
	player->SetTileMap(level);
	player2->SetTileMap(level);
	//Assign the tile map reference to the shot manager to check collisions when shots are shot
	shots->SetTileMap(level);
	//Assign the particle manager reference to the shot manager to add particles when shots collide
	shots->SetParticleManager(particles);
	//Assign the shot manager reference to the enemy manager so enemies can add shots
	enemies->SetShotManager(shots);

	return AppStatus::OK;
}
AppStatus Scene::LoadLevel(int stage)
{
	int size;
	int x, y, i;
	Tile tile;
	Point pos;
	int* map = nullptr;
	Object* obj;
	Bubble* bubl;
	Enemy* ene;
	AABB hitbox, area;
	numEnemies = 0;
	ClearLevel();
	size = LEVEL_WIDTH * LEVEL_HEIGHT;

	if (stage == 1)
	{
		map = new int[size] {
			    1,   3, 173, 173, 173, 173, 173, 173, 173, 173, 173, 173, 173, 173, 173,  2,
				2,   5,   0,   0,   0,   0,   0,   0, 103,   0,   0,   0,   0,   0,   0,  2,
				2,   5,   0,   0,   0,   0,   0,   0, 103,   0,   0,   0,   0,   0,   0,  2,
				2,   5,   0,   0,   0,   0,   0,   0, 103,   0,   0,   0,   0,   0,   0,  2,
				2,   5,   0,   0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,   0,  2,
				2,   3,   4,  12,  11,  11,  11,  11,  11,  11,  11,  11,  13,   0,  16,  2,
				2,   5,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  2,
				2,  43,  21,  17,  42,  42,  42,  42,  42,  42,  42,  42,  18,   0,  42,  2,
				2,   9,   6,  10,  19,  19,  19,  19,  19,  19,  19,  19,  20,   0,   7,  2,
				2,   5,   0,   0,    0,  0 ,   0,   0,   0,   0,   0,    0,   0,   0,   0,  2,
				2,   3,   4,  12,  11,  11,  11,  11,  11,  11,  11,  11,  13,   0,  16,  2,
				2,   5, 100,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 106,   0,  2,
				2,  43,  42,  42,  42,  42,  42,  42,  42,  42,  42,  42,  42,  42,  42,  2
			};
		player->InitScore();
		numEnemies = 3;
		

	}
	else if (stage == 2)
	{
		map = new int[size] {
			    150, 158, 171, 171, 153,   0, 172, 171, 171, 153,   0, 172, 171, 171, 171, 151,
				151, 162,   0,   0,   0, 103,   0,   0,   0,   0, 103,   0,   0,   0,   0, 151,
				151, 162, 156, 155, 155, 155, 157,   0,   0, 156, 155, 155, 155, 157,   0, 151,
				151, 162, 160, 164, 166, 166, 163,   0,   0, 165, 166, 166, 166, 161,   0, 151,
				151, 162, 160, 162,   0,    0,   0,   0,   0,   0,    0,   0,   0, 161,   0, 151,
				151, 162, 160, 158, 152, 152, 152, 167,   0, 168, 152, 152, 152, 161,   0, 151,
				151, 162, 160, 162,   0,   0, 102,   0,   0,   0,   0,   0,   0, 161,   0, 151,
				151, 162, 160, 159, 155, 155, 155, 157, 156, 155, 155, 155, 155, 161,   0, 151,
				151, 162, 165, 166, 166, 166, 166, 163, 165, 166, 166, 166, 166, 163,   0, 151,
				151, 162,   0,   0, 103,   0,   0,   0,   0,   0,   0, 103,   0,    0,   0, 151,
				151, 158, 152, 170, 154, 152, 169,   0,   0,   0, 168, 170, 154, 152, 152, 151,
				151, 162, 100,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 106,   0, 151,
				151, 159, 155, 155, 157,   0, 156, 155, 155, 157,   0, 156, 155, 155, 155, 151
			};
		numEnemies = 4;
	}
	else if (stage == 3)
	{
		map = new int[size] {
			174, 207, 193, 175, 192, 215, 207, 215, 215, 192, 193, 207, 215, 217, 192, 175,
			175, 208, 210, 214, 185, 212, 208, 210, 210, 185, 216, 208, 209, 211, 185, 175,
			175, 208, 209, 211, 185, 208, 208, 209, 210, 185, 208, 208, 210, 214, 192, 175,
			175, 192, 193, 193, 193, 193, 193, 193, 193, 193, 193, 193, 193, 193, 193, 175,
			175, 185, 105,   0,   0, 107, 107,   0, 107, 105,   0, 105,   0,   0, 105, 175,
			175, 185,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 175,
			175, 185,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 175,
			175, 185,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 175,
			175, 185,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 175,
			175, 185,   0,  0,   0,   0,   0,   0,   0,   0,   0,   0,  0,   0,   0, 175,
			175, 185,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 175,
			175, 185, 100,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 106,   0, 175,
			175, 176, 177, 177, 177, 177, 177, 177, 177, 177, 177, 177, 177, 177, 177, 175
		};
		numEnemies = 7;

	}
	else if (stage == 4)
	{
		map = new int[size] {
				197, 199, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 206, 196,
				196, 201,   0,  0,   0,   0,   0,   0, 105,   0,   0,   0,  0,   0,   0, 196,
				196, 202, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 196,
				196, 204, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 196,
				196, 201, 105,	 0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 105,   0, 196,
				196, 199, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 196,
				196, 201,   0, 105,   0,  0,   0,   0,   0,   0,  0,   0, 105,   0,   0, 196,
				196, 202, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 196,
				196, 204, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 205, 196,
				196, 201, 105,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 105,   0, 196,
				196, 199, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 200, 196,
				196, 201, 100,  0,   0,   0,   0,   0,   0,   0,   0,   0,  0, 106,   0, 196,
				196, 202, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 203, 196
			};

		numEnemies = 7;
	}
	else if (stage == 5)
	{
		map = new int[size] {
			  218, 220, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 227, 219,
			  219, 229,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 219,
			  219, 229,   0,   0, 222, 223,   0,   0,   0,   0, 222, 223,   0,   0,   0, 219,
			  219, 229,   0,   0, 230, 231,   0,   0,   0,   0, 230, 231,   0,   0,   0, 219,
			  219, 229,   0, 233,   0,   0,   0,   0,   0,   0,   0,   0, 233,   0,   0, 219,
			  219, 229,   0, 224, 232,   0,   0, 225, 226,   0,   0,   0, 224, 232,   0, 219,
			  219, 229,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 219,
			  219, 229,   0,   0, 222, 223,   0, 108,   0,   0, 222, 223,   0,   0,   0, 219,
			  219, 229,   0,   0, 230, 231,   0,   0,   0,   0, 230, 231,   0,   0,   0, 219,
			  219, 229,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 219,
			  219, 229,   0, 224, 232,   0,   0, 225, 226,   0,   0,   0, 224, 232,   0, 219,
			  219, 229, 100,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 106,   0, 219,
			  219, 221, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 219
			};
		numEnemies = 4;
	}
	else
	{
		//Error level doesn't exist or incorrect level number
		LOG("Failed to load level, stage %d doesn't exist", stage);
		return AppStatus::ERROR;
	}
	level->Load(map, LEVEL_WIDTH, LEVEL_HEIGHT);

	i = 0;
	for (y = 0; y < LEVEL_HEIGHT; ++y)
	{
		for (x = 0; x < LEVEL_WIDTH; ++x)
		{
			tile = (Tile)map[i];
			if (tile == Tile::EMPTY)
			{
				map[i] = 0;
			}
			else if (tile == Tile::PLAYER)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				player->SetPos(pos);
				map[i] = 0;
			}
			else if (tile == Tile::PLAYER2)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				player2->SetPos(pos);
				map[i] = 0;
			}
			else if (tile == Tile::ZENCHAN)
			{

				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				hitbox = enemies->GetEnemyHitBox(pos, EnemyType::SLIME);
				area = level->GetSweptAreaX(hitbox);
				enemies->Add(pos, EnemyType::SLIME, area);
				
			}
			else if (tile == Tile::DRUNK)
			{

				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;				
				hitbox = enemies->GetEnemyHitBox(pos, EnemyType::SD);
				area = level->GetSweptAreaX(hitbox);
				enemies->Add(pos, EnemyType::DRUNK, area);
				/*if(stage != 4)numEnemies++;*/

			}
			else if (tile == Tile::SD)
			{

				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				hitbox = enemies->GetEnemyHitBox(pos, EnemyType::SLIME);
				area = level->GetSweptAreaX(hitbox);
				enemies->Add(pos, EnemyType::SD, area);
				if (stage != 4)numEnemies++;

			}
			else if (tile == Tile::DRUNKR)
			{

				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				hitbox = enemies->GetEnemyHitBox(pos, EnemyType::SLIME);
				area = level->GetSweptAreaX(hitbox);
				enemies->Add(pos, EnemyType::DRUNK, area);
				/*numEnemies++;*/

			}
			else if (tile == Tile::BUBBLE)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				bubl = new Bubble(pos, Direction::LEFT);

				bubbles.push_back(bubl);
				map[i] = 0;
			}
			else if (tile == Tile::OBJECT)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				obj = new Object(pos);
				objects.push_back(obj);
				AllObjects++;
				map[i] = 0;
			}
			else if (tile == Tile::DOT)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				obj = new Object(pos, ObjectType::DOT);
				thunds.push_back(obj);
				map[i] = 0;
			}
			++i;
		}
	}
	delete[] map;

	return AppStatus::OK;
}
void Scene::RandomItemSpawn()
{
	
}
void Scene::BubbleSpawner()
{
	int maxTimeX = GetRandomValue(5, 10);
	int maxTimeY = GetRandomValue(5, 10);

	switch (stage)
	{
	case 2:
		Point p1 = { 80, 226 };
		Point p2 = { 160, 226 };
		if (eTimeSpawnX >= maxTimeX)
		{
			Bubble* bubl = new Bubble(p1, Direction::LEFT);
			bubbles.push_back(bubl);
			eTimeSpawnX = 0;
		}
		else if (eTimeSpawnY >= maxTimeY)
		{
			Bubble* bubl2 = new Bubble(p2, Direction::RIGHT);
			bubbles.push_back(bubl2);
			eTimeSpawnY = 0;
		}
		eTimeSpawnX += GetFrameTime();
		eTimeSpawnY += GetFrameTime();
	}
}
void Scene::avoidCrashingBubbles()
{
	eDbg += GetFrameTime();

	if (eDbg >= 5)
	{

		BubbleFromPlayer* buble = new BubbleFromPlayer({ 1000, 1000 }, Directions::LEFT);
		buble->Initialise();
		bubblesPlayer.push_back(buble);

		BubbleFromPlayer* bublee = new BubbleFromPlayer({ 1000, 1000 }, Directions::LEFT);
		buble->Initialise();
		bubblesPlayer.push_back(bublee);

		BubbleFromPlayer* bubles = new BubbleFromPlayer({ 1000, 1000 }, Directions::LEFT);
		buble->Initialise();
		bubblesPlayer.push_back(bubles);

		BubbleFromPlayer* bublex = new BubbleFromPlayer({ 1000, 1000 }, Directions::LEFT);
		buble->Initialise();
		bubblesPlayer.push_back(bublex);


		BubbleFromPlayer* bublea = new BubbleFromPlayer({ 1000, 1000 }, Directions::LEFT);
		buble->Initialise();
		bubblesPlayer.push_back(bublea);

		eDbg = 0;
	}
}
void Scene::PlayerBubbleSpawn()
{
	eBubblingTime += GetFrameTime();
	
	if (IsKeyPressed(KEY_L) && eBubblingTime >= .3 && !player->STOP)
	{
		
		if (player->IsLookingLeft())
		{
			BubbleFromPlayer* buble = new BubbleFromPlayer(player->GetPos(), Directions::LEFT);
			buble->Initialise();
			buble->SetTileMap(level);
			if (stage == 3) {
				buble->diffWindMap = true;
			}
			bubblesPlayer.push_back(buble);

		}
		else 
		{
			BubbleFromPlayer* buble = new BubbleFromPlayer(player->GetPos(), Directions::RIGHT);
			buble->Initialise();
			buble->SetTileMap(level);
			if (stage == 3) {
				buble->diffWindMap = true;
			}
			bubblesPlayer.push_back(buble);
		}
		eBubblingTime = 0;
	
	}
	if ((IsKeyPressed(KEY_H) && eBubblingTime >= .3 && !player2->STOP)&&(P2in==true))
	{

		if (player2->IsLookingLeft())
		{
			BubbleFromPlayer* buble = new BubbleFromPlayer(player2->GetPos(), Directions::RIGHTDEBUG);
			buble->Initialise();
			buble->SetTileMap(level);
			buble->isP2 = true;
			if (stage == 3) {
				buble->diffWindMap = true;
			}
			bubblesPlayer2.push_back(buble);

		}
		else if(player2->IsLookingRight())
		{
			BubbleFromPlayer* buble = new BubbleFromPlayer(player2->GetPos(), Directions::RIGHT);
			buble->Initialise();
			buble->SetTileMap(level);		
			buble->isP2 = true;
			if (stage == 3) {
				buble->diffWindMap = true;
			}
			bubblesPlayer2.push_back(buble);
		}
		eBubblingTime = 0;

	}
	
	
}
void Scene::DebugFRUITSANDENEMIES()
{
	Point posEnemy = { (float)player->GetPos().x + 64, (float)player->GetPos().y };
	if (IsKeyPressed(KEY_SIX))
	{
		Point p = { (float)player->GetPos().x + 32, (float)player->GetPos().y };
		Object* obj = new Object(p);
		objects.push_back(obj);
		AllObjects++;
	}
	if (IsKeyPressed(KEY_SEVEN))
	{
		AABB hitbox = enemies->GetEnemyHitBox(posEnemy, EnemyType::SLIME);
		AABB area = level->GetSweptAreaX(hitbox);
		enemies->Add(posEnemy, EnemyType::SLIME, area);
	}
	if (IsKeyPressed(KEY_EIGHT))
	{
		AABB hitbox = enemies->GetEnemyHitBox(posEnemy, EnemyType::SLIME);
		AABB area = level->GetSweptAreaX(hitbox);
		enemies->Add(posEnemy, EnemyType::DSLIME, area);
	}
	if (IsKeyPressed(KEY_NINE))
	{
		AABB hitbox = enemies->GetEnemyHitBox(posEnemy, EnemyType::SLIME);
		AABB area = level->GetSweptAreaX(hitbox);
		enemies->Add(posEnemy, EnemyType::DRUNK, area);
	}
	if (IsKeyPressed(KEY_ZERO))
	{
		AABB hitbox = enemies->GetEnemyHitBox(posEnemy, EnemyType::SLIME);
		AABB area = level->GetSweptAreaX(hitbox);
		enemies->Add(posEnemy, EnemyType::DDRUNK, area);
	}
	if (IsKeyPressed(KEY_P))
	{
		AABB hitbox = enemies->GetEnemyHitBox(posEnemy, EnemyType::SLIME);
		AABB area = level->GetSweptAreaX(hitbox);
		enemies->Add(posEnemy, EnemyType::SD, area);
	}
}
void Scene::Update()
{
	Point p1, p2;
	AABB box;
	AABB hitbox;

	PlayerBubbleSpawn();
	DebugFRUITSANDENEMIES();
	//Switch between the different debug modes: off, on (sprites & hitboxes), on (hitboxes) 
	if (IsKeyPressed(KEY_F1))
	{
		debug = (DebugMode)(((int)debug + 1) % (int)DebugMode::SIZE);
	}
	//Debug levels instantly
	if (numEnemies <= 0)
	{
		eTimeTrans += GetFrameTime();
		if (eTimeTrans > 7) {
			nextSceneTrigger = true;
			
		}
		
	}
	else {
		eTimeTrans = 0;
		nextSceneTrigger = false;
	}
	if (IsKeyPressed(KEY_ONE))
	{
		stage = 1;
		LoadLevel(1);
	}
	else if (IsKeyPressed(KEY_TWO))
	{
		stage = 2;
		LoadLevel(2);
	}
	else if (IsKeyPressed(KEY_THREE))
	{
		stage = 3;
		LoadLevel(3);
	}
	else if (IsKeyPressed(KEY_FOUR))
	{
		stage = 4;
		LoadLevel(4);
	}
	else if (IsKeyPressed(KEY_FIVE))
	{
		stage = 5;
		LoadLevel(5);
	}
	level->Update();
	hitbox = player->GetHitbox();
	hitbox = player2->GetHitbox();
	if (player->isThund == true)
	{
		for (BubbleFromPlayer* buble : bubblesPlayer)
		{
			buble->isThund = true;

		}
	}
	if (player2->isThund == true)
	{
		for (BubbleFromPlayer* buble : bubblesPlayer2)
		{
			buble->isThund = true;

		}
	}
	avoidCrashingBubbles();

	if (P2in == true)
	{
		player2->Update();
	}
	if (P1in == true)
	{
		player->Update();
	}
	for (Projectile* proj : projectiles)
	{
		proj->Update();
	}
	for (Projectile* proj : SDBOTTLES)
	{
		proj->Update();
	}
	for (Projectile* proj : thunders)
	{
		proj->Update();
	}
	enemies->Update(hitbox);
	shots->Update(hitbox);
	particles->Update();
	UpdateBubbles();
	BubbleSpawner();
	BubbleDespawn();

	if (P1in==true)
	{
		for (BubbleFromPlayer* buble : bubblesPlayer)
		{
			buble->SetPlayer(player);

		}
	}


	if (P2in == true)
	{
		for (BubbleFromPlayer* buble : bubblesPlayer2)
		{
			buble->SetPlayer2(player2);

		}
	}

	CheckCollisions();
}
void Scene::Render()
{
	BeginMode2D(camera);
	level->Render();

	if (debug == DebugMode::OFF || debug == DebugMode::SPRITES_AND_HITBOXES) {
		RenderObjects();
		enemies->Draw();
		player->Draw();
		player2->Draw();
		shots->Draw();
	}

	if (debug == DebugMode::SPRITES_AND_HITBOXES || debug == DebugMode::ONLY_HITBOXES) {
		RenderObjectsDebug(YELLOW);
		enemies->DrawDebug();
		if (P1in == true)
		{
			player->DrawDebug(GREEN);
		}
		if (P2in == true)
		{
			player->DrawDebug(GREEN);
		}
		shots->DrawDebug(GRAY);
	}
	if (player->IsGod()) {
		DrawText("God Mode", 0, WINDOW_HEIGHT - TILE_SIZE, 100, GOLD);
	}
	if (player2->IsGod()) {
		DrawText("God Mode p2", 0, WINDOW_HEIGHT - TILE_SIZE, 100, GOLD);
	}
	EndMode2D();
}
void Scene::Release()
{
	level->Release();
	player->Release();
	player2->Release();
}
void Scene::CheckCollisions()
{
	AABB player_box, obj_box, ene_box,player2_box;
	if (P1in == true)
	{
		player_box = player->GetHitbox();
	}
	if (P2in == true)
	{
		player2_box = player2->GetHitbox();
	}
	eTimeSDBH += GetFrameTime();
	if (eTimeSDBH >= 2 && stage == 5)
	{
		for (Enemy* enemy : enemies->GetEnemies())
		{
			if (!enemy->ableToDie)
			{
				Point midPos = { (float)enemy->GetPos().x + 32, (float)enemy->GetPos().y - 32 };

				Projectile* projo = new Projectile(midPos, Look::L);
				projectiles.push_back(projo);

				Projectile* proje = new Projectile(midPos, Look::LO);
				projectiles.push_back(proje);

				Projectile* proji = new Projectile(midPos, Look::O);
				projectiles.push_back(proji);

				Projectile* proju = new Projectile(midPos, Look::RO);
				projectiles.push_back(proju);

				Projectile* projp = new Projectile(midPos, Look::R);
				projectiles.push_back(projp);
			}
			
		}
		eTimeSDBH = 0;
	}
	if (P1in == true)
	{
		for (Enemy* enemy : enemies->GetEnemies())
		{
			AABB player_boxx;
			AABB player2_boxx;
			if (P1in == true)
			{
				player_boxx = player->GetHitbox();
			}
			if (P2in == true)
			{
				player2_boxx = player2->GetHitbox();
			}
			AABB enemy_hitbox = enemy->GetHitbox();
			if (player_boxx.TestAABB(enemy_hitbox) && enemy->ableToDie == true)
			{
				enemy->killed = true;

				
			}
			if (player2_boxx.TestAABB(enemy_hitbox) && enemy->ableToDie == true)
			{
				enemy->killed = true;


			}
			if (enemy->killed == true)
			{
				eTTF += GetFrameTime();
				if (eTTF >= 4)
				{
					yep = true;
				}
			}
			if (player_boxx.TestAABB(enemy_hitbox) && !enemy->ableToDie) {
				if (player->Ikilleable ) {
					Point posplayer = player->GetPos();
					posplayer.y -= 16;
					player->SetPos(posplayer);
					player->SetDeathAnim();
					player->toogleWasHit();
					player->DecLiv();

				}
			}
			
			AABB enemy_box;
			enemy_box = enemy->GetHitArea();
			if (!enemy->noSpawnMore)
			{

				eTimeBottle += GetFrameTime();

				if (player_boxx.TestAABB(enemy_box) && (P1in == true)) {


					enemy->isshooting = true;

				}

				if ((player2_boxx.TestAABB(enemy_box)) && (P2in == true)) {

					enemy->isshooting = true;
				}
				if (enemy->isshooting && !enemy->noSpawnMore && eTimeBottle > GetRandomValue(5, 8))
				{
					enemy->isshooting = false;
					if (player->GetPos().y > 32) {

						enemy->lerping = true;
						
					}
					if (player2->GetPos().y > 32) {

						enemy->lerping = true;

					}
					if (stage == 4 || stage == 3)
					{
						Projectile* proj = new Projectile(enemy->GetPos(), enemy->GetDir());
						projectiles.push_back(proj);
					}
					
					eTimeBottle = 0;


				}


			}

		}
		//bubble
		for (BubbleFromPlayer* bubble : bubblesPlayer)
		{
			AABB bubble_box = bubble->GetHitbox();

			if (bubble_box.TestAABB(player_box) && bubble->inCatch && !bubble->inShoot)
			{
				if (bubble->poped == false)
				{
					numEnemies--;
				}
			}
			for (BubbleFromPlayer* bubble2 : bubblesPlayer)
			{
				if (bubble == bubble2) continue;

				if (bubble2->issAlive)
				{
					AABB bubble_box2 = bubble2->GetHitbox();

					if (bubble_box.TestAABB(bubble_box2))
					{
						if (stage != 3 && stage != 5)
						{
							bubble->MoveBubbleToRandomNear();
							bubble2->MoveBubbleToRandomNear();
						}

						
						break;
					}
				}

			}
			if (player->IsMoving()) {
				if (player->IsLookingRight() && bubble_box.TestAABB(player_box))
				{
					bubble->MoveBubbleRightPlayer();

				}
				
				if (player->IsLookingLeft() && bubble_box.TestAABB(player_box))
				{
					bubble->MoveBubbleLeftPlayer();

				}
				
			}
			if (bubble->cameFromDown == true && stage != 5)
			{
				if (bubble->poped == false )
				{
					Point pos = bubble->GetPos();
					BubbleFromPlayer* part = new BubbleFromPlayer(pos, bubble->dire);
					part->Initialise();
					part->popedParticles = true;
					bubblesPlayer.push_back(part);
					player->IncrScore(50);

				}
				bubble->noZesty = true;
				bubble->issAlive = false;
				bubble->poped = true;
			}
			if (player2->IsMoving()) {
				if (player2->IsLookingRight() && bubble_box.TestAABB(player2_box))
				{
					bubble->MoveBubbleRightPlayer();

				}

				if (player2->IsLookingLeft() && bubble_box.TestAABB(player2_box))
				{
					bubble->MoveBubbleLeftiePlayer();

				}

			}

			for (Enemy* enemy : enemies->GetEnemies())
			{

				AABB enemy_box = enemy->GetHitbox();
				if (bubble_box.TestAABB(enemy_box) && bubble->canCollide && !bubble->inCatch && stage != 5)
				{
					if (stage == 1 || stage == 2) {
						bubble->enemytype = 0;
					}
					else if (stage == 4 || stage == 3) {
						bubble->enemytype = 1;

					}
					enemies->DestroyEnemy(enemy);
					bubble->SetAlive(false);
					bubble->inCatch = true;

					break;
				}


			}
			if (bubble_box.TestAABB(player_box) && bubble->inCatch && !bubble->inShoot)
			{
				ResourceManager::Instance().PlaySoundEffect(Resource::SFX_BUBBLE_POP);
				
				if (bubble->poped == false)
				{

					Point pos = bubble->GetPos();
					BubbleFromPlayer* part = new BubbleFromPlayer(pos,bubble->dire);
					part->Initialise();
					part->popedParticles = true;
					player->IncrScore(1000);
					bubblesPlayer.push_back(part);
				}
				bubble->poped = true;


				if (stage != 3 || stage != 4)
				{
					bubble->SetAnimationE((int)Animations::ZENCHAN_DEATH);

				}
				else {
					bubble->SetAnimationE((int)Animations::DRUNK_DEATH);

				}

				break;
			}
			
			if (bubble_box.TestAABB(player_box) && stage == 5 && !bubble->inShoot && player->isThund)
			{
				ResourceManager::Instance().PlaySoundEffect(Resource::SFX_BUBBLE_POP);
				if (bubble->poped == false)
				{
					Point pos = bubble->GetPos();
					BubbleFromPlayer* part = new BubbleFromPlayer(pos, bubble->dire);
					part->Initialise();
					part->popedParticles = true;
					bubblesPlayer.push_back(part);
					Projectile* proj = new Projectile(pos, player->GetDir(), true);
					proj->isThund = true;
					thunders.push_back(proj);
					bubble->issAlive = false;
				}
				bubble->poped = true;


				break;
			}

		
			if ((bubble->hasEndedFromCatch) && (bubble->poped == false) && !bubble->popedParticles) {
				Point pos = bubble->GetPos();
				pos.x += (SLIME_FRAME_SIZE - SLIME_PHYSICAL_WIDTH) / 2;
				AABB hitbox = enemies->GetEnemyHitBox(pos, EnemyType::SLIME);
				AABB area = level->GetSweptAreaX(hitbox);
				switch (bubble->enemytype) {
				case 0:

					bubble->hasEndedFromCatch = false;
					enemies->Add(pos, EnemyType::DSLIME, area);
					break;
				case 1:

					bubble->hasEndedFromCatch = false;
					enemies->Add(pos, EnemyType::DDRUNK, area);
					break;

				}

			}
			if (bubble->fruit == true && !bubble->isThund)
			{
				Object* obj = new Object(bubble->GetPos());
				objects.push_back(obj);
				AllObjects++;
				bubble->issAlive = false;
			}

		}

	}
	if (P2in == true)
	{
		for (BubbleFromPlayer* bubble : bubblesPlayer2)
		{
			AABB bubble_box = bubble->GetHitbox();
			for (BubbleFromPlayer* bubble2 : bubblesPlayer2)
			{
				if (bubble == bubble2) continue;

				if (bubble2->issAlive)
				{
					AABB bubble_box2 = bubble2->GetHitbox();

					if (bubble_box.TestAABB(bubble_box2))
					{
						if (stage != 3 && stage != 5)
						{
							bubble->MoveBubbleToRandomNear();
							bubble2->MoveBubbleToRandomNear();
						}


						break;
					}
				}

			}
			if (bubble_box.TestAABB(player2_box) && stage == 5 && !bubble->inShoot && player2->isThund)
			{
				ResourceManager::Instance().PlaySoundEffect(Resource::SFX_BUBBLE_POP);
				if (bubble->poped == false)
				{
					Point pos = bubble->GetPos();

					if (stage == 5)
					{

						BubbleFromPlayer* part = new BubbleFromPlayer(pos, bubble->dire);
						part->Initialise();
						part->popedParticles = true;
						bubblesPlayer.push_back(part);
						bubble->issAlive = false;

					}
					Projectile* proj = new Projectile(pos, player->GetDir(), true);
					proj->isThund = true;
					thunders.push_back(proj);
				}
				bubble->poped = true;


				break;
			}
			if (bubble_box.TestAABB(player2_box) && bubble->inCatch && !bubble->inShoot)
			{
				ResourceManager::Instance().PlaySoundEffect(Resource::SFX_BUBBLE_POP);

				if (bubble->poped == false)
				{
					Point pos = bubble->GetPos();
					BubbleFromPlayer* part = new BubbleFromPlayer(pos, bubble->dire);
					part->Initialise();
					part->popedParticles = true;
					bubblesPlayer.push_back(part);
					player2->IncrScore(1000);
					numEnemies--;
					bubble->poped = true;

				}
				bubble->poped = true;

				if (stage != 3 || stage != 4)
				{
					bubble->SetAnimationE((int)Animations::ZENCHAN_DEATH);

				}
				else {
					bubble->SetAnimationE((int)Animations::DRUNK_DEATH);

				}

				break;
			}
			if (player2->IsMoving()) {
				if (player2->IsLookingRight() && bubble_box.TestAABB(player2_box))
				{
					bubble->MoveBubbleRightPlayer();

				}
				if (player2->IsLookingLeft() && bubble_box.TestAABB(player_box))
				{
					bubble->MoveBubbleLeftPlayer();

				}
			}
			if (bubble->cameFromDown == true) 
			{
				if (bubble->poped == false)
				{
					Point pos = bubble->GetPos();
					BubbleFromPlayer* part = new BubbleFromPlayer(pos, bubble->dire);
					part->Initialise();
					part->popedParticles = true;
					bubblesPlayer.push_back(part);
					player2->IncrScore(50);

				}
				bubble->noZesty = true;
				bubble->issAlive = false;
				bubble->poped = true;
			}
			for (Enemy* enemy : enemies->GetEnemies())
			{

				AABB enemy_box = enemy->GetHitbox();
				if (bubble_box.TestAABB(enemy_box) && bubble->canCollide && !bubble->inCatch && stage != 5)
				{
					if (stage == 1 || stage == 2) {
						bubble->enemytype = 0;
					}
					else if (stage == 4 || stage == 3) {
						bubble->enemytype = 1;

					}
					enemies->DestroyEnemy(enemy);
					bubble->SetAlive(false);
					bubble->inCatch = true;

					break;
				}
				
				
			}
			
			if ((bubble->hasEndedFromCatch) && (bubble->poped == false)) {
				Point pos = bubble->GetPos();
				pos.x += (SLIME_FRAME_SIZE - SLIME_PHYSICAL_WIDTH) / 2;
				AABB hitbox = enemies->GetEnemyHitBox(pos, EnemyType::SLIME);
				AABB area = level->GetSweptAreaX(hitbox);
				switch (bubble->enemytype) {
				case 0:

					enemies->Add(pos, EnemyType::SLIME, area);
					bubble->issAlive = false;
					break;
				case 1:

					enemies->Add(pos, EnemyType::DRUNK, area);
					bubble->issAlive = false;
					break;

				}

			}
			if (bubble->fruit == true && !bubble->isThund)
			{
				Object* obj = new Object(bubble->GetPos());
				objects.push_back(obj);
				AllObjects++;
				bubble->issAlive = false;
			}

		}
		for (Enemy* enemy : enemies->GetEnemies())
		{
			AABB enemy_box = enemy->GetHitbox();
			if (player2_box.TestAABB(enemy_box) && !enemy->ableToDie) {
				if (player2->Ikilleable) {
					Point posplayer = player2->GetPos();
					posplayer.y -= 16;
					player2->SetPos(posplayer);
					player2->SetDeathAnim();
					player2->toogleWasHit();
					player2->DecLiv();

				}
			}
			if (player2_box.TestAABB(enemy_box) && enemy->ableToDie == true)
			{
				enemy->killed = true;


			}
		}
		
	}

	auto it = objects.begin();
	while (it != objects.end())
	{
		obj_box = (*it)->GetHitbox();
		if (player_box.TestAABB(obj_box) && P1in == true)
		{
			ResourceManager::Instance().PlaySoundEffect(Resource::SFX_PICKUP);

			player->IncrScore((*it)->Points());
			(*it)->DeleteHitbox();
			(*it)->point = true;


		}
		if (player2_box.TestAABB(obj_box) && P2in==true)
		{
			ResourceManager::Instance().PlaySoundEffect(Resource::SFX_PICKUP);

			player2->IncrScore((*it)->Points());
			(*it)->DeleteHitbox();
			(*it)->point = true;
			(*it)->P1 = false;

		}
		else
		{
			//Move to the next object
			++it;
		}
	}

	auto itten = thunds.begin();
	while (itten != thunds.end())
	{
		obj_box = (*itten)->GetHitbox();
		if (player_box.TestAABB(obj_box) && P1in == true)
		{
			ResourceManager::Instance().PlaySoundEffect(Resource::SFX_PICKUP);

			player->IncrScore((*itten)->Points());
			(*itten)->DeleteHitbox();
			(*itten)->point = true;
			player->isThund = true;

		}
		else if (player2_box.TestAABB(obj_box) && P2in == true)
		{
			ResourceManager::Instance().PlaySoundEffect(Resource::SFX_PICKUP);

			player2->IncrScore((*itten)->Points());
			(*itten)->DeleteHitbox();
			(*itten)->point = true;
			(*itten)->P1 = false;
			player2->isThund = true;

		}
		else
		{
			//Move to the next object
			++itten;
		}
	}
	for (Projectile* proj : projectiles)
	{
		
		AABB projectile_box = proj->GetHitbox();
		for (Enemy* enemy : enemies->GetEnemies())
		{
			AABB ene_box = enemy->GetHitbox();
			if (ene_box.TestAABB(projectile_box) && proj->isThund)
			{
				enemy->SDhp--;
				proj->isLive = false;
			}
		}
		

		if (player_box.TestAABB(projectile_box) && P1in == true && !proj->isThund)
		{
			if (player->Ikilleable) {
				Point posplayer = player->GetPos();
				posplayer.y -= 16;
				player->SetPos(posplayer);
				player->SetDeathAnim();
				player->toogleWasHit();
				player->DecLiv();

			}
		}
		if (player2_box.TestAABB(projectile_box)&& P2in==true && !proj->isThund)
		{
			if (player2->Ikilleable) {
				Point posplayer = player2->GetPos();
				posplayer.y -= 16;
				player2->SetPos(posplayer);
				player2->SetDeathAnim();
				player2->toogleWasHit();
				player2->DecLiv();

			}
		}

		

	}
	for (Projectile* proj : thunders)
	{

		AABB projectile_box = proj->GetHitbox();
		for (Enemy* enemy : enemies->GetEnemies())
		{
			AABB ene_box = enemy->GetHitbox();
			if (ene_box.TestAABB(projectile_box))
			{
				enemy->SDhp--;
				proj->isLive = false;
			}
		}


		



	}
	if (IsKeyDown(KEY_O)) {

		if (P1in == true)
		{
			Point positionPlayer;
			positionPlayer = player->GetPos();
			AABB hitbox = player->GetHitbox();
			AABB area = level->GetSweptAreaX(hitbox);
			enemies->Add(positionPlayer, EnemyType::BOTTLE, area);
		}

		if (P2in == true)
		{
			Point positionPlayer;
			positionPlayer = player2->GetPos();
			AABB hitbox = player2->GetHitbox();
			AABB area = level->GetSweptAreaX(hitbox);
			enemies->Add(positionPlayer, EnemyType::BOTTLE, area);
		}
	}
}
void Scene::BubbleDespawn()
{
	auto iterator = bubbles.begin();
	int i = 0;
	while (iterator != bubbles.end() && i < bubbles.size())
	{
		if (bubbles[i]->isAlive() == false)
		{
			//Delete the object
			delete* iterator;
			//Erase the object from the vector and get the iterator to the next valid element
			iterator = bubbles.erase(iterator);
		}
		else
		{
			//Move to the next object
			++iterator;
			++i;
		}
	}
	if (P1in == true)
	{
		auto iterate = bubblesPlayer.begin();
		int o = 0;
		while (iterate != bubblesPlayer.end() && o < bubblesPlayer.size())
		{
			if (bubblesPlayer[o]->isAlive() == false || !bubblesPlayer[o]->issAlive)
			{
				//Delete the object
				delete* iterate;
				//Erase the object from the vector and get the iterate to the next valid element
				iterate = bubblesPlayer.erase(iterate);
			}
			else
			{
				//Move to the next object
				++iterate;
				++o;
			}
		}
	}

	if (P2in == true)
	{
		auto iterate = bubblesPlayer2.begin();
		int o = 0;
		while (iterate != bubblesPlayer2.end() && o < bubblesPlayer2.size())
		{
			if (bubblesPlayer2[o]->isAlive() == false || !bubblesPlayer2[o]->issAlive)
			{
				//Delete the object
				delete* iterate;
				//Erase the object from the vector and get the iterate to the next valid element
				iterate = bubblesPlayer2.erase(iterate);
			}
			else
			{
				//Move to the next object
				++iterate;
				++o;
			}
		}
	}
}
void Scene::ClearLevel()
{
	for (Object* obj : objects)
	{
		delete obj;
	}
	objects.clear();
	for (Object* obje : thunds)
	{
		delete obje;
	}
	thunds.clear();
	for (Projectile* proj : projectiles)
	{
		delete proj;
	}
	projectiles.clear();
	for (Projectile* proj : SDBOTTLES)
	{
		delete proj;
	}
	projectiles.clear();
	for (Projectile* proj : thunders)
	{
		delete proj;
	}
	thunders.clear();
	for (Bubble* bubl : bubbles)
	{
		delete bubl;
	}
	bubbles.clear();
	if (P1in == true)
	{
		for (BubbleFromPlayer* buble : bubblesPlayer)
		{
			delete buble;
		}
		bubblesPlayer.clear();
	}
	if (P2in == true)
	{
		for (BubbleFromPlayer* buble : bubblesPlayer2)
		{
			delete buble;
		}
		bubblesPlayer2.clear();
	}
	enemies->Release();
	shots->Clear();
	particles->Clear();
}
void Scene::UpdateBubbles()
{
	for (Bubble* bubl : bubbles)
	{
		bubl->Update();
	}
	if (P1in == true)
	{
		for (BubbleFromPlayer* buble : bubblesPlayer)
		{
			buble->Update();
		}
	}
	if(P2in==true)
	{
		for (BubbleFromPlayer* buble : bubblesPlayer2)
		{
			buble->Update();
		}
	}

}
void Scene::RenderObjects()
{
	auto at = objects.begin();
	while (at != objects.end())
	{
		
		if ((*at)->point==true)
		{
			if ((*at)->pastTime(1) == false)
			{
				(*at)->Draw();
				(*at)->DrawPoints();
				if ((int)(*at)->framecounter % 3 ==0)
				{
					(*at)->PointsAnimation();
				}
			}
			else
			{
				//Delete the object
				delete* at;
				//Erase the object from the vector and get the iterator to the next valid element
				at=objects.erase(at);
				AllObjects--;
				
			}
			
		}
		else
		{
			(*at)->Draw();
		}
		if (at != objects.end())
		{
			++at;
		}

	}
	
	auto atee = thunds.begin();
	while (atee != thunds.end())
	{

		if ((*atee)->point == true)
		{
			if ((*atee)->pastTime(1) == false)
			{
				(*atee)->Draw();
				(*atee)->DrawPoints();
				if ((int)(*atee)->framecounter % 3 == 0)
				{
					(*atee)->PointsAnimation();
				}
			}
			else
			{
				//Delete the object
				delete* atee;
				//Erase the object from the vector and get the iterateeor to the next valid element
				atee = thunds.erase(atee);
				AllObjects--;

			}

		}
		else
		{
			(*atee)->Draw();
		}
		if (atee != thunds.end())
		{
			++atee;
		}

	}
	auto tate = thunders.begin();
	while (tate != thunders.end())
	{

		if((*tate)->isLive == false)
		{
			//Delete the object
			delete* tate;
			//Erase the object from the vector and get the iterator to the next valid element
			tate = thunders.erase(tate);

		}
		else
		{
			(*tate)->Draw();
			// Move to the next element
			++tate;
		}

	}
	for (Projectile* proje : projectiles)
	{
		proje->Draw();
	}
	for (Projectile* projec : SDBOTTLES)
	{
		projec->Draw();
	}
	for (Bubble* bubl : bubbles)
	{
		bubl->Draw();
	}
	if (P1in == true)
	{
		auto it = bubblesPlayer.begin();
		while (it != bubblesPlayer.end())
		{

			(*it)->Draw();
			++it;
		}
	}

	if (P2in == true)
	{
		auto it = bubblesPlayer2.begin();
		while (it != bubblesPlayer2.end())
		{

			(*it)->Draw();
			++it;
		}
	}

}
void Scene::RenderObjectsDebug(const Color& col) const
{
	for (Object* obj : objects)
	{
		obj->DrawDebug(col);
	}
	for (Projectile* proj : projectiles)
	{
		proj->DrawDebug(col);
	}
	for (Projectile* proj : SDBOTTLES)
	{
		proj->DrawDebug(col);
	}
	for (Bubble* bubl : bubbles)
	{
		bubl->DrawDebug(BLUE);
	}
	if (P1in == true)
	{
		for (BubbleFromPlayer* buble : bubblesPlayer)
		{
			buble->DrawDebug(BLUE);
		}
	}
	if (P2in == true)
	{
		for (BubbleFromPlayer* buble : bubblesPlayer2)
		{
			buble->DrawDebug(BLUE);
		}
	}

	
}
int Scene::Score() const
{
	if (this == nullptr)
	{
		return 000;
	}
	else
	{
		return player->GetScore();

	}
}
int Scene::Score2() const
{
	if (this == nullptr)
	{
		return 000;
	}
	else
	{
		return player2->GetScore();
	}
}
void Scene::ResetScore() const
{
	if(this != nullptr)
	{
		player->InitScore();
		player2->InitScore();
	}

}

