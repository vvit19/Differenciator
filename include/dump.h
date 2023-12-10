#ifndef DUMP_H
#define DUMP_H

#include "differenciator.h"

const char* const OUTPUT_DOT     = "data/dump.dot";
const char* const OUTPUT_TREE    = "data/tree.png";
const char* const OUTPUT_TEX     = "data/output.tex";
const char* const OUTPUT_GPI     = "data/output.gpi";
const char* const OUTPUT_TAYLOR  = "data/taylor.png";
const char* const OUTPUT_TANGENT = "data/tangent.png";

const char* const PHRAZES[] =
{
    "Я бы посчитал это в уме, но вспомнил, кому считаю \\\\ \n",
    "В советское время даже зародыши знали, что получается: \\\\ \n",
    "Нетрудно видеть, что: \\\\ \n",
    "Методом пристального взгляда получаем: \\\\ \n",
    "Используя опыт и смекалку можно догадаться, что: \\\\ \n",
    "Заметим, что: \\\\ \n",
    "РТ в этом году набрал умственно отсталых первокурсников \\\\ \n"
};

void TexDump           (Node* node_1, Node* node_2, const char* phraze, bool dump_derivative);
void TaylorGraphic     (Node* node_1, Node* node_2, double x);
void TangentGraphic    (Node* node,   Node* diff,   double x);
void DumpNode          (Node* node, Node* main_node, FILE* file, bool tex_dump);
void TexDumpBegin      ();
void TexDumpEnd        ();
void GraphDump         (Node* node);

const char* GetRandomPhraze ();

#endif
