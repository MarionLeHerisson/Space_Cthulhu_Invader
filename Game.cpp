#include "pch.h"
#include "StringHelpers.h"
#include "Game.h"
#include "EntityManager.h"

const float Game::PlayerSpeed = 100.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);

Game::Game()
	: mWindow(sf::VideoMode(840, 600), "Space Cthulhu Invaders", sf::Style::Close)
	//, mTexture()
	, mPlayer()
	, mFont()
	, mStatisticsText()
	, mStatisticsUpdateTime()
	, mStatisticsNumFrames(0)
	, mIsLookingUp(false)
	, mIsLookingDown(true)
	, mIsLookingRight(false)
	, mIsLookingLeft(false)
	, mRightIsHere(false)
	, mLeftIsHere(false)
	, mUpIsHere(false)
	, mDownIsHere(false)
	, mdirectionLooking("up")
{
	mWindow.setFramerateLimit(160);


	_background.loadFromFile("Media/Assets/background.jpg");
	_TextureFish.loadFromFile("Media/Assets/fish-angler-Sheet.png");
	_TextureBaby.loadFromFile("Media/Assets/baby.png");
	_TextureBubbleGreen.loadFromFile("Media/Assets/bubble-green.png");
	_TextureBubbleRed.loadFromFile("Media/Assets/bubble-red.png");
	_TextureLookingUp.loadFromFile("Media/Assets/cthulhu.png", sf::IntRect(0, 192, 64, 256));
	_TextureLookingDown.loadFromFile("Media/Assets/cthulhu.png", sf::IntRect(0, 0, 64, 64));
	_TextureLookingRight.loadFromFile("Media/Assets/cthulhu.png", sf::IntRect(0, 128, 64, 192));
	_TextureLookingLeft.loadFromFile("Media/Assets/cthulhu.png", sf::IntRect(0, 64, 64, 128));
	_TextureWeapon.loadFromFile("Media/Textures/SI_WeaponGreen.png");
	_TextureWeaponEnemy.loadFromFile("Media/Textures/SI_WeaponYellow.png");
	_TextureEnemy.loadFromFile("Media/Textures/SI_Enemy.png");
	//mTexture.loadFromFile("Media/Textures/SI_Player.png");
	mFont.loadFromFile("Media/Sansation.ttf");

	InitSprites();
}

void Game::ResetSprites()
{
	_IsGameOver = false;
	_IsEnemyWeaponFired = false;
	_IsPlayerWeaponFired = false;
	_IsPlayerTentacleFired = false;

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		entity->m_enabled = true;
	}
}

void Game::InitSprites()
{
	_lives = 3;
	_power = 0;
	_score = 0;
	_IsGameOver = false;
	_IsEnemyWeaponFired = false;
	_IsPlayerWeaponFired = false;
	_IsPlayerTentacleFired = false;



	//
	// Player
	//

	mPlayer.setTexture(_TextureLookingDown);
	mPlayer.setPosition(400.f, 250.f);//set to the middle of the screen
	std::shared_ptr<Entity> player = std::make_shared<Entity>();
	player->m_sprite = mPlayer;
	mPlayer.setTexture(_TextureLookingDown);
	player->m_type = EntityType::player;
	player->m_size = _TextureLookingDown.getSize();
	player->m_position = mPlayer.getPosition();
	EntityManager::m_Entities.push_back(player);



	mStatisticsText.setFont(mFont);
	mStatisticsText.setPosition(5.f, 5.f);
	mStatisticsText.setCharacterSize(10);

	//
	// Lives
	//

	_LivesText.setFillColor(sf::Color::Green);
	_LivesText.setFont(mFont);
	_LivesText.setPosition(10.f, 50.f);
	_LivesText.setCharacterSize(20);
	_LivesText.setString(std::to_string(_lives));

	//
	// Power
	//

	_PowerText.setFillColor(sf::Color::Green);
	_PowerText.setFont(mFont);
	_PowerText.setPosition(10.f, 150.f);
	_PowerText.setCharacterSize(20);
	_PowerText.setString(std::to_string(_power));

	//
	// Text
	//

	_ScoreText.setFillColor(sf::Color::Green);
	_ScoreText.setFont(mFont);
	_ScoreText.setPosition(10.f, 100.f);
	_ScoreText.setCharacterSize(20);
	_ScoreText.setString(std::to_string(_score));
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;
	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;

			processEvents();
			update(TimePerFrame);
		}

		updateStatistics(elapsedTime);
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;

		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;

		case sf::Event::Closed:
			mWindow.close();
			break;
		}
	}

}

void Game::update(sf::Time elapsedTime)
{
	if (mIsLookingUp) {
		mPlayer.setTexture(_TextureLookingUp);
	}
	else if (mIsLookingDown) {
		mPlayer.setTexture(_TextureLookingDown);
	}
	else if (mIsLookingLeft) {
		mPlayer.setTexture(_TextureLookingLeft);
	}
	else if (mIsLookingRight) {
		mPlayer.setTexture(_TextureLookingRight);
	}

	std::shared_ptr<Entity> player = std::make_shared<Entity>();
	player->m_sprite = mPlayer;
	player->m_type = EntityType::player;
	player->m_size = _TextureLookingUp.getSize();
	player->m_position = mPlayer.getPosition();
	EntityManager::m_Entities.push_back(player);
}

void Game::render()
{
	mWindow.clear();

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		mWindow.draw(entity->m_sprite);
	}

	mWindow.draw(mStatisticsText);
	mWindow.draw(mText);
	mWindow.draw(_LivesText);
	mWindow.draw(_ScoreText);
	mWindow.draw(_PowerText);
	mWindow.display();
}

void Game::updateStatistics(sf::Time elapsedTime)
{
	mStatisticsUpdateTime += elapsedTime;
	mStatisticsNumFrames += 1;

	if (mStatisticsUpdateTime >= sf::seconds(1.0f))
	{
		mStatisticsText.setString(
			"Frames / Second = " + toString(mStatisticsNumFrames) + "\n" +
			"Time / Update = " + toString(mStatisticsUpdateTime.asMicroseconds() / mStatisticsNumFrames) + "us");

		mStatisticsUpdateTime -= sf::seconds(1.0f);
		mStatisticsNumFrames = 0;
	}

	//
	// Handle collisions
	//

	if (mStatisticsUpdateTime >= sf::seconds(0.050f))
	{
		if (_IsGameOver == true)
			return;

		HandleTexts();
		HandleGameOver();
		HandleEnemiesSwitching();
		HandleCollisionWeaponEnemy();
		HandleCollisionWeaponPlayer();
		HandleCollisionWeaponBlock();
		HandleCollisionEnemyWeaponBlock();
		HandleCollisionBlockEnemy();
		HanldeWeaponMoves();
		HanldeEnemyWeaponMoves();
		HandleEnemyWeaponFiring();

	}
}

void Game::HandleTexts()
{
	std::string lives = "Lives: " + std::to_string(_lives);
	_LivesText.setString(lives);

	std::string power = "Power: " + std::to_string(_power);
	_PowerText.setString(power);

	std::string score = "Score: " + std::to_string(_score);
	_ScoreText.setString(score);
	return;
}

void Game::HandleCollisionEnemyWeaponBlock()
{
	for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::enemyWeapon)
		{
			continue;
		}

		for (std::shared_ptr<Entity> block : EntityManager::m_Entities)
		{
			if (block->m_type != EntityType::block)
			{
				continue;
			}

			if (block->m_enabled == false)
			{
				continue;
			}

			sf::FloatRect boundWeapon;
			boundWeapon = weapon->m_sprite.getGlobalBounds();

			sf::FloatRect boundBlock;
			boundBlock = block->m_sprite.getGlobalBounds();

			if (boundWeapon.intersects(boundBlock) == true)
			{
				weapon->m_enabled = false;
				_IsEnemyWeaponFired = false;
				//break;
				goto end2;
			}
		}
	}

end2:
	//nop
	return;
}

void Game::HandleCollisionWeaponPlayer()
{
	for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::enemyWeapon)
		{
			continue;
		}

		sf::FloatRect boundWeapon;
		boundWeapon = weapon->m_sprite.getGlobalBounds();

		sf::FloatRect boundPlayer;
		boundPlayer = EntityManager::GetPlayer()->m_sprite.getGlobalBounds();

		if (boundWeapon.intersects(boundPlayer) == true)
		{
			weapon->m_enabled = false;
			_IsEnemyWeaponFired = false;
			if (_score > 100) {
				_lives -= 3;
			}
			else {
				_lives--;
			}
			
			goto end;
		}
	}

end:
	//nop
	return;
}

void Game::HanldeEnemyWeaponMoves()
{
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemyWeapon)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;

		switch (entity->positionSpawn)
		{
		case 1 :
			y += 1.0f;
			if (y >= 600)
			{
				entity->m_enabled = false;
				_IsEnemyWeaponFired = false;
			}
			else
			{
				entity->m_sprite.setPosition(x, y);
			}
			break;
		case 2:
			x -= 1.0f;
			if (x <= 0)
			{
				entity->m_enabled = false;
				_IsEnemyWeaponFired = false;
			}
			else
			{
				entity->m_sprite.setPosition(x, y);
			}
			break;
		case 3:
			y -= 1.0f;
			if (y <= 0)
			{
				entity->m_enabled = false;
				_IsEnemyWeaponFired = false;
			}
			else
			{
				entity->m_sprite.setPosition(x, y);
			}
			break;
		case 4:
			x += 1.0f;
			if (x >= 600)
			{
				entity->m_enabled = false;
				_IsEnemyWeaponFired = false;
			}
			else
			{
				entity->m_sprite.setPosition(x, y);
			}
			break;
		}

		

		
	}
}

void Game::HandleEntitiesApparing() {

}

void Game::HandleEnemyWeaponFiring()
{
	if (_IsEnemyWeaponFired == true)
		return;

	std::vector<std::shared_ptr<Entity>>::reverse_iterator rit = EntityManager::m_Entities.rbegin();
	for (; rit != EntityManager::m_Entities.rend(); rit++)
	{
		std::shared_ptr<Entity> entity = *rit;

		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::enemy)
		{
			continue;
		}

		float x, y;
		x = entity->m_sprite.getPosition().x;
		y = entity->m_sprite.getPosition().y;
		y--;

		std::shared_ptr<Entity> sw = std::make_shared<Entity>();
		sw->m_sprite.setTexture(_TextureWeaponEnemy);
		sw->m_sprite.setPosition(
			x + _TextureWeaponEnemy.getSize().x / 2,
			y + _TextureWeaponEnemy.getSize().y);

		sw->m_sprite.setPosition(
			entity->m_sprite.getPosition().x + _TextureEnemy.getSize().x / 2,
			entity->m_sprite.getPosition().y - 10);

		sw->positionSpawn = entity->positionSpawn;

		sw->m_type = EntityType::enemyWeapon;
		sw->m_size = _TextureWeaponEnemy.getSize();
		EntityManager::m_Entities.push_back(sw);

		_IsEnemyWeaponFired = true;
		break;
	}
}

void Game::HandleCollisionBlockEnemy()
{
	// Handle collision ennemy blocks

	for (std::shared_ptr<Entity> enemy : EntityManager::m_Entities)
	{
		if (enemy->m_enabled == false)
		{
			continue;
		}

		if (enemy->m_type != EntityType::enemy)
		{
			continue;
		}

		for (std::shared_ptr<Entity> block : EntityManager::m_Entities)
		{
			if (block->m_type != EntityType::block)
			{
				continue;
			}

			if (block->m_enabled == false)
			{
				continue;
			}

			sf::FloatRect boundEnemy;
			boundEnemy = enemy->m_sprite.getGlobalBounds();

			sf::FloatRect boundBlock;
			boundBlock = block->m_sprite.getGlobalBounds();

			if (boundEnemy.intersects(boundBlock) == true)
			{
				EntityManager::GetPlayer()->m_enabled = false;
				goto end3;
			}
		}
	}

end3:
	//nop
	return;
}

void Game::HandleEnemiesSwitching()
{
	int r;
	srand(time(NULL));
	//
	// Handle Enemy apparence
	//

	//if entity is here : continu;
	//else : make rando entity appear

	if (!mUpIsHere) {//if no entity up
		std::shared_ptr<Entity> sw = std::make_shared<Entity>();
		sw->m_sprite.setPosition(415, 50);
		r = rand() % 3;
		switch (r) {
		case 0:
			sw->m_type = EntityType::enemy;
			sw->m_sprite.setTexture(_TextureEnemy);
			sw->m_size = _TextureEnemy.getSize();
			break;
		case 1:
			sw->m_type = EntityType::baby;
			sw->m_sprite.setTexture(_TextureBaby);
			sw->m_size = _TextureBaby.getSize();
			break;
		case 2:
			sw->m_type = EntityType::fish;
			sw->m_sprite.setTexture(_TextureFish);
			sw->m_size = _TextureFish.getSize();
			break;
		}
		sw->positionSpawn = 1;
		EntityManager::m_Entities.push_back(sw);
		mUpIsHere = true;
	}
	if (!mDownIsHere) {// down
		std::shared_ptr<Entity> sw = std::make_shared<Entity>();
		sw->m_sprite.setPosition(415, 500);
		r = rand() % 3;
		switch (r) {
		case 0:
			sw->m_type = EntityType::enemy;
			sw->m_sprite.setTexture(_TextureEnemy);
			sw->m_size = _TextureEnemy.getSize();
			break;
		case 1:
			sw->m_type = EntityType::baby;
			sw->m_sprite.setTexture(_TextureBaby);
			sw->m_size = _TextureBaby.getSize();
			break;
		case 2:
			sw->m_type = EntityType::fish;
			sw->m_sprite.setTexture(_TextureFish);
			sw->m_size = _TextureFish.getSize();
			break;
		}
		sw->positionSpawn = 3;
		EntityManager::m_Entities.push_back(sw);
		mDownIsHere = true;
	}
	if (!mLeftIsHere) {// left
		std::shared_ptr<Entity> sw = std::make_shared<Entity>();
		sw->m_sprite.setPosition(20, 250);
		r = rand() % 3;
		switch (r) {
		case 0:
			sw->m_type = EntityType::enemy;
			sw->m_sprite.setTexture(_TextureEnemy);
			sw->m_size = _TextureEnemy.getSize();
			break;
		case 1:
			sw->m_type = EntityType::baby;
			sw->m_sprite.setTexture(_TextureBaby);
			sw->m_size = _TextureBaby.getSize();
			break;
		case 2:
			sw->m_type = EntityType::fish;
			sw->m_sprite.setTexture(_TextureFish);
			sw->m_size = _TextureFish.getSize();
			break;
		}
		sw->positionSpawn = 4;
		EntityManager::m_Entities.push_back(sw);
		mLeftIsHere = true;
	}
	if (!mRightIsHere) {//right
		std::shared_ptr<Entity> sw = std::make_shared<Entity>();
		sw->m_sprite.setPosition(750, 250);
		r = rand() % 3;
		switch (r) {
		case 0:
			sw->m_type = EntityType::enemy;
			sw->m_sprite.setTexture(_TextureEnemy);
			sw->m_size = _TextureEnemy.getSize();
			break;
		case 1:
			sw->m_type = EntityType::baby;
			sw->m_sprite.setTexture(_TextureBaby);
			sw->m_size = _TextureBaby.getSize();
			break;
		case 2:
			sw->m_type = EntityType::fish;
			sw->m_sprite.setTexture(_TextureFish);
			sw->m_size = _TextureFish.getSize();
			break;
		}
		sw->positionSpawn = 2;
		EntityManager::m_Entities.push_back(sw);
		mRightIsHere = true;
	}

}
//}

void Game::HanldeWeaponMoves()
{
	//
	// Handle Weapon moves
	//

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		if (entity->m_type != EntityType::weapon)
		{
			continue;
		}

		float x, y;
		if (entity->movingDirection == "down") {
			x = entity->m_sprite.getPosition().x;
			y = entity->m_sprite.getPosition().y;
			y++;
			if (y >= 600)
			{
				entity->m_enabled = false;
				entity->~Entity();
				_IsPlayerWeaponFired = false;
				_IsPlayerTentacleFired = false;
			}
			entity->m_sprite.setPosition(x, y);
		}
		else if (entity->movingDirection == "right") {
			x = entity->m_sprite.getPosition().x;
			y = entity->m_sprite.getPosition().y;
			x++;
			if (x > 1200)
			{
				entity->m_enabled = false;
				entity->~Entity();
				_IsPlayerWeaponFired = false;
				_IsPlayerTentacleFired = false;
			}
			entity->m_sprite.setPosition(x, y);
		}
		else if (entity->movingDirection == "left") {
			x = entity->m_sprite.getPosition().x;
			y = entity->m_sprite.getPosition().y;
			x--;
			if (x <= 0)
			{
				entity->m_enabled = false;
				entity->~Entity();
				_IsPlayerWeaponFired = false;
				_IsPlayerTentacleFired = false;
			}
			entity->m_sprite.setPosition(x, y);
		}
		else if (entity->movingDirection == "up") {
			x = entity->m_sprite.getPosition().x;
			y = entity->m_sprite.getPosition().y;
			y--;
			if (y <= 0)
			{
				entity->m_enabled = false;
				entity->~Entity();
				_IsPlayerWeaponFired = false;
				_IsPlayerTentacleFired = false;
			}
			entity->m_sprite.setPosition(x, y);
		}
	}
}

void Game::HandleCollisionWeaponBlock()
{
	// Handle collision weapon blocks

	for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::weapon)
		{
			continue;
		}

		for (std::shared_ptr<Entity> block : EntityManager::m_Entities)
		{
			if (block->m_type != EntityType::block)
			{
				continue;
			}

			if (block->m_enabled == false)
			{
				continue;
			}

			sf::FloatRect boundWeapon;
			boundWeapon = weapon->m_sprite.getGlobalBounds();

			sf::FloatRect boundBlock;
			boundBlock = block->m_sprite.getGlobalBounds();

			if (boundWeapon.intersects(boundBlock) == true)
			{
				weapon->m_enabled = false;
				_IsPlayerWeaponFired = false;
				_IsPlayerTentacleFired = false;
				//break;
				goto end2;
			}
		}
	}

end2:
	//nop
	return;
}

void Game::HandleCollisionWeaponEnemy()
{
	// Handle collision weapon enemies

	for (std::shared_ptr<Entity> weapon : EntityManager::m_Entities)
	{
		if (weapon->m_enabled == false)
		{
			continue;
		}

		if (weapon->m_type != EntityType::weapon)
		{
			continue;
		}

		for (std::shared_ptr<Entity> enemy : EntityManager::m_Entities)
		{
			if (enemy->m_type == EntityType::enemy)
			{
				if (enemy->m_enabled == false)
				{
					continue;
				}

				sf::FloatRect boundWeapon;
				boundWeapon = weapon->m_sprite.getGlobalBounds();

				sf::FloatRect boundEnemy;
				boundEnemy = enemy->m_sprite.getGlobalBounds();

				if (boundWeapon.intersects(boundEnemy) == true)
				{
					enemy->m_enabled = false;
					weapon->m_enabled = false;
					enemy->~Entity();
					weapon->~Entity();
					switch (enemy->positionSpawn)
					{
					case 1:
						mUpIsHere = false;
						break;
					case 2:
						mRightIsHere = false;
						break;
					case 3:
						mDownIsHere = false;
						break;
					case 4:
						mLeftIsHere = false;
						break;
					}
					_IsPlayerWeaponFired = false;
					_IsPlayerTentacleFired = false;
					_score += 10;
					//break;
					goto end;
				}
			}
			else if (enemy->m_type == EntityType::baby) {
				
				if (enemy->m_enabled == false)
				{
					continue;
				}
				sf::FloatRect boundWeapon;
				boundWeapon = weapon->m_sprite.getGlobalBounds();

				sf::FloatRect boundEnemy;
				boundEnemy = enemy->m_sprite.getGlobalBounds();

				if (boundWeapon.intersects(boundEnemy) == true)
				{
					enemy->m_enabled = false;
					weapon->m_enabled = false;
					enemy->~Entity();
					weapon->~Entity();
					switch (enemy->positionSpawn)
					{
					case 1:
						mUpIsHere = false;
						break;
					case 2:
						mRightIsHere = false;
						break;
					case 3:
						mDownIsHere = false;
						break;
					case 4:
						mLeftIsHere = false;
						break;
					}
					_IsPlayerWeaponFired = false;
					_IsPlayerTentacleFired = false;
					if (weapon->isTentacle) {
						_power += 10;
					}
					//break;
					goto end;
				}
				
			}
			else if (enemy->m_type == EntityType::fish) {
				
				if (enemy->m_enabled == false)
				{
					continue;
				}
				sf::FloatRect boundWeapon;
				boundWeapon = weapon->m_sprite.getGlobalBounds();

				sf::FloatRect boundEnemy;
				boundEnemy = enemy->m_sprite.getGlobalBounds();

				if (boundWeapon.intersects(boundEnemy) == true)
				{
					enemy->m_enabled = false;
					weapon->m_enabled = false;
					enemy->~Entity();
					weapon->~Entity();
					switch (enemy->positionSpawn)
					{
					case 1:
						mUpIsHere = false;
						break;
					case 2:
						mRightIsHere = false;
						break;
					case 3:
						mDownIsHere = false;
						break;
					case 4:
						mLeftIsHere = false;
						break;
					}
					_IsPlayerWeaponFired = false;
					_IsPlayerTentacleFired = false;
					if (weapon->isTentacle) {
						if(_power>50){
							_lives += 2;
						}
						else {
							_lives += 1;
						}
					}
					//break;
					goto end;
				}
				
				
			}
		}
	}

end:
	//nop
	return;
}

void Game::HandleGameOver()
{


	if (EntityManager::GetPlayer()->m_enabled == false)
	{
		DisplayGameOver();
	}

	if (_lives == 0)
	{
		DisplayGameOver();
	}
}

void Game::DisplayGameOver()
{
	if (_lives == 0)
	{
		mText.setFillColor(sf::Color::Green);
		mText.setFont(mFont);
		mText.setPosition(200.f, 200.f);
		mText.setCharacterSize(30);

		mText.setString("GAME OVER  (-R for a new chance-)");

		_IsGameOver = true;
	}
	else
	{
		ResetSprites();
	}
}

void Game::GameAgain()
{
	mText.setString("");
	_lives = 5;
	_score = 0;
	_power = 0;

	InitSprites();	
	
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	if (_IsGameOver) {
		if (key == sf::Keyboard::R) {
			GameAgain();
		}
	}

	else if (key == sf::Keyboard::Up) {
		mIsLookingUp = isPressed;
		mdirectionLooking = "up";
	}
	else if (key == sf::Keyboard::Down) {
		mIsLookingDown = isPressed;
		mdirectionLooking = "down";
	}
	else if (key == sf::Keyboard::Left) {
		mIsLookingLeft = isPressed;
		mdirectionLooking = "left";
	}
	else if (key == sf::Keyboard::Right) {
		mIsLookingRight = isPressed;
		mdirectionLooking = "right";
		;
	}

	if (!_IsGameOver) {

	
		if (key == sf::Keyboard::Space)
		{
			if (isPressed == false)
			{
				return;
			}

			if (_IsPlayerWeaponFired == true)
			{
				return;
			}

			std::shared_ptr<Entity> sw = std::make_shared<Entity>();
			int posX = 0;
			int posY = 0;

			if (mdirectionLooking == "down") {
				sw->movingDirection = "down";
				posX = EntityManager::GetPlayer()->m_sprite.getPosition().x + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().x / 2;
				posY = EntityManager::GetPlayer()->m_sprite.getPosition().y + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().y;
			}
			else if (mdirectionLooking == "left") {
				sw->movingDirection = "left";
				posX = EntityManager::GetPlayer()->m_sprite.getPosition().x;
				posY = EntityManager::GetPlayer()->m_sprite.getPosition().y + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().y / 2;
			}
			else if (mdirectionLooking == "right") {
				sw->movingDirection = "right";
				posX = EntityManager::GetPlayer()->m_sprite.getPosition().x + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().x;
				posY = EntityManager::GetPlayer()->m_sprite.getPosition().y + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().y / 2;
			}
			else if (mdirectionLooking == "up") {
				sw->movingDirection = "up";
				posX = EntityManager::GetPlayer()->m_sprite.getPosition().x + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().x / 2;
				posY = EntityManager::GetPlayer()->m_sprite.getPosition().y;
			}

			sw->m_sprite.setTexture(_TextureBubbleRed);
			sw->m_sprite.setPosition(posX, posY);
			sw->m_type = EntityType::weapon;
			sw->m_size = _TextureBubbleRed.getSize();
			EntityManager::m_Entities.push_back(sw);

			//_IsPlayerWeaponFired = true;
		}

		if (key == sf::Keyboard::E)
		{
			if (isPressed == false)
			{
				return;
			}

			if (_IsPlayerTentacleFired == true)
			{
				return;
			}

			int posX = 0;
			int posY = 0;

			std::shared_ptr<Entity> sw = std::make_shared<Entity>();

			if (mdirectionLooking == "down") {
				sw->movingDirection = "down";
				posX = EntityManager::GetPlayer()->m_sprite.getPosition().x + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().x / 2;
				posY = EntityManager::GetPlayer()->m_sprite.getPosition().y + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().y;
			}
			else if (mdirectionLooking == "left") {
				sw->movingDirection = "left";
				posX = EntityManager::GetPlayer()->m_sprite.getPosition().x;
				posY = EntityManager::GetPlayer()->m_sprite.getPosition().y + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().y / 2;
			}
			else if (mdirectionLooking == "right") {
				sw->movingDirection = "right";
				posX = EntityManager::GetPlayer()->m_sprite.getPosition().x + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().x;
				posY = EntityManager::GetPlayer()->m_sprite.getPosition().y + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().y / 2;
			}
			else if (mdirectionLooking == "up") {
				sw->movingDirection = "up";
				posX = EntityManager::GetPlayer()->m_sprite.getPosition().x + EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().x / 2;
				posY = EntityManager::GetPlayer()->m_sprite.getPosition().y;
			}

			sw->m_sprite.setTexture(_TextureBubbleGreen);
			sw->m_sprite.setPosition(posX, posY);
			sw->m_type = EntityType::weapon;
			sw->m_size = _TextureBubbleGreen.getSize();
			sw->isTentacle = true;
			EntityManager::m_Entities.push_back(sw);

			//_IsPlayerTentacleFired = true;
		}
	}
}

void Game::SpawnEntities()
{
	typeEnemy = rand() % 3;

	if (!mRightIsHere) {
		typeEnemy = rand() % 3;
		_Enemies[0].setTexture(_TextureFish);
		_Enemies[0].setPosition(750.f, EntityManager::GetPlayer()->m_sprite.getPosition().y + EntityManager::GetPlayer()->m_sprite.getPosition().y);
		std::shared_ptr<Entity> se = std::make_shared<Entity>();
		switch (typeEnemy)
		{
		case '0':
			se->m_type = EntityType::baby;
			break;
		case '1':
			se->m_type = EntityType::fish;
			break;
		case '2':
			se->m_type = EntityType::enemy;
			break;
		}
		se->positionSpawn =2;
		se->m_size = _TextureFish.getSize();
		se->m_position = _Enemies[3].getPosition();
		se->m_enabled = true;
		EntityManager::m_Entities.push_back(se);
		mRightIsHere = true;
	}
	if (!mLeftIsHere) {
		typeEnemy = rand() % 3;
		_Enemies[1].setTexture(_TextureFish);
		_Enemies[1].setPosition(20.f, EntityManager::GetPlayer()->m_sprite.getPosition().y + EntityManager::GetPlayer()->m_sprite.getPosition().y);
		std::shared_ptr<Entity> se = std::make_shared<Entity>();
		switch (typeEnemy)
		{
		case '0':
			se->m_type = EntityType::baby;
			break;
		case '1':
			se->m_type = EntityType::fish;
			break;
		case '2':
			se->m_type = EntityType::enemy;
			break;
		}
		se->positionSpawn = 4;
		se->m_size = _TextureFish.getSize();
		se->m_position = _Enemies[3].getPosition();
		se->m_enabled = true;
		EntityManager::m_Entities.push_back(se);
		mLeftIsHere = true;
	}
	if (!mUpIsHere) {
		typeEnemy = rand() % 3;
		_Enemies[2].setTexture(_TextureFish);
		_Enemies[2].setPosition(400.f, 20.f);
		std::shared_ptr<Entity> se = std::make_shared<Entity>();
		switch (typeEnemy)
		{
		case '0':
			se->m_type = EntityType::baby;
			break;
		case '1':
			se->m_type = EntityType::fish;
			break;
		case '2':
			se->m_type = EntityType::enemy;
			break;
		}
		se->positionSpawn = 1;
		se->m_size = _TextureFish.getSize();
		se->m_position = _Enemies[3].getPosition();
		se->m_enabled = true;
		EntityManager::m_Entities.push_back(se);
		mUpIsHere = true;
	}
	if (!mDownIsHere) {
		typeEnemy = rand() % 3;
		_Enemies[3].setTexture(_TextureFish);
		_Enemies[3].setPosition(400.f, 550.f);
		std::shared_ptr<Entity> se = std::make_shared<Entity>();
		switch (typeEnemy)
		{
		case '0':
			se->m_type = EntityType::baby;
			break;
		case '1':
			se->m_type = EntityType::fish;
			break;
		case '2':
			se->m_type = EntityType::enemy;
			break;
		}
		se->positionSpawn = 3;
		se->m_size = _TextureFish.getSize();
		se->m_position = _Enemies[3].getPosition();
		se->m_enabled = true;
		EntityManager::m_Entities.push_back(se);
		mDownIsHere = true;
	}


}



