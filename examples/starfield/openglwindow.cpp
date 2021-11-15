#include "openglwindow.hpp"

#include <imgui.h>

#include <cppitertools/itertools.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void OpenGLWindow::initializeGL() {
  abcg::glClearColor(0, 0, 0, 1);

  // Enable depth buffering
  abcg::glEnable(GL_DEPTH_TEST);

  // Create program
  m_program = createProgramFromFile(getAssetsPath() + "depth.vert",
                                    getAssetsPath() + "depth.frag");

  // Load model
  m_asteroid.loadObj(getAssetsPath() + "asteroid.obj");
  m_ship.loadObj(getAssetsPath() + "ship.obj");
  m_planetRing.loadObj(getAssetsPath() + "planetRing.obj");
  m_planetRound.loadObj(getAssetsPath() + "planetRound.obj");

  m_asteroid.setupVAO(m_program);
  m_ship.setupVAO(m_program);
  m_planetRing.setupVAO(m_program);
  m_planetRound.setupVAO(m_program);

  // Camera at (0,0,0) and looking towards the negative z
  m_viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  // Setup stars
  for (const auto index : iter::range(m_numStars)) {
    auto &position{m_starPositions.at(index)};
    auto &rotation{m_starRotations.at(index)};

    randomizeStar(position, rotation);
  }
  for (const auto index : iter::range(m_numPlanets)) {
    auto &position{m_planetPositions.at(index)};
    auto &rotation{m_planetRotations.at(index)};

    randomizePlanet(position, rotation);
  }

}

void OpenGLWindow::randomizeStar(glm::vec3 &position, glm::vec3 &rotation) {
  // Get random position
  // x and y coordinates in the range [-20, 20]
  // z coordinates in the range [-100, 0]
  std::uniform_real_distribution<float> distPosXY(-20.0f, 20.0f);
  std::uniform_real_distribution<float> distPosZ(-100.0f, 0.0f);

  position = glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                       distPosZ(m_randomEngine));

  //  Get random rotation axis
  std::uniform_real_distribution<float> distRotAxis(-1.0f, 1.0f);

  rotation = glm::normalize(glm::vec3(distRotAxis(m_randomEngine),
                                      distRotAxis(m_randomEngine),
                                      distRotAxis(m_randomEngine)));
}

void OpenGLWindow::randomizePlanet(glm::vec3 &position, glm::vec3 &rotation) {
  // Get random position
  // x and y coordinates in the range [-20, 20]
  // z coordinates in the range [-100, 0]
  std::uniform_real_distribution<float> distPosXY(-10.0f, 10.0f);
  std::uniform_real_distribution<float> distPosZ(-100.0f, 0.0f);
  float X = distPosXY(m_randomEngine); 
  float Y = distPosXY(m_randomEngine);

  if(Y <= 0 && X < 0){
    if(abs(X)>=5 && abs(Y)>=5) position = glm::vec3(X-10.0f, Y-10.0f, distPosZ(m_randomEngine));
    else if(abs(X) > abs(Y)) position = glm::vec3(X-10.0f, Y, distPosZ(m_randomEngine));
    else if(abs(X) < abs(Y)) position = glm::vec3(X, Y-0.0f, distPosZ(m_randomEngine));
  }
  if(Y <= 0 && X > 0){
    if(X>=5 && abs(Y)>=5) position = glm::vec3(X+10.0f, Y-10.0f, distPosZ(m_randomEngine));
    else if(X>abs(Y)) position = glm::vec3(X+10.0f, Y, distPosZ(m_randomEngine));
    else if(X<abs(Y)) position = glm::vec3(X, Y-10.0f, distPosZ(m_randomEngine));
  }
  if(Y > 0 && X < 0){
    if(abs(X)>=5 && Y>=5) position = glm::vec3(X-10.0f, Y+10.0f, distPosZ(m_randomEngine));
    else if(abs(X) > Y) position = glm::vec3(X-10.0f, Y, distPosZ(m_randomEngine));
    else if(abs(X) < Y) position = glm::vec3(X, Y+10.0f, distPosZ(m_randomEngine));
  }
  if(Y>0 && X>0){
    if(X>=5 && Y>=5) position = glm::vec3(X+10.0f, Y+10.0f, distPosZ(m_randomEngine));
    else if(X>Y) position = glm::vec3(X+10.0f, Y, distPosZ(m_randomEngine));
    else if(X<Y) position = glm::vec3(X, Y+10.0f, distPosZ(m_randomEngine));
  }
  //  Get random rotation axis
  std::uniform_real_distribution<float> distRotAxis(-0.0005f, 0.0005f);

  rotation = glm::normalize(glm::vec3(distRotAxis(m_randomEngine), distRotAxis(m_randomEngine), 0.0f));
}

void OpenGLWindow::paintGL() {
  update();

  // Clear color buffer and depth buffer
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  abcg::glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  abcg::glUseProgram(m_program);

  // Get location of uniform variables (could be precomputed)
  const GLint viewMatrixLoc{
      abcg::glGetUniformLocation(m_program, "viewMatrix")};
  const GLint projMatrixLoc{
      abcg::glGetUniformLocation(m_program, "projMatrix")};
  const GLint modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  const GLint colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Set uniform variables used by every scene object
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform4f(colorLoc, 0.8f, 0.4f, 0.2f, 1.0f);  // Brown

  // Render each star
  for (const auto index : iter::range(m_numStars)) {
    const auto &position{m_starPositions.at(index)};
    const auto &rotation{m_starRotations.at(index)};

    // Compute model matrix of the current star
    glm::mat4 modelMatrix{1.0f};
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
    modelMatrix = glm::rotate(modelMatrix, m_angle, rotation);

    // Set uniform variable
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    m_asteroid.render();
    
  }

  //render da ship
  // Compute model matrix of the current star
  glm::mat4 modelMatrix{1.0f};
  //modelMatrix = glm::translate(modelMatrix, m_shipPosition);
  modelMatrix = glm::scale(modelMatrix, glm::vec3(5.0f));
  modelMatrix = glm::rotate(modelMatrix, m_angle, m_shipRotation);
  abcg::glUniform4f(colorLoc, 1.0f, 0.0f, 0.0f, 1.0f);  // Brown
  m_ship.render();

  
  

  // Render each planet
  for (const auto index : iter::range(m_numPlanets)) {
    const auto &position{m_planetPositions.at(index)};
    const auto &rotation{m_planetRotations.at(index)};

    // Compute model matrix of the current star
    glm::mat4 modelMatrix{1.0f};
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(2.0f));
    modelMatrix = glm::rotate(modelMatrix, m_angle, rotation);

    // Set uniform variable
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);
    
    if(index < 3){
      abcg::glUniform4f(colorLoc, 0.93f, 0.82f, 0.0f, 1.0f);
      m_planetRing.render();
    }
    if(index >= 3 && index < 6){
      abcg::glUniform4f(colorLoc, 0.0f, 0.5f, 0.9f, 1.0f);
      m_planetRound.render();
    }
    if(index >= 6 && index < 9){
      abcg::glUniform4f(colorLoc, 0.0f, 0.5f, 0.5f, 1.0f);
      m_planetRound.render();
    }
    if(index >= 9 && index < 12){
      abcg::glUniform4f(colorLoc, 0.8f, 0.0f, 0.6f, 1.0f);
      m_planetRing.render();
    }
  }

  abcg::glUseProgram(0);
}

void OpenGLWindow::paintUI() {
  abcg::OpenGLWindow::paintUI();
  {
    const auto widgetSize{ImVec2(218, 62)};
    ImGui::SetNextWindowPos(ImVec2(m_viewportWidth - widgetSize.x - 5, 5));
    ImGui::SetNextWindowSize(widgetSize);
    ImGui::Begin("Widget window", nullptr, ImGuiWindowFlags_NoDecoration);

    {
      ImGui::PushItemWidth(120);
      static std::size_t currentIndex{};
      const std::vector<std::string> comboItems{"Perspective", "Orthographic"};

      if (ImGui::BeginCombo("Projection",
                            comboItems.at(currentIndex).c_str())) {
        for (const auto index : iter::range(comboItems.size())) {
          const bool isSelected{currentIndex == index};
          if (ImGui::Selectable(comboItems.at(index).c_str(), isSelected))
            currentIndex = index;
          if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::PopItemWidth();

      ImGui::PushItemWidth(170);
      const auto aspect{static_cast<float>(m_viewportWidth) /
                        static_cast<float>(m_viewportHeight)};
      if (currentIndex == 0) {
        m_projMatrix =
            glm::perspective(glm::radians(m_FOV), aspect, 0.01f, 100.0f);

        ImGui::SliderFloat("FOV", &m_FOV, 5.0f, 179.0f, "%.0f degrees");
      } else {
        m_projMatrix = glm::ortho(-20.0f * aspect, 20.0f * aspect, -20.0f,
                                  20.0f, 0.01f, 100.0f);
      }
      ImGui::PopItemWidth();
    }

    ImGui::End();
  }
}

void OpenGLWindow::resizeGL(int width, int height) {
  m_viewportWidth = width;
  m_viewportHeight = height;
}

void OpenGLWindow::terminateGL() {
  m_asteroid.terminateGL();
  m_planetRing.terminateGL();
  m_planetRound.terminateGL();
  m_ship.terminateGL();
  abcg::glDeleteProgram(m_program);
}

void OpenGLWindow::update() {
  // Animate angle by 90 degrees per second
  const float deltaTime{static_cast<float>(getDeltaTime())};
  m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);
  // Update stars
  for (const auto index : iter::range(m_numStars)) {
    auto &position{m_starPositions.at(index)};
    auto &rotation{m_starRotations.at(index)};

    // Z coordinate increases by 10 units per second
    position.z += deltaTime * 10.0f;

    // If this star is behind the camera, select a new random position and
    // orientation, and move it back to -100
    if (position.z > 0.1f) {
      randomizeStar(position, rotation);
      position.z = -100.0f;  // Back to -100
    }
  }

  //update planets
  for (const auto index : iter::range(m_numPlanets)) {
    auto &position{m_planetPositions.at(index)};
    auto &rotation{m_planetRotations.at(index)};

    // Z coordinate increases by 10 units per second
    position.z += deltaTime * 10.0f;

    // If this star is behind the camera, select a new random position and
    // orientation, and move it back to -100
    if (position.z > 0.1f) {
      randomizePlanet(position, rotation);
      position.z = -100.0f;  // Back to -100
    }
  }

}