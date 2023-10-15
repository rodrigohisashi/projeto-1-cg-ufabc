#ifndef CENARIO_HPP_
#define CENARIO_HPP_

#include "abcgOpenGL.hpp"

#include "gamedata.hpp"

class Cenario {
public:
  void create(GLuint program);
  void destroy();
  void paint();

private:
  GLuint m_program{};
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLint m_colorLoc{};
  GLint m_rotationLoc{};
  GLint m_scaleLoc{};
  GLint m_translationLoc{};

  glm::vec2 m_translation{0.0f, 0.0f};
  float m_rotation{};
  float m_scale{1.0f};
  glm::vec4 m_color{};
};
#endif