#include "jacares.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Jacares::create(GLuint program) {
  destroy();

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Clear the list with the alligators VBO and resizes to accomodate six alligators
  m_jacares.clear();
  m_jacares.resize(6);

  // Cont used to define the position and direction of each alligator
  int cont = 0;

  for (auto &jacare : m_jacares) {
    // Create the alligator and defines the direction
    if (cont == 2 || cont == 3) {
      jacare = makeJacare({}, 0.18f, -1);
    } else {
      jacare = makeJacare({}, 0.18f, 1);
    }
    // Change the position of each alligator
    switch (cont) {
    case 0:
      jacare.m_translation = glm::vec2(+0.5f, 0.45f);
      break;
    case 1:
      jacare.m_translation = glm::vec2(-1.0f, 0.45f);
      break;
    case 2:
      jacare.m_translation = glm::vec2(1.0f, 0.15f);
      break;
    case 3:
      jacare.m_translation = glm::vec2(-0.5f, 0.15f);
      break;
    case 4:
      jacare.m_translation = glm::vec2(0.5f, -0.5f);
      break;
    case 5:
      jacare.m_translation = glm::vec2(-1.0f, -0.5f);
      break;
    }
    cont += 1;
  }
}

void Jacares::paint() {

  abcg::glUseProgram(m_program);

  for (auto const &jacare : m_jacares) {

    abcg::glBindVertexArray(jacare.m_VAO);

    abcg::glUniform1f(m_scaleLoc, jacare.m_scale);
    abcg::glUniform1f(m_rotationLoc, jacare.m_rotation);
    abcg::glUniform2fv(m_translationLoc, 1, &jacare.m_translation.x);

    abcg::glUniform4fv(m_colorLoc, 1, &jacare.m_color.r);
    abcg::glDrawElements(GL_TRIANGLES, 13 * 3, GL_UNSIGNED_INT, nullptr);
  }

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Jacares::destroy() {
  for (auto &jacare : m_jacares) {
    abcg::glDeleteBuffers(1, &jacare.m_VBO);
    abcg::glDeleteBuffers(1, &jacare.m_EBO);
    abcg::glDeleteVertexArrays(1, &jacare.m_VAO);
  }
}

void Jacares::update(float deltaTime) {
  for (auto &jacare : m_jacares) {
    // Make the movement in the respective's direction
    jacare.m_translation.x += 1.3f * deltaTime * jacare.m_direcao;
    // Make the alligator appear in the other side of the window
    if (jacare.m_translation.x > +1.5f && jacare.m_direcao == 1)
      jacare.m_translation.x -= 3.0f;
    if (jacare.m_translation.x < -1.5f && jacare.m_direcao == -1)
      jacare.m_translation.x += 3.0f;
  }
}

Jacares::Jacare Jacares::makeJacare(glm::vec2 translation, float scale,
                                    int direcao) {
  Jacare jacare;

  // Reset alligator attributes
  jacare.m_rotation = direcao == 1 ? 0.0f : M_PI;
  jacare.m_translation = translation;
  jacare.m_scale = scale;
  jacare.m_color = glm::vec4(0.0f, 0.5f, 0.0f, 1.0f);
  jacare.m_direcao = direcao;

  std::array positions{
      // head
      glm::vec2{+2.5f, +0.8f},
      glm::vec2{+3.4f, +0.8f},
      glm::vec2{+3.5f, +0.7f},
      glm::vec2{+3.5f, +0.3f},
      glm::vec2{+3.4f, +0.2f},
      glm::vec2{+2.5f, +0.2f},

      glm::vec2{+2.0f, +0.9f},
      glm::vec2{+2.0f, +0.1f},

      // body
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

      // tail
      glm::vec2{-2.0f, +0.5f},
      glm::vec2{+0.0f, +0.8f},
      glm::vec2{+0.0f, +0.2f},

  };

  std::array const indices{// head
                           6, 7, 1, 1, 2, 7, 2, 3, 7, 3, 4, 7,
                           // body
                           8, 9, 10, 8, 10, 11, 0, 8, 11, 0, 5, 8,
                           // fins
                           10, 12, 13, 14, 9, 15, 16, 17, 11, 18, 19, 8,
                           // tail
                           22, 20, 21};
  // clang-format on
  // Generate VBO
  abcg::glGenBuffers(1, &jacare.m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, jacare.m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate EBO
  abcg::glGenBuffers(1, &jacare.m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jacare.m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(),
                     GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // Get location of attributes in the program
  GLint positionAttribute{abcg::glGetAttribLocation(m_program, "inPosition")};

  // Create VAO
  abcg::glGenVertexArrays(1, &jacare.m_VAO);

  // Bind vertex attributes to current VAO
  abcg::glBindVertexArray(jacare.m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, jacare.m_VBO);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, jacare.m_EBO);

  // End of binding to current VAO
  abcg::glBindVertexArray(0);

  return jacare;
}
