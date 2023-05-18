#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScreen>
#include <QTimer>

// Custom taskbar-like widget
class TaskbarWidget : public QWidget
{
  public:
    TaskbarWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint | Qt::Tool);
        setAttribute(Qt::WA_TranslucentBackground);
        setStyleSheet("background-color: rgba(0, 0, 0, 50%);");

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(10, 5, 10, 5);

        QLabel *label = new QLabel("Widget attached to taskbar", this);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);

        // Get the available screen geometry
        QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
        int taskbarHeight = 50; // Adjust the height as needed

        // Position the taskbar widget at the bottom of the screen
        move(screenGeometry.bottomLeft().x(), screenGeometry.bottomLeft().y() - taskbarHeight);

        // Set the widget width to match the screen width
        setFixedWidth(screenGeometry.width());

        // Show the taskbar widget
        show();
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create the taskbar widget
    TaskbarWidget taskbarWidget;

    return a.exec();
}
