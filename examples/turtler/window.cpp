#include "window.hpp"
#include "imgui.h"

void Window::onEvent(SDL_Event const &event) {
  // Keyboard events
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_UP)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Right));
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.set(gsl::narrow<size_t>(Input::Accelerate));
  }
  if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_UP)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Up));
    if (event.key.keysym.sym == SDLK_DOWN)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Down));
    if (event.key.keysym.sym == SDLK_LEFT)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Left));
    if (event.key.keysym.sym == SDLK_RIGHT)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Right));
    if (event.key.keysym.sym == SDLK_SPACE)
      m_gameData.m_input.reset(gsl::narrow<size_t>(Input::Accelerate));
  }
}

void Window::onCreate() {

  auto const assetsPath{abcg::Application::getAssetsPath()};

  // Load a new font
  auto const filename{assetsPath + "emulogic.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 20.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError("Cannot load font file");
  }

  // Create program to render the other objects
  m_objectsProgram =
      abcg::createOpenGLProgram({{.source = assetsPath + "objects.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "objects.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  abcg::glClearColor(0, 0, 0, 1);

#if !defined(__EMSCRIPTEN__)
  abcg::glEnable(GL_PROGRAM_POINT_SIZE);
#endif

  // Start pseudo-random number generator
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());

  m_timer = 20.0f;
  restart();
}

void Window::restart() {
  m_gameData.m_state = State::Playing;
  m_cenario.create(m_objectsProgram);

  m_tartaruga.create(m_objectsProgram);

  m_jacare.create(m_objectsProgram);
  m_timer = 20.0f;
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Wait 2 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 2) {
    restart();
    return;
  }

  m_timer -= deltaTime; // Atualize o temporizador

  m_tartaruga.update(m_gameData, deltaTime);
  m_jacare.update(deltaTime);
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  m_cenario.paint();
  m_tartaruga.paint(m_gameData);
  m_jacare.paint();
  if (m_gameData.m_state == State::Playing) {
    checkWinCondition();
    checkCollisions();
    checkTimer();
  }
}

void Window::checkTimer() {
  if (m_timer <= 0.0f) {
    m_gameData.m_state = State::GameOver;
    m_restartWaitTimer.restart();
  }
}

void Window::checkWinCondition() {
  if (m_tartaruga.m_translation.y > 0.85f) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();

  {
    auto const size{ImVec2(300, 85)};
    auto const position{ImVec2((m_viewportSize.x - size.x) / 2.0f,
                               (m_viewportSize.y - size.y) / 2.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == State::GameOver) {

      ImGui::GetForegroundDrawList()->AddRectFilled(
          ImVec2(0, 0), ImGui::GetIO().DisplaySize,
          IM_COL32(0, 0, 0, 200)); // Cor escura de fundo
      ImGui::TextColored(ImVec4(1, 0, 0, 1),
                         "Game Over!"); // Texto "Game Over!" em vermelho
      ImGui::SetWindowFontScale(1.5f);

    } else if (m_gameData.m_state == State::Win) {
      ImGui::Text("You Win!");
    } else if (m_gameData.m_state == State::Playing) {
      ImGui::SetNextWindowPos(
          ImVec2(10, 10)); // Define a posição do temporizador
      ImGui::Begin("Temporizador", nullptr,
                   ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoInputs);
      ImGui::SetWindowSize(ImVec2(200, 100));
      ImGui::Text("Tempo: %.1f", m_timer); // Exibe o temporizador na tela
      ImGui::End();
    }

    ImGui::PopFont();
    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;

  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_objectsProgram);

  m_tartaruga.destroy();
  m_cenario.destroy();
  m_jacare.destroy();
}

void Window::checkCollisions() {
  // Check collision between ship and asteroids
  for (auto const &jacare : m_jacare.m_jacares) {
    float dx = m_tartaruga.m_translation.x - jacare.m_translation.x;
    float dy = m_tartaruga.m_translation.y - jacare.m_translation.y;

    float normX;
    float normY;
    if ((m_tartaruga.m_translation.x > jacare.m_translation.x &&
         jacare.m_direcao == 1) ||
        (m_tartaruga.m_translation.x < jacare.m_translation.x &&
         jacare.m_direcao == -1)) {
      normX = dx / jacare.m_scale * 0.27;
    } else {
      normX = dx / jacare.m_scale * 0.7;
    }

    if ((m_tartaruga.m_translation.y > jacare.m_translation.y &&
         jacare.m_direcao == 1) ||
        ((m_tartaruga.m_translation.y < jacare.m_translation.y &&
          jacare.m_direcao == -1))) {
      normY = dy / jacare.m_scale * 1;
    } else {
      normY = dy / jacare.m_scale * 1.3;
    }
    float distance = sqrt(normX * normX + normY * normY);

    if (distance < 1.0f) {
      m_gameData.m_state = State::GameOver;
      m_restartWaitTimer.restart();
    }
  }
}