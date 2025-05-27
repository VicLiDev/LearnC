/*************************************************************************
    > File Name: xml_demo.c
    > Author: LiHongjin
    > Mail: 872648180@qq.com
    > Created Time: Tue 27 May 2025 03:22:15 PM CST
 ************************************************************************/

#include <libxml/parser.h>
#include <libxml/tree.h>

int main()
{
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root = xmlNewNode(NULL, BAD_CAST "Person");
    xmlDocSetRootElement(doc, root);

    xmlNewChild(root, NULL, BAD_CAST "Name", BAD_CAST "Alice");
    xmlNewChild(root, NULL, BAD_CAST "Age", BAD_CAST "30");

    xmlSaveFormatFileEnc("person.xml", doc, "UTF-8", 1);

    doc = xmlReadFile("person.xml", NULL, 0);
    root = xmlDocGetRootElement(doc);
    xmlNodePtr node = root->children;
    while (node) {
        if (!xmlStrcmp(node->name, BAD_CAST "Name")) {
            xmlUnlinkNode(node);
            xmlFreeNode(node);
            break;
        }
        node = node->next;
    }
    xmlNewChild(root, NULL, BAD_CAST "Email", BAD_CAST "alice@example.com");
    xmlSaveFormatFileEnc("person_modified.xml", doc, "UTF-8", 1);

    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}
