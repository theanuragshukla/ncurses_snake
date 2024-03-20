#include <cstdlib>
#include <ncurses.h>
#include <unistd.h>

#define MAX_SCORE 256
typedef struct {
  int x;
  int y;
} vec2;

class Snake {

public:

  Snake() {
    init();
  }

  void init(){
    initscr();
    scr_width = 40;
    scr_height = 20;
    win = initscr();
    curs_set(0);
    noecho();
    nodelay(win, true);
    keypad(win, true);

    head = {2, 2};
    dir = {1, 0};
    score = 0;
    highscore = 0;
    apple = {rand()%scr_width, rand()%scr_height};
    skip = false;
    state = RUNNING;
  }

  void start(){
    state = RUNNING;
    gameLoop();
  }


private:
  enum STATE {RUNNING, PAUSED, GAMEOVER,  TERMINATE};
  int scr_width;
  int scr_height;
  WINDOW *win;
  vec2 head;
  vec2 dir;
  vec2 segments[MAX_SCORE + 1];
  int score;
  vec2 apple;
  bool skip;
  STATE state;
  int highscore;
  char key;



  void draw_border(int y, int x, int width, int height) {
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + width * 2 + 1, ACS_URCORNER);
    for (int i = 1; i < width * 2 + 1; i++) {
      mvaddch(y, x + i, ACS_HLINE);
    }
    for (int i = 1; i < height + 1; i++) {
      mvaddch(y + i, x, ACS_VLINE);
      mvaddch(y + i, x + width * 2 + 1, ACS_VLINE);
    }
    mvaddch(y + height + 1, x, ACS_LLCORNER);
    mvaddch(y + height + 1, x + width * 2 + 1, ACS_LRCORNER);
    for (int i = 1; i < width * 2 + 1; i++) {
      mvaddch(y + height + 1, x + i, ACS_HLINE);
    }
  }

  void input(){
    int ch = wgetch(win);
    key = ch;
    if (skip == true) {
      skip = false;
      return;
    }
    switch (ch) {
      case KEY_UP:
        if(dir.y == 1){
          skip=true;
          break;
        }
        dir.y = -1;
        dir.x = 0;
        break;
      case KEY_DOWN:
        if(dir.y == -1){
          skip=true;
          break;
        }
        dir.y = 1;
        dir.x = 0;
        break;
      case KEY_LEFT:
        if(dir.x == 1){
          skip=true;
          break;
        }
        dir.y = 0;
        dir.x = -1;
        break;
      case KEY_RIGHT:
        if(dir.x == -1){
          skip=true;
          break;
        }
        dir.y = 0;
        dir.x = 1;
        break;
      case '\e':
        if(state== RUNNING){
          state=PAUSED;
        }
        break;
      case 'q':
        if(state!=RUNNING){
          exitGame();
        }
        break;
      case ' ': 
        if(state == PAUSED){
          resume();
        } else if(state == RUNNING) {
          pause();
        }
        break;
      case 'r':
        if(state==GAMEOVER || state==PAUSED){
          restart();
        }
        break;
    }
  }
  void update(){
    for (int i = score; i > 0; i--) {
      segments[i] = segments[i - 1];
    }
    segments[0] = head;
    head.y += dir.y;
    head.x += dir.x;
    if(head.x == apple.x && head.y == apple.y){
      score++;
      apple.x = rand()%scr_width;
      apple.y = rand()%scr_height;
    }
  }

  void draw_score(){
    mvprintw(0, 1, "|Score: %d | Highscore: %d |", score, highscore);
  }

  void draw(){
    erase();
    draw_border(0, 0, scr_width, scr_height);
    draw_score();
    mvaddch(head.y, head.x*2, 'O');
    mvaddch(apple.y, apple.x*2, '@');
    for(int i = 0; i<score; i++){
      mvaddch(segments[i].y, segments[i].x*2, 'o');
    }
  }

  void pauseGame(){
    erase();
    draw_border(0, 0, scr_width, scr_height);
    draw_score();
    mvprintw(scr_height/2, scr_width/2, "Game Paused");
    mvprintw(scr_height/2 + 1, scr_width/2, "Press 'space' to resume");
    mvprintw(scr_height/2 + 2, scr_width/2, "Press 'r' to restart");
    mvprintw(scr_height/2 + 3, scr_width/2, "Press 'q' to quit");
    refresh();
  }

  void gameLoop(){
    while(true){
      input();
      if(state == PAUSED){
        pauseGame();
        continue;
      }       
      if(state == GAMEOVER){
        printGameOver();
        continue;
      }
      if(state == TERMINATE){
        break;
      }
      update();
      detectCollision();
      draw();
      usleep(100000);
    }
    endwin();
  }

  void restart(){
    init();
    start();
  }

  void exitGame(){
    state = TERMINATE;
  }
  void resume(){
    state = RUNNING;
  }

  void pause(){
    state = PAUSED;
  }

  void gameOver(){
    state = GAMEOVER;
  }

  void printGameOver(){
    erase();
    draw_border(0, 0, scr_width, scr_height);
    draw_score();
    if(score > highscore){
      highscore = score;
    }
    mvprintw(scr_height/2, scr_width/2, "Game Over");
    mvprintw(scr_height/2 + 1, scr_width/2, "Press 'r' to restart");
    mvprintw(scr_height/2 + 2, scr_width/2, "Press 'q' to quit");
    refresh();
  }

  void detectCollision(){
    if(head.x <= 0 || head.x > scr_width || head.y <= 0 || head.y > scr_height){
      gameOver();
    }
    for(int i = 0; i<score; i++){
      if(head.x == segments[i].x && head.y == segments[i].y){
        gameOver();
      }
    }
  }

};

