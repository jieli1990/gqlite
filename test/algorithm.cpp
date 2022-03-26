#include "operand/SimilarityOp.h"
#include "SubGraph.h"
#include <fstream>
#include <fmt/printf.h>
#include <chrono>
#include <fmt/color.h>
#define ASSERT_EQ(result, bool_result) \
  {\
    auto start = std::chrono::high_resolution_clock::now();\
    if ((result) != (bool_result)) { \
      auto elapsed = std::chrono::high_resolution_clock::now() - start;\
      auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()/1000.0;\
      fmt::print(fg(fmt::color::red), "ASSERT_EQ: " #result " is not " #bool_result "\t\tcost {}s\n", microseconds); \
      ret = -1;\
    }else{\
      auto elapsed = std::chrono::high_resolution_clock::now() - start;\
      auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()/1000.0;\
      fmt::print("ASSERT_EQ: " #result " is passed\t\t\t\tcost {}s\n", microseconds);\
    }\
  }

#define ASSERT_EQ_PERF(result, bool_result, times, limit_time)
    

char* ltrim(char* str) {
  while (isspace(*str)) ++str;
  return str;
}

std::string fisrt_vertex(char* start) {
  char* begin = start;
  while (!isspace(*start) && *start != '-') ++start;
  return std::string(begin, start);
}

std::string last_vertex(char* end) {
  char* offset = end;
  while (!isspace(*end) && *end != '-') --end;
  return std::string(end + 1, offset);
}

#define MAX_READ_SIZE 512
GSubGraph* createGraph(const char* dotfile) {
  std::ifstream fs;
  fs.open(dotfile);
  GSubGraph* g = new GSubGraph();
  char line[MAX_READ_SIZE] = { 0 };
  while (fs.getline(line, MAX_READ_SIZE)) {
    char* end = strchr(line, ';');
    if (end != nullptr) {
      char* start = ltrim(line);
      std::string from = fisrt_vertex(start);
      g->addVertex(from);
      std::string to = last_vertex(end);
      g->addVertex(to);
      GraphEdgeDirection direct = GraphEdgeDirection::Bidrection;
      if (strstr(line, "->")) {
        direct = GraphEdgeDirection::To;
      }
      g->addEdge(from, to, direct);
    }
    memset(line, 0, MAX_READ_SIZE);
  }
  fs.close();
  return g;
}

void releaseGraph(GSubGraph*g) {
  delete g;
}

int main()
{
  int ret = 0;
  GSubGraph* g1 = createGraph("./test/graphs/simple_g.dot");
  GSubGraph* gs = createGraph("./test/graphs/simple_g.dot");
  ASSERT_EQ(*g1==*gs, true);
  GSubGraph* g2 = createGraph("./test/graphs/g4.dot");
  GSubGraph* g3 = createGraph("./test/graphs/simple_g_2.dot");
  GSubGraph* g4 = createGraph("./test/graphs/g4_2.dot");
  GSubGraph* g5 = createGraph("./test/graphs/simple_g_3.dot");
  ASSERT_EQ(distance(g1, g2)==1, true);
  releaseGraph(g1);
  releaseGraph(g2);
  releaseGraph(g3);
  releaseGraph(g4);
  releaseGraph(g5);
  return ret;
}