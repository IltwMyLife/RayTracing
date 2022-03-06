#include "RayTracing/Cursor.hpp"

#include "RayTracing/Application.hpp"

Cursor::Cursor(Application& app) : m_app(app)
{
}

Cursor::~Cursor()
{
}

void Cursor::init()
{
	m_mousePos.x = m_app.getWindow().getSize().x / 2.0f;
	m_mousePos.y = m_app.getWindow().getSize().y / 2.0f;
}

void Cursor::handleEvent(sf::Event event, uint32_t& framesStill)
{
	switch (event.type)
	{
		case sf::Event::EventType::MouseMoved:
		{
			if (m_isHidden)
			{
				auto windowSize = m_app.getWindow().getSize();

				int mx = event.mouseMove.x - windowSize.x / 2;
				int my = event.mouseMove.y - windowSize.y / 2;
				m_mousePos.x += mx;
				m_mousePos.y += my;
				sf::Mouse::setPosition(sf::Vector2i(windowSize.x / 2, windowSize.y / 2), m_app.getWindow());
				if (mx != 0 || my != 0) framesStill = 1;
			}
			break;
		}
	}
}

void Cursor::update(float dt)
{
	
}

void Cursor::draw(sf::RenderWindow& window)
{
}

void Cursor::hide()
{
	m_isHidden = true;
}

void Cursor::show()
{
	m_isHidden = false;
}

bool Cursor::isHidden() const
{
	return m_isHidden;
}

sf::Vector2f Cursor::getPos() const
{
	auto windowSize = m_app.getWindow().getSize();

	sf::Vector2f pos;
	pos.x	= ((float)m_mousePos.x / windowSize.x - 0.5f) * m_sensitivity;
	pos.y = ((float)m_mousePos.y / windowSize.y - 0.5f) * m_sensitivity;

	return pos;
}
