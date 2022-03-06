#include "RayTracing/Camera.hpp"

#include "RayTracing/Application.hpp"

Camera::Camera(Application& app) : m_app(app)
{
}

Camera::~Camera()
{
}

void Camera::init()
{
}

void Camera::handleEvent(sf::Event event)
{
	switch (event.type)
	{
		case sf::Event::EventType::KeyPressed:
		{
			if (event.key.code == sf::Keyboard::Escape)
			{
				m_app.getWindow().setMouseCursorVisible(true);
				m_app.getCursor().show();
			}
			else if (event.key.code == sf::Keyboard::W) m_wasdUD[0] = true;
			else if (event.key.code == sf::Keyboard::A) m_wasdUD[1] = true;
			else if (event.key.code == sf::Keyboard::S) m_wasdUD[2] = true;
			else if (event.key.code == sf::Keyboard::D) m_wasdUD[3] = true;
			else if (event.key.code == sf::Keyboard::Space) m_wasdUD[4] = true;
			else if (event.key.code == sf::Keyboard::LShift) m_wasdUD[5] = true;
			break;
		}
		case sf::Event::EventType::KeyReleased:
		{
			if (event.key.code == sf::Keyboard::W) m_wasdUD[0] = false;
			else if (event.key.code == sf::Keyboard::A) m_wasdUD[1] = false;
			else if (event.key.code == sf::Keyboard::S) m_wasdUD[2] = false;
			else if (event.key.code == sf::Keyboard::D) m_wasdUD[3] = false;
			else if (event.key.code == sf::Keyboard::Space) m_wasdUD[4] = false;
			else if (event.key.code == sf::Keyboard::LShift) m_wasdUD[5] = false;
			break;
		}
	}
}

void Camera::update(float dt, uint32_t& framesStill)
{
	auto cursorPos = m_app.getCursor().getPos();

	sf::Vector3f dir = sf::Vector3f(0.0f, 0.0f, 0.0f);
	sf::Vector3f dirTemp;

	if (m_wasdUD[0]) dir = sf::Vector3f(1.0f, 0.0f, 0.0f);
	else if (m_wasdUD[2]) dir = sf::Vector3f(-1.0f, 0.0f, 0.0f);
	if (m_wasdUD[1]) dir += sf::Vector3f(0.0f, -1.0f, 0.0f);
	else if (m_wasdUD[3]) dir += sf::Vector3f(0.0f, 1.0f, 0.0f);

	dirTemp.x = dir.x * cos(-cursorPos.y);
	dirTemp.y = dir.y;
	dirTemp.z = -dir.x * sin(-cursorPos.y);
	
	dir.x = dirTemp.x * cos(cursorPos.x) - dirTemp.y * sin(cursorPos.x);
	dir.y = dirTemp.x * sin(cursorPos.x) + dirTemp.y * cos(cursorPos.x);
	dir.z = dirTemp.z;

	m_pos += dir * m_speed * dt;

	if (m_wasdUD[4]) m_pos.z -= m_speed * dt;
	else if (m_wasdUD[5]) m_pos.z += m_speed* dt;

	for (int i = 0; i < 6; i++)
	{
		if (m_wasdUD[i])
		{
			framesStill = 1;
			break;
		}
	}
}

void Camera::draw(sf::RenderWindow& window)
{
}

sf::Vector3f Camera::getPos() const
{
	return m_pos;
}
