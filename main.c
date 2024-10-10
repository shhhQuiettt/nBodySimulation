#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"


typedef struct Node {
    uint32_t children; // this id and the next 3
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
    array->elements = realloc(array->elements, sizeof(Node) * array->capacity * capacityMultiplier);
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

struct QuadTree {
    DynamicArrayNode* nodes;
};

int main() {
  DynamicArrayNode *arr = newDynamicArrayUnInt();
  Node  n = {0, {1, 1, 1, 1}};

  addElement(arr, n);

  printf("Length: %d\n", arr->length);
  printf("Capacity: %d\n", arr->capacity);
  printf("Element: %d\n", arr->elements[0].children);
  printf("Element: %f\n", arr->elements[0].Squeare.x);

  return 0;
}
