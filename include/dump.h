#ifndef DUMP_H
#define DUMP_H

#include "differenciator.h"

const char* const OUTPUT_DOT = "dump.dot";
const char* const OUTPUT_PNG = "tree.png";
const char* const OUTPUT_TXT = "output.txt";
const char* const OUTPUT_TEX = "output.tex";

const char* const PHRAZES[] =
{
    "Я бы посчитал это в уме, но вспомнил, кому считаю \n",
    "В советское время даже зародыши знали, что получается: \n",
    "Нетрудно видеть, что: \n",
    "Методом пристального взгляда получаем: \n",
    "Используя опыт и смекалку можно догадаться, что: \n",
    "Заметим, что: \n"
};

void TexDumpDerivative (Node* node, Node* diff_node, const char* phraze);
void TexDumpNode (Node* node, Node* main_node, FILE* file);
void TexDumpBegin ();
void TexDumpEnd   ();
void GraphDump    (Node* node);

const char* GetRandomPhraze ();

#endif
