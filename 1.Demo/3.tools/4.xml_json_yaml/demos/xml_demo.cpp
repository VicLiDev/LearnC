/*************************************************************************
    > File Name: xml_demo.cpp
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 27 May 2025 03:22:42 PM CST
 ************************************************************************/

#include <iostream>
#include <tinyxml2.h>

using namespace tinyxml2;

int main()
{
    XMLDocument doc;
    XMLNode* root = doc.NewElement("Person");
    doc.InsertFirstChild(root);

    XMLElement* name = doc.NewElement("Name");
    name->SetText("Alice");
    root->InsertEndChild(name);

    XMLElement* age = doc.NewElement("Age");
    age->SetText(30);
    root->InsertEndChild(age);

    doc.SaveFile("person.xml");

    doc.LoadFile("person.xml");
    XMLElement* ageElement = doc.FirstChildElement("Person")->FirstChildElement("Age");
    ageElement->SetText(31);

    XMLElement* person = doc.FirstChildElement("Person");
    person->DeleteChild(person->FirstChildElement("Name"));

    XMLElement* email = doc.NewElement("Email");
    email->SetText("alice@example.com");
    person->InsertEndChild(email);

    doc.SaveFile("person_modified.xml");
    return 0;
}
