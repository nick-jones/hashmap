#include <stdlib.h>
#include <check.h>
#include "../src/hashmap.h"
 
START_TEST(test_hashmap_create) {
  HashMap *map = hashmap_create(5);

  ck_assert_int_eq(map->capacity, 5);
  ck_assert_int_eq(map->size, 0);

  hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_get) {
  HashMap *map = hashmap_create(5);
  const char *key = "foo";
  const char *value = "bar";
  unsigned int retrieved;
  const char *result;

  hashmap_put(map, key, value);
  retrieved = hashmap_get(map, key, &result);

  ck_assert_str_eq(result, value);
  ck_assert_int_eq(retrieved, 1);

  hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_contains) {
  HashMap *map = hashmap_create(5);

  hashmap_put(map, "foo", "bar");
  
  ck_assert_int_eq(hashmap_contains(map, "foo"), 1);
  ck_assert_int_eq(hashmap_contains(map, "baz"), 0);

  hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_locate_entry) {
  HashMap *map = hashmap_create(5);
  const char *key = "foo";
  const char *value = "bar";

  hashmap_put(map, key, value);
  Entry *entry = hashmap_locate_entry(map, key);

  ck_assert_str_eq(entry->key, key);
  ck_assert_str_eq(entry->value, value);

  hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_put) {
  HashMap *map = hashmap_create(5);
  const char *key = "foo";
  const char *value = "bar";
  const char *value_replacement = "baz";
  const char *result;  

  ck_assert_int_eq(hashmap_put(map, key, value), 1);
  ck_assert_int_eq(hashmap_get(map, key, &result), 1);
  ck_assert_str_eq(result, value);
  ck_assert_int_eq(map->size, 1);
  
  ck_assert_int_eq(hashmap_put(map, key, value_replacement), 1);
  ck_assert_int_eq(hashmap_get(map, key, &result), 1);
  ck_assert_str_eq(result, value_replacement);
  ck_assert_int_eq(map->size, 1);
  
  hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_entry_for_key) {
  HashMap *map = hashmap_create(5);
  Entry *entry;
  const char *key = "foo";

  hashmap_put(map, key, "bar");

  // Existing entry
  entry = hashmap_entry_for_key(map, key);
  ck_assert_str_eq(entry->key, key);

  // Expecting a new entry
  entry = hashmap_entry_for_key(map, "baz");
  ck_assert(entry->key == NULL);
  
  hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_create_entry) {
  Entry *entry;

  entry = hashmap_create_entry();
  ck_assert(entry->key == NULL);
  ck_assert(entry->value == NULL);
  ck_assert(entry->next == NULL);

  hashmap_destroy_entry(entry);
}
END_TEST

START_TEST(test_hashmap_index) {
  HashMap *map = hashmap_create(5);

  ck_assert_int_eq(hashmap_index(map, "a"), 0);
  ck_assert_int_eq(hashmap_index(map, "b"), 1);
  ck_assert_int_eq(hashmap_index(map, "c"), 2);
  ck_assert_int_eq(hashmap_index(map, "d"), 3);
  ck_assert_int_eq(hashmap_index(map, "e"), 4);
  ck_assert_int_eq(hashmap_index(map, "f"), 0);

  hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_hash) {
  ck_assert_int_eq(hashmap_hash("a"), 177670);
  // Consistency check..
  ck_assert_int_eq(hashmap_hash("a"), 177670);
}
END_TEST

START_TEST(test_hashmap_replace_entry_value) {
  Entry *entry;

  entry = hashmap_create_entry();
  hashmap_populate_entry(entry, "foo", "bar");
  hashmap_replace_entry_value(entry, "baz");

  ck_assert_str_eq(entry->value, "baz");

  hashmap_destroy_entry(entry);
}
END_TEST

START_TEST(test_hashmap_populate_entry) {
  Entry *entry;
  const char *key = "foo";
  const char *value = "bar";

  entry = hashmap_create_entry();
  hashmap_populate_entry(entry, key, value);

  ck_assert_str_eq(entry->key, key);
  ck_assert_str_eq(entry->value, value);
  ck_assert(entry->next == NULL);

  hashmap_destroy_entry(entry);
}
END_TEST

START_TEST(test_hashmap_remove) {
  HashMap *map = hashmap_create(5);
  const char *key = "foo";
  unsigned int result;

  result = hashmap_remove(map, key);
  ck_assert_int_eq(result, 0);

  hashmap_put(map, key, "bar");
  ck_assert_int_eq(hashmap_contains(map, key), 1);
  ck_assert_int_eq(map->size, 1);  

  result = hashmap_remove(map, key);
  ck_assert_int_eq(result, 1);
  ck_assert_int_eq(hashmap_contains(map, key), 0);  
  ck_assert_int_eq(map->size, 0);

  hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_clear) {
  HashMap *map = hashmap_create(5);

  hashmap_put(map, "foo", "bar");
  hashmap_put(map, "baz", "boo");

  hashmap_clear(map);

  ck_assert_int_eq(map->size, 0);
  ck_assert_int_eq(hashmap_contains(map, "foo"), 0);
  ck_assert_int_eq(hashmap_contains(map, "baz"), 0);

  hashmap_destroy(map);
}
END_TEST

START_TEST(test_hashmap_clear_index) {
  HashMap *map = hashmap_create(5);

  hashmap_put(map, "a", "test");
  hashmap_clear_index(map, 0);

  ck_assert_int_eq(map->size, 0);
  ck_assert_int_eq(hashmap_contains(map, "a"), 0);

  hashmap_destroy(map);
}
END_TEST

Suite * hashmap_suite(void) {
  Suite *s = suite_create("HashMap");

  TCase *tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_hashmap_create);
  tcase_add_test(tc_core, test_hashmap_get);
  tcase_add_test(tc_core, test_hashmap_contains);
  tcase_add_test(tc_core, test_hashmap_locate_entry);
  tcase_add_test(tc_core, test_hashmap_put);
  tcase_add_test(tc_core, test_hashmap_entry_for_key);
  tcase_add_test(tc_core, test_hashmap_create_entry);
  tcase_add_test(tc_core, test_hashmap_index);
  tcase_add_test(tc_core, test_hashmap_hash);
  tcase_add_test(tc_core, test_hashmap_replace_entry_value);
  tcase_add_test(tc_core, test_hashmap_populate_entry);
  tcase_add_test(tc_core, test_hashmap_remove);
  tcase_add_test(tc_core, test_hashmap_clear);
  tcase_add_test(tc_core, test_hashmap_clear_index);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s = hashmap_suite();
  SRunner *sr = srunner_create(s);
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
