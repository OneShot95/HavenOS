#include "Profile.h"

#include <QSettings>

#include <functional>

namespace {

void withStore(const std::function<void(QSettings &)> &use) {
  const QByteArray path = qgetenv("HAVEN_SETTINGS_FILE");
  if (!path.isEmpty()) {
    QSettings store(QString::fromLocal8Bit(path), QSettings::IniFormat);
    use(store);
    return;
  }
  QSettings store(QSettings::IniFormat, QSettings::UserScope, QStringLiteral("HavenOS"),
                  QStringLiteral("shell"));
  use(store);
}

}  // namespace

QStringList Profile::rooms() {
  return {QStringLiteral("Kitchen"),       QStringLiteral("Living Room"), QStringLiteral("Bedroom"),
          QStringLiteral("Hallway"),       QStringLiteral("Office"),      QStringLiteral("Other")};
}

bool Profile::isComplete() const {
  return rooms().contains(room) && !homeName.trimmed().isEmpty();
}

Profile Profile::load() {
  Profile profile;
  withStore([&profile](QSettings &store) {
    profile.homeName = store.value(QStringLiteral("homeName")).toString().trimmed();
    profile.room = store.value(QStringLiteral("room")).toString().trimmed();
  });
  return profile;
}

void Profile::save() const {
  const QString name = homeName.trimmed();
  const QString roomName = room.trimmed();
  withStore([&name, &roomName](QSettings &store) {
    store.setValue(QStringLiteral("homeName"), name);
    store.setValue(QStringLiteral("room"), roomName);
    store.sync();
  });
}

QStringList Profile::loadShopping() {
  QStringList items;
  withStore([&items](QSettings &store) {
    items = store.value(QStringLiteral("shopping")).toStringList();
  });
  QStringList cleaned;
  for (const QString &item : items) {
    const QString trimmed = item.trimmed();
    if (!trimmed.isEmpty()) {
      cleaned.push_back(trimmed);
    }
  }
  return cleaned;
}

void Profile::saveShopping(const QStringList &items) {
  QStringList cleaned;
  for (const QString &item : items) {
    const QString trimmed = item.trimmed();
    if (!trimmed.isEmpty()) {
      cleaned.push_back(trimmed);
    }
  }
  withStore([&cleaned](QSettings &store) {
    store.setValue(QStringLiteral("shopping"), cleaned);
    store.sync();
  });
}
