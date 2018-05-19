#include "nt-parser/load-raw-sent.h"

#include <cassert>
#include <fstream>

#include "cnn/dict.h"
#include "nt-parser/compressed-fstream.h"

using namespace std;

namespace parser {


RawOracle::~RawOracle() {}

inline bool is_ws(char x) { //check whether the character is a space or tab delimiter
  return (x == ' ' || x == '\t');
}

inline bool is_not_ws(char x) {
  return (x != ' ' && x != '\t');
}

void RawOracle::ReadSentenceView(const std::string& line, cnn::Dict* dict, vector<int>* sent) {
  unsigned cur = 0;
  while(cur < line.size()) {
    while(cur < line.size() && is_ws(line[cur])) { ++cur; }
    unsigned start = cur;
    while(cur < line.size() && is_not_ws(line[cur])) { ++cur; }
    unsigned end = cur;
    if (end > start) {
      unsigned x = dict->Convert(line.substr(start, end - start));
      sent->push_back(x);
    }
  }
  assert(sent->size() > 0); // empty sentences not allowed
}

void TopDownRawOracle::load_oracle(const string& file, bool is_training) {
  cerr << "Loading top-down oracle from " << file << " [" << (is_training ? "training" : "non-training") << "] ...\n";
  cnn::compressed_ifstream in(file.c_str());
  assert(in);
  while(getline(in, line)) {
    ++lc;
    //cerr << "line number = " << lc << endl;
    cur_acts.clear();
    if (line.size() == 0 || line[0] == '#') continue;
    sents.resize(sents.size() + 1);
    auto& cur_sent = sents.back();
    ReadSentenceView(line, pd, &cur_sent.pos);
    getline(in, line);
    ReadSentenceView(line, d, &cur_sent.raw);
    lc += 3;
  }
  cerr << "Loaded " << sents.size() << " sentences\n";
  cerr << "    cumulative      action vocab size: " << ad->size() << endl;
  cerr << "    cumulative    terminal vocab size: " << d->size() << endl;
  cerr << "    cumulative nonterminal vocab size: " << nd->size() << endl;
  cerr << "    cumulative         pos vocab size: " << pd->size() << endl;
}
} // namespace parser
