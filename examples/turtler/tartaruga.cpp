#include "tartaruga.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Tartaruga::create(GLuint program) {
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
      // turtle body
      glm::vec2{+0.5f, +0.5f},
      glm::vec2{+0.5f, +0.0f},
      glm::vec2{+0.00f, +0.5f},
      glm::vec2{+0.0f, +0.0f},

      // head and tail
      glm::vec2{+0.1f, +0.6f},
      glm::vec2{+0.4f, +0.6f},
      glm::vec2{+0.1f, -0.1f},
      glm::vec2{+0.4f, -0.1f},

      // fins
      glm::vec2{+0.0f, +0.5f},
      glm::vec2{-0.1f, +0.7f},
      glm::vec2{-0.2f, +0.6f},

      glm::vec2{+0.5f, +0.5f},
      glm::vec2{+0.6f, +0.7f},
      glm::vec2{+0.7f, +0.6f},

      glm::vec2{+0.0f, +0.0f},
      glm::vec2{-0.2f, -0.1f},
      glm::vec2{-0.1f, -0.2f},

      glm::vec2{+0.5f, +0.0f},
      glm::vec2{+0.6f, -0.2f},
      glm::vec2{+0.7f, -0.1f},

  };

  // // Normalize
  // for (auto &position : positions) {
  //   position /= glm::vec2{15.5f, 15.5f};
  // }

  std::array const indices{0, 1, 2, 1, 2, 3,
                           // head and tail
                           4, 5, 7, 4, 6, 7,
                           // fins
                           8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
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

void Tartaruga::paint(const GameData &gameData) {
  if (gameData.m_state != State::Playing)
    return;

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_VAO);

  abcg::glUniform1f(m_scaleLoc, m_scale);
  abcg::glUniform1f(m_rotationLoc, m_rotation);
  abcg::glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Restart thruster blink timer every 100 ms
  if (m_trailBlinkTimer.elapsed() > 100.0 / 1000.0)
    m_trailBlinkTimer.restart();

  m_color = glm::vec4(138.0f / 255.0f, 154.0f / 255.0f, 91.0f / 255.0f, 1.0f);

  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glDrawElements(GL_TRIANGLES, 8 * 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Tartaruga::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Tartaruga::update(const GameData &gameData, float deltaTime) {

  if (gameData.m_input[gsl::narrow<size_t>(Input::Left)] &&
      m_translation.x > -0.95f)
    m_translation.x -= 1.0f * deltaTime;

  if (gameData.m_input[gsl::narrow<size_t>(Input::Right)] &&
      m_translation.x < +0.85f)
    m_translation.x += 1.0f * deltaTime;

  if (gameData.m_input[gsl::narrow<size_t>(Input::Down)] &&
      m_translation.y > -0.95f)
    m_translation.y -= 1.0f * deltaTime;

  if (gameData.m_input[gsl::narrow<size_t>(Input::Up)] &&
      m_translation.y < +0.85f)
    m_translation.y += 1.0f * deltaTime;
}
