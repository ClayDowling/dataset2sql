#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

void table_sql(cJSON *rows, const char *table);
void row_sql(cJSON *row, const char *table);
void dataset_walk(cJSON *document);

int main(int argc, char **argv) {
  char *filename = "local.json";
  FILE *src = NULL;

  if (argc > 1) {
    filename = argv[1];
  }

  struct stat sb;
  if (stat(filename, &sb)) {
    perror(filename);
    return 1;
  }

  src = fopen(filename, "r");
  if (NULL == src) {
    perror(filename);
    return 1;
  }
  char *buffer = calloc(1, sb.st_size + 1);
  fread((void *)buffer, sb.st_size, 1, src);
  fclose(src);

  cJSON *doc = cJSON_Parse(buffer);
  dataset_walk(doc);

  free(buffer);

  return 0;
}

void dataset_walk(cJSON *first) {
  for (cJSON *cur = first->child; cur != NULL; cur = cur->next) {
    const char *tablename = cur->string;
    table_sql(cur, tablename);
  }
}

void table_sql(cJSON *rows, const char *table) {
  int rowcount = cJSON_GetArraySize(rows);
  cJSON *row = NULL;
  for (int i = 0; i < rowcount; ++i) {
    row = cJSON_GetArrayItem(rows, i);
    row_sql(row, table);
  }
}

void row_sql(cJSON *row, const char *table) {
  printf("INSERT INTO [%s] (", table);
  for (cJSON *cur = row->child; cur != NULL; cur = cur->next) {
    if (cur != row->child) {
      printf(", ");
    }
    printf("%s", cur->string);
  }
  printf(") VALUES (");
  for (cJSON *cur = row->child; cur != NULL; cur = cur->next) {
    if (cur != row->child) {
      printf(", ");
    }
    if (cJSON_IsNumber(cur)) {
      if (cur->valuedouble == cur->valueint) {
        printf("%d", cur->valueint);
      } else {
        printf("%f", cur->valuedouble);
      }
    } else if (cJSON_IsNull(cur)) {
      printf("NULL");
    } else {
      printf("'%s'", cJSON_GetStringValue(cur));
    }
  }
  printf(");\n");
}