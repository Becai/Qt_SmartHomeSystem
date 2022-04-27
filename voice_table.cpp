#include "voice_table.h"

const int table_size = 12;
VoiceTable::VoiceTable() {
  this->table = new table_node_t[table_size];
  table[0] = table_node_t{"吃什么", "我不到啊"};
  table[1] = table_node_t{"", ""};
  table[2] = table_node_t{"", ""};
  table[3] = table_node_t{"", ""};
  table[4] = table_node_t{"", ""};
  table[5] = table_node_t{"", ""};
  table[6] = table_node_t{"", ""};
  table[7] = table_node_t{"", ""};
  table[8] = table_node_t{"", ""};
  table[9] = table_node_t{"", ""};
  table[10] = table_node_t{"", ""};
  table[11] = table_node_t{"", ""};
}

QString VoiceTable::query(QString str) {
  for (int i = 0; i < table_size; i++) {
    QString key = this->table[i].key;
    if (str.contains(key)) {
      return table[i].ans;
    }
  }
  return "";
}
