#include "RayTracing/Application.hpp"

#include <imgui-SFML.h>
#include <imgui.h>

Application::Application() : m_cursor(*this), m_camera(*this)
{
	m_window.create(sf::VideoMode(1920, 1080), "RayTracing", sf::Style::Fullscreen);
	ImGui::SFML::Init(m_window);
	m_window.setMouseCursorVisible(false);

	m_firstFrame.create(m_window.getSize().x, m_window.getSize().y);
	m_firstFrameSprite		  = sf::Sprite(m_firstFrame.getTexture());
	m_firstFrameSpriteFlipped = sf::Sprite(m_firstFrame.getTexture());
	m_firstFrameSpriteFlipped.setScale(sf::Vector2f{1.0f, -1.0f});
	m_firstFrameSpriteFlipped.setPosition(sf::Vector2f{0.0f, static_cast<float>(m_window.getSize().y)});

	m_secondFrame.create(m_window.getSize().x, m_window.getSize().y);
	m_secondFrameSprite		   = sf::Sprite(m_secondFrame.getTexture());
	m_secondFrameSpriteFlipped = sf::Sprite(m_firstFrame.getTexture());
	m_secondFrameSpriteFlipped.setScale(sf::Vector2f{1.0f, -1.0f});
	m_secondFrameSpriteFlipped.setPosition(sf::Vector2f{0.0f, static_cast<float>(m_window.getSize().y)});

	m_shader.loadFromFile("assets/shaders/Shader.frag", sf::Shader::Fragment);
	m_shader.setUniform("u_resolution", static_cast<sf::Vector2f>(m_window.getSize()));
}

Application::~Application()
{
	ImGui::SFML::Shutdown();
}

void Application::run()
{
	m_cursor.init();

	while (m_window.isOpen())
	{
		m_dt = m_clock.restart();

		handleEvent();
		update(m_dt.asSeconds());
		draw();
	}
}

sf::RenderWindow& Application::getWindow()
{
	return m_window;
}

Camera& Application::getCamera()
{
	return m_camera;
}

Cursor& Application::getCursor()
{
	return m_cursor;
}

void Application::handleEvent()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		switch (event.type)
		{
			case sf::Event::EventType::Closed:
			{
				m_window.close();
				break;
			}
			case sf::Event::EventType::KeyPressed:
			{
				if (event.key.code == sf::Keyboard::Key::Escape)
					m_window.close();
				else if (event.key.code == sf::Keyboard::Key::F1)
				{
					if (m_cursor.isHidden())
					{
						m_cursor.show();
						m_window.setMouseCursorVisible(true);
					}
					else
					{
						m_cursor.hide();
						m_window.setMouseCursorVisible(false);
						sf::Mouse::setPosition(sf::Vector2i(m_window.getSize().x / 2, m_window.getSize().y / 2), m_window);
					}
				}
			}
		}

		if (!m_cursor.isHidden()) ImGui::SFML::ProcessEvent(event);

		m_cursor.handleEvent(event, m_framesStill);
		m_camera.handleEvent(event);
	}
}

void Application::update(float dt)
{
	if (!m_cursor.isHidden()) ImGui::SFML::Update(m_window, m_dt);

	m_camera.update(dt, m_framesStill);
}

void Application::draw()
{
	if (!m_cursor.isHidden())
	{
		ImGui::Begin("Settings");

		static int lastRaysCount		   = m_raysCount;
		static int lastMaxReflectionsCount = m_maxReflectionsCount;

		if (lastRaysCount != m_raysCount || lastMaxReflectionsCount != m_maxReflectionsCount)
		{
			lastRaysCount			= m_raysCount;
			lastMaxReflectionsCount = m_maxReflectionsCount;
			m_framesStill			= 1;
		}
		ImGui::SliderInt("Rays count", &m_raysCount, 0, 124);
		ImGui::SliderInt("Reflections count", &m_maxReflectionsCount, 0, 124);
		ImGui::End();
	}

	m_window.clear();

	m_shader.setUniform("u_rays_count", m_raysCount);
	m_shader.setUniform("u_max_ref", m_maxReflectionsCount);
	m_shader.setUniform("u_camera_pos", m_camera.getPos());
	m_shader.setUniform("u_mouse_pos", static_cast<sf::Vector2f>(m_cursor.getPos()));
	m_shader.setUniform("u_time", m_dt.asSeconds());
	m_shader.setUniform("u_sample_part", 1.0f / m_framesStill);
	m_shader.setUniform("u_seed1", sf::Vector2f((float)m_dist(m_e2), (float)m_dist(m_e2)) * 999.0f);
	m_shader.setUniform("u_seed2", sf::Vector2f((float)m_dist(m_e2), (float)m_dist(m_e2)) * 999.0f);

	if (m_framesStill % 2 == 1)
	{
		m_shader.setUniform("u_sample", m_firstFrame.getTexture());
		m_secondFrame.draw(m_firstFrameSpriteFlipped, &m_shader);
		m_window.draw(m_secondFrameSprite);
	}
	else
	{
		m_shader.setUniform("u_sample", m_secondFrame.getTexture());
		m_firstFrame.draw(m_secondFrameSpriteFlipped, &m_shader);
		m_window.draw(m_firstFrameSprite);
	}

	m_framesStill++;

	if (!m_cursor.isHidden()) ImGui::SFML::Render(m_window);

	m_window.display();
}
