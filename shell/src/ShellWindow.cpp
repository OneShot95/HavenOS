#include "ShellWindow.h"

#include <QComboBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QDateTime>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QLinearGradient>
#include <QList>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QPushButton>
#include <QSignalBlocker>
#include <QStackedWidget>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

namespace {

constexpr auto kBannerLine1 = "Desktop prototype — not running on the tablet.";
constexpr auto kBannerLine2 = "UI prototype, not a flashable OS image.";

struct ScenePalette {
  QColor top;
  QColor high;
  QColor horizon;
  QColor ground;
  QColor body;
  QColor glow;
};

QFont interFont(int pixelSize, QFont::Weight weight = QFont::Normal) {
  QFont font;
  font.setFamilies({QStringLiteral("Inter"), QStringLiteral("DejaVu Sans")});
  font.setPixelSize(pixelSize);
  font.setWeight(weight);
  return font;
}

QString pageStyle() {
  return QStringLiteral(
      "QWidget { background-color: transparent; color: #f4efe6; }"
      "QLabel { background: transparent; }"
      "QWidget#banner {"
      "  background-color: rgba(28, 24, 20, 242);"
      "  border: 1px solid rgba(244, 239, 230, 48);"
      "  border-radius: 999px;"
      "}"
      "QWidget#nav { background: transparent; border: none; }"
      "QWidget#nav QPushButton { min-width: 0px; padding: 0 16px; }"
      "QFrame#heroScrim {"
      "  background-color: rgba(10, 12, 16, 228);"
      "  border: 1px solid rgba(244, 239, 230, 50);"
      "  border-radius: 36px;"
      "}"
      "QFrame#slot {"
      "  background-color: rgba(32, 28, 24, 242);"
      "  border: 1px solid rgba(244, 239, 230, 48);"
      "  border-radius: 36px;"
      "}"
      "QLabel#statusLine {"
      "  background-color: rgba(12, 14, 18, 230);"
      "  color: #f4efe6;"
      "  border-radius: 999px;"
      "  padding: 10px 22px;"
      "}"
      "QPushButton {"
      "  background-color: rgba(36, 31, 26, 242);"
      "  color: #f4efe6;"
      "  border: 1px solid rgba(244, 239, 230, 48);"
      "  border-radius: 999px;"
      "  min-height: 64px;"
      "  min-width: 148px;"
      "  padding: 0 28px;"
      "}"
      "QPushButton:pressed { background-color: #3a332b; }"
      "QPushButton[active=\"true\"] {"
      "  background-color: #342a20;"
      "  border: 1px solid #d08a45;"
      "  color: #f4efe6;"
      "}"
      "QPushButton:disabled {"
      "  background-color: #1a1815;"
      "  color: #7d756b;"
      "  border: 1px solid #3c362e;"
      "}"
      "QComboBox {"
      "  background-color: #241f1a;"
      "  color: #f4efe6;"
      "  border: 1px solid rgba(244, 239, 230, 48);"
      "  border-radius: 999px;"
      "  min-height: 64px;"
      "  padding: 0 28px;"
      "}"
      "QComboBox::drop-down { border: none; width: 36px; }"
      "QLineEdit {"
      "  background-color: #241f1a;"
      "  color: #f4efe6;"
      "  border: 1px solid rgba(244, 239, 230, 48);"
      "  border-radius: 999px;"
      "  min-height: 64px;"
      "  padding: 0 24px;"
      "  selection-background-color: #342a20;"
      "}"
      "QComboBox QAbstractItemView {"
      "  background-color: #1e1b17;"
      "  color: #f4efe6;"
      "  border-radius: 18px;"
      "  selection-background-color: #342a20;"
      "  selection-color: #f4efe6;"
      "}");
}

ShellWindow::DayBand bandForHour(int hour) {
  if (hour >= 22 || hour < 5) {
    return ShellWindow::DayBand::Night;
  }
  if (hour < 8) {
    return ShellWindow::DayBand::Dawn;
  }
  if (hour < 12) {
    return ShellWindow::DayBand::Morning;
  }
  if (hour < 17) {
    return ShellWindow::DayBand::Afternoon;
  }
  return ShellWindow::DayBand::Dusk;
}

ScenePalette paletteFor(ShellWindow::DayBand band) {
  using Band = ShellWindow::DayBand;
  switch (band) {
  case Band::Night:
    return {QColor(QStringLiteral("#070b14")), QColor(QStringLiteral("#121a30")),
            QColor(QStringLiteral("#1a3058")), QColor(QStringLiteral("#0c1016")),
            QColor(QStringLiteral("#f4f1e8")), QColor(QStringLiteral("#9eb4d4"))};
  case Band::Dawn:
    return {QColor(QStringLiteral("#1a2040")), QColor(QStringLiteral("#6a4a68")),
            QColor(QStringLiteral("#ffc09a")), QColor(QStringLiteral("#1a1412")),
            QColor(QStringLiteral("#ffe0c0")), QColor(QStringLiteral("#ffb088"))};
  case Band::Morning:
    return {QColor(QStringLiteral("#163e6e")), QColor(QStringLiteral("#2f78c0")),
            QColor(QStringLiteral("#f6d48a")), QColor(QStringLiteral("#1a2830")),
            QColor(QStringLiteral("#fff6d2")), QColor(QStringLiteral("#ffe7a4"))};
  case Band::Afternoon:
    return {QColor(QStringLiteral("#1a5690")), QColor(QStringLiteral("#3d88c8")),
            QColor(QStringLiteral("#ffe6a8")), QColor(QStringLiteral("#16303a")),
            QColor(QStringLiteral("#fffaf0")), QColor(QStringLiteral("#fff0c2"))};
  case Band::Dusk:
    return {QColor(QStringLiteral("#241433")), QColor(QStringLiteral("#6a3058")),
            QColor(QStringLiteral("#ff7a3c")), QColor(QStringLiteral("#1a1014")),
            QColor(QStringLiteral("#ffb07a")), QColor(QStringLiteral("#ff7848"))};
  }
  return {};
}

QString weatherCardText(ShellWindow::WeatherLook look) {
  using Look = ShellWindow::WeatherLook;
  switch (look) {
  case Look::Clear:
    return QStringLiteral("Preview: Clear");
  case Look::Cloudy:
    return QStringLiteral("Preview: Cloudy");
  case Look::Rain:
    return QStringLiteral("Preview: Rain");
  case Look::Snow:
    return QStringLiteral("Preview: Snow");
  case Look::Fog:
    return QStringLiteral("Preview: Fog");
  case Look::Storm:
    return QStringLiteral("Preview: Storm");
  case Look::Off:
    return QStringLiteral("Not connected");
  }
  return QStringLiteral("Not connected");
}

ShellWindow::WeatherLook weatherFromHook(const QByteArray &value) {
  using Look = ShellWindow::WeatherLook;
  const QString key = QString::fromLatin1(value).trimmed().toLower();
  if (key == QStringLiteral("clear")) {
    return Look::Clear;
  }
  if (key == QStringLiteral("cloudy")) {
    return Look::Cloudy;
  }
  if (key == QStringLiteral("rain")) {
    return Look::Rain;
  }
  if (key == QStringLiteral("snow")) {
    return Look::Snow;
  }
  if (key == QStringLiteral("fog")) {
    return Look::Fog;
  }
  if (key == QStringLiteral("storm")) {
    return Look::Storm;
  }
  return Look::Off;
}

void paintCloud(QPainter &painter, const QRectF &blob, const QColor &color) {
  painter.setPen(Qt::NoPen);
  painter.setBrush(color);
  painter.drawEllipse(blob);
  painter.drawEllipse(blob.adjusted(blob.width() * 0.28, blob.height() * 0.18, blob.width() * 0.22,
                                    blob.height() * 0.05));
  painter.drawEllipse(blob.adjusted(-blob.width() * 0.18, blob.height() * 0.28, -blob.width() * 0.22,
                                    blob.height() * 0.02));
}

}  // namespace

ShellWindow::ShellWindow(QWidget *parent) : QWidget(parent) {
  setAttribute(Qt::WA_OpaquePaintEvent);
  setAutoFillBackground(false);
  setStyleSheet(pageStyle());
  readPreviewHooks();

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(0);
  root->addWidget(buildBanner());

  m_body = new QStackedWidget;
  m_body->setAttribute(Qt::WA_TranslucentBackground);
  m_body->addWidget(buildWizard());

  auto *shell = new QWidget;
  shell->setAttribute(Qt::WA_TranslucentBackground);
  auto *shellLayout = new QVBoxLayout(shell);
  shellLayout->setContentsMargins(0, 0, 0, 0);
  shellLayout->setSpacing(0);
  shellLayout->addWidget(buildNav());

  m_stack = new QStackedWidget;
  m_stack->setAttribute(Qt::WA_TranslucentBackground);
  m_stack->addWidget(buildHome());
  m_stack->addWidget(buildTimer());
  m_stack->addWidget(buildStore());
  m_stack->addWidget(buildShopping());
  m_stack->addWidget(buildRecipes());
  m_stack->addWidget(buildApps());
  m_stack->addWidget(buildSettings());
  m_stack->addWidget(buildPower());
  shellLayout->addWidget(m_stack, 1);
  m_body->addWidget(shell);
  root->addWidget(m_body, 1);

  setPage(0);
  updateClock();
  refreshScene();

  WeatherLook initial = WeatherLook::Off;
  const QByteArray weatherHook = qgetenv("HAVEN_PREVIEW_WEATHER");
  if (!weatherHook.isEmpty()) {
    initial = weatherFromHook(weatherHook);
  }
  if (m_weatherLook != nullptr) {
    const QSignalBlocker blocker(m_weatherLook);
    m_weatherLook->setCurrentIndex(static_cast<int>(initial));
  }
  setWeatherLook(initial);
  loadRoute();

  auto *clockTimer = new QTimer(this);
  connect(clockTimer, &QTimer::timeout, this, [this] {
    updateClock();
    if (!m_timerRunning || m_timerRemaining <= 0) {
      return;
    }
    --m_timerRemaining;
    if (m_timerRemaining == 0) {
      m_timerRunning = false;
    }
    refreshTimerUi();
  });
  clockTimer->start(1000);

  auto *sceneTimer = new QTimer(this);
  connect(sceneTimer, &QTimer::timeout, this, [this] { refreshScene(); });
  sceneTimer->start(60000);
}

void ShellWindow::readPreviewHooks() {
  m_previewHour = -1;
  const QByteArray hourHook = qgetenv("HAVEN_PREVIEW_HOUR");
  if (hourHook.isEmpty()) {
    return;
  }
  bool ok = false;
  const int hour = QString::fromLatin1(hourHook).trimmed().toInt(&ok);
  if (ok && hour >= 0 && hour <= 23) {
    m_previewHour = hour;
  }
}

int ShellWindow::effectiveHour() const {
  if (m_previewHour >= 0) {
    return m_previewHour;
  }
  return QDateTime::currentDateTime().time().hour();
}

void ShellWindow::updateClock() {
  const QDateTime now = QDateTime::currentDateTime();
  if (m_previewHour >= 0) {
    m_clock->setText(QStringLiteral("%1:00").arg(m_previewHour, 2, 10, QLatin1Char('0')));
    m_seconds->setText(QStringLiteral("00"));
    if (m_clockNote != nullptr) {
      m_clockNote->setText(QStringLiteral("Scene preview, not the live clock"));
    }
  } else {
    m_clock->setText(now.toString(QStringLiteral("HH:mm")));
    m_seconds->setText(now.toString(QStringLiteral("ss")));
    if (m_clockNote != nullptr) {
      m_clockNote->setText(QStringLiteral("Local time from this computer"));
    }
  }
  m_weekday->setText(now.toString(QStringLiteral("dddd")));
  m_date->setText(now.toString(QStringLiteral("d MMMM yyyy")));
}

void ShellWindow::refreshScene() {
  const DayBand band = bandForHour(effectiveHour());
  if (band == m_band) {
    return;
  }
  m_band = band;
  update();
}

void ShellWindow::setWeatherLook(WeatherLook look) {
  m_weather = look;
  if (m_weatherStatus != nullptr) {
    m_weatherStatus->setText(weatherCardText(look));
  }
  if (m_weatherDetail != nullptr) {
    m_weatherDetail->setText(look == WeatherLook::Off
                                 ? QStringLiteral("No live weather. This slot is a placeholder.")
                                 : QStringLiteral("Not a live reading. This is a look preview."));
  }
  update();
}

void ShellWindow::paintEvent(QPaintEvent *event) {
  Q_UNUSED(event);
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);

  const QRect bounds = rect();
  const ScenePalette scene = paletteFor(m_band);
  QLinearGradient sky(bounds.topLeft(), bounds.bottomLeft());
  sky.setColorAt(0.00, scene.top);
  sky.setColorAt(0.18, scene.top);
  sky.setColorAt(0.36, scene.high);
  sky.setColorAt(0.52, scene.horizon);
  sky.setColorAt(0.70, scene.ground);
  sky.setColorAt(1.00, scene.ground.darker(140));
  painter.fillRect(bounds, sky);

  const qreal sx = bounds.width() / 1280.0;
  const qreal sy = bounds.height() / 800.0;
  const qreal horizonY = bounds.height() * 0.72;

  painter.setPen(Qt::NoPen);
  QColor glow = scene.glow;
  glow.setAlpha(110);
  painter.setBrush(glow);
  qreal bodyX = 700;
  qreal bodyY = 250;
  qreal bodyR = 36;
  switch (m_band) {
  case DayBand::Night:
    bodyX = 720;
    bodyY = 250;
    bodyR = 26;
    break;
  case DayBand::Dawn:
    bodyX = 560;
    bodyY = 390;
    bodyR = 44;
    break;
  case DayBand::Morning:
    bodyX = 700;
    bodyY = 250;
    bodyR = 46;
    break;
  case DayBand::Afternoon:
    bodyX = 760;
    bodyY = 230;
    bodyR = 50;
    break;
  case DayBand::Dusk:
    bodyX = 520;
    bodyY = 400;
    bodyR = 48;
    break;
  }
  const QPointF bodyCenter(bodyX * sx, bodyY * sy);
  painter.drawEllipse(bodyCenter, bodyR * 2.4 * sx, bodyR * 2.4 * sy);
  painter.setBrush(scene.body);
  painter.drawEllipse(bodyCenter, bodyR * sx, bodyR * sy);

  if (m_band == DayBand::Night &&
      (m_weather == WeatherLook::Off || m_weather == WeatherLook::Clear)) {
    painter.setBrush(QColor(244, 239, 230, 180));
    const int stars[12][2] = {{140, 210}, {260, 250}, {420, 190}, {560, 270}, {700, 210}, {860, 180},
                               {180, 320}, {340, 300}, {640, 240}, {760, 300}, {1120, 220}, {1200, 280}};
    for (const auto &star : stars) {
      painter.drawEllipse(QPointF(star[0] * sx, star[1] * sy), 1.6 * sx, 1.6 * sy);
    }
  }

  auto cloudColor = QColor(QStringLiteral("#c5ced6"));
  if (m_band == DayBand::Night || m_band == DayBand::Dusk) {
    cloudColor = QColor(QStringLiteral("#6d7380"));
  }
  if (m_weather == WeatherLook::Storm) {
    cloudColor = QColor(QStringLiteral("#3e4654"));
  }
  cloudColor.setAlpha(210);

  const bool cloudy = m_weather == WeatherLook::Cloudy || m_weather == WeatherLook::Rain ||
                      m_weather == WeatherLook::Snow || m_weather == WeatherLook::Storm;
  if (cloudy) {
    paintCloud(painter, QRectF(480 * sx, 200 * sy, 250 * sx, 64 * sy), cloudColor);
    paintCloud(painter, QRectF(680 * sx, 250 * sy, 230 * sx, 58 * sy), cloudColor);
    paintCloud(painter, QRectF(560 * sx, 310 * sy, 200 * sx, 50 * sy), cloudColor);
  }

  if (m_weather == WeatherLook::Rain || m_weather == WeatherLook::Storm) {
    const QColor wash = m_weather == WeatherLook::Storm ? QColor(12, 16, 32, 80) : QColor(30, 48, 72, 40);
    painter.fillRect(bounds.adjusted(0, static_cast<int>(150 * sy), 0, static_cast<int>(-260 * sy)), wash);
    painter.setPen(QPen(QColor(232, 238, 244, m_weather == WeatherLook::Storm ? 150 : 190), 2));
    for (int i = 0; i < 56; ++i) {
      const qreal x = ((i * 83) % 1280) * sx;
      const qreal y = (170 + (i * 47) % 300) * sy;
      painter.drawLine(QPointF(x, y), QPointF(x - 14 * sx, y + 28 * sy));
    }
  }

  if (m_weather == WeatherLook::Snow) {
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(244, 246, 248, 210));
    for (int i = 0; i < 36; ++i) {
      const qreal x = ((i * 131) % 1280) * sx;
      const qreal y = (100 + (i * 67) % 360) * sy;
      const qreal radius = (2.0 + (i % 3)) * sx;
      painter.drawEllipse(QPointF(x, y), radius, radius);
    }
  }

  if (m_weather == WeatherLook::Fog) {
    for (int i = 0; i < 4; ++i) {
      QLinearGradient band(0, horizonY - 40 * sy + i * 28 * sy, 0, horizonY + 20 * sy + i * 28 * sy);
      band.setColorAt(0.0, QColor(214, 218, 216, 0));
      band.setColorAt(0.5, QColor(214, 218, 216, 70));
      band.setColorAt(1.0, QColor(214, 218, 216, 0));
      painter.fillRect(QRectF(0, horizonY - 50 * sy + i * 26 * sy, bounds.width(), 70 * sy), band);
    }
  }

  if (m_weather == WeatherLook::Storm) {
    painter.setPen(QPen(QColor(244, 236, 200, 170), 2));
    QPainterPath bolt;
    bolt.moveTo(760 * sx, 200 * sy);
    bolt.lineTo(730 * sx, 270 * sy);
    bolt.lineTo(770 * sx, 278 * sy);
    bolt.lineTo(720 * sx, 360 * sy);
    painter.drawPath(bolt);
    QPainterPath bolt2;
    bolt2.moveTo(1040 * sx, 220 * sy);
    bolt2.lineTo(1010 * sx, 280 * sy);
    bolt2.lineTo(1048 * sx, 290 * sy);
    bolt2.lineTo(1000 * sx, 350 * sy);
    painter.drawPath(bolt2);
  }

  QPainterPath hills;
  hills.moveTo(0, horizonY + 8 * sy);
  hills.quadTo(bounds.width() * 0.22, horizonY - 18 * sy, bounds.width() * 0.46, horizonY + 6 * sy);
  hills.quadTo(bounds.width() * 0.72, horizonY + 20 * sy, bounds.width(), horizonY - 4 * sy);
  hills.lineTo(bounds.width(), bounds.height());
  hills.lineTo(0, bounds.height());
  hills.closeSubpath();
  painter.setPen(Qt::NoPen);
  QColor hill = scene.ground.darker(125);
  hill.setAlpha(230);
  painter.setBrush(hill);
  painter.drawPath(hills);
  paintLiftedShadows(painter);
}

namespace {

void paintLiftShadow(QPainter &painter, const QRect &rect, int radius) {
  if (rect.isEmpty()) {
    return;
  }
  painter.setPen(Qt::NoPen);
  struct Layer {
    int dx;
    int dy;
    int grow;
    int alpha;
  };
  const Layer layers[] = {
      {4, 14, 10, 18},
      {3, 10, 6, 30},
      {2, 7, 3, 44},
      {1, 4, 1, 58},
  };
  for (const Layer &layer : layers) {
    painter.setBrush(QColor(6, 8, 12, layer.alpha));
    const QRect shadow = rect.adjusted(-layer.grow, -layer.grow / 3, layer.grow, layer.grow)
                             .translated(layer.dx, layer.dy);
    painter.drawRoundedRect(shadow, radius + layer.grow, radius + layer.grow);
  }
}

}  // namespace

void ShellWindow::paintLiftedShadows(QPainter &painter) {
  const auto frames = findChildren<QFrame *>();
  for (QFrame *frame : frames) {
    if (!frame->isVisible()) {
      continue;
    }
    const QString name = frame->objectName();
    if (name != QLatin1String("slot") && name != QLatin1String("heroScrim")) {
      continue;
    }
    const QPoint origin = frame->mapTo(this, QPoint(0, 0));
    const int radius = qMin(36, qMin(frame->width(), frame->height()) / 2);
    paintLiftShadow(painter, QRect(origin, frame->size()), radius);
  }

  const auto buttons = findChildren<QPushButton *>();
  for (QPushButton *button : buttons) {
    if (!button->isVisible()) {
      continue;
    }
    const QPoint origin = button->mapTo(this, QPoint(0, 0));
    paintLiftShadow(painter, QRect(origin, button->size()), button->height() / 2);
  }

  const auto lines = findChildren<QLabel *>();
  for (QLabel *label : lines) {
    if (!label->isVisible() || label->objectName() != QLatin1String("statusLine")) {
      continue;
    }
    const QPoint origin = label->mapTo(this, QPoint(0, 0));
    paintLiftShadow(painter, QRect(origin, label->size()), label->height() / 2);
  }

  if (QWidget *banner = findChild<QWidget *>(QStringLiteral("banner"))) {
    if (banner->isVisible()) {
      const QPoint origin = banner->mapTo(this, QPoint(0, 0));
      paintLiftShadow(painter, QRect(origin, banner->size()), banner->height() / 2);
    }
  }

  if (m_weatherLook != nullptr && m_weatherLook->isVisible()) {
    const QPoint origin = m_weatherLook->mapTo(this, QPoint(0, 0));
    paintLiftShadow(painter, QRect(origin, m_weatherLook->size()), m_weatherLook->height() / 2);
  }
}

void ShellWindow::setPage(int index) {
  m_stack->setCurrentIndex(index);
  for (int i = 0; i < m_nav.size(); ++i) {
    m_nav.at(i)->setProperty("active", i == index);
    m_nav.at(i)->style()->unpolish(m_nav.at(i));
    m_nav.at(i)->style()->polish(m_nav.at(i));
    m_nav.at(i)->update();
  }
}

QWidget *ShellWindow::buildBanner() {
  auto *wrap = new QWidget;
  auto *wrapLayout = new QHBoxLayout(wrap);
  wrapLayout->setContentsMargins(24, 16, 24, 6);

  auto *banner = new QFrame;
  banner->setObjectName(QStringLiteral("banner"));
  banner->setFixedHeight(72);
  auto *textCol = new QVBoxLayout(banner);
  textCol->setContentsMargins(36, 10, 36, 10);
  textCol->setSpacing(0);

  auto *line1 = new QLabel(QString::fromUtf8(kBannerLine1));
  line1->setFont(interFont(18, QFont::Medium));
  line1->setAlignment(Qt::AlignCenter);
  auto *line2 = new QLabel(QString::fromUtf8(kBannerLine2));
  line2->setFont(interFont(16));
  line2->setAlignment(Qt::AlignCenter);
  line2->setStyleSheet(QStringLiteral("color: #c4b49a;"));
  textCol->addWidget(line1);
  textCol->addWidget(line2);
  wrapLayout->addWidget(banner);
  return wrap;
}

QWidget *ShellWindow::buildNav() {
  auto *nav = new QWidget;
  nav->setObjectName(QStringLiteral("nav"));

  auto *row = new QHBoxLayout(nav);
  row->setContentsMargins(32, 16, 32, 18);
  row->setSpacing(16);

  auto *mark = new QLabel(QStringLiteral("HAVEN"));
  QFont markFont = interFont(14, QFont::DemiBold);
  markFont.setLetterSpacing(QFont::AbsoluteSpacing, 3.0);
  mark->setFont(markFont);
  mark->setStyleSheet(QStringLiteral("color: #d08a45;"));
  row->addWidget(mark, 0, Qt::AlignVCenter);
  row->addStretch(1);

  const QStringList labels = {
      QStringLiteral("Home"),   QStringLiteral("Timer"), QStringLiteral("Store"),
      QStringLiteral("List"),   QStringLiteral("Recipes"), QStringLiteral("Apps"),
      QStringLiteral("Settings"), QStringLiteral("Power"),
  };
  for (const QString &label : labels) {
    row->addWidget(makeNavButton(label));
  }
  return nav;
}

QPushButton *ShellWindow::makeNavButton(const QString &text) {
  auto *button = new QPushButton(text);
  button->setFont(interFont(20, QFont::Medium));
  button->setCursor(Qt::PointingHandCursor);
  button->setFocusPolicy(Qt::NoFocus);
  const int index = m_nav.size();
  m_nav.push_back(button);
  connect(button, &QPushButton::clicked, this, [this, index] { setPage(index); });
  return button;
}

QWidget *ShellWindow::buildHome() {
  auto *page = new QWidget;
  page->setAttribute(Qt::WA_TranslucentBackground);
  auto *layout = new QVBoxLayout(page);
  layout->setContentsMargins(32, 20, 32, 28);
  layout->setSpacing(20);

  auto *hero = new QHBoxLayout;
  hero->setSpacing(24);

  auto *clockPlate = new QFrame;
  clockPlate->setObjectName(QStringLiteral("heroScrim"));
  clockPlate->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  auto *clockCol = new QVBoxLayout(clockPlate);
  clockCol->setContentsMargins(28, 16, 28, 16);
  clockCol->setSpacing(6);

  auto *timeRow = new QHBoxLayout;
  timeRow->setSpacing(10);
  m_clock = new QLabel;
  m_clock->setFont(interFont(96));
  m_seconds = new QLabel;
  m_seconds->setFont(interFont(28));
  m_seconds->setStyleSheet(QStringLiteral("color: #c4b49a;"));
  m_seconds->setContentsMargins(0, 0, 0, 14);
  timeRow->addWidget(m_clock, 0, Qt::AlignBottom);
  timeRow->addWidget(m_seconds, 0, Qt::AlignBottom);
  timeRow->addStretch(1);
  clockCol->addLayout(timeRow);

  m_clockNote = new QLabel(QStringLiteral("Local time from this computer"));
  m_clockNote->setFont(interFont(16));
  m_clockNote->setStyleSheet(QStringLiteral("color: #c4b49a;"));
  clockCol->addWidget(m_clockNote);

  auto *metaPlate = new QFrame;
  metaPlate->setObjectName(QStringLiteral("heroScrim"));
  metaPlate->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
  auto *meta = new QVBoxLayout(metaPlate);
  meta->setContentsMargins(28, 16, 28, 16);
  meta->setSpacing(2);

  auto *homeEyebrow = new QLabel(QStringLiteral("HOME NAME"));
  QFont eyebrowFont = interFont(13, QFont::DemiBold);
  eyebrowFont.setLetterSpacing(QFont::AbsoluteSpacing, 1.8);
  homeEyebrow->setFont(eyebrowFont);
  homeEyebrow->setStyleSheet(QStringLiteral("color: #d08a45;"));
  homeEyebrow->setAlignment(Qt::AlignRight);

  m_homeValue = new QLabel(QStringLiteral("Not set"));
  m_homeValue->setFont(interFont(32));
  m_homeValue->setAlignment(Qt::AlignRight);

  auto *roomEyebrow = new QLabel(QStringLiteral("ROOM"));
  roomEyebrow->setFont(eyebrowFont);
  roomEyebrow->setStyleSheet(QStringLiteral("color: #d08a45;"));
  roomEyebrow->setAlignment(Qt::AlignRight);
  m_roomValue = new QLabel(QStringLiteral("Not set"));
  m_roomValue->setFont(interFont(22, QFont::Medium));
  m_roomValue->setAlignment(Qt::AlignRight);

  m_weekday = new QLabel;
  m_weekday->setFont(interFont(22, QFont::Medium));
  m_weekday->setAlignment(Qt::AlignRight);
  m_date = new QLabel;
  m_date->setFont(interFont(20));
  m_date->setStyleSheet(QStringLiteral("color: #c4b49a;"));
  m_date->setAlignment(Qt::AlignRight);

  meta->addWidget(homeEyebrow);
  meta->addWidget(m_homeValue);
  meta->addSpacing(8);
  meta->addWidget(roomEyebrow);
  meta->addWidget(m_roomValue);
  meta->addSpacing(14);
  meta->addWidget(m_weekday);
  meta->addWidget(m_date);

  hero->addWidget(clockPlate, 0, Qt::AlignLeft | Qt::AlignTop);
  hero->addStretch(1);
  hero->addWidget(metaPlate, 0, Qt::AlignRight | Qt::AlignTop);
  layout->addLayout(hero);
  layout->addStretch(1);

  auto *cards = new QHBoxLayout;
  cards->setSpacing(24);
  cards->addWidget(makeSlot(QStringLiteral("Weather"),
                             QStringLiteral("No live weather. This slot is a placeholder."),
                             &m_weatherStatus, &m_weatherDetail));
  cards->addWidget(makeSlot(QStringLiteral("Devices"),
                             QStringLiteral("No live devices. This slot is a placeholder.")));
  cards->addWidget(makeSlot(QStringLiteral("Battery"),
                             QStringLiteral("No live charge. This slot is a placeholder.")));
  layout->addLayout(cards, 0);

  auto *foot = new QLabel(QStringLiteral("Wi-Fi and Matter are not connected."));
  foot->setObjectName(QStringLiteral("statusLine"));
  foot->setFont(interFont(16));
  layout->addWidget(foot, 0, Qt::AlignLeft);
  return page;
}

QFrame *ShellWindow::makeSlot(const QString &title, const QString &detail, QLabel **statusOut,
                              QLabel **detailOut) {
  auto *frame = new QFrame;
  frame->setObjectName(QStringLiteral("slot"));
  auto *layout = new QVBoxLayout(frame);
  layout->setContentsMargins(28, 26, 28, 26);
  layout->setSpacing(8);

  auto *eyebrow = new QLabel(title.toUpper());
  QFont eyebrowFont = interFont(13, QFont::DemiBold);
  eyebrowFont.setLetterSpacing(QFont::AbsoluteSpacing, 1.8);
  eyebrow->setFont(eyebrowFont);
  eyebrow->setStyleSheet(QStringLiteral("color: #d08a45;"));

  auto *status = new QLabel(QStringLiteral("Not connected"));
  status->setFont(interFont(28));

  auto *body = new QLabel(detail);
  body->setWordWrap(true);
  body->setFont(interFont(16));
  body->setStyleSheet(QStringLiteral("color: #c4b49a;"));

  if (statusOut != nullptr) {
    *statusOut = status;
  }
  if (detailOut != nullptr) {
    *detailOut = body;
  }

  layout->addWidget(eyebrow);
  layout->addSpacing(10);
  layout->addWidget(status);
  layout->addWidget(body);
  layout->addStretch(1);
  return frame;
}

QFrame *ShellWindow::makeStatusRow(const QString &name) {
  auto *row = new QFrame;
  row->setObjectName(QStringLiteral("slot"));
  row->setMinimumHeight(68);
  auto *layout = new QHBoxLayout(row);
  layout->setContentsMargins(36, 10, 36, 10);

  auto *label = new QLabel(name);
  label->setFont(interFont(20, QFont::Medium));
  auto *status = new QLabel(QStringLiteral("Not connected"));
  status->setFont(interFont(18));
  status->setStyleSheet(QStringLiteral("color: #c4b49a;"));
  layout->addWidget(label);
  layout->addStretch(1);
  layout->addWidget(status);
  return row;
}

QVBoxLayout *ShellWindow::beginPage(QWidget *page, const QString &heading) {
  page->setAttribute(Qt::WA_TranslucentBackground);
  auto *layout = new QVBoxLayout(page);
  layout->setContentsMargins(32, 24, 32, 24);
  layout->setSpacing(12);

  auto *title = new QLabel(heading);
  title->setObjectName(QStringLiteral("statusLine"));
  title->setFont(interFont(36));
  layout->addWidget(title, 0, Qt::AlignLeft);
  return layout;
}

QWidget *ShellWindow::buildApps() {
  auto *page = new QWidget;
  auto *layout = beginPage(page, QStringLiteral("Apps"));

  auto *body = new QLabel(QStringLiteral(
      "No apps are installed. This prototype cannot launch apps yet."));
  body->setObjectName(QStringLiteral("statusLine"));
  body->setWordWrap(true);
  body->setFont(interFont(20));
  layout->addWidget(body, 0, Qt::AlignLeft);
  layout->addStretch(1);
  return page;
}

QWidget *ShellWindow::buildSettings() {
  auto *page = new QWidget;
  auto *layout = beginPage(page, QStringLiteral("Settings"));

  auto *body = new QLabel(QStringLiteral(
      "Nothing on this page is live. Weather look is a preview, not a forecast."));
  body->setObjectName(QStringLiteral("statusLine"));
  body->setWordWrap(true);
  body->setFont(interFont(18));
  layout->addWidget(body, 0, Qt::AlignLeft);

  auto *homeLabel = new QLabel(QStringLiteral("Home"));
  homeLabel->setFont(interFont(18, QFont::Medium));
  layout->addWidget(homeLabel, 0, Qt::AlignLeft);

  auto *profileRow = new QHBoxLayout;
  profileRow->setSpacing(12);
  m_settingsName = new QLineEdit;
  m_settingsName->setPlaceholderText(QStringLiteral("Home name"));
  m_settingsName->setMinimumHeight(64);
  m_settingsRoom = new QComboBox;
  m_settingsRoom->setFont(interFont(20, QFont::Medium));
  m_settingsRoom->setMinimumHeight(64);
  m_settingsRoom->setMinimumWidth(220);
  for (const QString &room : Profile::rooms()) {
    m_settingsRoom->addItem(room);
  }
  auto *saveHome = new QPushButton(QStringLiteral("Save home"));
  saveHome->setFont(interFont(20, QFont::Medium));
  profileRow->addWidget(m_settingsName, 1);
  profileRow->addWidget(m_settingsRoom);
  profileRow->addWidget(saveHome);
  layout->addLayout(profileRow);
  connect(saveHome, &QPushButton::clicked, this, [this] {
    const QString name = m_settingsName->text().trimmed();
    const QString room = m_settingsRoom->currentText();
    if (name.isEmpty() || !Profile::rooms().contains(room)) {
      return;
    }
    m_profile.homeName = name;
    m_profile.room = room;
    m_profile.save();
    applyProfile();
  });

  auto *lookLabel = new QLabel(QStringLiteral("Weather look"));
  lookLabel->setFont(interFont(18, QFont::Medium));
  lookLabel->setStyleSheet(QStringLiteral("color: #f4efe6;"));
  layout->addWidget(lookLabel, 0, Qt::AlignLeft);

  m_weatherLook = new QComboBox;
  m_weatherLook->setFont(interFont(20, QFont::Medium));
  m_weatherLook->setMinimumHeight(64);
  m_weatherLook->setMinimumWidth(360);
  m_weatherLook->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  m_weatherLook->addItem(QStringLiteral("Off"));
  m_weatherLook->addItem(QStringLiteral("Preview: Clear"));
  m_weatherLook->addItem(QStringLiteral("Preview: Cloudy"));
  m_weatherLook->addItem(QStringLiteral("Preview: Rain"));
  m_weatherLook->addItem(QStringLiteral("Preview: Snow"));
  m_weatherLook->addItem(QStringLiteral("Preview: Fog"));
  m_weatherLook->addItem(QStringLiteral("Preview: Storm"));
  layout->addWidget(m_weatherLook, 0, Qt::AlignLeft);
  connect(m_weatherLook, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int index) {
    if (index < 0) {
      return;
    }
    setWeatherLook(static_cast<WeatherLook>(index));
  });

  layout->addWidget(makeStatusRow(QStringLiteral("Wi-Fi")));
  layout->addWidget(makeStatusRow(QStringLiteral("Matter")));
  layout->addWidget(makeStatusRow(QStringLiteral("Weather")));
  layout->addWidget(makeStatusRow(QStringLiteral("Battery")));
  layout->addStretch(1);
  return page;
}

QWidget *ShellWindow::buildPower() {
  auto *page = new QWidget;
  auto *layout = beginPage(page, QStringLiteral("Power"));

  auto *body = new QLabel(QStringLiteral(
      "Not implemented. This prototype cannot shut down, restart, or sleep a device."));
  body->setObjectName(QStringLiteral("statusLine"));
  body->setWordWrap(true);
  body->setFont(interFont(18));
  layout->addWidget(body, 0, Qt::AlignLeft);
  layout->addSpacing(8);

  const QStringList actions = {
      QStringLiteral("Shut down"),
      QStringLiteral("Restart"),
      QStringLiteral("Sleep"),
  };
  for (const QString &action : actions) {
    auto *button = new QPushButton(action);
    button->setFont(interFont(20, QFont::Medium));
    button->setEnabled(false);
    button->setMinimumWidth(320);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(button, 0, Qt::AlignLeft);
  }
  layout->addStretch(1);
  return page;
}

void ShellWindow::loadRoute() {
  const bool forceWizard = qEnvironmentVariableIsSet("HAVEN_FORCE_WIZARD");
  const QString seededName = QString::fromLocal8Bit(qgetenv("HAVEN_PROFILE_NAME")).trimmed();
  const QString seededRoom = QString::fromLocal8Bit(qgetenv("HAVEN_PROFILE_ROOM")).trimmed();
  if (!seededName.isEmpty() && Profile::rooms().contains(seededRoom)) {
    m_profile.homeName = seededName;
    m_profile.room = seededRoom;
    m_profile.save();
  } else {
    m_profile = Profile::load();
  }

  if (forceWizard || !m_profile.isComplete()) {
    showWizard();
  } else {
    showShell();
  }

  const QString open = QString::fromLatin1(qgetenv("HAVEN_OPEN")).trimmed().toLower();
  if (!forceWizard && m_profile.isComplete()) {
    if (open == QLatin1String("timer")) {
      setPage(1);
    } else if (open == QLatin1String("store")) {
      setPage(2);
    } else if (open == QLatin1String("shopping")) {
      setPage(3);
    } else if (open == QLatin1String("recipes")) {
      setPage(4);
    }
  }
  if (qEnvironmentVariableIsSet("HAVEN_TIMER_START")) {
    m_timerDuration = 300;
    m_timerRemaining = 300;
    m_timerRunning = true;
    refreshTimerUi();
  }
}

void ShellWindow::applyProfile() {
  const QString name = m_profile.homeName.trimmed().isEmpty() ? QStringLiteral("Not set")
                                                              : m_profile.homeName.trimmed();
  const QString room = m_profile.room.trimmed().isEmpty() ? QStringLiteral("Not set")
                                                          : m_profile.room.trimmed();
  if (m_homeValue != nullptr) {
    m_homeValue->setText(name);
  }
  if (m_roomValue != nullptr) {
    m_roomValue->setText(room);
  }
  if (m_settingsName != nullptr && m_profile.isComplete()) {
    m_settingsName->setText(m_profile.homeName.trimmed());
  }
  if (m_settingsRoom != nullptr && Profile::rooms().contains(m_profile.room)) {
    const QSignalBlocker blocker(m_settingsRoom);
    m_settingsRoom->setCurrentText(m_profile.room);
  }
}

void ShellWindow::showWizard() {
  if (m_wizard != nullptr) {
    m_wizard->setCurrentIndex(0);
  }
  if (m_body != nullptr) {
    m_body->setCurrentIndex(0);
  }
}

void ShellWindow::showShell() {
  applyProfile();
  if (m_body != nullptr) {
    m_body->setCurrentIndex(1);
  }
  setPage(0);
}

void ShellWindow::finishWizard() {
  const QString name = m_wizardName == nullptr ? QString() : m_wizardName->text().trimmed();
  if (name.isEmpty() || !Profile::rooms().contains(m_wizardRoom)) {
    return;
  }
  m_profile.homeName = name;
  m_profile.room = m_wizardRoom;
  m_profile.save();
  showShell();
}

void ShellWindow::refreshTimerUi() {
  const int minutes = m_timerRemaining / 60;
  const int seconds = m_timerRemaining % 60;
  if (m_timerDigits != nullptr) {
    m_timerDigits->setText(QStringLiteral("%1:%2")
                               .arg(minutes, 2, 10, QLatin1Char('0'))
                               .arg(seconds, 2, 10, QLatin1Char('0')));
  }
  QString state = QStringLiteral("Ready");
  if (m_timerRunning) {
    state = QStringLiteral("Running");
  } else if (m_timerRemaining == 0) {
    state = QStringLiteral("Finished");
  } else if (m_timerRemaining != m_timerDuration) {
    state = QStringLiteral("Paused");
  }
  if (m_timerState != nullptr) {
    m_timerState->setText(state);
  }
  if (m_timerStart != nullptr) {
    m_timerStart->setEnabled(!m_timerRunning);
  }
  if (m_timerPause != nullptr) {
    m_timerPause->setEnabled(m_timerRunning);
  }
}

void ShellWindow::setTimerDuration(int seconds) {
  if (m_timerRunning || seconds <= 0) {
    return;
  }
  m_timerDuration = seconds;
  m_timerRemaining = seconds;
  refreshTimerUi();
}

void ShellWindow::startTimer() {
  if (m_timerRemaining <= 0) {
    m_timerRemaining = m_timerDuration;
  }
  m_timerRunning = true;
  refreshTimerUi();
}

void ShellWindow::pauseTimer() {
  m_timerRunning = false;
  refreshTimerUi();
}

void ShellWindow::cancelTimer() {
  m_timerRunning = false;
  m_timerRemaining = m_timerDuration;
  refreshTimerUi();
}

QWidget *ShellWindow::buildTimer() {
  auto *page = new QWidget;
  auto *layout = beginPage(page, QStringLiteral("Timer"));

  m_timerState = new QLabel(QStringLiteral("Ready"));
  m_timerState->setObjectName(QStringLiteral("statusLine"));
  m_timerState->setFont(interFont(18, QFont::Medium));
  layout->addWidget(m_timerState, 0, Qt::AlignLeft);

  auto *face = new QFrame;
  face->setObjectName(QStringLiteral("slot"));
  auto *faceLayout = new QVBoxLayout(face);
  faceLayout->setContentsMargins(32, 28, 32, 28);
  m_timerDigits = new QLabel(QStringLiteral("05:00"));
  m_timerDigits->setAlignment(Qt::AlignCenter);
  m_timerDigits->setFont(interFont(84));
  auto *note = new QLabel(QStringLiteral("Offline. This timer only counts down on this computer."));
  note->setAlignment(Qt::AlignCenter);
  note->setWordWrap(true);
  note->setFont(interFont(16));
  note->setStyleSheet(QStringLiteral("color: #c4b49a;"));
  faceLayout->addWidget(m_timerDigits);
  faceLayout->addWidget(note);
  layout->addWidget(face);

  auto *presets = new QHBoxLayout;
  presets->setSpacing(12);
  const QList<QPair<QString, int>> choices = {
      {QStringLiteral("1 min"), 60},
      {QStringLiteral("5 min"), 300},
      {QStringLiteral("10 min"), 600},
  };
  for (const auto &choice : choices) {
    auto *button = new QPushButton(choice.first);
    button->setFont(interFont(20, QFont::Medium));
    const int seconds = choice.second;
    connect(button, &QPushButton::clicked, this, [this, seconds] { setTimerDuration(seconds); });
    presets->addWidget(button);
  }
  presets->addStretch(1);
  layout->addLayout(presets);

  auto *controls = new QHBoxLayout;
  controls->setSpacing(12);
  m_timerStart = new QPushButton(QStringLiteral("Start"));
  m_timerPause = new QPushButton(QStringLiteral("Pause"));
  auto *cancel = new QPushButton(QStringLiteral("Cancel"));
  for (QPushButton *button : {m_timerStart, m_timerPause, cancel}) {
    button->setFont(interFont(20, QFont::Medium));
    button->setMinimumWidth(180);
  }
  m_timerPause->setEnabled(false);
  connect(m_timerStart, &QPushButton::clicked, this, [this] { startTimer(); });
  connect(m_timerPause, &QPushButton::clicked, this, [this] { pauseTimer(); });
  connect(cancel, &QPushButton::clicked, this, [this] { cancelTimer(); });
  controls->addWidget(m_timerStart);
  controls->addWidget(m_timerPause);
  controls->addWidget(cancel);
  controls->addStretch(1);
  layout->addLayout(controls);
  layout->addStretch(1);
  refreshTimerUi();
  return page;
}

QWidget *ShellWindow::buildStore() {
  auto *page = new QWidget;
  auto *layout = beginPage(page, QStringLiteral("Haven Store"));

  auto *body = new QLabel(QStringLiteral(
      "Offline. This list is written into the prototype. It is not stock, it has no prices, "
      "and this page cannot take an order."));
  body->setObjectName(QStringLiteral("statusLine"));
  body->setWordWrap(true);
  body->setFont(interFont(18));
  layout->addWidget(body, 0, Qt::AlignLeft);

  const QStringList samples = {
      QStringLiteral("Lamp"),
      QStringLiteral("Plug"),
      QStringLiteral("Sensor"),
  };
  for (const QString &name : samples) {
    auto *row = new QFrame;
    row->setObjectName(QStringLiteral("slot"));
    auto *rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(28, 16, 28, 16);
    auto *title = new QLabel(name);
    title->setFont(interFont(22, QFont::Medium));
    auto *mark = new QLabel(QStringLiteral("Sample"));
    mark->setFont(interFont(16));
    mark->setStyleSheet(QStringLiteral("color: #c4b49a;"));
    rowLayout->addWidget(title);
    rowLayout->addStretch(1);
    rowLayout->addWidget(mark);
    layout->addWidget(row);
  }
  layout->addStretch(1);
  return page;
}

void ShellWindow::refreshShopping() {
  if (m_shoppingRows == nullptr) {
    return;
  }
  while (QLayoutItem *item = m_shoppingRows->takeAt(0)) {
    if (QWidget *widget = item->widget()) {
      widget->deleteLater();
    }
    delete item;
  }
  if (m_shopping.isEmpty()) {
    auto *empty = new QLabel(QStringLiteral("Nothing saved on this computer."));
    empty->setFont(interFont(18));
    empty->setStyleSheet(QStringLiteral("color: #c4b49a;"));
    m_shoppingRows->addWidget(empty, 0, Qt::AlignLeft);
    return;
  }
  for (int i = 0; i < m_shopping.size(); ++i) {
    auto *row = new QFrame;
    row->setObjectName(QStringLiteral("slot"));
    auto *rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(28, 8, 16, 8);
    auto *title = new QLabel(m_shopping.at(i));
    title->setFont(interFont(20, QFont::Medium));
    auto *remove = new QPushButton(QStringLiteral("Remove"));
    remove->setFont(interFont(18, QFont::Medium));
    connect(remove, &QPushButton::clicked, this, [this, i] { removeShoppingItem(i); });
    rowLayout->addWidget(title, 1);
    rowLayout->addWidget(remove);
    m_shoppingRows->addWidget(row);
  }
}

void ShellWindow::addShoppingItem() {
  if (m_shoppingEntry == nullptr) {
    return;
  }
  const QString item = m_shoppingEntry->text().trimmed();
  if (item.isEmpty()) {
    return;
  }
  m_shopping.push_back(item);
  m_shoppingEntry->clear();
  Profile::saveShopping(m_shopping);
  refreshShopping();
}

void ShellWindow::removeShoppingItem(int index) {
  if (index < 0 || index >= m_shopping.size()) {
    return;
  }
  m_shopping.removeAt(index);
  Profile::saveShopping(m_shopping);
  refreshShopping();
}

QWidget *ShellWindow::buildShopping() {
  auto *page = new QWidget;
  auto *layout = beginPage(page, QStringLiteral("Shopping"));

  auto *body = new QLabel(QStringLiteral(
      "Offline. Items stay on this computer. Nothing is ordered, and nothing is sent."));
  body->setObjectName(QStringLiteral("statusLine"));
  body->setWordWrap(true);
  body->setFont(interFont(18));
  layout->addWidget(body, 0, Qt::AlignLeft);

  auto *entryRow = new QHBoxLayout;
  entryRow->setSpacing(12);
  m_shoppingEntry = new QLineEdit;
  m_shoppingEntry->setPlaceholderText(QStringLiteral("Add an item"));
  m_shoppingEntry->setMinimumHeight(64);
  auto *add = new QPushButton(QStringLiteral("Add"));
  add->setFont(interFont(20, QFont::Medium));
  entryRow->addWidget(m_shoppingEntry, 1);
  entryRow->addWidget(add);
  layout->addLayout(entryRow);
  connect(add, &QPushButton::clicked, this, [this] { addShoppingItem(); });
  connect(m_shoppingEntry, &QLineEdit::returnPressed, this, [this] { addShoppingItem(); });

  m_shopping = Profile::loadShopping();
  auto *rows = new QWidget;
  m_shoppingRows = new QVBoxLayout(rows);
  m_shoppingRows->setContentsMargins(0, 0, 0, 0);
  m_shoppingRows->setSpacing(12);
  layout->addWidget(rows);
  layout->addStretch(1);
  refreshShopping();
  return page;
}

QWidget *ShellWindow::buildRecipes() {
  auto *page = new QWidget;
  auto *layout = beginPage(page, QStringLiteral("Recipes"));

  auto *body = new QLabel(QStringLiteral(
      "Offline. These recipes are written into the prototype. There is no account and no meal planner."));
  body->setObjectName(QStringLiteral("statusLine"));
  body->setWordWrap(true);
  body->setFont(interFont(18));
  layout->addWidget(body, 0, Qt::AlignLeft);

  const QList<QPair<QString, QString>> recipes = {
      {QStringLiteral("Porridge"), QStringLiteral("Oats, water, a pinch of salt. Heat until thick.")},
      {QStringLiteral("Toast"), QStringLiteral("Bread, toasted.")},
      {QStringLiteral("Soup"), QStringLiteral("Water, vegetables, salt. Simmer.")},
  };
  for (const auto &recipe : recipes) {
    auto *card = new QFrame;
    card->setObjectName(QStringLiteral("slot"));
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(28, 18, 28, 18);
    cardLayout->setSpacing(6);
    auto *title = new QLabel(recipe.first);
    title->setFont(interFont(22, QFont::Medium));
    auto *detail = new QLabel(recipe.second);
    detail->setWordWrap(true);
    detail->setFont(interFont(16));
    detail->setStyleSheet(QStringLiteral("color: #c4b49a;"));
    cardLayout->addWidget(title);
    cardLayout->addWidget(detail);
    layout->addWidget(card);
  }
  layout->addStretch(1);
  return page;
}

QWidget *ShellWindow::buildWizard() {
  auto *page = new QWidget;
  page->setAttribute(Qt::WA_TranslucentBackground);
  auto *outer = new QVBoxLayout(page);
  outer->setContentsMargins(48, 12, 48, 28);

  m_wizard = new QStackedWidget;
  m_wizard->setAttribute(Qt::WA_TranslucentBackground);

  auto addStep = [this](const QString &kicker, const QString &title, const QString &body) {
    auto *step = new QWidget;
    step->setAttribute(Qt::WA_TranslucentBackground);
    auto *layout = new QVBoxLayout(step);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addStretch(1);
    auto *card = new QFrame;
    card->setObjectName(QStringLiteral("slot"));
    card->setMaximumWidth(860);
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(36, 28, 36, 28);
    cardLayout->setSpacing(14);
    auto *stepLabel = new QLabel(kicker);
    stepLabel->setFont(interFont(14, QFont::DemiBold));
    stepLabel->setStyleSheet(QStringLiteral("color: #d08a45;"));
    auto *heading = new QLabel(title);
    heading->setFont(interFont(32));
    heading->setWordWrap(true);
    auto *copy = new QLabel(body);
    copy->setWordWrap(true);
    copy->setFont(interFont(18));
    copy->setStyleSheet(QStringLiteral("color: #c4b49a;"));
    cardLayout->addWidget(stepLabel);
    cardLayout->addWidget(heading);
    cardLayout->addWidget(copy);
    layout->addWidget(card, 0, Qt::AlignHCenter);
    layout->addStretch(1);
    m_wizard->addWidget(step);
    return cardLayout;
  };

  auto *welcome = addStep(QStringLiteral("Step 1 of 6"), QStringLiteral("Welcome"),
                          QStringLiteral("This is a desktop prototype, not a flashable OS image, and it is not "
                                         "running on the tablet. English only. Next you can name the home and pick a room."));
  auto *welcomeNext = new QPushButton(QStringLiteral("Continue"));
  welcomeNext->setFont(interFont(20, QFont::Medium));
  welcomeNext->setMinimumWidth(220);
  connect(welcomeNext, &QPushButton::clicked, this, [this] { m_wizard->setCurrentIndex(1); });
  welcome->addWidget(welcomeNext, 0, Qt::AlignLeft);

  auto *nameLayout =
      addStep(QStringLiteral("Step 2 of 6"), QStringLiteral("Home name"),
              QStringLiteral("This name is saved on this computer. It is not looked up online."));
  m_wizardName = new QLineEdit;
  m_wizardName->setPlaceholderText(QStringLiteral("Home name"));
  m_wizardName->setMinimumHeight(64);
  nameLayout->addWidget(m_wizardName);
  auto *nameRow = new QHBoxLayout;
  auto *nameBack = new QPushButton(QStringLiteral("Back"));
  auto *nameNext = new QPushButton(QStringLiteral("Continue"));
  for (QPushButton *button : {nameBack, nameNext}) {
    button->setFont(interFont(20, QFont::Medium));
    button->setMinimumWidth(180);
  }
  nameNext->setEnabled(false);
  connect(m_wizardName, &QLineEdit::textChanged, nameNext, [nameNext](const QString &text) {
    nameNext->setEnabled(!text.trimmed().isEmpty());
  });
  connect(nameBack, &QPushButton::clicked, this, [this] { m_wizard->setCurrentIndex(0); });
  connect(nameNext, &QPushButton::clicked, this, [this] { m_wizard->setCurrentIndex(2); });
  nameRow->addWidget(nameBack);
  nameRow->addWidget(nameNext);
  nameRow->addStretch(1);
  nameLayout->addLayout(nameRow);

  auto *roomLayout = addStep(QStringLiteral("Step 3 of 6"), QStringLiteral("Room"),
                             QStringLiteral("Pick the room this panel stands in. Nothing here is detected."));
  auto *grid = new QGridLayout;
  grid->setSpacing(12);
  int column = 0;
  int gridRow = 0;
  for (const QString &room : Profile::rooms()) {
    auto *button = new QPushButton(room);
    button->setFont(interFont(20, QFont::Medium));
    button->setMinimumHeight(64);
    m_roomButtons.push_back(button);
    connect(button, &QPushButton::clicked, this, [this, button, room] {
      m_wizardRoom = room;
      for (QPushButton *other : m_roomButtons) {
        other->setProperty("active", other == button);
        other->style()->unpolish(other);
        other->style()->polish(other);
        other->update();
      }
      if (m_roomNext != nullptr) {
        m_roomNext->setEnabled(true);
      }
    });
    grid->addWidget(button, gridRow, column);
    ++column;
    if (column == 3) {
      column = 0;
      ++gridRow;
    }
  }
  roomLayout->addLayout(grid);
  auto *roomRow = new QHBoxLayout;
  auto *roomBack = new QPushButton(QStringLiteral("Back"));
  m_roomNext = new QPushButton(QStringLiteral("Continue"));
  for (QPushButton *button : {roomBack, m_roomNext}) {
    button->setFont(interFont(20, QFont::Medium));
    button->setMinimumWidth(180);
  }
  m_roomNext->setEnabled(false);
  connect(roomBack, &QPushButton::clicked, this, [this] { m_wizard->setCurrentIndex(1); });
  connect(m_roomNext, &QPushButton::clicked, this, [this] { m_wizard->setCurrentIndex(3); });
  roomRow->addWidget(roomBack);
  roomRow->addWidget(m_roomNext);
  roomRow->addStretch(1);
  roomLayout->addLayout(roomRow);

  const QString skipBody = QStringLiteral("Not available in this desktop prototype.");
  struct Skip {
    const char *kicker;
    const char *title;
    int index;
    bool last;
  };
  const Skip skips[] = {
      {"Step 4 of 6", "Wi-Fi", 3, false},
      {"Step 5 of 6", "Matter", 4, false},
      {"Step 6 of 6", "AI", 5, true},
  };
  for (const Skip &skip : skips) {
    auto *skipLayout = addStep(QString::fromUtf8(skip.kicker), QString::fromUtf8(skip.title), skipBody);
    auto *skipRow = new QHBoxLayout;
    auto *back = new QPushButton(QStringLiteral("Back"));
    auto *next = new QPushButton(skip.last ? QStringLiteral("Finish") : QStringLiteral("Skip"));
    for (QPushButton *button : {back, next}) {
      button->setFont(interFont(20, QFont::Medium));
      button->setMinimumWidth(180);
    }
    const int index = skip.index;
    const bool last = skip.last;
    connect(back, &QPushButton::clicked, this, [this, index] { m_wizard->setCurrentIndex(index - 1); });
    connect(next, &QPushButton::clicked, this, [this, index, last] {
      if (last) {
        finishWizard();
      } else {
        m_wizard->setCurrentIndex(index + 1);
      }
    });
    skipRow->addWidget(back);
    skipRow->addWidget(next);
    skipRow->addStretch(1);
    skipLayout->addLayout(skipRow);
  }

  outer->addWidget(m_wizard, 1);
  return page;
}
