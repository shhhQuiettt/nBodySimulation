#include "quadtree.h"
#include <inttypes.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct Body {
  float mass;
  Vector2 position;
  Vector2 velocity;
  Vector2 acceleration;
} Body;

uint32_t subdivide(QuadTree *tree, uint32_t nodeID);

QuadTree *buildTree(Body *bodies, uint32_t nBodies, float worldWidth,
                    float worldHeight);


void updateAcceleration(Body *bodies, QuadTree *gravityTree, uint32_t nBody,
                        float maxSizeDistanceQuotient);

void updateVelocitiesAndPositions(Body *bodies, uint32_t nBody);

void drawVelocities(Body *bodies, uint32_t nBody);
void drawAcceleration(Body *bodies, uint32_t nBody);
