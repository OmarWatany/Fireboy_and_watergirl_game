#include <SFML/Audio.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Mouse.hpp>
// define some functions
using namespace sf;
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
void drawWinningMenu(RenderWindow &window, int time);
void drawCreditesMenu(RenderWindow &window, Music &menumusic);
void toggle(bool &var);
void level1(RenderWindow &window);
