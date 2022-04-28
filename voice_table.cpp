#include "voice_table.h"

const int table_size = 4;
VoiceTable::VoiceTable() {
  this->table = new table_node_t[table_size];
  table[0] = table_node_t{"吃什么", "我不到啊"};
  table[1] = table_node_t{"天气", "有你在的每一天都是晴天捏"};
  table[2] = table_node_t{
      "笑话",
      "我领着一带生蚝走在路上，突然生蚝掉进了泥土里，原来这就是，蚝喜欢你"};
  table[3] = table_node_t{"你好", "主人，你好"};
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
