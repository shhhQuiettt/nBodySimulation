#include "lib/barneshut.h"
#include "lib/quadtree.h"

#include "raylib.h"
#include <inttypes.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

#define WORLD_HEIGHT 3000
#define WORLD_WIDTH 3000

#define FRAME_WIDTH 1000
#define FRAME_HEIGHT 1000

#define TARGET_FPS 60

#define N_BODIES 3

int main() {
  const uint32_t nBodies = N_BODIES;
  Body *bodies = malloc(nBodies * sizeof(Body));

  /* SetRandomSeed(time(NULL)); */
  SetRandomSeed(45);

  const uint32_t massLowerBound = 10;
  const uint32_t massUpperBound = 50;

  for (uint32_t i = 0; i < nBodies - 1; ++i) {
    float mass = GetRandomValue(massLowerBound, massUpperBound);
    float x = GetRandomValue(WORLD_WIDTH / 2 - FRAME_WIDTH / 2,
                             WORLD_WIDTH / 2 + FRAME_WIDTH / 2);
    float y = GetRandomValue(WORLD_HEIGHT / 2 - FRAME_HEIGHT / 2,
                             WORLD_HEIGHT / 2 + FRAME_HEIGHT / 2);

    float vx = GetRandomValue(-25, 25);
    float vy = GetRandomValue(-25, 25);

    bodies[i] =
        (Body){mass, (Vector2){x, y}, (Vector2){vx, vy}, (Vector2){0, 0}};
  }

  bodies[nBodies - 1] = (Body){
      100000,
      (Vector2){WORLD_WIDTH/2.0, WORLD_HEIGHT/2.0},
      (Vector2){0, 0},
  };

  InitWindow(FRAME_WIDTH, FRAME_HEIGHT, "N body simulaion");
  Camera2D camera = {0};
  camera.zoom = 1.0f;

  SetTargetFPS(TARGET_FPS);
  while (!WindowShouldClose()) {
    /* printf("%fl\rn", GetFrameTime()); */

    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
      Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
      camera.offset = GetMousePosition();
      camera.target = mouseWorldPos;

      float scaleFactor = 1.0f + (0.25f * fabs(wheel));
      if (wheel < 0)
        scaleFactor = 1.0f / scaleFactor;

      camera.zoom = Clamp(camera.zoom * scaleFactor, 0.125f, 64.0f);
    }
    QuadTree *tree = buildTree(bodies, nBodies, WORLD_WIDTH, WORLD_HEIGHT);
    BeginDrawing();
    ClearBackground(BLACK);
    BeginMode2D(camera);

    for (uint32_t i = 0; i < nBodies; ++i) {
      DrawCircleV(bodies[i].position,
                  bodies[i].mass <= massUpperBound
                      ? 1 + 2 * (bodies[i].mass - massLowerBound) /
                                (massUpperBound - massLowerBound)
                      : 12,
                  WHITE);
      /* DrawPixelV(bodies[i].position, WHITE); */
      // Small text of bodyid
    }
    /* drawVelocities(bodies, nBodies); */
    /* drawAcceleration(bodies, nBodies); */

    updateAcceleration(bodies, tree, nBodies, 1000);
    EndDrawing();
    for (uint32_t i = 0; i < nBodies; ++i) {
      if (bodies[i].position.x < 0 || bodies[i].position.x > WORLD_WIDTH ||
          bodies[i].position.y < 0 || bodies[i].position.y > WORLD_HEIGHT) {
        // dumb solution for now
        perror("Body out of bounds\n");
        printf("Body %d out of bounds\n", i);
        printf("Position: %f %f\n", bodies[i].position.x, bodies[i].position.y);
        printf("Velocity: %f %f\n", bodies[i].velocity.x, bodies[i].velocity.y);
        printf("Acceleration: %f %f\n", bodies[i].acceleration.x,
               bodies[i].acceleration.y);
        printf("Mass: %f\n", bodies[i].mass);
        printf("Velocity magnitude %f\n", Vector2Length(bodies[i].velocity));
        exit(1);
      }
    }
    updateVelocitiesAndPositions(bodies, nBodies);

    freeTree(tree);
  }

  return 0;
}
