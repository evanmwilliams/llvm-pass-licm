#include <stdio.h>

void dummy_decl();

void dummy_decl()
{
  printf("Dummy decl\n");
}

// Function to test basic loop invariant
void basic_invariant_test()
{
  int x = 5;
  int y = 10;
  int result = 0;

  for (int i = 0; i < 100; i++)
  {
    result += x * y;
  }

  printf("Basic Invariant Test Result: %d\n", result);
}

// Function to test invariant involving arrays
void array_invariant_test()
{
  int arr[100];
  int factor = 7;

  for (int i = 0; i < 100; i++)
  {
    arr[i] = i * factor + 2 * 3;
  }

  printf("Array Invariant Test:\n");
  for (int i = 0; i < 100; i++)
  {
    printf("%d ", arr[i]);
  }
  printf("\n");
}

// Function to test nested loops with invariant
void nested_loop_invariant_test()
{
  int result = 0;
  int invariant_val = 4 + 5;

  for (int i = 0; i < 50; i++)
  {
    for (int j = 0; j < 50; j++)
    {
      result += invariant_val;
    }
  }

  printf("Nested Loop Invariant Test Result: %d\n", result);
}

// Function to test mixed invariant and non-invariant expressions
void mixed_invariant_test()
{
  int result = 0;
  int x = 5;
  int y = 10;
  int invariant_val = 7 + 3;

  for (int i = 0; i < 100; i++)
  {
    result += x * i + invariant_val + y * 3;
  }

  printf("Mixed Invariant Test Result: %d\n", result);
}

int main()
{
  dummy_decl();
  basic_invariant_test();
  array_invariant_test();
  nested_loop_invariant_test();
  mixed_invariant_test();

  return 0;
}
