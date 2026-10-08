#include "ShellWindow.h"

#include <QApplication>
#include <QByteArray>
#include <QColor>
#include <QDebug>
#include <QFont>
#include <QPalette>
#include <QPixmap>
#include <QTimer>

namespace {

QFont interFont(int pixelSize, QFont::Weight weight = QFont::Normal) {
  QFont font;
  font.setFamilies({QStringLiteral("Inter"), QStringLiteral("DejaVu Sans")});
  font.setPixelSize(pixelSize);
  font.setWeight(weight);
  return font;
}

}  // namespace

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QApplication::setStyle(QStringLiteral("Fusion"));
  app.setApplicationName(QStringLiteral("HavenOS Shell"));
  app.setFont(interFont(18));

  QPalette palette;
  const QColor windowColor(QStringLiteral("#141210"));
  const QColor text(QStringLiteral("#f4efe6"));
  palette.setColor(QPalette::Window, windowColor);
  palette.setColor(QPalette::WindowText, text);
  palette.setColor(QPalette::Base, QColor(QStringLiteral("#1e1b17")));
  palette.setColor(QPalette::Text, text);
  palette.setColor(QPalette::Button, QColor(QStringLiteral("#28241e")));
  palette.setColor(QPalette::ButtonText, text);
  palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(QStringLiteral("#7d756b")));
  palette.setColor(QPalette::Highlight, QColor(QStringLiteral("#d08a45")));
  palette.setColor(QPalette::HighlightedText, windowColor);
  app.setPalette(palette);

  // Platform plugin stays outside the program: X11/Wayland on the desktop,
  // or whatever QT_QPA_PLATFORM the environment sets.
  const bool fullscreen = app.arguments().contains(QStringLiteral("--fullscreen"));
  ShellWindow window;
  window.setWindowTitle(QStringLiteral("HavenOS — desktop prototype"));
  if (fullscreen) {
    window.showFullScreen();
  } else {
    window.setFixedSize(1280, 800);
    window.show();
  }

  const QByteArray shot = qgetenv("HAVEN_SCREENSHOT");
  const bool smoke = qEnvironmentVariableIsSet("HAVEN_SMOKE");
  if (!shot.isEmpty() || smoke) {
    QTimer::singleShot(150, &app, [&app, &window, shot, smoke] {
      if (!shot.isEmpty()) {
        const QPixmap pix = window.grab();
        const bool ok = !pix.isNull() && pix.width() > 0 && pix.save(QString::fromLocal8Bit(shot), "PNG");
        if (!ok) {
          qWarning("haven-shell: failed to write %s", shot.constData());
          app.exit(1);
          return;
        }
        qInfo("haven-shell: wrote %s (%dx%d)", shot.constData(), pix.width(), pix.height());
        app.exit(0);
        return;
      }
      qInfo("haven-shell: smoke ok");
      app.exit(window.isVisible() ? 0 : 1);
    });
  }

  return app.exec();
}
