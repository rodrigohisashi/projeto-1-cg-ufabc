#include "cenario.hpp"

void Cenario::create(GLuint program) {
  destroy();

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_rotation = 0.0f;
  // Set up the vertices for the beach
  std::array positions{
      glm::vec2{-1.0f, 0.7f},
      glm::vec2{+1.0f, 0.7f},
      glm::vec2{+1.0f, -1.0f},
      glm::vec2{-1.0f, -1.0f},
  };

  std::array indices{0, 1, 2, 2, 3, 0};

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

void Cenario::paint() {
  abcg::glUseProgram(m_program);

  // Paint the water portion
  abcg::glBindVertexArray(m_VAO);
  abcg::glUniform1f(m_scaleLoc, 1.0f);
  abcg::glUniform2f(m_translationLoc, 0.0f, 0.4f);
  m_color = glm::vec4(22.0f / 255.0f, 160.0f / 255.0f, 171.0f / 255.0f, 1.0f);
  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

  // Paint the sand portion
  abcg::glUniform2f(m_translationLoc, 0.0f, 0.0f);
  m_color = glm::vec4(220.0f / 255.0f, 203.0f / 255.0f, 181.0f / 255.0f, 1.0f);
  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);
  abcg::glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);
  abcg::glUseProgram(0);
}

void Cenario::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}
