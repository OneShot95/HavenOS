#pragma once

#include <QString>
#include <QStringList>

struct Profile {
  QString homeName;
  QString room;

  bool isComplete() const;
  static QStringList rooms();
  static Profile load();
  void save() const;

  static QStringList loadShopping();
  static void saveShopping(const QStringList &items);
};
