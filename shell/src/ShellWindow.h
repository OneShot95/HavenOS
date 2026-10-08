#pragma once

#include "Profile.h"

#include <QStringList>
#include <QVector>
#include <QWidget>

class QComboBox;
class QFrame;
class QLabel;
class QLineEdit;
class QPainter;
class QPaintEvent;
class QPushButton;
class QStackedWidget;
class QVBoxLayout;

class ShellWindow : public QWidget {
public:
  enum class DayBand { Night, Dawn, Morning, Afternoon, Dusk };
  enum class WeatherLook { Off, Clear, Cloudy, Rain, Snow, Fog, Storm };

  explicit ShellWindow(QWidget *parent = nullptr);

protected:
  void paintEvent(QPaintEvent *event) override;

private:

  void updateClock();
  void refreshScene();
  void setWeatherLook(WeatherLook look);
  int effectiveHour() const;
  void setPage(int index);
  QWidget *buildBanner();
  QWidget *buildNav();
  QWidget *buildHome();
  QWidget *buildApps();
  QWidget *buildSettings();
  QWidget *buildPower();
  QWidget *buildTimer();
  QWidget *buildStore();
  QWidget *buildShopping();
  QWidget *buildRecipes();
  QWidget *buildWizard();
  void refreshShopping();
  void refreshLocalLine();
  void addShoppingItem();
  void removeShoppingItem(int index);
  void loadRoute();
  void applyProfile();
  void showWizard();
  void showShell();
  void finishWizard();
  void refreshTimerUi();
  void setTimerDuration(int seconds);
  void startTimer();
  void pauseTimer();
  void cancelTimer();
  QFrame *makeSlot(const QString &title, const QString &detail, QLabel **statusOut = nullptr,
                   QLabel **detailOut = nullptr);
  QFrame *makeStatusRow(const QString &name);
  QPushButton *makeNavButton(const QString &text);
  QVBoxLayout *beginPage(QWidget *page, const QString &heading);
  void readPreviewHooks();
  void paintLiftedShadows(QPainter &painter);

  QStackedWidget *m_stack = nullptr;
  QVector<QPushButton *> m_nav;
  QLabel *m_clock = nullptr;
  QLabel *m_seconds = nullptr;
  QLabel *m_clockNote = nullptr;
  QLabel *m_weekday = nullptr;
  QLabel *m_date = nullptr;
  QLabel *m_homeValue = nullptr;
  QLabel *m_roomValue = nullptr;
  QLabel *m_weatherStatus = nullptr;
  QLabel *m_weatherDetail = nullptr;
  QLabel *m_homeLocal = nullptr;
  QComboBox *m_weatherLook = nullptr;
  QStackedWidget *m_body = nullptr;
  QStackedWidget *m_wizard = nullptr;
  QLineEdit *m_wizardName = nullptr;
  QPushButton *m_roomNext = nullptr;
  QLineEdit *m_settingsName = nullptr;
  QComboBox *m_settingsRoom = nullptr;
  QVector<QPushButton *> m_roomButtons;
  QString m_wizardRoom;
  QLabel *m_timerDigits = nullptr;
  QLabel *m_timerState = nullptr;
  QPushButton *m_timerStart = nullptr;
  QPushButton *m_timerPause = nullptr;
  QVBoxLayout *m_shoppingRows = nullptr;
  QLineEdit *m_shoppingEntry = nullptr;
  QStringList m_shopping;
  int m_timerRemaining = 300;
  int m_timerDuration = 300;
  bool m_timerRunning = false;
  Profile m_profile;
  DayBand m_band = DayBand::Night;
  WeatherLook m_weather = WeatherLook::Off;
  int m_previewHour = -1;
};
