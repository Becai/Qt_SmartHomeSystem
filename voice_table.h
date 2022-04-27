#ifndef VOICE_TABLE_H
#define VOICE_TABLE_H

#include <QString>
typedef struct {
  QString key;  //关键词
  QString ans;  //回复语句
} table_node_t;

class VoiceTable {
 public:
  VoiceTable();
  QString query(QString);

 private:
  table_node_t *table;
};

#endif  // VOICE_TABLE_H
