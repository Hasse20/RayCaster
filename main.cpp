/*
 * Include directories: glfw-3.3.9.bin.WIN64\glfw-3.3.9.bin.WIN64\include
 * Link directories: glfw-3.3.9.bin.WIN64\lib-vc2022
 * Link libraries: glfw3.lib; opengl32.lib; glu32.lib; user32.lib; gdi32.lib;
 * shell32.lib
 */

#include <Windows.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#include "map.hpp"
#include "player.hpp"
#include "world.hpp"

using namespace std;;

class Display {
 public:
  using Ptr = shared_ptr<Display>;

  int m_width;
  int m_height;
  GLFWwindow* m_window;

  Display(int width, int height, GLFWwindow* window)
      : m_width{width}, m_height{height}, m_window{window} {}

  virtual void init() = 0;
  virtual void render() = 0;
  virtual ~Display() = default;
};

class DisplayGame2D : public Display {
  World& m_world;

 public:
  DisplayGame2D(int width, int height, GLFWwindow* window, World& world)
      : Display{width, height, window}, m_world{world} {}

  void init() override {
    glfwMakeContextCurrent(m_window);
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0.0, m_width, m_height, 0.0);
  }

  private:
  void drawMap2D() {
    int xo, yo;
    for (int y = 0; y < m_world.m_map.m_height; y++) {
      for (int x = 0; x < m_world.m_map.m_width; x++) {
        if (m_world.m_map.get(x, y) == 1) {
          glColor3f(1, 1, 1);
        } else {
          glColor3f(0, 0, 0);
        }
        constexpr int size = 64;
        xo = x * size;
        yo = y * size;

        glBegin(GL_QUADS);
        glVertex2i(0 + xo + 1, 0 + yo + 1);
        glVertex2i(0 + xo + 1, size + yo - 1);
        glVertex2i(size + xo - 1, size + yo - 1);
        glVertex2i(size + xo - 1, 0 + yo + 1);
        glEnd();
      }
    }
  }

  void drawPlayer2D() {
    glColor3f(1, 1, 0);
    glPointSize(8);
    glLineWidth(4);

    glBegin(GL_POINTS);
    glVertex2f(m_world.m_player.position.x, m_world.m_player.position.y);
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(m_world.m_player.position.x, m_world.m_player.position.y);
    glVertex2f(m_world.m_player.position.x + m_world.m_player.direction.x * 20,
               m_world.m_player.position.y + m_world.m_player.direction.y * 20);
    glEnd();
  }

  void drawRays2D(const World& world, const Player& player) {
    float rayAngle =
        fixAngle(player.direction.angle + 30);  // ray set back 30 degrees

    for (int r = 0; r < 120; r++) {
      auto collition = world.findCollition(player.position, rayAngle);

      if (collition) {
        if (collition->orientation == Orientation::Horizontal) {
          glColor3f(0.8, 0, 0);
        } else {
          glColor3f(0.6, 0, 0);
        }

        // Draw 2D rays
        glLineWidth(2);
        glBegin(GL_LINES);
        glVertex2i(player.position.x, player.position.y);
        glVertex2i(collition->position.x, collition->position.y);
        glEnd();
      }
      rayAngle = fixAngle(rayAngle - 0.5f);  // Go to next ray
    }
  }

 public:
  void render() override {
    glfwMakeContextCurrent(m_window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawPlayer2D();
    drawRays2D(m_world, m_world.m_player);
    glfwSwapBuffers(m_window);
  }
};

class DisplayGame3D : public Display {
  World& m_world;
  int m_nrOfRays;
  float rayScreenWidth;

 public:
  DisplayGame3D(int nrOfRays, int width, int height, GLFWwindow* window, World& world)
      : Display{width, height, window},
        m_world{world},
        m_nrOfRays{nrOfRays},
        rayScreenWidth{(float)width / nrOfRays} {}

  void init() override {
    glfwMakeContextCurrent(m_window);
    glClearColor(0.3, 0.3, 0.3, 0);
    gluOrtho2D(0.0, m_width, m_height, 0.0);
  }

 private:
  void drawRays3D(const World& world, const Player& player) {
    glColor3f(0.55, 0.55, 0.55);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(m_width, 0);
    glVertex2i(m_width, m_height/2);
    glVertex2i(0, m_height / 2);
    glEnd();

    glColor3f(0.45, 0.45, 0.45);
    glBegin(GL_QUADS);
    glVertex2i(0, m_height / 2);
    glVertex2i(m_width, m_height / 2);
    glVertex2i(m_width, m_height);
    glVertex2i(0, m_height);
    glEnd();

    float angleStep = 60.0f / m_nrOfRays;
    float rayAngle =
        fixAngle(player.direction.angle + 30);  // ray set back 30 degrees

    for (int r = 0; r < m_nrOfRays; r++) {
      auto collition = world.findCollition(player.position, rayAngle);

      if (collition) {
        if (collition->orientation == Orientation::Horizontal) {
          glColor3f(0.8, 0, 0);
        } else {
          glColor3f(0.6, 0, 0);
        }

        constexpr int scale = 64;
        float deltaAngle = fixAngle(player.direction.angle - rayAngle);
        float distance =
            collition->distance * cos(degToRad(deltaAngle));  // Fix fisheye
        int lineHeight = (scale * m_height) / distance;
        if (lineHeight > m_height) { // Limit height
          lineHeight = m_height;
        }                                  
        int lineStart = m_height/2 - (lineHeight/2);
         
        // Draw 3D vertical walls
        glLineWidth(lroundf(rayScreenWidth+0.5f));
        glBegin(GL_LINES);
        glVertex2i((r+0.5f) * rayScreenWidth, lineStart);
        glVertex2i((r+0.5f) * rayScreenWidth, lineStart + lineHeight);
        glEnd();
      }
      rayAngle = fixAngle(rayAngle - angleStep);  // Go to next ray
    }
  }

 public:
  void render() override {
    glfwMakeContextCurrent(m_window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawRays3D(m_world, m_world.m_player);
    glfwSwapBuffers(m_window);
  }
};

// Workaround begin
// Connect function pointer to member function of Game method

function<void(GLFWwindow* window, int key, int scancode, int action, int mods)>
    buttonsCallback;

void globalButtonsCallback(GLFWwindow* window, int key, int scancode,
                           int action, int mods) {
  buttonsCallback(window, key, scancode, action, mods);
}
// Workaround end

class Game {
  World& m_world;
  vector<Display::Ptr /*Coolt*/> m_displays;

  void buttonsCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    
    if (key == 'a' || key == 'A') {
      m_world.m_player.turnLeft();
    } else if (key == 'd' || key == 'D') {
      m_world.m_player.turnRight();
    } else if (key == 'w' || key == 'W') {
      m_world.m_player.moveForward(m_world);
    } else if (key == 's' || key == 'S') {
      m_world.m_player.moveBackward(m_world);
    } else {
      cout << key << endl;
    }
  }

 public:
  Game(World& world, const vector<Display::Ptr>& displays)
      : m_world{world}, m_displays{displays} {}

  void init() {
    m_world.m_player.init();
    ::buttonsCallback = [this](GLFWwindow* window, int key, int scancode,
                               int action, int mods) {
      buttonsCallback(window, key, scancode, action, mods);
    };

    for (auto aDisplay : m_displays) {     
      glfwSetKeyCallback(aDisplay->m_window, globalButtonsCallback);
    }
  }

  void runOneIteration() {
    for (auto& aDisplay : m_displays) {
      aDisplay->render();
      glfwPollEvents();
    }
  }
};

IWorld::Ptr argh;

int main(int argc, char* argv[]) {
  glfwInit();

  auto window2d = glfwCreateWindow(640, 640, "2Dview", nullptr, nullptr);
  auto window3d = glfwCreateWindow(960, 640, "3Dview", nullptr, nullptr);

  if (!window2d || !window3d) {
    std::cerr << "Failed to create GLFW window\n";
    glfwTerminate();
    return -1;
  }

  World world;

  auto display2D = make_shared<DisplayGame2D>(640, 640, window2d, world);
  auto display3D = make_shared<DisplayGame3D>(320, 960, 640, window3d, world);

  display2D->init();
  display3D->init();

  Game game{world, {display2D, display3D}};
  game.init();

  while (!glfwWindowShouldClose(window2d) && !glfwWindowShouldClose(window3d)) {
    game.runOneIteration();

  }

  glfwDestroyWindow(window2d);
  glfwDestroyWindow(window3d);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}
