#include <stdio.h>
#include "gfx.h"
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

int wid = 800, ht = 800;
int planet_number_per_screen = 10;
float lift = -10;
float gravity = 0.5f;
//generate random size circles with different colors moving along the screen with speed set that updates when needed


typedef struct {
  int xc, yc;
  int radius;
  float speed;
  int r, g, b;

} Planet;

typedef struct {

    int x, y;
    int r;
    float vy;

} Star;

/*typedef struct {

    int x, y;
    int w, h;
    float vy;

} Rocket;

typedef struct {

    int xc, yc;
    int w, h;
    float speed;
    float r,g,b;

} Obstacle;*/

void init_window(int, int);
int circles_collision(int, int, int, int, int, int);
void generateCircles(Planet p[]);
void gfx_star(int, int, int);
int check_collision(Star *, Planet *);
void player_movement(Star *);
int game_mode1(Star *player, Planet p[]);
//void generateObs(Obstacle p[10]);

int main(){
    srand(time(NULL));
    int planet_number_per_screen = 10;
    init_window(wid, ht);

    time_t start_time;
    time_t current_time;
    double elapsed_time = 0;

    Star player;
    Planet p[planet_number_per_screen];
    generateCircles(p);

    player.x = 50;
    player.y = ht / 2;
    player.r = 15;
    player.vy = 0;

    int game_over = 0;
    int running = 1;
    player.y = ht / 2;
    player.r = 15;
    player.vy = 0;

    start_time = time(NULL);

    while(running){

        gfx_clear();
        if (gfx_event_waiting()) {
            char c = gfx_wait();

            if (c == 'q') {
                running = 0;
            }

            if (c == 'r') {
                // restart game
                generateCircles(p);
                player.x = 50;
                player.y = ht / 2;
                player.vy = 0;
                game_over = 0;
                start_time = time(NULL);

            }

            if (!game_over && (c == 'u' || c == 'U')) {
                player.vy = lift;
            }
        }

        if (!game_over) {
            current_time = time(NULL);
            elapsed_time = (int)difftime(current_time, start_time);
            game_over = game_mode1(&player, p);
        } else {
            // optional "game over" message
            gfx_color(255, 255, 255);
            gfx_text(wid / 2 - 60, ht / 2, "GAME OVER");
            gfx_text(wid / 2 - 90, ht / 2 + 20, "Press r to restard or q to quit");
        }
        char score_text[50];
        sprintf(score_text, "Score: %.1f s", elapsed_time);

        gfx_color(255, 255, 255);
        gfx_text(20, 30, score_text);

        if (game_over) {
         char final_score[50];
         sprintf(final_score, "Final Score (time): %.1f s", elapsed_time);
         gfx_text(wid/2 - 80, ht/2 + 50, final_score);
       }



        gfx_flush();
        usleep(16000);


   }
   return 0;
}


int game_mode1(Star *player, Planet p[]){

    player->vy += gravity;
    player->y += player->vy;
    player->x += 1;

    if (player->x - player->r > wid) player->x = 0;

    if (player->y < player->r){ player->y = player->r; player->vy = 0;}
    if (player->y > ht - player->r) { player->y = ht - player->r; player->vy = 0; }

    gfx_star(player->x, player->y, player->r);


        for (int i = 0; i<planet_number_per_screen; i++){

            p[i].xc += p[i].speed;
            if (p[i].xc - p[i].radius > wid){

                p[i].xc = -p[i].radius;
                p[i].yc = p[i].radius + rand() % (ht - 2 * p[i].radius);        
            }

            gfx_color(p[i].r, p[i].g, p[i].b);
            gfx_circle(p[i].xc, p[i].yc, p[i].radius);

            if (check_collision(player, &p[i])) {
                return 1;

            }
            if (player->y >= ht - player->r || player->y <= player->r)return 1; }

       return 0;

}

void init_window(int w, int h){

    gfx_open(w, h, "game.c");
    gfx_wait();

}

int circles_collision(int x1, int y1, int r1, int x2, int y2, int r2){

    int dx = x1 - x2;
    int dy = y1 - y2;
    int dist = dx*dx + dy*dy;
    int rad = (r1 + r2) * (r1 + r2);
    return dist < rad;

}


void generateCircles(Planet p[10]){


    for (int i = 0; i < planet_number_per_screen; i++){

        int valid = 0;

        while(!valid){


            p[i].radius = 10 + rand() % 40;
            p[i].xc = p[i].radius + rand() % (wid - 2 * p[i].radius);
            p[i].yc = p[i].radius + rand() % (ht - 2 * p[i].radius);
            p[i].speed = 1 + rand() % 3;

            p[i].r = rand() % 256;
            p[i].g = rand() % 256;
            p[i].b = rand() % 256;

            valid = 1;
            for (int j = 0; j<i; j++){
                if (circles_collision(p[i].xc, p[i].yc, p[i].radius, p[j].xc, p[j].yc, p[j].radius)){

                    valid = 0;
                    break;

                }

            }


        }

   }
}


void gfx_star(int xc, int yc, int r) {
    // A 5-point star is drawn by connecting every 2nd vertex of a pentagon.
    double angle_deg = -90;  // top point facing up
    double step = 72;        // 360/5

    int x[5], y[5];

    // Compute the 5 points of star
    for (int i = 0; i < 5; i++) {
        double rad = (angle_deg + step * i) * M_PI / 180.0;
        x[i] = xc + r * cos(rad);
        y[i] = yc + r * sin(rad);
    }

    // Connect every 2nd point (star pattern)
    for (int i = 0; i < 5; i++) {
        int j = (i + 2) % 5;
        gfx_line(x[i], y[i], x[j], y[j]);
    }
}

void player_movement(Star *player){

        if (!gfx_event_waiting())return;

            char input = gfx_wait();

            if (input == 'u' || input == 'U'){

                player->vy = lift;
               // if (p) p->vy = lift;
            }

            else if (input == 'q'){
                exit(0);
            }


}

int check_collision(Star *player, Planet *p){

    int dx = p->xc - player->x;
    int dy = p->yc - player->y;

    int distance = dx*dx + dy*dy;
    int rad = p->radius + player->r;

    return (distance < rad * rad);

 }

