#include "raylib.h"
#include "raymath.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BODIES 128
#define WORLD_HEIGHT 600
#define WORLD_WIDTH 600

typedef struct Body {
  float mass;
  Vector2 position;
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

DynamicArrayNode *addElement(DynamicArrayNode *array, Node element) {
  const uint8_t capacityMultiplier = 2;
  if (array->length == array->capacity) {
    array->elements = realloc(array->elements, sizeof(Node) * array->capacity *
                                                   capacityMultiplier);
    array->capacity *= capacityMultiplier;
  }

  array->elements[array->length] = element;
  array->length += 1;

  return array;
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

bool isLeaf(Node node) { return node.children == 0; }

bool isEmpty(Node node) { return node.mass == 0; }

Node initNode(Rectangle square) {
  return (Node){
      0, 0, (Vector2){-1, -1},
      (Vector2){square.x + square.width / 2, square.y + square.height / 2},
      square};
}

uint32_t subdivide(QuadTree *tree, uint32_t nodeID) {
  // sanity check
  Node *currentNode = &tree->nodes->elements[nodeID];
  if (!isEmpty(*currentNode) || !isLeaf(*currentNode)) {
    printf("Sanity check failed: isEmpty: %d; isLeaf: %d",
           isEmpty(*currentNode), isLeaf(*currentNode));
  }
  uint32_t childrenID = tree->nodes->length;

  int leftWidth = currentNode->Squeare.width / 2;
  int rightWidth = currentNode->Squeare.width - leftWidth;
  int topHeight = currentNode->Squeare.height / 2;
  int bottomHeight = currentNode->Squeare.height - topHeight;

  Node topLeft = initNode((Rectangle){currentNode->Squeare.x, currentNode->Squeare.y, leftWidth, topHeight});
  Node topRight = initNode((Rectangle){currentNode->Squeare.x + leftWidth, currentNode->Squeare.y, rightWidth, topHeight});
  Node bottomLeft = initNode((Rectangle){currentNode->Squeare.x, currentNode->Squeare.y + topHeight, leftWidth, bottomHeight});
  Node bottomRight = initNode((Rectangle){currentNode->Squeare.x + leftWidth, currentNode->Squeare.y + topHeight, rightWidth, bottomHeight});
  tree->nodes = addElement(tree->nodes, topLeft);
  tree->nodes = addElement(tree->nodes, topRight);
  tree->nodes = addElement(tree->nodes, bottomLeft);
  tree->nodes = addElement(tree->nodes, bottomRight);

  currentNode->children = childrenID;
  return childrenID;
}

QuadTree *buildTree(Body *bodies, uint32_t nBodies) {
  QuadTree *tree = malloc(sizeof(QuadTree));
  tree->nodes = newDynamicArrayNode();
  Node firstNode =
      (Node){0, 0, (Vector2){-1, -1},
             (Vector2){(float)WORLD_WIDTH / 2, (float)WORLD_HEIGHT / 2},
             (Rectangle){0, 0, WORLD_WIDTH, WORLD_HEIGHT}};

  tree->nodes = addElement(tree->nodes, firstNode);

  for (int bodyID = 0; bodyID < nBodies; ++bodyID) {
    int nodeID = 0;
    while (0 == 0) {
      Node *currentNode = &tree->nodes->elements[nodeID];
      if (isLeaf(*currentNode)) {
        if (isEmpty(*currentNode)) {
          currentNode->mass = bodies[bodyID].mass;
          currentNode->centerOfMass = bodies[bodyID].position;
        } else {
          uint32_t childrenID = subdivide(tree, nodeID);
          // is it correct?
          float newMass = currentNode->mass + bodies[bodyID].mass;
          Vector2 scaledOrginal =
              Vector2Scale(currentNode->centerOfMass, currentNode->mass);
          Vector2 scaledNew =
              Vector2Scale(bodies[bodyID].position, bodies[bodyID].mass);
          Vector2 newCenterOfMass =
              Vector2Scale(Vector2Add(scaledNew, scaledOrginal), 1 / newMass);
          currentNode->mass = newMass;
          currentNode->centerOfMass = newCenterOfMass;
        }
      }
    }
  }
}

int main() {
  const int nBodies = 3;
  Body bodies[MAX_BODIES];

  for (int i = 0; i < nBodies; ++i) {
    bodies[i] = (Body){1, {20 * (i + 1), 20 * (i + 1)}};
  }

  DynamicArrayNode *arr = newDynamicArrayNode();

  for (int i = 0; i < nBodies; ++i) {
    printf("Body %d: %f %f\n", i, bodies[i].position.x, bodies[i].position.y);
  }

  freeDynamicArray(arr);
  return 0;
}
