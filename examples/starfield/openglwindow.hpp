#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <random>

#include "abcg.hpp"
#include "model.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  static const int m_numStars{500};
  static const int m_numPlanets{30};

  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  std::default_random_engine m_randomEngine;

  Model m_asteroid;
  Model m_ship;
  Model m_planet;

  std::array<glm::vec3, m_numStars> m_starPositions;
  std::array<glm::vec3, m_numStars> m_starRotations;
  std::array<glm::vec3, m_numPlanets> m_planetPositions;
  std::array<glm::vec3, m_numPlanets> m_planetRotations;
  float m_angle{};

  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  float m_FOV{130.0f};

  void randomizeStar(glm::vec3 &position, glm::vec3 &rotation);
  void randomizePlanet(glm::vec3 &position, glm::vec3 &rotation);
  void update();
};

#endif