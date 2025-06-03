/*************************************************************************
    > File Name: /home/lhj/Projects/LearnC/1.Demo/29.xml_json_yaml/demos/json_demo.cpp
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 27 May 2025 03:21:54 PM CST
 ************************************************************************/

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main()
{
    json j;
    j["name"] = "Alice";
    j["age"] = 30;
    j["skills"] = {"C++", "Python", "JSON"};

    std::ofstream("data.json") << j.dump(4);

    std::ifstream in("data.json");
    json j2;
    in >> j2;
    std::cout << "Name: " << j2["name"] << "\n";

    j2["age"] = 31;
    j2.erase("skills");
    j2["email"] = "alice@example.com";

    std::ofstream("data_modified.json") << j2.dump(4);
    return 0;
}
