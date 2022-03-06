#pragma once

#include <SFML/System/Vector3.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Application;

class Cursor
{
public:
	Cursor(Application& app);
	~Cursor();

	void init();
	void handleEvent(sf::Event event, uint32_t& framesStill);
	void update(float dt);
	void draw(sf::RenderWindow& window);

	void hide();
	void show();
	bool isHidden() const;

	sf::Vector2f getPos() const;

private:
	Application& m_app;

	float m_sensitivity = 3.0f;

	bool m_isHidden = true;

	sf::Vector2f m_mousePos;
};
