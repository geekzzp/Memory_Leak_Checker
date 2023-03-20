#ifndef CS100_MEMCHECK_H
#define CS100_MEMCHECK_H 1

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_RECORDS 1000

struct Record
{
  void *ptr;             // address of the allocated memory
  size_t size;           // size of the allocated memory
  int line_no;           // line number, at which a call to malloc or calloc happens
  const char *file_name; // name of the file, in which the call to malloc or calloc happens
};

struct Record records[MAX_RECORDS];
int num = 0;
void *recorded_malloc(size_t size, int line, const char *file)
{
  void *ptr = malloc(size);
  if (ptr != NULL)
  {
    records[++num].ptr = ptr;
    records[num].size = size;
    records[num].line_no = line;
    records[num].file_name = file;
    // YOUR CODE HERE
  }
  return ptr;
}

void *recorded_calloc(size_t cnt, size_t each_size, int line, const char *file)
{
  void *ptr = calloc(cnt, each_size);
  if (ptr != NULL)
  {
    records[++num].ptr = ptr;
    records[num].size = cnt * each_size;
    records[num].line_no = line;
    records[num].file_name = file;
    // YOUR CODE HERE
  }
  return ptr;
}

void recorded_free(void *ptr, int line, const char *file)
{
  if(ptr==NULL) return;
  int tag = 0;
  for (int i = 1; i <= num; i++)
  {
    if (records[i].ptr == ptr)
    {
      records[i].ptr = NULL;
      free(ptr);
      tag = 1;
      break;
    }
  }
  if (!tag)
  {
    printf("Invalid free in file %s, line %d\n", file, line);
  }
  // YOUR CODE HERE
}

void check_leak(void) __attribute__((destructor));

void check_leak(void)
{
  printf("Summary:\n");
  size_t tot = 0;
  int count = 0;
  for (int i = 1; i <= num; i++)
  {
    if (records[i].ptr != NULL)
    {
      printf("%lu bytes memory not freed (allocated in file %s, line %d)\n", records[i].size, records[i].file_name, records[i].line_no);
      tot += records[i].size;
      count++;
    }
  }
  if (tot)
  {
    printf("%d allocation records not freed (%lu bytes in total).\n", count, tot);
  }
  else
  {
    printf("All allocations have been freed.\n");
  }
  // YOUR CODE HERE
}

#define malloc(SIZE) recorded_malloc(SIZE, __LINE__, __FILE__)
#define calloc(CNT, EACH_SIZE) recorded_calloc(CNT, EACH_SIZE, __LINE__, __FILE__)
#define free(PTR) recorded_free(PTR, __LINE__, __FILE__)

#endif // CS100_MEMCHECK_H
