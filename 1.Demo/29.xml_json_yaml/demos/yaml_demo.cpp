/*************************************************************************
    > File Name: yaml_demo.cpp
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 27 May 2025 03:23:32 PM CST
 ************************************************************************/

#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>

int main()
{
    YAML::Node node;
    node["name"] = "Alice";
    node["age"] = 30;

    YAML::Node skills;
    skills.push_back("C++");
    skills.push_back("Python");
    skills.push_back("YAML");
    node["skills"] = skills;

    std::ofstream fout("data.yaml");
    fout << node;

    YAML::Node readNode = YAML::LoadFile("data.yaml");

    if (readNode["name"].IsScalar()) {
        std::cout << "Name: " << readNode["name"].as<std::string>() << "\n";
    }

    readNode["age"] = 31;
    readNode.remove("skills");
    readNode["email"] = "alice@example.com";

    std::ofstream fout2("data_modified.yaml");
    fout2 << readNode;

    return 0;
}

