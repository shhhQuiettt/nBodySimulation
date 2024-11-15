#include "quadtree.h"
#include <inttypes.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define GRAVITATIONAL_CONSTANT 6.67430e0

typedef struct Body {
  float mass;
  Vector2 position;
  Vector2 velocity;
  Vector2 acceleration;
} Body;

uint32_t subdivide(QuadTree *tree, uint32_t nodeID) {
  // sanity check
  if (nodeID >= tree->nodes->length) {
    printf("Subdivide: Node id %d is out of bounds\n", nodeID);
    exit(1);
  }
  Node *currentNode = &tree->nodes->elements[nodeID];
  if (isEmpty(*currentNode) || !isLeaf(*currentNode)) {
    printf("Sanity check failed: isEmpty: %d; isLeaf: %d",
           isEmpty(*currentNode), isLeaf(*currentNode));
    exit(1);
  }
  uint32_t childrenID = tree->nodes->length;
  currentNode->children = childrenID;

  int leftWidth = currentNode->Squeare.width / 2;
  int rightWidth = currentNode->Squeare.width - leftWidth;
  int topHeight = currentNode->Squeare.height / 2;
  int bottomHeight = currentNode->Squeare.height - topHeight;

  Node topLeft =
      initNode((Rectangle){currentNode->Squeare.x, currentNode->Squeare.y,
                           leftWidth, topHeight},
               childrenID + TOP_RIGHT);
  Node topRight =
      initNode((Rectangle){currentNode->Squeare.x + leftWidth,
                           currentNode->Squeare.y, rightWidth, topHeight},
               childrenID + BOTTOM_LEFT);
  Node bottomLeft = initNode((Rectangle){currentNode->Squeare.x,
                                         currentNode->Squeare.y + topHeight,
                                         leftWidth, bottomHeight},
                             childrenID + BOTTOM_RIGHT);
  Node bottomRight = initNode((Rectangle){currentNode->Squeare.x + leftWidth,
                                          currentNode->Squeare.y + topHeight,
                                          rightWidth, bottomHeight},
                              currentNode->next);

  addElement(&tree->nodes, topLeft);
  addElement(&tree->nodes, topRight);
  addElement(&tree->nodes, bottomLeft);
  addElement(&tree->nodes, bottomRight);

  return childrenID;
}

QuadTree *buildTree(Body *bodies, uint32_t nBodies, float worldWidth,
                    float worldHeight) {
  QuadTree *tree = malloc(sizeof(QuadTree));
  tree->nodes = newDynamicArrayNode();
  Node firstNode = (Node){0,
                          0,
                          0.0,
                          (Vector2){-1, -1},
                          (Vector2){worldWidth / 2, worldHeight / 2},
                          (Rectangle){0, 0, worldWidth, worldHeight}};

  addElement(&tree->nodes, firstNode);

  for (uint32_t bodyID = 0; bodyID < nBodies; ++bodyID) {
    /* bool bodyAdded = false; */
    /* printTree(tree); */
    /* printf("\n"); */
    uint32_t nodeID = 0;
    while (0 == 0) {
      if (nodeID >= tree->nodes->length) {
        printf("Node id %d is out of bounds\n", nodeID);
        exit(1);
      }
      Node *currentNode = &tree->nodes->elements[nodeID];
      /* printf("Body %d, node %d\n", bodyID, nodeID); */
      if (isLeaf(*currentNode)) {
        if (isEmpty(*currentNode)) {
          currentNode->mass = bodies[bodyID].mass;
          currentNode->centerOfMass = bodies[bodyID].position;
          break;
        } else {
          // edge case: two bodies have the same position
          if (Vector2DistanceSqr(bodies[bodyID].position,
                                 currentNode->centerOfMass) < 1.0f) {
            currentNode->mass += bodies[bodyID].mass;
            break;
          }
          uint32_t childrenID = subdivide(tree, nodeID);
          currentNode = &tree->nodes->elements[nodeID];
          currentNode->children = childrenID;

          // move the current node mass to the new children because it is one
          // particle
          int oldCenterOfMassOffset =
              childOffset(currentNode->centerOfMass, currentNode->SquereCenter);
          tree->nodes->elements[childrenID + oldCenterOfMassOffset].mass =
              currentNode->mass;
          tree->nodes->elements[childrenID + oldCenterOfMassOffset]
              .centerOfMass = currentNode->centerOfMass;

          // is it correct?
          //
          float newParentMass = currentNode->mass + bodies[bodyID].mass;
          Vector2 scaledOrginalCenterOfMass =
              Vector2Scale(currentNode->centerOfMass, currentNode->mass);
          Vector2 scaledBodyCenterOfMass =
              Vector2Scale(bodies[bodyID].position, bodies[bodyID].mass);
          Vector2 newParentCenterOfMass = Vector2Scale(
              Vector2Add(scaledBodyCenterOfMass, scaledOrginalCenterOfMass),
              1 / newParentMass);
          currentNode->mass = newParentMass;
          currentNode->centerOfMass = newParentCenterOfMass;

          int offset =
              childOffset(bodies[bodyID].position, currentNode->SquereCenter);

          nodeID = childrenID + offset;
          continue;
        }
      } else {
        int offset =
            childOffset(bodies[bodyID].position, currentNode->SquereCenter);
        nodeID = currentNode->children + offset;
        continue;
      }
    }
  }
  return tree;
}

void updateAcceleration(Body *bodies, QuadTree *gravityTree, uint32_t nBody,
                        float maxSizeDistanceQuotient) {
  for (uint32_t bodyId = 0; bodyId < nBody; ++bodyId) {

    bodies[bodyId].acceleration = (Vector2){0.0, 0.0};

    uint32_t nodeId = 0;
    do {
      Node *currentNode = &gravityTree->nodes->elements[nodeId];
      float distance =
          Vector2Distance(bodies[bodyId].position, currentNode->centerOfMass);

      if (distance < 5) {
        distance = 0;
      }

      float size = currentNode->Squeare.width;
      if (!isLeaf(*currentNode)) {
        /* if (distance / size < maxSizeDistanceQuotient &&
         * !isLeaf(*currentNode)) { */
        nodeId = currentNode->children;
        continue;
      }

      if (currentNode->mass > 0 && distance > 0) {
        /* Vector2 accDirection = */
        /*     Vector2Subtract(currentNode->centerOfMass,
         * bodies[bodyId].position); */

        /* float accMagnitude = currentNode->mass / ((distance * distance)); */
        /* /1* printf("acc direction: %f %f Magnitude: %f\n", accDirection.x,
         * *1/ */
        /* /1*        accDirection.y, accMagnitude); *1/ */
        /* Vector2 acceleration = Vector2Scale(accDirection, accMagnitude); */
        Vector2 accDirection = Vector2Normalize(Vector2Subtract(
            currentNode->centerOfMass, bodies[bodyId].position));
        float accMagnitude = currentNode->mass / (distance * distance);
        Vector2 acceleration = Vector2Scale(accDirection, accMagnitude);

        bodies[bodyId].acceleration =
            Vector2Add(bodies[bodyId].acceleration, acceleration);
      }

      nodeId = currentNode->next;

    } while (nodeId != 0);

    bodies[bodyId].acceleration =
        Vector2Scale(bodies[bodyId].acceleration, GRAVITATIONAL_CONSTANT);
  }
}

void updateVelocitiesAndPositions(Body *bodies, uint32_t nBody) {
  for (uint32_t bodyId = 0; bodyId < nBody; ++bodyId) {
    bodies[bodyId].velocity =
        Vector2Add(bodies[bodyId].velocity,
                   Vector2Scale(bodies[bodyId].acceleration, GetFrameTime()));

    bodies[bodyId].position =
        Vector2Add(bodies[bodyId].position,
                   Vector2Scale(bodies[bodyId].velocity, GetFrameTime()));
  }
}

void drawVelocities(Body *bodies, uint32_t nBody) {
  for (uint32_t bodyId = 0; bodyId < nBody; ++bodyId) {
    DrawLineV(bodies[bodyId].position,
              Vector2Add(bodies[bodyId].position,
                         Vector2Scale(bodies[bodyId].velocity, 1)),
              RED);
    printf("Magnitude of velocity of body %d: %f\n", bodyId,
           Vector2Length(bodies[bodyId].velocity));
  }
  printf("\n");
}

void drawAcceleration(Body *bodies, uint32_t nBody) {
  for (uint32_t bodyId = 0; bodyId < nBody; ++bodyId) {
    DrawLineV(bodies[bodyId].position,
              Vector2Add(bodies[bodyId].position,
                         Vector2Scale(bodies[bodyId].acceleration, 1)),
              YELLOW);
  }
}
