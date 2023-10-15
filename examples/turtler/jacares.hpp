#ifndef JACARES_HPP_
#define JACARES_HPP_

#include <list>

#include "abcgOpenGL.hpp"

#include "gamedata.hpp"

class Jacares {
public:
  void create(GLuint program);
  void paint();
  void destroy();
  void update(float deltaTime);

  struct Jacare {
    GLuint m_VAO{};
    GLuint m_VBO{};
    GLuint m_EBO{};

    glm::vec4 m_color{1};
    float m_rotation{};
    float m_scale{};
    int m_direcao{};
    glm::vec2 m_translation{};
    glm::vec2 m_velocity{};
    glm::vec2 m_position;
  };

  std::list<Jacare> m_jacares;

  Jacare makeJacare(glm::vec2 translation = {}, float scale = 0.18f,
                    int direcao = 1);

private:
  GLuint m_program{};
  GLint m_translationLoc{};
  GLint m_colorLoc{};
  GLint m_scaleLoc{};
  GLint m_rotationLoc{};
};
#endif