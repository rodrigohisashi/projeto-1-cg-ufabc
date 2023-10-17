# Turtler

Turtler é um jogo baseado no clássico Frogger, onde o objetivo é guiar uma tartaruga até o mar, evitando os jacarés pelo caminho.

## Autores
- Rodrigo Hisashi Takeuti (RA: 11201921914)
- Lucas Dantas de Lima da Silva (RA: 11201921341)

## Sobre o Jogo
O jogo foi desenvolvido em C++, utilizando o framework ABCg, se baseando no código do projeto Asteroids desenvolvido pelo professor durante o curso. Adaptamos classes existentes do projeto para se adequar as necessidades de nosso jogo. Os desenhos dos animais foram feitos usando o Geogebra, com as coordenadas sendo adaptadas para se manter na janela de exibição do jogo.

No jogo temos como personagem principal uma tartaruga, que deve passar por três fileiras de jacarés e chegar a mar, ganhando o jogo quando chega ao mar, porém, se a tartaruga encostar em algum dos jacarés no caminho o usuário perde o jogo e deve começar do inicío. 

O jogo está disponível em [Turtler](https://rodrigohisashi.github.io/projeto-1-cg-ufabc/turtler/).

## Como Jogar
- Use as setas do teclado para mover a tartaruga para cima, para baixo, para a esquerda e para a direita.
- Evite os jacarés para chegar com segurança à água no lado oposto.

## Descrição da Implementação
Temos como classes: cenario.cpp, gamedata.cpp, jacares.cpp, tartaruga.cpp e window.cpp. As funções das classes são as seguintes:

- **cenario.cpp:** Responsável por gerar o cenário do jogo, com uma parte sendo dedicada a praia e outra parte dedicada ao mar;
- **gamedata.cpp:** Utilizado pelo window para armazenar o estado do jogo { Playing, GameOver, Win } e realizar a interface do input { Right, Left, Down, Up } usada para mover a tartaruga;
- **jacares.cpp:** Responsável por gerar os jacarés na tela e os mover pela tela automaticamente; 
- **tartaruga.cpp:** Responsável por gerar a tartaruga na tela e a mover pela tela baseado no input do usuário;
- **window.cpp:** Responsável por mostrar o jogo na tela e gerenciar a vitória e derrota do jogo, monitorando as colisões da tartaruga com os jacarés e a posição da tartaruga para sua vitória.

### cenario.cpp
No caso do cenário criamos um VBO formado por um retângulo usando para isso a função Cenario::create():
```cpp
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

  // Unchanged part of create function...
}
```
Para a aparência da praia, alteramos a cor na função Cenario::paint():
```cpp
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
```

### jacares.cpp
No caso do jacares.cpp, adaptamos o código usado para gerar os asteroids no projeto da aula. Na função Jacares:create(), usamos uma lista para armazenar os objetos dos jacarés. Além disso, usamos um contador para definir a posição e direção do vetor, sendo a direção definida através de um if, passando a direção para a função Jacares::makeJacare(), enquanto a posição é definida em um switch case. 
```cpp
void Jacares::create(GLuint program) {
  // Unchanged part of create function... 

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
```
A função Jacares::makeJacare() define a rotação através da direção. O jacaré é formado por um conjunto de pontos que por si formam diversos triângulos.
```cpp
Jacares::Jacare Jacares::makeJacare(glm::vec2 translation, float scale,
                                    int direcao) {
  Jacare jacare;

  // Reset turtle attributes
  jacare.m_rotation = direcao == 1 ? 0.0f : M_PI;
  jacare.m_translation = translation;
  jacare.m_scale = scale;
  jacare.m_color = glm::vec4(0.0f, 0.5f, 0.0f, 1.0f);
  jacare.m_direcao = direcao;

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

  std::array const indices{// Head
                           6, 7, 1, 1, 2, 7, 2, 3, 7, 3, 4, 7,
                           // corpo
                           8, 9, 10, 8, 10, 11, 0, 8, 11, 0, 5, 8,
                           // fins
                           10, 12, 13, 14, 9, 15, 16, 17, 11, 18, 19, 8,
                           // tail
                           22, 20, 21};
  // clang-format on
  
  // Unchanged part of function...

  return jacare;
}
```
As funções Jacares::paint() e Jacares::destroy() não tem grandes alterações, sendo muito semelhantes ao exemplo da aula.
No caso do movimento do jacaré, ele é realizado na função Jacares::update(), em que o movimento é realizado horizontalmente, e o jacaré é transferido para o outro lado após ultrapassar um limite.
```cpp
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
```

### tartaruga.cpp
As funções Tartaruga::paint() e Tartaruga::destroy() não tem grandes alterações, sendo muito semelhantes ao exemplo da aula.
No caso da função Tartaruga::create(), alteramos somente a forma da tartagura, que é formada por diversos pontos e triângulos formados por esses pontos.
```cpp
void Tartaruga::create(GLuint program) {
  // Unchanged part of create function...

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

  std::array const indices{0, 1, 2, 1, 2, 3,
                           // head and tail
                           4, 5, 7, 4, 6, 7,
                           // fins
                           8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19};
  // clang-format on
  // Unchanged part of create function...
}
```
Além disso, alteramos o update para que a tartaruga se mova conforme o input do usuário e impedir que mesma saia da tela:
```cpp
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
```

### window.cpp
No caso do window.cpp, alteramos Window::restart(), Window::onUpdate(), Window::onPaint(), Window::OnPaintUI() e Window::onDestroy() para chamar as respectivas funções de cenario, jacares e tartaruga.
```cpp
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
    checkCollisions();
  }
}

void Window::onDestroy() {
  abcg::glDeleteProgram(m_objectsProgram);

  m_tartaruga.destroy();
  m_cenario.destroy();
  m_jacare.destroy();
}
```
No método `Window::onPaintUI()`, implementamos a lógica para renderizar a interface do usuário usando a biblioteca ImGui. Dependendo do estado atual do jogo, diferentes elementos de interface são exibidos para o usuário. O método gerencia as seguintes situações:

- **Game Over:** Caso o estado do jogo seja definido como "GameOver", uma janela é exibida no centro da tela com um fundo escuro e a mensagem "Game Over!" em vermelho, em um tamanho de fonte aumentado para maior destaque.

- **Vitória:** Se o estado do jogo for "Win", a mensagem "You Win!" é exibida.

- **Jogando:** Quando o jogo está em andamento, uma janela de temporizador é exibida no canto da tela, mostrando o tempo decorrido desde o início do jogo.

```cpp
oid Window::onPaintUI() {
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
      ImVec2 center = ImVec2(
          (ImGui::GetIO().DisplaySize.x - ImGui::CalcTextSize("Game Over!").x) *
              0.5f,
          (ImGui::GetIO().DisplaySize.y - ImGui::CalcTextSize("Game Over!").y) *
              0.5f);

      ImGui::GetForegroundDrawList()->AddRectFilled(
          ImVec2(0, 0), ImGui::GetIO().DisplaySize,
          IM_COL32(0, 0, 0, 200));       // Cor escura de fundo
      ImGui::SetCursorScreenPos(center); // Posiciona no centro da tela
      ImGui::TextColored(ImVec4(1, 0, 0, 1),
                         "Game Ovessr!"); // Texto "Game Over!" em vermelho
      ImGui::SetWindowFontScale(1.5f);  // Aumenta o tamanho do texto em 2 vezes

    } else if (m_gameData.m_state == State::Win) {
     } else if (m_gameData.m_state == State::Win) {
    // Configurando a fonte para ser maior e centralizada
    ImGui::SetWindowFontScale(1.5f); // Aumenta o tamanho da fonte em 1.5 vezes
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("You Win!").x) / 2);
    ImGui::Text("You Win!");

    // Adicionando um emoji e uma mensagem especial para comemorar a vitória
    ImGui::SetCursorPosX((ImGui::GetWindowSize().x - ImGui::CalcTextSize("🎉 Parabéns! Você venceu! 🎉").x) / 2);
    ImGui::Text("🎉 Parabéns! Você venceu! 🎉");

    // Redefinindo o tamanho da fonte para o padrão
    ImGui::SetWindowFontScale(1.0f);
}

    } else if (m_gameData.m_state == State::Playing) {
      ImGui::SetNextWindowPos(
          ImVec2(10, 10)); // Define a posição do temporizador
      ImGui::Begin("Temporizador", nullptr,
                   ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
                       ImGuiWindowFlags_NoInputs);
      ImGui::SetWindowSize(
          ImVec2(200, 100)); // Ajuste o tamanho da janela conforme necessário
      ImGui::Text("Tempo: %.1f", m_timer); // Exibe o temporizador na tela
      ImGui::End();
    }

    ImGui::PopFont();
    ImGui::End();
  }
}
```

Alteramos também a função Window::checkWinCondition(), para que o jogo seja ganho quando a tartaruga chegar a um certo ponto do mapa.
```cpp
void Window::checkWinCondition() {
  if (m_tartaruga.m_translation.y > 0.85f) {
    m_gameData.m_state = State::Win;
    m_restartWaitTimer.restart();
  }
}
```
Por fim, alteramos a função void Window::checkCollisions() para verificar a colisão entre a tartaruga e jacarés baseando se na distância entre eles.
```cpp
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
```

## Licença
Este projeto está licenciado sob a licenca do MIT. Consulte o arquivo LICENSE.md para obter mais detalhes.

