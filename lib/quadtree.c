#include <inttypes.h>
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
  uint32_t children; // this id and the next 3 in the array are children
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

Node initNode(Rectangle square, uint32_t next) {
  return (Node){
      0,
      next,
      0.0,
      (Vector2){-1, -1},
      (Vector2){square.x + square.width / 2, square.y + square.height / 2},
      square};
}

void printNode(Node node, uint32_t nodeID) {
  printf("Node %d: Center of mass %f %f, mass: %f children: %d ", nodeID,
         node.centerOfMass.x, node.centerOfMass.y, node.mass, node.children);
  printf("Square: x: %f, y: %f, width: %f, height: %f\n", node.Squeare.x,
         node.Squeare.y, node.Squeare.width, node.Squeare.height);
}

DynamicArrayNode *newDynamicArrayNode() {
  const uint32_t initialCapacity = 1024;
  DynamicArrayNode *arr = malloc(sizeof(DynamicArrayNode));
  arr->elements = malloc(initialCapacity * sizeof(Node));

  arr->length = 0;
  arr->capacity = initialCapacity;
  return arr;
}

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

void freeDynamicArray(DynamicArrayNode *array) {
  free(array->elements);
  free(array);
}

void printTree(QuadTree *tree) {
  for (uint32_t i = 0; i < tree->nodes->length; ++i) {
    Node node = tree->nodes->elements[i];
    printNode(node, i);
  }
}

bool isLeaf(Node node) { return node.children == 0; }

bool isEmpty(Node node) { return node.mass == 0; }

void freeTree(QuadTree *tree) {
  freeDynamicArray(tree->nodes);
  free(tree);
}

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
