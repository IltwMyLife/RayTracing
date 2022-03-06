#pragma once

#include <random>

#include <SFML/Graphics.hpp>

#include "Camera.hpp"
#include "Cursor.hpp"

class Application
{
public:
	Application();
	~Application();

	void run();

	sf::RenderWindow& getWindow();
	Camera&			  getCamera();
	Cursor&			  getCursor();

private:
	void handleEvent();
	void update(float dt);
	void draw();

private:
	sf::RenderWindow m_window;

	sf::Clock m_clock;
	sf::Time  m_dt;

	Camera m_camera;
	Cursor m_cursor;

	sf::RenderTexture m_firstFrame;
	sf::Sprite		  m_firstFrameSprite;
	sf::Sprite		  m_firstFrameSpriteFlipped;

	sf::RenderTexture m_secondFrame;
	sf::Sprite		  m_secondFrameSprite;
	sf::Sprite		  m_secondFrameSpriteFlipped;

	sf::Shader m_shader;

	std::random_device				 m_rd;
	std::mt19937					 m_e2{m_rd()};
	std::uniform_real_distribution<> m_dist{0.0f, 1.0f};

	uint32_t m_framesStill = 1;

	int m_raysCount = 1;
	int m_maxReflectionsCount = 1;
};