#include "jacare.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Jacare::create(GLuint program) {
  destroy();

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Reset turtle attributes
  m_rotation = 0.0f;
  m_translation = glm::vec2(0, -0.95f);
  m_scale = 0.15f;

  std::array positions{
      // jacare head
      glm::vec2{+2.5f, +0.8f},
      glm::vec2{+3.4f, +0.8f},
      glm::vec2{+3.5f, +0.7f},
      glm::vec2{+3.5f, +0.3f},
      glm::vec2{+3.4f, +0.2f},
      glm::vec2{+2.5f, +0.2f},

      glm::vec2{+2.0f, +0.9f},
      glm::vec2{+2.0f, +0.1f},

      // corpo
      glm::vec2{+2.0f, +0.0f}, // 6
      glm::vec2{+0.0f, 0.0f},
      glm::vec2{+0.0f, +1.0f},
      glm::vec2{+2.0f, +1.0f},

      // fins
      glm::vec2{+0.2f, +1.2f}, // 10
      glm::vec2{+0.4f, +1.0f},

      glm::vec2{+0.2f, -0.2f}, // 12
      glm::vec2{+0.4f, 0.0f},

      glm::vec2{+1.8f, +1.2f},
      glm::vec2{+1.6f, +1.0f},
      glm::vec2{+1.8f, -0.2f},
      glm::vec2{+1.6f, +0.0f},

      // Tail
      glm::vec2{-2.0f, +0.5f},
      glm::vec2{+0.0f, +0.8f},
      glm::vec2{+0.0f, +0.2f},

  };

  // Normalize
  // for (auto &position : positions) {
  //   position /= glm::vec2{3.5f, 2.4f};
  // }

  std::array const indices{// Head
                           6, 7, 1, 1, 2, 7, 2, 3, 7, 3, 4, 7,
                           // corpo
                           8, 9, 10, 8, 10, 11, 0, 8, 11, 0, 5, 8,
                           // fins
                           10, 12, 13, 14, 9, 15, 16, 17, 11, 18, 19, 8,
                           // tail
                           22, 20, 21};
  // clang-format on
  // Generate VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);
}

void Jacare::paint(const GameData &gameData) {
  if (gameData.m_state != State::Playing)
    return;

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_VAO);

  abcg::glUniform1f(m_scaleLoc, m_scale);
  abcg::glUniform1f(m_rotationLoc, m_rotation);
  abcg::glUniform2fv(m_translationLoc, 1, &m_translation.x);

  m_color = glm::vec4(0.0f, 0.5f, 0.0f, 1.0f);
  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glDrawElements(GL_TRIANGLES, 13 * 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Jacare::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Jacare::update(const GameData &gameData, float deltaTime) {

  m_translation.x += 1.0f * deltaTime;

  if (m_translation.x > +1.5f)
      m_translation.x -= 3.0f;
  
}
