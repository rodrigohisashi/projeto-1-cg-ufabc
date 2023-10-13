#include "cobra.hpp"

#include <glm/gtx/fast_trigonometry.hpp>
#include <glm/gtx/rotate_vector.hpp>

void Cobra::create(GLuint program) {
  destroy();

  m_program = program;

  // Get location of uniforms in the program
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");
  m_rotationLoc = abcg::glGetUniformLocation(m_program, "rotation");
  m_scaleLoc = abcg::glGetUniformLocation(m_program, "scale");
  m_translationLoc = abcg::glGetUniformLocation(m_program, "translation");

  // Reset snake attributes
  m_rotation = 0.0f;
  m_translation = glm::vec2(0);
  m_velocity = glm::vec2(0);

  // Define the snake's body as a triangle
  std::array positions{glm::vec2{0.0f, 0.1f}, glm::vec2{-0.1f, -0.1f},
                       glm::vec2{0.1f, -0.1f}};

  // Normalize
  for (auto &position : positions) {
    position /= glm::vec2{
        0.1f, 0.1f}; // Adjust the divisor as needed for the snake size
  }

  std::array const indices{0, 1, 2};

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
void Cobra::paint(const GameData &gameData) {
  if (gameData.m_state != State::Playing)
    return;

  abcg::glUseProgram(m_program);

  abcg::glBindVertexArray(m_VAO);

  abcg::glUniform1f(m_scaleLoc, m_scale);
  abcg::glUniform1f(m_rotationLoc, m_rotation);
  abcg::glUniform2fv(m_translationLoc, 1, &m_translation.x);

  // Customize the color for the snake, if needed
  abcg::glUniform4fv(m_colorLoc, 1, &m_color.r);

  // You might want to add logic here to handle different paint conditions for
  // the snake For example, you could change the color or appearance of the
  // snake depending on certain conditions

  // Draw the snake
  abcg::glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);

  abcg::glUseProgram(0);
}

void Cobra::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Cobra::update(const GameData &gameData, float deltaTime) {
  // Handle the input for movement
  if (gameData.m_input[static_cast<size_t>(Input::Up)] &&
      gameData.m_state == State::Playing) {
    // Move the snake in the forward direction
    // Change the position based on the current direction the snake is facing
    // Adjust the speed according to deltaTime
    auto const forward{glm::vec2{0.0f, 1.0f}}; // Adjust this direction based on
                                               // the snake's current direction
    m_position += forward * deltaTime; // Adjust the speed as needed
  }

  // Handle the logic for updating the snake's state
  // This includes growing the snake, checking for collisions, etc.
  // You need to define this logic based on the specific mechanics of your
  // snake game For instance, you might need to check for collisions with the
  // food or with the boundaries of the game area Additionally, you might need
  // to update the length and position of the snake accordingly Update the
  // score or any other necessary game parameters You can also update the
  // speed or direction of the snake based on certain conditions

  // Add any other logic specific to the behavior of the snake in your game
  // For example, you might need to implement the logic for the snake's
  // movement, growth, and collision detection here You might also need to
  // handle the speed and direction of the snake based on the input or other
  // game conditions
}