/*************************************************************************
    > File Name: demos/json_demo.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 27 May 2025 03:21:04 PM CST
 ************************************************************************/

#include <jansson.h>
#include <stdio.h>

int main()
{
    json_t *root = json_object();
    json_object_set_new(root, "name", json_string("Alice"));
    json_object_set_new(root, "age", json_integer(30));

    json_t *skills = json_array();
    json_array_append_new(skills, json_string("C"));
    json_array_append_new(skills, json_string("JSON"));
    json_object_set_new(root, "skills", skills);

    json_dump_file(root, "data.json", JSON_INDENT(4));

    json_error_t error;
    json_t *read_root = json_load_file("data.json", 0, &error);
    json_t *name = json_object_get(read_root, "name");
    printf("Name: %s\n", json_string_value(name));

    json_object_set_new(read_root, "age", json_integer(31));
    json_object_del(read_root, "skills");
    json_object_set_new(read_root, "email", json_string("alice@example.com"));

    json_dump_file(read_root, "data_modified.json", JSON_INDENT(4));
    json_decref(read_root);
    return 0;
}
