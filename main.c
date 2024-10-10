#include "raylib.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BODIES 128
#define WORLD_HEIGHT 600;
#define WORLD_WIDTH 600

typedef struct Body {
  float mass;
  Vector2 position;
} Body;

typedef struct Node {
  uint32_t children; // this id and the next 3
  float mass;
  Vector2 centerOfMass;
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

DynamicArrayNode *newDynamicArrayUnInt() {
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

struct QuadTree {
  DynamicArrayNode *nodes;
};

int main() {
  const int nBodies = 3;
  Body bodies[MAX_BODIES];

  for (int i = 0; i < nBodies; ++i) {
    bodies[i] = ( Body ){ 1, {20 * (i + 1), 20 * (i + 1)} };
  }

  DynamicArrayNode *arr = newDynamicArrayUnInt();

  for (int i = 0; i < nBodies; ++i) {
      printf("Body %d: %f %f\n", i, bodies[i].position.x, bodies[i].position.y);
  }

  freeDynamicArray(arr);
  return 0;
}
