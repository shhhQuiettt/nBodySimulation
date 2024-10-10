#include "raylib.h"
#include "raymath.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_BODIES 10000
#define WORLD_HEIGHT 600
#define WORLD_WIDTH 600

typedef struct Body {
  float mass;
  Vector2 position;
  Vector2 velocity;
  Vector2 acceleration;
} Body;

typedef struct Node {
  uint32_t children; // this id and the next 3
  float mass;
  Vector2 centerOfMass;
  Vector2 SquereCenter;
  Rectangle Squeare;
} Node;

typedef struct DynamicArrayNode {
  Node *elements;
  uint32_t length;
  uint32_t capacity;
} DynamicArrayNode;

void addElement(DynamicArrayNode **array, Node element) {
  const uint8_t capacityMultiplier = 2;
  if ((*array)->length == (*array)->capacity) {
    (*array)->elements =
        realloc((*array)->elements,
                sizeof(Node) * (*array)->capacity * capacityMultiplier);
    (*array)->capacity *= capacityMultiplier;
  }

  (*array)->elements[(*array)->length] = element;
  (*array)->length += 1;
}

DynamicArrayNode *newDynamicArrayNode() {
  const uint32_t initialCapacity = 256;
  DynamicArrayNode *arr = malloc(sizeof(DynamicArrayNode));
  arr->elements = malloc(initialCapacity * sizeof(Node));

  arr->length = 0;
  arr->capacity = initialCapacity;
  return arr;
}

void freeDynamicArray(DynamicArrayNode *array) {
  free(array->elements);
  free(array);
}

typedef struct QuadTree {
  DynamicArrayNode *nodes;
} QuadTree;

void printNode(Node node, uint32_t nodeID) {
  printf("Node %d: Center of mass %f %f, mass: %f children: %d ", nodeID,
         node.centerOfMass.x, node.centerOfMass.y, node.mass, node.children);
  printf("Square: x: %f, y: %f, width: %f, height: %f\n", node.Squeare.x,
         node.Squeare.y, node.Squeare.width, node.Squeare.height);
}

void printTree(QuadTree *tree) {
  for (uint32_t i = 0; i < tree->nodes->length; ++i) {
    Node node = tree->nodes->elements[i];
    printNode(node, i);
  }
}

bool isLeaf(Node node) { return node.children == 0; }

bool isEmpty(Node node) { return node.mass == 0; }

enum CHILD_OFFSET {
  TOP_LEFT = 0,
  TOP_RIGHT = 1,
  BOTTOM_LEFT = 2,
  BOTTOM_RIGHT = 3
};

enum CHILD_OFFSET childOffset(Vector2 position, Vector2 parentCenter) {
  if (position.x < parentCenter.x) {
    if (position.y < parentCenter.y) {
      return TOP_LEFT;
    } else {
      return BOTTOM_LEFT;
    }
  } else {
    if (position.y < parentCenter.y) {
      return TOP_RIGHT;
    } else {
      return BOTTOM_RIGHT;
    }
  }
}

Node initNode(Rectangle square) {
  return (Node){
      0, 0, (Vector2){-1, -1},
      (Vector2){square.x + square.width / 2, square.y + square.height / 2},
      square};
}

uint32_t subdivide(QuadTree *tree, uint32_t nodeID) {
  /* printf("Subdividing node %d\n", nodeID); */

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

  Node topLeft = initNode((Rectangle){
      currentNode->Squeare.x, currentNode->Squeare.y, leftWidth, topHeight});
  Node topRight =
      initNode((Rectangle){currentNode->Squeare.x + leftWidth,
                           currentNode->Squeare.y, rightWidth, topHeight});
  Node bottomLeft = initNode((Rectangle){currentNode->Squeare.x,
                                         currentNode->Squeare.y + topHeight,
                                         leftWidth, bottomHeight});
  Node bottomRight = initNode((Rectangle){currentNode->Squeare.x + leftWidth,
                                          currentNode->Squeare.y + topHeight,
                                          rightWidth, bottomHeight});

  addElement(&tree->nodes, topLeft);
  addElement(&tree->nodes, topRight);
  addElement(&tree->nodes, bottomLeft);
  addElement(&tree->nodes, bottomRight);

  return childrenID;
}

QuadTree *buildTree(Body *bodies, uint32_t nBodies) {
  QuadTree *tree = malloc(sizeof(QuadTree));
  tree->nodes = newDynamicArrayNode();
  Node firstNode =
      (Node){0, 0, (Vector2){-1, -1},
             (Vector2){(float)WORLD_WIDTH / 2, (float)WORLD_HEIGHT / 2},
             (Rectangle){0, 0, WORLD_WIDTH, WORLD_HEIGHT}};

  addElement(&tree->nodes, firstNode);

  for (uint32_t bodyID = 0; bodyID < nBodies; ++bodyID) {
      /* printf("Body %d\n", bodyID); */
    /* bool bodyAdded = false; */
    /* printTree(tree); */
    /* printf("\n"); */
    int nodeID = 0;
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

int main() {
  const int nBodies = 5000;
  Body bodies[MAX_BODIES];

  /* SetRandomSeed(time(NULL)); */
  SetRandomSeed(423);
  for (int i = 0; i < nBodies; ++i) {
    /* float mass = GetRandomValue(1, 10); */
    float mass = 5;
    float x = GetRandomValue(0, WORLD_WIDTH);
    float y = GetRandomValue(0, WORLD_HEIGHT);

    bodies[i] = (Body){mass, (Vector2){x, y}, (Vector2){0, 0}, (Vector2){0, 0}};
  }

  QuadTree *tree = buildTree(bodies, nBodies);
  printTree(tree);

  /* printTree(tree); */
  InitWindow(WORLD_WIDTH, WORLD_HEIGHT, "N body simulaion");
  SetTargetFPS(27);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    for (int i = 0; i < nBodies; ++i) {
      DrawCircleV(bodies[i].position, 2, WHITE);
        //Small text of bodyid
    }

    /* for (int i = 0; i < tree->nodes->length; ++i) { */
    /*   Node node = tree->nodes->elements[i]; */
    /*   DrawRectangleLinesEx(node.Squeare, 1, RED); */
    /* } */
    EndDrawing();
  }

  /* for (kint i = 0; i < nBodies; ++i) { */
  /*   printf("Body %d: %f %f\n", i, bodies[i].position.x,
   * bodies[i].position.y); */
  /* } */

  return 0;
}
