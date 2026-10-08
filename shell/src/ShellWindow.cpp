#include "ShellWindow.h"

#include <QComboBox>
#include <QDateTime>
#include <QFont>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
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
  root->addWidget(buildNav());

  m_stack = new QStackedWidget;
  m_stack->setAttribute(Qt::WA_TranslucentBackground);
  m_stack->addWidget(buildHome());
  m_stack->addWidget(buildApps());
  m_stack->addWidget(buildSettings());
  m_stack->addWidget(buildPower());
  root->addWidget(m_stack, 1);

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

  auto *clockTimer = new QTimer(this);
  connect(clockTimer, &QTimer::timeout, this, [this] { updateClock(); });
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
      QStringLiteral("Home"),
      QStringLiteral("Apps"),
      QStringLiteral("Settings"),
      QStringLiteral("Power"),
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

  auto *homeValue = new QLabel(QStringLiteral("Not set"));
  homeValue->setFont(interFont(32));
  homeValue->setAlignment(Qt::AlignRight);

  m_weekday = new QLabel;
  m_weekday->setFont(interFont(22, QFont::Medium));
  m_weekday->setAlignment(Qt::AlignRight);
  m_date = new QLabel;
  m_date->setFont(interFont(20));
  m_date->setStyleSheet(QStringLiteral("color: #c4b49a;"));
  m_date->setAlignment(Qt::AlignRight);

  meta->addWidget(homeEyebrow);
  meta->addWidget(homeValue);
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
