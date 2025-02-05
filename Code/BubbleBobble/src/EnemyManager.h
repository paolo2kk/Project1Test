#pragma once
#include "Enemy.h"
#include "Turret.h"
#include "Slime.h"
#include "Drunk.h"
#include "TileMap.h"
#include "SuperDrunk.h"
#include "ShotManager.h"
#include "DrunkEvil.h"
#include "SlimeEvil.h"

class EnemyManager
{
public:
	EnemyManager();
	~EnemyManager();

	AppStatus Initialise();

	//Set the ShotManager reference for managing enemy shots
	void SetShotManager(ShotManager* shots);
	void DestroyEnemy(Enemy* enemy);
	void SetTileMap(TileMap* level);

	//Add a new enemy with the given position, type, action area and looking direction
	void Add(const Point& pos, EnemyType type, const AABB& area, Look look = Look::RIGHT);

	//Retrieve the hitbox of an enemy based on the given position and type
	AABB GetEnemyHitBox(const Point& pos, EnemyType type) const;
	
	//Update enemies according to their logic. If the given player hitbox is visible to them,
	//they will shoot by adding shots to the ShotManager
	void Update(const AABB& player_hitbox);

	//Draw all enemies
	void Draw() const;

	//Draw enemy hitboxes for debugging
	void DrawDebug() const;
	
	//Delete all enemies and clear the enemy vector
	void Release();
	const std::vector<Enemy*>& GetEnemies() const {
		return enemies;
	}
private:
	std::vector<Enemy*> enemies;
	TileMap* map;
	//Reference to the ShotManager object
	//This class does not own the object, it only holds a reference to it
	ShotManager *shots;
};

