#include "Bottle.h"
#include "StaticImage.h"
#include <cstdlib>
#include "iostream"

Projectile::Projectile(const Point& p, Look view) : Entity(p, 2, 2, TILE_SIZE, TILE_SIZE)
{
	framecounter = 0;
	direction = view;
	Rectangle rc;
	const int n = TILE_SIZE;
	rc = { n, n + 6, n, n }; 
	SetAnimation((int)Animations::DRUNK_PROJECTILE);
}
Projectile::Projectile(const Point& p, Look view, bool a) : Entity(p, TILE_SIZE, TILE_SIZE, TILE_SIZE, TILE_SIZE)
{
	if (a == true)
	{
		framecounter = 0;
		direction = view;
		Rectangle rc;
		const int n = TILE_SIZE;
		rc = { n, n + 6, n, n };
		SetAnimation((int)Animations::THUNDER);
	}
}

void Projectile::SetAnimation(int id)
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->SetAnimation(id);
}
Projectile::~Projectile()
{

}
void Projectile::Release()
{

	render->Release();
}
void Projectile::Update()
{
	Sprite* sprite = dynamic_cast<Sprite*>(render);
	sprite->Update();
	pos += dir;
	MoveX();
}
void Projectile::MoveX()
{
	
	if (direction == Look::LEFT && !isThund) 
	{
		dir = { -2, 0 };
	}
	else if (direction == Look::RIGHT && !isThund)
	{
		dir = { 2, 0 };
	}
	if (direction == Look::LEFT && isThund)
	{
		dir = { 2, 0 };
	}
	else if (direction == Look::RIGHT && isThund)
	{
		dir = { -2, 0 };
	}

	if (direction == Look::L)
	{
		dir = {-2, 0};
	}
	else if (direction == Look::LO)
	{
		dir = { -2, 2 };
	}
	else if (direction == Look::O)
	{
		dir = { 0, 2 };
	}
	else if (direction == Look::RO)
	{
		dir = { 2, 2 };
	}
	else if (direction == Look::R)
	{
		dir = { 2, 0};
	}
}
void Projectile::DrawDebug(const Color& col) const
{
	Entity::DrawHitbox(pos.x, pos.y, width, height, col);
}
bool Projectile::pastTime(float time)
{
	framecounter++;
	if (framecounter / 60 == time)
	{
		framecounter = 0;
		return true;
	}
	return false;
}
