#pragma once

#include <SFML/System/Vector3.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class Application;

class Camera
{
public:
	Camera(Application& app);
	~Camera();

	void init();
	void handleEvent(sf::Event event);
	void update(float dt, uint32_t& framesStill);
	void draw(sf::RenderWindow& window);

	sf::Vector3f getPos() const;

private:
	Application& m_app;

	float m_speed = 5.0f;
	bool m_wasdUD[6] = {false, false, false, false, false, false};
	sf::Vector3f m_pos		 = {-5.0f, 0.0f, 0.0f};
};