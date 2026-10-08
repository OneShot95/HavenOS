#pragma once

#include <QVector>
#include <QWidget>

class QComboBox;
class QFrame;
class QLabel;
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
  QLabel *m_weatherStatus = nullptr;
  QLabel *m_weatherDetail = nullptr;
  QComboBox *m_weatherLook = nullptr;
  DayBand m_band = DayBand::Night;
  WeatherLook m_weather = WeatherLook::Off;
  int m_previewHour = -1;
};
