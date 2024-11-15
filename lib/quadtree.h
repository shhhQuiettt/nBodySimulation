#ifndef QUADTREE_H
#define QUADTREE_H

#include <inttypes.h>
#include <raylib.h>
#include <raymath.h>

typedef struct Node {
  uint32_t children; // this id and the next 3
  uint32_t next;     // next Node in acceleration update
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

typedef struct QuadTree {
  DynamicArrayNode *nodes;
} QuadTree;

enum CHILD_OFFSET {
  TOP_LEFT = 0,
  TOP_RIGHT = 1,
  BOTTOM_LEFT = 2,
  BOTTOM_RIGHT = 3
};

Node initNode(Rectangle square, uint32_t next);

void printNode(Node node, uint32_t nodeID);

DynamicArrayNode *newDynamicArrayNode();

void addElement(DynamicArrayNode **array, Node element);

void freeDynamicArray(DynamicArrayNode *array);

void printTree(QuadTree *tree);

void freeTree(QuadTree *tree);

bool isLeaf(Node node);

bool isEmpty(Node node);

enum CHILD_OFFSET childOffset(Vector2 position, Vector2 parentCenter);

#endif
