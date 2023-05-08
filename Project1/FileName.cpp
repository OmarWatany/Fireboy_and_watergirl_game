// notes :-RenderWindow &window,RectangleShape &menu
// for increasing performance change Keyboard evnets handling in the menu
// functions to be like play function
// cause it's a nested loop unlike the level1 func.
// i changed the way the while loop work from while(window.isOpen()) to
// while (Menu.isActive) so i can close the loop without closing the window.
#include <SFML/Audio.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Mouse.hpp>
#include <iostream>
#include <string>

// setting width and height of the window
// float width = 1210, height = 850;
float width = 1720 / 2.0, height = 1300 / 2.0;

using namespace sf;
float getCenter(Text text) {
  return (width - text.getGlobalBounds().width) / 2;
}

bool backMusicIsActive = true;
bool EfxIsActive = false;

unsigned int playedtimes = 0;

struct Menu {
  bool isActive = false;
  // dynamic text array size but i won't make a dynamic array,
  // for using in moving functions.
  int size;
  Font font;
  Text mainmenu[7];
  int selected = 0;
  void draw(RenderWindow &window) {
    for (int i = 0; i < size; i++) {
      window.draw(mainmenu[i]);
    }
  }
  void MoveDown() {
    if (selected < size - 1) // not in exit (last option in the menu)
    {
      mainmenu[selected].setFillColor(Color::White);
      selected++;
      mainmenu[selected].setFillColor(Color{255, 204, 0});
    } else {
      mainmenu[selected].setFillColor(Color::White);
      selected = 0;
      mainmenu[selected].setFillColor(Color{255, 204, 0});
    }
  }
  void MoveUp() {
    if (selected > 0) // not in play (first option in the menu)
    {
      mainmenu[selected].setFillColor(Color::White);
      selected--;
      mainmenu[selected].setFillColor(Color{255, 204, 0});
    } else {
      mainmenu[selected].setFillColor(Color::White);
      selected = size - 1;
      mainmenu[selected].setFillColor(Color{255, 204, 0});
    }
  }
  bool hover(int xPos, int yPos) {
    for (int i = 0; i < size; i++) {
      if (xPos > mainmenu[i].getGlobalBounds().left &&
          xPos < (mainmenu[i].getGlobalBounds().left +
                  mainmenu[i].getGlobalBounds().width) &&
          yPos > mainmenu[i].getGlobalBounds().top &&
          yPos < (mainmenu[i].getGlobalBounds().top +
                  mainmenu[i].getGlobalBounds().height)) {
        mainmenu[selected].setFillColor(Color::White);
        selected = i;
        mainmenu[selected].setFillColor(Color{255, 204, 0});
        return true;
      }
    }
    return false;
  }
  void setSelected(int n) { selected = n; }
  int pressed() { return selected; }
  void setcharsize(int n) {
    for (int i = 0; i < size; i++) {
      mainmenu[i].setCharacterSize(1.1 * (n * (height / 1300)));
    }
  }
  void setTextFont() {
    for (int i = 0; i < size; i++) {
      mainmenu[i].setFont(font);
    }
  }

  void setTextPosition(int n) {
    for (int i = 0; i < size; i++) {
      mainmenu[i].setPosition(Vector2f(
          getCenter(mainmenu[i]),
          ((height / 2) - (mainmenu[0].getGlobalBounds().height * size)) +
              (i * (mainmenu[0].getGlobalBounds().height)) * 2));
    }
  }
  // big menu background
  Texture background;
  Sprite bg;
  void setbackgroud(Texture &background, Sprite &bg) {
    background.loadFromFile("assets/TempleHallForest.png");
    bg.setTexture(background);
    bg.setScale(width / background.getSize().x,
                height / background.getSize().y);
  }
  // small menus background
  Texture GameNameForest;
  Sprite logo;
  Texture sbackground;
  Sprite smenu;
  void setsbackgroud() {
    sbackground.loadFromFile("assets/smallmenuback.png");
    GameNameForest.loadFromFile("assets/GameNameForest.png");
    smenu.setTexture(sbackground);
    smenu.setScale(Vector2f(0.8 * (width / 1210), 0.8 * (height / 850)));
    smenu.setPosition(
        ((width - (sbackground.getSize().x * smenu.getScale().x)) / 2),
        (50 + GameNameForest.getSize().y) * (height / 850));
    logo.setTexture(GameNameForest);
    logo.setScale(Vector2f((width / 1210), (height / 850)));
    logo.setPosition(
        (width - (GameNameForest.getSize().x) * logo.getScale().x) / 2,
        50 * (height / 850));
  }
} menu, optionsMenu, pausemenu, losingmenu, winingmenu, creditesmenu;

struct bottomssprite {
  Texture txt;
  Sprite sprite;
  void setsprites(String str) {
    txt.loadFromFile(str);
    sprite.setTexture(txt);
    sprite.setScale(width / 1210, height / 850);
  }
  // if mouse position.y > pausesprite getGlobalBounds.top &&
  // mouse position.x > pausesprite getGlobalBounds.left &&
  // mouse position.y < pausesprite getGlobalBounds.top + sprite_width
  // mouse position.x < pausesprite getGlobalBounds.left + sprite_height
  bool hover(int xPos, int yPos) {
    if (xPos > sprite.getGlobalBounds().left &&
        xPos <
            (sprite.getGlobalBounds().left + sprite.getGlobalBounds().width) &&
        yPos > sprite.getGlobalBounds().top &&
        yPos <
            (sprite.getGlobalBounds().top + sprite.getGlobalBounds().height)) {
      return true;
    }
    return false;
  }
} paustitle, resumebtm, retrybtm, endbtm, okbtm, musicbtm, efxbtm, settingsbtm,
    backbtm;

// gravity
// slow computer: gravity =0.13 , moving speed = 3.5 , jumping =-5.5
// fast computer: gravity = 0.035 moving speed = 0.5 , jumping =-0.9
float gravity = 0.13 / 1300 * height, fireboy_Vy = 0, watergirl_Vy = 0,
      box_Vy = 0, vx = 3.5 / 1720 * width, vy = -5.5 / 1300 * height;

// for animation
int x = 0, y = 0, a = 0, f = 0, g = 0;

// define some functions
bool doesIntersectElevator(Sprite &player, Sprite &elevator1,
                           Sprite &elevator2);
bool doesIntersectBox(Sprite &player, RectangleShape &box,
                      RectangleShape ground[]);
bool doesIntersect(Sprite &player, RectangleShape ground[]);
void Fmove(Sprite &fireboy, RectangleShape &rWall, RectangleShape &rWall2,
           RectangleShape &rWall3, RectangleShape &rWall4,
           RectangleShape &lWall, RectangleShape &lWall2,
           RectangleShape &lWall3, RectangleShape &lWall4,
           RectangleShape &lWall5, RectangleShape &lWall6,
           RectangleShape &lWall7, RectangleShape &lWall8,
           RectangleShape ground[], RectangleShape &box, Sprite &elevator1,
           Sprite &elevator2, Event event, Sound fbjumpSound);
void Wmove(Sprite &watergirl, RectangleShape &rWall, RectangleShape &rWall2,
           RectangleShape &rWall3, RectangleShape &rWall4,
           RectangleShape &lWall, RectangleShape &lWall2,
           RectangleShape &lWall3, RectangleShape &lWall4,
           RectangleShape &lWall5, RectangleShape &lWall6,
           RectangleShape &lWall7, RectangleShape &lWall8,
           RectangleShape ground[], RectangleShape &box, Sprite &elevator1,
           Sprite &elevator2, Event event, Sound wgjumpSound);
void scaleSprite(Sprite &sprite, int windowWidth, int windowHeight);
void scaleRectangles(sf::Vector2u windowSize, sf::RectangleShape ground[],
                     int size);
void scaleRectangles(int windowWidth, int windowHeight,
                     sf::RectangleShape &ground);
void scaleBackground(Sprite &backgroundPic, const Vector2u &windowSize);
void scalePosition(float &xPos, float &yPos, const sf::RenderWindow &window);
void drawMenu(RenderWindow &window);
void drawOptionsMenu(RenderWindow &window);
void drawPauseMenu(RenderWindow &window);
void drawLosingMenu(RenderWindow &window);
void drawWinningMenu(RenderWindow &window, Text &gameTime);
void drawCreditesMenu(RenderWindow &window);
void toggle(bool &var);
void level1(RenderWindow &window);

int main() {
  RenderWindow window(VideoMode(width, height), "Fireboy and Watergirl");

  Image icon;
  icon.loadFromFile("assets/icon.png");
  window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

  menu.isActive = true;
  drawMenu(window);
  return 0;
}

int menumusicisplaying = 0;
void drawMenu(RenderWindow &window) {
  // making menu options
  menu.font.loadFromFile("assets/varsity_regular.ttf");
  menu.size = 4;

  menu.mainmenu[0].setFillColor(Color{204, 153, 0});
  menu.mainmenu[0].setString("Play");

  menu.mainmenu[1].setFillColor(Color::White);
  menu.mainmenu[1].setString("Options");

  menu.mainmenu[2].setFillColor(Color::White);
  menu.mainmenu[2].setString("Credits");

  menu.mainmenu[3].setFillColor(Color::White);
  menu.mainmenu[3].setString("Exit");

  menu.setTextFont();
  menu.setcharsize(90);
  menu.setTextPosition(100);
  menu.setbackgroud(menu.background, menu.bg);

  Music backgroundMusic;
  backgroundMusic.openFromFile("assets/MenuMusic.wav");
  backgroundMusic.setLoop(true);
  if (backMusicIsActive && (menumusicisplaying != 2)) {
    backgroundMusic.play();
    menumusicisplaying = backgroundMusic.getStatus();
  }

  while (menu.isActive && window.isOpen()) {

    menu.hover(Mouse::getPosition(window).x, Mouse::getPosition(window).y);

    Event event;
    while (window.pollEvent(event)) {
      if (event.type == Event::Closed) {
        window.close();
        break;
      } else if (event.type == Event::KeyPressed) {
        if (event.key.code == Keyboard::Up) {
          menu.MoveUp();
        }
        if (event.key.code == Keyboard::Down) {
          menu.MoveDown();
        }
      } else if ((event.type == Event::KeyReleased) &&
                     (event.key.code == Keyboard::Enter) ||
                 ((event.type == Event::MouseButtonReleased) &&
                  (menu.hover(Mouse::getPosition(window).x,
                              Mouse::getPosition(window).y)))) {
        switch (menu.selected) {
        case 0:
          window.clear();
          menu.isActive = false;
          backgroundMusic.stop();
          level1(window);
          break;
        case 1:
          window.clear();
          menu.setSelected(0);
          optionsMenu.isActive = true;
          menu.isActive = false;
          backgroundMusic.stop();
          drawOptionsMenu(window);
          break;
        case 2:
          window.clear();
          menu.setSelected(0);
          creditesmenu.isActive = true;
          menu.isActive = false;
          drawCreditesMenu(window);
          break;
        case 3:
          window.close();
          break;
        }
      }
    }
    window.clear();
    window.draw(menu.bg);
    menu.draw(window);
    window.display();
  }
}

void level1(RenderWindow &window) {
  // Calculate the percentage of the window dimensions to use for scaling the
  // grounds
  float scaleX = (float)window.getSize().x / 1210.f;
  float scaleY = (float)window.getSize().y / 850.f;

  // stopwatch
  Clock gameClock;
  Font font;
  font.loadFromFile("assets/font.ttf");
  Text timeText;
  timeText.setFont(font);
  timeText.setCharacterSize(static_cast<float>(55) / 1720 * width);
  timeText.setFillColor(Color(255, 215, 0));
  timeText.setPosition(static_cast<float>(780) / 1720 * width, 0);

  // background music
  Music backgroundMusic;
  backgroundMusic.openFromFile("assets/background-music.wav");
  backgroundMusic.setLoop(true);

  // button jump sound
  SoundBuffer buttonBuffer;
  buttonBuffer.loadFromFile("assets/button.wav");
  Sound buttonSound;
  buttonSound.setBuffer(buttonBuffer);

  // lever jump sound
  SoundBuffer leverBuffer;
  leverBuffer.loadFromFile("assets/Lever.wav");
  Sound leverSound;
  leverSound.setBuffer(leverBuffer);

  // fb jump sound
  SoundBuffer fbjumpBuffer;
  fbjumpBuffer.loadFromFile("assets/Jump_fb.wav");
  Sound fbjumpSound;
  fbjumpSound.setBuffer(fbjumpBuffer);

  // wg jump sound
  SoundBuffer wgjumpBuffer;
  wgjumpBuffer.loadFromFile("assets/Jump_fb.wav");
  Sound wgjumpSound;
  wgjumpSound.setBuffer(wgjumpBuffer);

  // door sound
  SoundBuffer doorBuffer;
  doorBuffer.loadFromFile("assets/door2.wav");
  Sound doorSound;
  doorSound.setBuffer(doorBuffer);

  // death sound
  SoundBuffer death_buffer;
  death_buffer.loadFromFile("assets/Death.wav");
  Sound death_sound;
  death_sound.setBuffer(death_buffer);

  // diamond sound
  SoundBuffer diamond_buffer;
  diamond_buffer.loadFromFile("assets/Diamond.wav");
  Sound diamond_sound;
  diamond_sound.setBuffer(diamond_buffer);

  // background picture
  Texture backgroundTexture;
  backgroundTexture.loadFromFile("assets/background.png");
  Sprite backgroundPic(backgroundTexture);
  backgroundPic.setScale(1.0f, 0.978f);
  scaleBackground(backgroundPic, window.getSize());

  // box
  RectangleShape box(Vector2f(60.0f, 60.0f));
  Texture boxtxt;
  box.setPosition(871.0F, 350.0F);
  boxtxt.loadFromFile("assets/box.png");
  box.setTexture(&boxtxt);
  scaleRectangles(width, height, box);

  // crating fireboy  diamonds
  Texture red1;
  red1.loadFromFile("assets/redDiamond.png");

  // diamond1
  Sprite redDiamond1(red1);
  float x1 = 845.f, y1 = 1120.f;
  scalePosition(x1, y1, window);
  redDiamond1.setPosition(x1, y1);
  redDiamond1.setScale(width / 1720, height / 1300);

  // diamond 2
  Sprite redDiamond2(red1);
  float x2 = 270.f, y2 = 550.f;
  scalePosition(x2, y2, window);
  redDiamond2.setPosition(x2, y2);
  redDiamond2.setScale(width / 1720, height / 1300);

  // diamond 3
  Sprite redDiamond3(red1);
  float x3 = 400.f, y3 = 80.f;
  scalePosition(x3, y3, window);
  redDiamond3.setPosition(x3, y3);
  redDiamond3.setScale(width / 1720, height / 1300);
  // diamond 4
  Sprite redDiamond4(red1);
  float x4 = 760.f, y4 = 160.f;
  scalePosition(x4, y4, window);
  redDiamond4.setPosition(x4, y4);
  redDiamond4.setScale(width / 1720, height / 1300);

  // crating watergirl  diamonds
  Texture blue1;
  blue1.loadFromFile("assets/blueDiamond.png");

  // diamond1
  Sprite blueDiamond1(blue1);
  float x5 = 1200.f, y5 = 1120.f;
  scalePosition(x5, y5, window);
  blueDiamond1.setPosition(x5, y5);
  blueDiamond1.setScale(width / 1720, height / 1300);
  // diamond2
  Sprite blueDiamond2(blue1);
  float x6 = 980.f, y6 = 600.f;
  scalePosition(x6, y6, window);
  blueDiamond2.setPosition(x6, y6);
  blueDiamond2.setScale(width / 1720, height / 1300);

  // diamond3
  Sprite blueDiamond3(blue1);
  float x7 = 55.f, y7 = 210.f;
  scalePosition(x7, y7, window);
  blueDiamond3.setPosition(x7, y7);
  blueDiamond3.setScale(width / 1720, height / 1300);

  // diamond4
  Sprite blueDiamond4(blue1);
  float x8 = 940.f, y8 = 170.f;
  scalePosition(x8, y8, window);
  blueDiamond4.setPosition(x8, y8);
  blueDiamond4.setScale(width / 1720, height / 1300);

  // creating lava
  RectangleShape lava(Vector2f(118.f, 10.f));
  lava.setPosition(838.f, 1238.f);
  scaleRectangles(width, height, lava);

  // creating lake
  RectangleShape lake(Vector2f(129.f, 10.f));
  lake.setPosition(1173.f, 1238.f);
  scaleRectangles(width, height, lake);

  // creating green goo
  RectangleShape green_goo(Vector2f(105.f, 10.f));
  green_goo.setPosition(1105.f, 968.f);
  scaleRectangles(width, height, green_goo);

  // creating two walls
  // left wall
  RectangleShape left_wall(Vector2f(10.f, 1800.f));
  left_wall.setPosition(20.f, 0);
  scaleRectangles(width, height, left_wall);

  // right wall
  RectangleShape right_wall(Vector2f(10.f, 1800.f));
  right_wall.setPosition(1670, 0);
  scaleRectangles(width, height, right_wall);

  // Creating grounds for each floor
  RectangleShape ground[] = {
      // ground floor
      RectangleShape(Vector2f(1950.f, 10.f)),
      // groundright
      RectangleShape(Vector2f(10.f, 80.f)),
      RectangleShape(Vector2f(10.f, 35.f)),
      RectangleShape(Vector2f(40.f, 10.f)),
      // top
      RectangleShape(Vector2f(1950.f, 10.f)),
      // floor1
      RectangleShape(Vector2f(390.f, 30.f)),
      // floor2 straight right
      RectangleShape(Vector2f(500.f, 15.f)),
      // floor2 inclined *******
      RectangleShape(Vector2f(10.f, 70.f)),
      // floor2 straight left
      RectangleShape(Vector2f(530.f, 15.f)),
      // floor3 straight right
      RectangleShape(Vector2f(580.f, 20.f)),
      // floor3 inclined ******
      RectangleShape(Vector2f(10.f, 35.f)),
      // floor3 straight left
      RectangleShape(Vector2f(460.f, 20.f)),
      // floor4 straight left
      RectangleShape(Vector2f(1050.f, 30.f)),
      // floor4 straight right
      RectangleShape(Vector2f(240.f, 10.f)),
      // floor4 middle obstacle straight up
      RectangleShape(Vector2f(225.f, 10.f)),
      // floor4 top of square left
      RectangleShape(Vector2f(155.f, 10.f)),
      // floor4 width of square left
      RectangleShape(Vector2f(10.f, 90.f)),
      // floor4 middle obstacle left
      RectangleShape(Vector2f(10.f, 50.f)),
      // floor4 middle obstacle incline *****
      RectangleShape(Vector2f(10.f, 55.f)),
      // floor5 middle straight
      RectangleShape(Vector2f(420.f, 10.f)),
      // floor5 straight right
      RectangleShape(Vector2f(250.f, 10.f)),
      // floor5 middle right incline
      RectangleShape(Vector2f(10.f, 35.f)),
      // floor5 middle left incline
      RectangleShape(Vector2f(10.f, 35.f)),
      // floor5 middle up straight
      RectangleShape(Vector2f(80.f, 10.f)),
      // floor5 left obstacle
      RectangleShape(Vector2f(10.f, 35.f)),
      RectangleShape(Vector2f(10.f, 35.f)),
      RectangleShape(Vector2f(50.f, 10.f)),
      RectangleShape(Vector2f(70.f, 10.f)),
      RectangleShape(Vector2f(10.f, 20.f)),
  };

  // ground editing
  {
    // ground floor
    ground[0].setPosition(0.f, 815.f);
    // groundright
    ground[1].setPosition(1090.f, 750.f); // small right wall
    ground[2].setPosition(1115.f, 730.f);
    ground[2].setRotation(45);
    ground[3].setPosition(1125.f, 730.f);
    // top
    ground[4].setPosition(0.f, 35.f);
    // floor1
    ground[5].setPosition(0.f, 700.f);
    // floor2 straight right
    ground[6].setPosition(540.f, 650.f);
    // floor2 inclined
    //***
    ground[7].setPosition(530.f, 595.f);
    // ground[7].setRotation(-45);
    //  floor2 straight left
    ground[8].setPosition(-5.f, 585.f);
    // floor3 straight right
    ground[9].setPosition(620.f, 470.f);
    // floor3 inclined
    //****
    ground[10].setPosition(610.f, 450.f);
    // ground[10].setRotation(-45);
    //  floor3 straight left
    ground[11].setPosition(160.f, 440.f);
    // floor4 straight left
    ground[12].setPosition(0.f, 320.f);
    // floor4 straight right
    ground[13].setPosition(810.f, 330.f);
    // floor4 middle obstacle straight up
    ground[14].setPosition(590.f, 270.f);
    // floor4 top of square left
    ground[15].setPosition(0.f, 215.f);
    // floor4 width of square left
    ground[16].setPosition(170.f, 225.f); // small  left wall
    // floor4 middle obstacle left
    ground[17].setPosition(580.f, 290.f); // small right wall up
    // floor4 middle obstacle incline
    //****
    ground[18].setPosition(805.f, 280.f);
    // ground[18].setRotation(-45);
    //  floor5 middle straight
    ground[19].setPosition(430.f, 185.f);
    // floor5 straight right
    ground[20].setPosition(930.f, 185.f);
    // floor5 middle right incline
    //****
    ground[21].setPosition(920.f, 160.f);
    // ground[21].setRotation(-45);
    //  floor5 middle left incline
    //*****
    ground[22].setPosition(845.f, 160.f);
    // ground[22].setRotation(45);
    //  floor5 middle up straight
    ground[23].setPosition(850.f, 155.f);
    // floor5 left obstacle
    //***
    ground[24].setPosition(430.f, 170.f);
    // ground[24].setRotation(-45);
    //****
    ground[25].setPosition(365.f, 150.f);
    // ground[25].setRotation(-45);
    ground[26].setPosition(380.f, 165.f);
    ground[27].setPosition(310.f, 135.f);
    //****
    ground[28].setPosition(290.f, 135.f);
    // ground[28].setRotation(45);
  }
  scaleRectangles(window.getSize(), ground, 29);
  ground[28].setPosition(-300, 0);

  // button1
  RectangleShape butt1(Vector2f(18, 18));
  butt1.setPosition(Vector2f(463, 646));
  scaleRectangles(width, height, butt1);

  // button2
  RectangleShape butt2(Vector2f(18, 18));
  butt2.setPosition(Vector2f(1311, 477));
  scaleRectangles(width, height, butt2);

  // LEVER
  Texture lever;
  lever.loadFromFile("assets/lever.png");
  RectangleShape lvr(Vector2f(20, 65));
  lvr.setTexture(&lever);
  lvr.setPosition(Vector2f(460, 886));
  scaleRectangles(width, height, lvr);
  lvr.setOrigin(
      Vector2f(lvr.getLocalBounds().width, lvr.getLocalBounds().height) /
      1.05f);
  lvr.setRotation(45);

  ////starting position of the lever////
  RectangleShape stlvr(Vector2f(5, 40));
  stlvr.setPosition(Vector2f(479, 867));
  scaleRectangles(width, height, stlvr);

  ////end position of the lever////
  RectangleShape endlvr(Vector2f(5, 40));
  endlvr.setPosition(Vector2f(417, 867));
  scaleRectangles(width, height, endlvr);

  // Elevator1 // the right one with the buttons
  Texture elvv;
  elvv.loadFromFile("assets/elev2.png");
  Sprite elevator1;
  elevator1.setTexture(elvv);
  elevator1.setPosition(Vector2f(1500, 553));
  scaleSprite(elevator1, width, height);

  // Elevator1 START fo2
  RectangleShape STRTelv1(Vector2f(160, 5));
  STRTelv1.setPosition(1500, 490);
  scaleRectangles(width, height, STRTelv1);

  // Elevator1   END t7t
  RectangleShape ENDelv1(Vector2f(160, 5));
  ENDelv1.setPosition(1500, 704);
  scaleRectangles(width, height, ENDelv1);

  // ELEVATOR2 the left one with the lever
  Texture elv;
  elv.loadFromFile("assets/elev1.png");
  Sprite elevator2;
  elevator2.setTexture(elv);
  elevator2.setPosition(Vector2f(37, 673));
  scaleSprite(elevator2, width, height);

  // Elevator 2 START FO2
  RectangleShape STRTelv2(Vector2f(160, 5));
  STRTelv2.setPosition(37, 673);
  scaleRectangles(width, height, STRTelv2);

  // ELEVATOR2 END t7t
  RectangleShape ENDelv2(Vector2f(160, 5));
  ENDelv2.setPosition(37, 840);
  scaleRectangles(width, height, ENDelv2);
  // making and editing fireboy
  Texture text;
  text.loadFromFile("assets/fireboysheet.png");
  Sprite fireboy(text);
  fireboy.setTextureRect(sf::IntRect(5, 4 * 100, 85, 112));
  fireboy.setPosition(77, 1140);
  fireboy.setScale(0.85, 0.85);
  scaleSprite(fireboy, width, height);

  // making and editing watergirl
  Texture texting;
  texting.loadFromFile("assets/watergirlsheet.png");
  Sprite watergirl(texting);
  watergirl.setTextureRect(sf::IntRect(5, 4 * 130, 120, 120));
  watergirl.setPosition(78, 960);
  watergirl.setScale(0.85, 0.85);
  scaleSprite(watergirl, width, height);

  // doors
  // fireboy door
  Texture bdtexture;
  bdtexture.loadFromFile("assets/bdoor.png");
  Sprite bdoor(bdtexture);
  bdoor.setPosition(sf::Vector2f(1368.0f, 153.0f));
  bdoor.setTextureRect(sf::IntRect(0, 0, 110, 130));
  scaleSprite(bdoor, width, height);

  // watergirl door
  Texture gdtexture;
  gdtexture.loadFromFile("assets/gdoor.png");
  Sprite gdoor(gdtexture);
  gdoor.setPosition(sf::Vector2f(1500.0f, 153.0f));
  gdoor.setTextureRect(sf::IntRect(0, 0, 110, 130));
  scaleSprite(gdoor, width, height);
  bool buttonsoundplayed = true;

  Clock clock;
  // main event
  Event ev;
  int temp = 0;

  // settings button
  settingsbtm.setsprites("assets/settings.png");
  settingsbtm.sprite.setPosition(
      (width - (settingsbtm.sprite.getGlobalBounds().width)) / 2,
      (height - (0.5 * (settingsbtm.sprite.getGlobalBounds().height))));

  // Main Game loop
  while (window.isOpen()) {

    while (window.pollEvent(ev)) {
      if (ev.type == Event::Closed) {
        window.close();
      }
    }
    if (!menu.isActive && !pausemenu.isActive && !winingmenu.isActive &&
        !losingmenu.isActive) {

      // stopwatch
      Time gameTime = gameClock.getElapsedTime();
      temp = gameTime.asSeconds();
      int minutes = temp / 60;
      int seconds = temp % 60;

      timeText.setString(std::to_string(minutes) + ":" +
                         (seconds < 10 ? "0" : "") + std::to_string(seconds));

      // playing music
      if (backMusicIsActive && (backgroundMusic.getStatus() != 2)) {
        backgroundMusic.play();
      }

      // fireboy diamonds
      {
        if (fireboy.getGlobalBounds().intersects(
                redDiamond1.getGlobalBounds())) {
          redDiamond1.move(9999.f, 0.f);
          if (EfxIsActive) {
            diamond_sound.play();
          }
        }
        if (fireboy.getGlobalBounds().intersects(
                redDiamond2.getGlobalBounds())) {
          redDiamond2.move(9999.f, 0.f);
          if (EfxIsActive) {
            diamond_sound.play();
          }
        }
        if (fireboy.getGlobalBounds().intersects(
                redDiamond2.getGlobalBounds())) {
          redDiamond2.move(9999.f, 0.f);
          if (EfxIsActive) {
            diamond_sound.play();
          }
        }
        if (fireboy.getGlobalBounds().intersects(
                redDiamond3.getGlobalBounds())) {
          redDiamond3.move(9999.f, 0.f);
          if (EfxIsActive) {
            diamond_sound.play();
          }
        }
        if (fireboy.getGlobalBounds().intersects(
                redDiamond4.getGlobalBounds())) {
          redDiamond4.move(9999.f, 0.f);
          if (EfxIsActive) {
            diamond_sound.play();
          }
        }
      }

      // watergirl diamonds
      {
        if (watergirl.getGlobalBounds().intersects(
                blueDiamond1.getGlobalBounds())) {
          blueDiamond1.move(9999.f, 0.f);
          if (EfxIsActive) {
            diamond_sound.play();
          }
        }
        if (watergirl.getGlobalBounds().intersects(
                blueDiamond2.getGlobalBounds())) {
          blueDiamond2.move(9999.f, 0.f);
          if (EfxIsActive) {
            diamond_sound.play();
          }
        }
        if (watergirl.getGlobalBounds().intersects(
                blueDiamond3.getGlobalBounds())) {
          blueDiamond3.move(9999.f, 0.f);
          if (EfxIsActive) {
            diamond_sound.play();
          }
        }
        if (watergirl.getGlobalBounds().intersects(
                blueDiamond4.getGlobalBounds())) {
          blueDiamond4.move(9999.f, 0.f);
          if (EfxIsActive) {
            diamond_sound.play();
          }
        }
      }

      // fireboy and watergirl death
      {
        if ((fireboy.getGlobalBounds().intersects(lake.getGlobalBounds())) ||
            (fireboy.getGlobalBounds().intersects(
                green_goo.getGlobalBounds()))) {
          fireboy.setPosition(Vector2f(-2000, 33330));
          backgroundMusic.pause();
          if (EfxIsActive) {
            death_sound.play();
          }
          losingmenu.isActive = true;
          gameClock.restart();
          // to hide fireboy and watergirl after dying
          window.clear();
          window.draw(backgroundPic);
          window.draw(lvr);
          window.draw(elevator1);
          window.draw(elevator2);
          window.draw(box);
          window.draw(redDiamond1);
          window.draw(redDiamond2);
          window.draw(redDiamond3);
          window.draw(redDiamond4);
          window.draw(blueDiamond1);
          window.draw(blueDiamond2);
          window.draw(blueDiamond3);
          window.draw(blueDiamond4);
          window.draw(bdoor);
          window.draw(gdoor);
          window.draw(fireboy);
          window.draw(watergirl);
          window.draw(timeText);
          drawLosingMenu(window);
        }
        if ((watergirl.getGlobalBounds().intersects(lava.getGlobalBounds())) ||
            (watergirl.getGlobalBounds().intersects(
                green_goo.getGlobalBounds()))) {
          watergirl.setPosition(Vector2f(-2000, 33330));
          backgroundMusic.pause();
          if (EfxIsActive) {
            death_sound.play();
          }
          losingmenu.isActive = true;
          gameClock.restart();
          window.clear();
          window.draw(backgroundPic);
          window.draw(lvr);
          window.draw(elevator1);
          window.draw(elevator2);
          window.draw(box);
          window.draw(redDiamond1);
          window.draw(redDiamond2);
          window.draw(redDiamond3);
          window.draw(redDiamond4);
          window.draw(blueDiamond1);
          window.draw(blueDiamond2);
          window.draw(blueDiamond3);
          window.draw(blueDiamond4);
          window.draw(bdoor);
          window.draw(gdoor);
          window.draw(fireboy);
          window.draw(watergirl);
          window.draw(timeText);
          drawLosingMenu(window);
        }
      }

      // pause
      if (Keyboard::isKeyPressed(Keyboard::Key::P)) {
        pausemenu.isActive = true;
      }
      if (Mouse::isButtonPressed(Mouse::Button::Left)) {
        if (settingsbtm.hover(Mouse::getPosition(window).x,
                              Mouse::getPosition(window).y)) {
          pausemenu.isActive = true;
        }
      }

      // exit when esc is pressed
      if (Keyboard::isKeyPressed(Keyboard::Key::Q)) {
        window.clear();
        menu.isActive = true;
        if (backMusicIsActive) {
          backgroundMusic.stop();
        }
        drawMenu(window);
      }

      // to test wining menu
      if (Keyboard::isKeyPressed(Keyboard::Key::Y)) {
        winingmenu.isActive = true;
        backgroundMusic.stop();
        drawWinningMenu(window, timeText);
      }

      // zatoona

      // lever moving right and left
      {
        if ((fireboy.getGlobalBounds().intersects(lvr.getGlobalBounds())) ||
            (watergirl.getGlobalBounds().intersects(lvr.getGlobalBounds()))) {
          if (((fireboy.getGlobalBounds().intersects(
                   stlvr.getGlobalBounds())) &&
               (!lvr.getGlobalBounds().intersects(endlvr.getGlobalBounds()))) ||
              ((watergirl.getGlobalBounds().intersects(
                   stlvr.getGlobalBounds())) &&
               (!lvr.getGlobalBounds().intersects(endlvr.getGlobalBounds()))))

          {
            lvr.rotate(-90);
            if (EfxIsActive)
              leverSound.play();
          }
        }
        if ((fireboy.getGlobalBounds().intersects(lvr.getGlobalBounds())) ||
            (watergirl.getGlobalBounds().intersects(lvr.getGlobalBounds()))) {
          if (((fireboy.getGlobalBounds().intersects(
                   endlvr.getGlobalBounds())) &&
               (!lvr.getGlobalBounds().intersects(stlvr.getGlobalBounds()))) ||
              ((watergirl.getGlobalBounds().intersects(
                   endlvr.getGlobalBounds())) &&
               (!lvr.getGlobalBounds().intersects(stlvr.getGlobalBounds()))))

          {
            lvr.rotate(90);
            if (EfxIsActive)
              leverSound.play();
          }
        }
      }

      // elevator2 moves with the lever
      {
        if (((lvr.getGlobalBounds().intersects(endlvr.getGlobalBounds()))) &&
            ((!elevator2.getGlobalBounds().intersects(
                ENDelv2.getGlobalBounds())))) {
          elevator2.move(0, 0.5);
        }
        if (((lvr.getGlobalBounds().intersects(stlvr.getGlobalBounds()))) &&
            ((!elevator2.getGlobalBounds().intersects(
                STRTelv2.getGlobalBounds())))) {
          elevator2.move(0, -1);
          if (doesIntersectElevator(fireboy, elevator1, elevator2)) {
            fireboy.move(0, -1);
          }
          if (doesIntersectElevator(watergirl, elevator1, elevator2)) {
            watergirl.move(0, -1);
          }
        }
      }

      // elevator1 moves with the buttons
      {
        if (elevator1.getGlobalBounds().intersects(ENDelv1.getGlobalBounds()) ||
            elevator1.getGlobalBounds().intersects(STRTelv1.getGlobalBounds()))
          buttonsoundplayed = false;
        if ((((fireboy.getGlobalBounds().intersects(butt1.getGlobalBounds())) ||
              (fireboy.getGlobalBounds().intersects(
                  butt2.getGlobalBounds()))) ||
             ((watergirl.getGlobalBounds().intersects(
                  butt1.getGlobalBounds())) ||
              (watergirl.getGlobalBounds().intersects(
                  butt2.getGlobalBounds())))) &&
            (!elevator1.getGlobalBounds().intersects(
                ENDelv1.getGlobalBounds()))) {
          if (EfxIsActive && !buttonsoundplayed) {
            buttonSound.play();
            buttonsoundplayed = true;
          }
          elevator1.move(0, 1);
        }
        if (((!fireboy.getGlobalBounds().intersects(butt1.getGlobalBounds())) &&
             (!fireboy.getGlobalBounds().intersects(
                 butt2.getGlobalBounds()))) &&
            ((!watergirl.getGlobalBounds().intersects(
                 butt1.getGlobalBounds())) &&
             (!watergirl.getGlobalBounds().intersects(
                 butt2.getGlobalBounds()))) &&
            (!elevator1.getGlobalBounds().intersects(
                STRTelv1.getGlobalBounds()))) {
          if (EfxIsActive && !buttonsoundplayed) {
            buttonSound.play();
            buttonsoundplayed = true;
          }
          elevator1.move(0, -1.5);
          if (doesIntersectElevator(fireboy, elevator1, elevator2)) {
            fireboy.move(0, -2.5);
          }
          if (doesIntersectElevator(watergirl, elevator1, elevator2)) {
            watergirl.move(0, -2.5);
          }
        }
      }

      // sprites not walking through the lever
      {
        if ((fireboy.getGlobalBounds().intersects(lvr.getGlobalBounds()))) {
          if ((fireboy.getGlobalBounds().intersects(lvr.getGlobalBounds())) &&
              ((fireboy.getGlobalBounds().intersects(
                  endlvr.getGlobalBounds())))) {
            fireboy.move(-5, 0);
          } else if ((fireboy.getGlobalBounds().intersects(
                         lvr.getGlobalBounds())) &&
                     ((fireboy.getGlobalBounds().intersects(
                         stlvr.getGlobalBounds())))) {
            fireboy.move(5, 0);
          }
        }
        if ((watergirl.getGlobalBounds().intersects(lvr.getGlobalBounds()))) {
          if ((watergirl.getGlobalBounds().intersects(lvr.getGlobalBounds())) &&
              ((watergirl.getGlobalBounds().intersects(
                  endlvr.getGlobalBounds())))) {
            watergirl.move(-5, 0);
          } else if ((watergirl.getGlobalBounds().intersects(
                         lvr.getGlobalBounds())) &&
                     ((watergirl.getGlobalBounds().intersects(
                         stlvr.getGlobalBounds())))) {
            watergirl.move(5, 0);
          }
        }
      }

      // box
      {
        // box gravity
        if ((!box.getGlobalBounds().intersects(ground[14].getGlobalBounds())) &&
            (!box.getGlobalBounds().intersects(ground[12].getGlobalBounds()))) {
          box_Vy += gravity;
        } else {
          box_Vy = 0;
        }
        box.move(0, box_Vy);

        // box movements with fireboy
        if (Keyboard::isKeyPressed(Keyboard::Key::Right) &&
            !doesIntersectBox(fireboy, box, ground) &&
            fireboy.getGlobalBounds().intersects(box.getGlobalBounds()) &&
            !box.getGlobalBounds().intersects(ground[17].getGlobalBounds()) &&
            (fireboy.getPosition().x < box.getPosition().x)) {
          box.move(vx, 0);
        } else if (fireboy.getGlobalBounds().intersects(
                       box.getGlobalBounds()) &&
                   !doesIntersectBox(fireboy, box, ground) &&
                   !box.getGlobalBounds().intersects(
                       ground[16].getGlobalBounds()) &&
                   Keyboard::isKeyPressed(Keyboard::Key::Left) &&
                   (fireboy.getPosition().x > box.getPosition().x)) {
          box.move(-vx, 0);
        }

        // box movements with watergirl
        if (Keyboard::isKeyPressed(Keyboard::Key::D) &&
            !doesIntersectBox(watergirl, box, ground) &&
            watergirl.getGlobalBounds().intersects(box.getGlobalBounds()) &&
            !box.getGlobalBounds().intersects(ground[17].getGlobalBounds()) &&
            (watergirl.getPosition().x < box.getPosition().x)) {
          box.move(vx, 0);
        } else if (watergirl.getGlobalBounds().intersects(
                       box.getGlobalBounds()) &&
                   !doesIntersectBox(watergirl, box, ground) &&
                   !box.getGlobalBounds().intersects(
                       ground[16].getGlobalBounds()) &&
                   Keyboard::isKeyPressed(Keyboard::Key::A) &&
                   (watergirl.getPosition().x > box.getPosition().x)) {
          box.move(-vx, 0);
        }
      }

      Fmove(fireboy, right_wall, ground[1], ground[17], ground[22], left_wall,
            ground[16], ground[10], ground[7], ground[18], ground[21],
            ground[24], ground[25], ground, box, elevator1, elevator2, ev,
            fbjumpSound);
      if (fireboy_Vy == vy && EfxIsActive)
        fbjumpSound.play();
      // watergirl moving function
      Wmove(watergirl, right_wall, ground[1], ground[17], ground[22], left_wall,
            ground[16], ground[10], ground[7], ground[18], ground[21],
            ground[24], ground[25], ground, box, elevator1, elevator2, ev,
            wgjumpSound);
      if (watergirl_Vy == vy && EfxIsActive)
        wgjumpSound.play();
      // doors mechanism
      {
        if (fireboy.getGlobalBounds().intersects(bdoor.getGlobalBounds())) {
          if (f < 8) {

            if (clock.getElapsedTime().asSeconds() >= 0.15) {
              f++;
              if (EfxIsActive && f == 1)
                doorSound.play();

              clock.restart();
            }

            bdoor.setTextureRect(sf::IntRect(0, f * 173, 110, 130));
          }
        }
        if (watergirl.getGlobalBounds().intersects(gdoor.getGlobalBounds())) {
          if (g < 8) {
            if (clock.getElapsedTime().asSeconds() >= 0.15) {
              g++;
              if (EfxIsActive && g == 1)
                doorSound.play();
              clock.restart();
            }
            if (EfxIsActive)
              doorSound.play();

            gdoor.setTextureRect(sf::IntRect(0, g * 158, 110, 130));
          }
        }
      }

      // winnig
      {
        if (fireboy.getGlobalBounds().intersects(bdoor.getGlobalBounds()) &&
            watergirl.getGlobalBounds().intersects(gdoor.getGlobalBounds()) &&
            f == 8 && g == 8) {
          fireboy.setPosition(-100, 0);
          watergirl.setPosition(-100, 0);
          winingmenu.isActive = true;
          backgroundMusic.stop();
          drawWinningMenu(window, timeText);
        }
      }

    } else {
      // if paused
      backgroundMusic.pause();
      drawPauseMenu(window);
    }
    // drawing
    {
      window.clear();
      window.draw(backgroundPic);
      window.draw(lvr);
      window.draw(elevator1);
      window.draw(elevator2);
      window.draw(box);
      window.draw(redDiamond1);
      window.draw(redDiamond2);
      window.draw(redDiamond3);
      window.draw(redDiamond4);
      window.draw(blueDiamond1);
      window.draw(blueDiamond2);
      window.draw(blueDiamond3);
      window.draw(blueDiamond4);
      window.draw(bdoor);
      window.draw(gdoor);
      window.draw(fireboy);
      window.draw(watergirl);
      window.draw(timeText);
      if (!pausemenu.isActive) {
        window.draw(settingsbtm.sprite);
      }
      window.display();
    }
  }
}

bool doesIntersect(Sprite &player, RectangleShape ground[]) {

  for (int i = 0; i < 29; i++) {
    if (player.getGlobalBounds().intersects(ground[i].getGlobalBounds()) &&
        i != 1 && i != 7 && i != 10 && i != 16 && i != 17 && i != 18 &&
        i != 21 && i != 22 && i != 24 && i != 25 && i != 28) {
      // Check if the bottom side of the player sprite intersects with the
      // top side of the ground rectangle
      if (player.getPosition().y + player.getGlobalBounds().height >=
              ground[i].getPosition().y &&
          player.getPosition().y + player.getGlobalBounds().height <=
              ground[i].getPosition().y + ground[i].getGlobalBounds().height &&
          player.getPosition().x + player.getGlobalBounds().width >
              ground[i].getPosition().x &&
          player.getPosition().x <
              ground[i].getPosition().x + ground[i].getGlobalBounds().width) {
        return true;
      }
    }
  }
  return false;
}
bool doesIntersectElevator(Sprite &player, Sprite &elevator1,
                           Sprite &elevator2) {
  // checking for collision with the right elevator
  if (player.getPosition().y + player.getGlobalBounds().height >=
          elevator1.getPosition().y &&
      player.getPosition().y + player.getGlobalBounds().height <=
          elevator1.getPosition().y + elevator1.getGlobalBounds().height &&
      player.getPosition().x + player.getGlobalBounds().width >
          elevator1.getPosition().x &&
      player.getPosition().x <
          elevator1.getPosition().x + elevator1.getGlobalBounds().width) {
    return true;
  }
  // checking for collision with the left elevator
  else if (player.getPosition().y + player.getGlobalBounds().height >=
               elevator2.getPosition().y &&
           player.getPosition().y + player.getGlobalBounds().height <=
               elevator2.getPosition().y + elevator2.getGlobalBounds().height &&
           player.getPosition().x + player.getGlobalBounds().width >
               elevator2.getPosition().x &&
           player.getPosition().x <
               elevator2.getPosition().x + elevator2.getGlobalBounds().width) {
    return true;
  }
  return false;
}

bool doesIntersectBox(Sprite &player, RectangleShape &box,
                      RectangleShape ground[]) {
  // checking for collision with the box
  if (!doesIntersect(player, ground) &&
      player.getPosition().y + player.getGlobalBounds().height >=
          box.getPosition().y &&
      player.getPosition().y + player.getGlobalBounds().height <=
          box.getPosition().y + box.getGlobalBounds().height &&
      player.getPosition().x + player.getGlobalBounds().width - 13 >
          box.getPosition().x &&
      player.getPosition().x <
          box.getPosition().x + box.getGlobalBounds().width - 13) {
    return true;
  }
  return false;
}

void Fmove(Sprite &fireboy, RectangleShape &rWall, RectangleShape &rWall2,
           RectangleShape &rWall3, RectangleShape &rWall4,
           RectangleShape &lWall, RectangleShape &lWall2,
           RectangleShape &lWall3, RectangleShape &lWall4,
           RectangleShape &lWall5, RectangleShape &lWall6,
           RectangleShape &lWall7, RectangleShape &lWall8,
           RectangleShape ground[], RectangleShape &box, Sprite &elevator1,
           Sprite &elevator2, Event event, Sound fbjumpSound) {
  if (!(Keyboard::isKeyPressed(Keyboard::Right)) &&
      !(Keyboard::isKeyPressed(Keyboard::Left)) &&
      !(Keyboard::isKeyPressed(Keyboard::Up)) && fireboy_Vy == 0) {
    fireboy.setTextureRect(sf::IntRect(5, 4 * 100, 85, 105));
  }

  // Check if the top side of the player sprite intersects with the bottom side
  // of the grounds
  for (int i = 0; i < 29; i++) {
    if (fireboy.getGlobalBounds().intersects(ground[i].getGlobalBounds()) &&
        !doesIntersect(fireboy, ground) &&
        fireboy.getPosition().y > ground[i].getPosition().y && i != 1 &&
        i != 2 && i != 3 && i != 16 && i != 17 && i != 7) {
      fireboy.setPosition(fireboy.getPosition().x,
                          ground[i].getPosition().y +
                              ground[i].getGlobalBounds().height);
      fireboy_Vy += gravity;
    }
  }

  if (fireboy.getGlobalBounds().intersects(elevator1.getGlobalBounds()) &&
      !doesIntersectElevator(fireboy, elevator1, elevator2) &&
      fireboy.getPosition().y - elevator1.getGlobalBounds().height >
          elevator1.getPosition().y) {
    fireboy.setPosition(fireboy.getPosition().x,
                        elevator1.getPosition().y +
                            elevator1.getGlobalBounds().height);
    fireboy_Vy += gravity;
  } else if (fireboy.getGlobalBounds().intersects(
                 elevator2.getGlobalBounds()) &&
             !doesIntersectElevator(fireboy, elevator1, elevator2) &&
             fireboy.getPosition().y - elevator2.getGlobalBounds().height >
                 elevator2.getPosition().y) {
    fireboy.setPosition(fireboy.getPosition().x,
                        elevator2.getPosition().y +
                            elevator2.getGlobalBounds().height);
    fireboy_Vy += gravity;
  }

  // to the right
  if ((Keyboard::isKeyPressed(Keyboard::Key::Right)) &&
      (!fireboy.getGlobalBounds().intersects(rWall.getGlobalBounds())) &&
      (!fireboy.getGlobalBounds().intersects(rWall2.getGlobalBounds())) &&
      (!fireboy.getGlobalBounds().intersects(rWall3.getGlobalBounds())) &&
      (!fireboy.getGlobalBounds().intersects(rWall4.getGlobalBounds())) &&
      (!fireboy.getGlobalBounds().intersects(
          lWall4.getGlobalBounds())) && // intended, not a mistake
      !(fireboy.getGlobalBounds().intersects(box.getGlobalBounds()) &&
        box.getGlobalBounds().intersects(rWall3.getGlobalBounds()))) {
    fireboy.move(vx, 0.0f);
    a++;
    if (a % 30 == 0) {
      x++;
    }
    y = 0;
    x = x % 5;
    fireboy.setTextureRect(sf::IntRect(x * 113, y * 95, 95, 95));
  }
  // to the left
  if (Keyboard::isKeyPressed(Keyboard::Key::Left) &&
      (!fireboy.getGlobalBounds().intersects(lWall.getGlobalBounds())) &&
      (!fireboy.getGlobalBounds().intersects(lWall2.getGlobalBounds())) &&
      (!fireboy.getGlobalBounds().intersects(lWall3.getGlobalBounds())) &&
      (!fireboy.getGlobalBounds().intersects(lWall4.getGlobalBounds())) &&
      (!fireboy.getGlobalBounds().intersects(lWall5.getGlobalBounds())) &&
      (!fireboy.getGlobalBounds().intersects(lWall6.getGlobalBounds())) &&
      (!fireboy.getGlobalBounds().intersects(lWall7.getGlobalBounds())) &&
      (!fireboy.getGlobalBounds().intersects(lWall8.getGlobalBounds())) &&
      !(fireboy.getGlobalBounds().intersects(box.getGlobalBounds()) &&
        box.getGlobalBounds().intersects(lWall2.getGlobalBounds()))) {
    fireboy.move(-vx, 0.0f);
    a++;
    if (a % 30 == 0) {
      x++;
    }
    y = 1;
    x = x % 5;
    fireboy.setTextureRect(sf::IntRect(x * 113, y * 95, 100, 95));
  }
  // fireboy jumping
  if ((!doesIntersect(fireboy, ground)) &&
      !doesIntersectBox(fireboy, box, ground) &&
      !doesIntersectElevator(fireboy, elevator1, elevator2)) {
    fireboy_Vy += gravity;
  } else {
    fireboy_Vy = 0;
  }
  if ((Keyboard::isKeyPressed(Keyboard::Key::Up)) &&
      ((doesIntersect(fireboy, ground)) ||
       doesIntersectBox(fireboy, box, ground) ||
       doesIntersectElevator(fireboy, elevator1, elevator2))) {
    fireboy_Vy = vy;
    if (EfxIsActive)
      fbjumpSound.play();
    a++;
    if (a % 40 == 0)
      x++;
    y = 2;
    x = x % 4;
    fireboy.setTextureRect(sf::IntRect(x * 113, y * 95, 100, 95));
  }
  // upwards
  fireboy.move(0, fireboy_Vy);

  if ((fireboy_Vy > 0) && !doesIntersect(fireboy, ground) &&
      !doesIntersectBox(fireboy, box, ground) &&
      !doesIntersectElevator(fireboy, elevator1, elevator2) &&
      !Keyboard::isKeyPressed(Keyboard::Right) &&
      !Keyboard::isKeyPressed(Keyboard::Left)) {
    x++;
    y = 3;
    x = x % 4;
    fireboy.setTextureRect(sf::IntRect(x * 113, y * 95, 100, 95));
  }
}

void Wmove(Sprite &watergirl, RectangleShape &rWall, RectangleShape &rWall2,
           RectangleShape &rWall3, RectangleShape &rWall4,
           RectangleShape &lWall, RectangleShape &lWall2,
           RectangleShape &lWall3, RectangleShape &lWall4,
           RectangleShape &lWall5, RectangleShape &lWall6,
           RectangleShape &lWall7, RectangleShape &lWall8,
           RectangleShape ground[], RectangleShape &box, Sprite &elevator1,
           Sprite &elevator2, Event event, Sound wgjumpSound) {

  if (!(Keyboard::isKeyPressed(Keyboard::W)) &&
      !(Keyboard::isKeyPressed(Keyboard::A)) &&
      !(Keyboard::isKeyPressed(Keyboard::D)) && watergirl_Vy == 0) {
    watergirl.setTextureRect(sf::IntRect(5, 4 * 130, 120, 120));
  }

  // Check if the top side of the player sprite intersects with the bottom side
  // of the grounds
  for (int i = 0; i < 29; i++) {
    if (watergirl.getGlobalBounds().intersects(ground[i].getGlobalBounds()) &&
        !doesIntersect(watergirl, ground) &&
        watergirl.getPosition().y > ground[i].getPosition().y && i != 1 &&
        i != 2 && i != 3 && i != 16 && i != 17 && i != 7) {
      watergirl.setPosition(watergirl.getPosition().x,
                            ground[i].getPosition().y +
                                ground[i].getGlobalBounds().height);
      watergirl_Vy += gravity;
    }
  }
  if (watergirl.getGlobalBounds().intersects(elevator1.getGlobalBounds()) &&
      !doesIntersectElevator(watergirl, elevator1, elevator2) &&
      watergirl.getPosition().y - elevator1.getGlobalBounds().height >
          elevator1.getPosition().y) {
    watergirl.setPosition(watergirl.getPosition().x,
                          elevator1.getPosition().y +
                              elevator1.getGlobalBounds().height);
    watergirl_Vy += gravity;
  } else if (watergirl.getGlobalBounds().intersects(
                 elevator2.getGlobalBounds()) &&
             !doesIntersectElevator(watergirl, elevator1, elevator2) &&
             watergirl.getPosition().y - elevator2.getGlobalBounds().height >
                 elevator2.getPosition().y) {
    watergirl.setPosition(watergirl.getPosition().x,
                          elevator2.getPosition().y +
                              elevator2.getGlobalBounds().height);
    watergirl_Vy += gravity;
  }
  // to the right
  if ((Keyboard::isKeyPressed(Keyboard::Key::D)) &&
      (!watergirl.getGlobalBounds().intersects(rWall.getGlobalBounds())) &&
      (!watergirl.getGlobalBounds().intersects(rWall2.getGlobalBounds())) &&
      (!watergirl.getGlobalBounds().intersects(rWall3.getGlobalBounds())) &&
      (!watergirl.getGlobalBounds().intersects(rWall4.getGlobalBounds())) &&
      (!watergirl.getGlobalBounds().intersects(lWall4.getGlobalBounds())) &&
      !(watergirl.getGlobalBounds().intersects(box.getGlobalBounds()) &&
        box.getGlobalBounds().intersects(rWall3.getGlobalBounds()))) {
    watergirl.move(vx, 0.0f);
    a++;
    if (a % 30 == 0) {
      x++;
    }
    y = 1;
    x = x % 5;
    watergirl.setTextureRect(sf::IntRect(x * 175, y * 133, 120, 120));
  }
  // to the left
  if ((Keyboard::isKeyPressed(Keyboard::Key::A)) &&
      (!watergirl.getGlobalBounds().intersects(lWall.getGlobalBounds())) &&
      (!watergirl.getGlobalBounds().intersects(lWall2.getGlobalBounds())) &&
      (!watergirl.getGlobalBounds().intersects(lWall3.getGlobalBounds())) &&
      (!watergirl.getGlobalBounds().intersects(lWall4.getGlobalBounds())) &&
      (!watergirl.getGlobalBounds().intersects(lWall5.getGlobalBounds())) &&
      (!watergirl.getGlobalBounds().intersects(lWall6.getGlobalBounds())) &&
      (!watergirl.getGlobalBounds().intersects(lWall7.getGlobalBounds())) &&
      (!watergirl.getGlobalBounds().intersects(lWall8.getGlobalBounds())) &&
      !(watergirl.getGlobalBounds().intersects(box.getGlobalBounds()) &&
        box.getGlobalBounds().intersects(lWall2.getGlobalBounds()))) {
    watergirl.move(-vx, 0.0f);
    a++;
    if (a % 50 == 0) {
      x++;
    }
    y = 0;
    x = x % 5;
    watergirl.setTextureRect(sf::IntRect(x * 175, y * 133, 120, 120));
  }
  // watergirl jumping
  if ((!doesIntersect(watergirl, ground)) &&
      !doesIntersectBox(watergirl, box, ground) &&
      !doesIntersectElevator(watergirl, elevator1, elevator2)) {
    watergirl_Vy += gravity;
  } else {
    watergirl_Vy = 0;
  }
  if ((Keyboard::isKeyPressed(Keyboard::Key::W)) &&
      (doesIntersect(watergirl, ground) ||
       doesIntersectBox(watergirl, box, ground) ||
       doesIntersectElevator(watergirl, elevator1, elevator2))) {
    watergirl_Vy = vy;
    if (EfxIsActive)
      wgjumpSound.play();
    a++;
    if (a % 30 == 0)
      x++;
    y = 2;
    x = x % 4;
    watergirl.setTextureRect(sf::IntRect(x * 175, y * 133, 120, 120));
  }
  // to the up
  watergirl.move(0, watergirl_Vy);

  if (watergirl_Vy > 0 && !doesIntersect(watergirl, ground) &&
      !doesIntersectBox(watergirl, box, ground) &&
      !Keyboard::isKeyPressed(Keyboard::D) &&
      !Keyboard::isKeyPressed(Keyboard::A)) {
    x++;
    y = 3;
    x = x % 4;
    watergirl.setTextureRect(sf::IntRect(x * 175, y * 133, 120, 120));
  }
}

void scaleSprite(Sprite &sprite, int windowWidth, int windowHeight) {
  float xScale = (float)windowWidth / 1720;
  float yScale = (float)windowHeight / 1300;
  float xPos = sprite.getPosition().x * xScale;
  float yPos = sprite.getPosition().y * yScale;
  sprite.setScale(sprite.getScale().x * xScale, sprite.getScale().y * yScale);
  sprite.setPosition(xPos, yPos);
}

void scaleRectangles(sf::Vector2u windowSize, sf::RectangleShape ground[],
                     int size) {
  // Calculate the scaling factors for x and y axis
  float scaleX = (float)windowSize.x / 1210.f;
  float scaleY = (float)windowSize.y / 850.f;
  for (int i = 0; i < size; i++) {
    sf::Vector2f position = ground[i].getPosition();
    sf::Vector2f size = ground[i].getSize();
    position.x *= scaleX;
    position.y *= scaleY;
    size.x *= scaleX;
    size.y *= scaleY;
    ground[i].setPosition(position);
    ground[i].setSize(size);
  }
}

void scaleRectangles(int windowWidth, int windowHeight,
                     sf::RectangleShape &ground) {
  // Calculate the scaling factors for x and y axis
  float scaleX = (float)windowWidth / 1720.f;
  float scaleY = (float)windowHeight / 1300.f;
  Vector2f position = ground.getPosition();
  Vector2f size = ground.getSize();
  position.x *= scaleX;
  position.y *= scaleY;
  size.x *= scaleX;
  size.y *= scaleY;
  ground.setPosition(position);
  ground.setSize(size);
}

void scaleBackground(Sprite &backgroundPic, const Vector2u &windowSize) {
  float scaleX = (float)windowSize.x / 1720.f;
  float scaleY = (float)windowSize.y / 1300.f;
  backgroundPic.setScale(scaleX, scaleY * 0.978f);
  backgroundPic.setPosition(-14.f * scaleX, 10.f * scaleY);
}

void scalePosition(float &xPos, float &yPos, const sf::RenderWindow &window) {
  float xScale = window.getSize().x / 1720.f; // calculate x scaling factor
  float yScale = window.getSize().y / 1300.f; // calculate y scaling factor
  // scale the diamond position
  xPos *= xScale;
  yPos *= yScale;
}

void drawOptionsMenu(RenderWindow &window) {
  optionsMenu.setbackgroud(optionsMenu.background, optionsMenu.bg);
  paustitle.setsprites("assets/pausedlogo.png");
  okbtm.setsprites("assets/okbtm.png");

  paustitle.sprite.setPosition(
      (width - (paustitle.sprite.getGlobalBounds().width)) / 2,
      ((400 * (height / 850))));

  okbtm.sprite.setPosition(
      (width - (paustitle.sprite.getGlobalBounds().width)) / 2,
      ((650 * (height / 850))));

  optionsMenu.setsbackgroud();

  while (optionsMenu.isActive) {
    if (backMusicIsActive) {
      musicbtm.setsprites("assets/musicactv.png");
      musicbtm.sprite.setPosition((width / 2) + 50, ((498 * (height / 850))));
    } else {
      musicbtm.setsprites("assets/musicnactv.png");
      musicbtm.sprite.setPosition((width / 2) + 50, ((500 * (height / 850))));
    }
    if (EfxIsActive) {
      efxbtm.setsprites("assets/soundactv.png");
    } else {
      efxbtm.setsprites("assets/soundnactv.png");
    }

    efxbtm.sprite.setPosition(
        ((width / 2) - efxbtm.sprite.getGlobalBounds().width) - 50,
        ((500 * (height / 850))));

    Event ev;
    while (window.pollEvent(ev)) {
      if (ev.type == Event::Closed) {
        window.close();
        break;
      } else if (ev.type == Event::KeyReleased) {
        if (ev.key.code == Keyboard::Escape) {
          window.clear();
          optionsMenu.isActive = false;
          menu.isActive = true;
          drawMenu(window);
        }
      } else if (ev.type == Event::MouseButtonReleased) {
        if (okbtm.hover(Mouse::getPosition(window).x,
                        Mouse::getPosition(window).y)) {
          window.clear();
          optionsMenu.isActive = false;
          menu.isActive = true;
          drawMenu(window);
        } else if (musicbtm.hover(Mouse::getPosition(window).x,
                                  Mouse::getPosition(window).y)) {
          toggle(backMusicIsActive);
        } else if (efxbtm.hover(Mouse::getPosition(window).x,
                                Mouse::getPosition(window).y)) {
          toggle(EfxIsActive);
        }
      }
    }

    window.clear();
    window.draw(optionsMenu.bg);
    window.draw(optionsMenu.smenu);
    window.draw(optionsMenu.logo);
    window.draw(paustitle.sprite);
    window.draw(okbtm.sprite);
    window.draw(musicbtm.sprite);
    window.draw(efxbtm.sprite);
    window.display();
  }
}

void drawPauseMenu(RenderWindow &window) {

  paustitle.setsprites("assets/pausedlogo.png");
  resumebtm.setsprites("assets/resumebtm.png");
  retrybtm.setsprites("assets/retrybtm.png");
  endbtm.setsprites("assets/endbtm.png");

  paustitle.sprite.setPosition(
      (width - (paustitle.sprite.getGlobalBounds().width)) / 2,
      ((400 * (height / 850))));

  retrybtm.sprite.setPosition((width / 2) + 50, ((550 * (height / 850))));

  resumebtm.sprite.setPosition(
      ((width / 2) - resumebtm.sprite.getGlobalBounds().width) - 50,
      ((550 * (height / 850))));

  endbtm.sprite.setPosition(
      (width - (paustitle.sprite.getGlobalBounds().width)) / 2,
      ((650 * (height / 850))));

  pausemenu.setsbackgroud();
  while (pausemenu.isActive) {
    if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) {
      pausemenu.isActive = false;
    }
    if (Mouse::isButtonPressed(Mouse::Button::Left)) {
      if (resumebtm.hover(Mouse::getPosition(window).x,
                          Mouse::getPosition(window).y)) {
        pausemenu.isActive = false;
      } else if (retrybtm.hover(Mouse::getPosition(window).x,
                                Mouse::getPosition(window).y)) {
        pausemenu.isActive = false;
        window.clear();
        // gameclock.restart();
        level1(window);
      } else if (endbtm.hover(Mouse::getPosition(window).x,
                              Mouse::getPosition(window).y)) {
        pausemenu.isActive = false;
        menu.isActive = true;
        window.clear();
        drawMenu(window);
      }
    }
    window.draw(pausemenu.smenu);
    window.draw(pausemenu.logo);
    window.draw(paustitle.sprite);
    window.draw(resumebtm.sprite);
    window.draw(retrybtm.sprite);
    window.draw(endbtm.sprite);
    window.display();
  }
}

void drawWinningMenu(RenderWindow &window, Text &gameTime) {
  gameTime.setCharacterSize(static_cast<float>(90) / 1720 * width);
  gameTime.setPosition((width - gameTime.getGlobalBounds().width )/ 2, ((400 * (height / 850))));

  retrybtm.setsprites("assets/retrybtm.png");
  endbtm.setsprites("assets/endbtm.png");

  endbtm.sprite.setPosition((width / 2) + 50, ((550 * (height / 850))));

  retrybtm.sprite.setPosition(
      ((width / 2) - retrybtm.sprite.getGlobalBounds().width) - 50,
      ((550 * (height / 850))));

  winingmenu.setsbackgroud();
  while (winingmenu.isActive) {
    if (Mouse::isButtonPressed(Mouse::Button::Left)) {
      if (retrybtm.hover(Mouse::getPosition(window).x,
                         Mouse::getPosition(window).y)) {
        winingmenu.isActive = false;
        window.clear();
        level1(window);
      } else if (endbtm.hover(Mouse::getPosition(window).x,
                              Mouse::getPosition(window).y)) {
        winingmenu.isActive = false;
        menu.isActive = true;
        window.clear();
        drawMenu(window);
      }
    }
    window.draw(winingmenu.smenu);
    window.draw(winingmenu.logo);
    window.draw(paustitle.sprite);
    window.draw(retrybtm.sprite);
    window.draw(endbtm.sprite);
    window.draw(gameTime);
    window.display();
  }
}

void drawLosingMenu(RenderWindow &window) {
  paustitle.setsprites("assets/gameover.png");
  retrybtm.setsprites("assets/retrybtm.png");
  endbtm.setsprites("assets/endbtm.png");

  paustitle.sprite.setPosition(
      (width - (paustitle.sprite.getGlobalBounds().width)) / 2,
      ((400 * (height / 850))));

  endbtm.sprite.setPosition((width / 2) + 50, ((550 * (height / 850))));

  retrybtm.sprite.setPosition(
      ((width / 2) - retrybtm.sprite.getGlobalBounds().width) - 50,
      ((550 * (height / 850))));

  losingmenu.setsbackgroud();
  while (losingmenu.isActive) {
    if (Mouse::isButtonPressed(Mouse::Button::Left)) {
      if (retrybtm.hover(Mouse::getPosition(window).x,
                         Mouse::getPosition(window).y)) {
        losingmenu.isActive = false;
        window.clear();
        level1(window);
      } else if (endbtm.hover(Mouse::getPosition(window).x,
                              Mouse::getPosition(window).y)) {
        losingmenu.isActive = false;
        menu.isActive = true;
        window.clear();
        drawMenu(window);
      }
    }
    window.draw(losingmenu.smenu);
    window.draw(losingmenu.logo);
    window.draw(paustitle.sprite);
    window.draw(retrybtm.sprite);
    window.draw(endbtm.sprite);
    window.display();
  }
}

void drawCreditesMenu(RenderWindow &window) {
  // making menu credits
  creditesmenu.font.loadFromFile("assets/flame.ttf");
  creditesmenu.size = 7;

  creditesmenu.mainmenu[0].setFillColor(Color::White);
  creditesmenu.mainmenu[0].setString("OMAR EL-EZABY");

  creditesmenu.mainmenu[1].setFillColor(Color::White);
  creditesmenu.mainmenu[1].setString("OMAR WATANY");

  creditesmenu.mainmenu[2].setFillColor(Color::White);
  creditesmenu.mainmenu[2].setString("OMAR EL-HAKIM");

  creditesmenu.mainmenu[3].setFillColor(Color::White);
  creditesmenu.mainmenu[3].setString("OMAR TEBRY");

  creditesmenu.mainmenu[4].setFillColor(Color::White);
  creditesmenu.mainmenu[4].setString("OMAR IBRAHIM");

  creditesmenu.mainmenu[5].setFillColor(Color::White);
  creditesmenu.mainmenu[5].setString("AHMED ALI");

  creditesmenu.mainmenu[6].setFillColor(Color::White);
  creditesmenu.mainmenu[6].setString("KAREEM ABDEEN");

  creditesmenu.setTextFont();
  creditesmenu.setcharsize(90);
  creditesmenu.setTextPosition(100);
  creditesmenu.setbackgroud(creditesmenu.background, creditesmenu.bg);

  backbtm.setsprites("assets/backbtm.png");
  backbtm.sprite.setPosition(50 * (height / 1210), 50 * (height / 850));

  while (creditesmenu.isActive) {
    Event event;
    while (window.pollEvent(event)) {
      if (event.type == Event::Closed) {
        window.close();
        break;
      } else if (event.type == Event::KeyReleased) {
        if (event.key.code == Keyboard::Escape) {
          window.clear();
          creditesmenu.isActive = false;
          menu.isActive = true;
          drawMenu(window);
        }
      }
      if (Mouse::isButtonPressed(Mouse::Button::Left)) {
        if (backbtm.hover(Mouse::getPosition(window).x,
                          Mouse::getPosition(window).y)) {
          window.clear();
          creditesmenu.isActive = false;
          menu.isActive = true;
          drawMenu(window);
        }
      }
    }
    window.clear();
    window.draw(creditesmenu.bg);
    creditesmenu.draw(window);
    window.draw(backbtm.sprite);
    window.display();
  }
}

void toggle(bool &var) {
  if (var) {
    var = false;
  } else {
    var = true;
  }
}
