#include "window.hpp"

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
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 50.0f);
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

  restart();
}

void Window::restart() {
  m_gameData.m_state = State::Playing;
  m_cenario.create(m_objectsProgram);

  m_tartaruga.create(m_objectsProgram);

  m_jacare.create(m_objectsProgram);
}

void Window::onUpdate() {
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Wait 2 seconds before restarting
  if (m_gameData.m_state != State::Playing &&
      m_restartWaitTimer.elapsed() > 2) {
    restart();
    return;
  }

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
      ImGui::Text("Game Over!");
    } else if (m_gameData.m_state == State::Win) {
      ImGui::Text("You Win!");
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
