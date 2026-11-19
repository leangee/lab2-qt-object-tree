#include "mainwindow.h"

#include <QDebug>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

MainWindow::~MainWindow()
{
    qDebug() << "[DESTROY] MainWindow";
}

void MainWindow::setupUi()
{
    setObjectName("MainWindow");
    setWindowTitle("Дерево объектов Qt");
    resize(560, 560);

    m_rootLayout = new QVBoxLayout(this);

    LogLabel *title = new LogLabel(
        "Демонстрация дерева объектов QObject и QWidget", this);
    m_rootLayout->addWidget(title);

    LogLineEdit *input = new LogLineEdit(this);
    input->setPlaceholderText("Виджет — ребёнок главного окна");
    m_rootLayout->addWidget(input);

    m_status = new LogLabel("Статус: окно создано", this);
    m_status->setWordWrap(true);
    m_rootLayout->addWidget(m_status);

    m_panel = new LogPanel("Панель-владелец", this);
    m_panelLayout = new QVBoxLayout(m_panel);

    LogLabel *panelLabel = new LogLabel("Дочерняя метка панели", m_panel);
    m_panelLayout->addWidget(panelLabel);

    LogButton *staticButton =
        new LogButton("Кнопка, принадлежащая панели", m_panel);
    m_panelLayout->addWidget(staticButton);

    m_movable = new LogButton("Перемещаемый виджет (в панели)", m_panel);
    m_panelLayout->addWidget(m_movable);
    m_rootLayout->addWidget(m_panel);

    m_moveButton = new LogButton("Переместить виджет: панель <-> окно", this);
    connect(m_moveButton, &QPushButton::clicked,
            this, &MainWindow::moveMovableWidget);
    m_rootLayout->addWidget(m_moveButton);

    m_deleteButton = new LogButton("Удалить панель (deleteLater)", this);
    connect(m_deleteButton, &QPushButton::clicked,
            this, &MainWindow::deletePanel);
    m_rootLayout->addWidget(m_deleteButton);

    LogButton *detachButton = new LogButton("Демонстрация setParent(nullptr)", this);
    connect(detachButton, &QPushButton::clicked,
            this, &MainWindow::demoDetaching);
    m_rootLayout->addWidget(detachButton);

    m_rootLayout->addStretch();

    setStatus("Готово. Откройте Application Output, чтобы видеть сообщения qDebug().");
}

void MainWindow::setStatus(const QString &text)
{
    if (!m_status.isNull())
        m_status->setText("Статус: " + text);
}

void MainWindow::moveMovableWidget()
{
    if (m_movable.isNull()) {
        setStatus("Перемещаемый виджет уже уничтожен.");
        return;
    }

    if (m_panel.isNull()) {
        setStatus("Панель уже удалена. Перемещение обратно невозможно.");
        return;
    }

    if (m_movable->parentWidget() == m_panel.data()) {
        if (!m_panelLayout.isNull())
            m_panelLayout->removeWidget(m_movable.data());

        m_movable->setParent(this);
        m_rootLayout->addWidget(m_movable.data());
        m_movable->setText("Перемещаемый виджет (главное окно)");
        setStatus("Новым родителем виджета стало главное окно.");
    } else {
        m_rootLayout->removeWidget(m_movable.data());
        m_movable->setParent(m_panel.data());
        m_panelLayout->addWidget(m_movable.data());
        m_movable->setText("Перемещаемый виджет (в панели)");
        setStatus("Виджет снова стал ребёнком панели.");
    }

    m_movable->show();
    qDebug() << "[SET_PARENT] Новый родитель:"
             << m_movable->parentWidget()->objectName();
}

void MainWindow::deletePanel()
{
    if (m_panel.isNull()) {
        setStatus("Панель уже удалена.");
        return;
    }

    if (!m_movable.isNull() && m_movable->parentWidget() == m_panel.data()) {
        setStatus("Панель и её дочерние виджеты будут удалены автоматически.");
    } else {
        setStatus("Панель будет удалена; перемещённый виджет останется в окне.");
    }

    m_moveButton->setEnabled(false);
    m_deleteButton->setEnabled(false);

    qDebug() << "[DELETE_LATER] Запрошено удаление панели";
    m_panel->deleteLater();
}

void MainWindow::demoDetaching()
{
    LogPanel *tempOwner = new LogPanel("TempOwner", this);
    tempOwner->hide();

    new LogLabel("Ребёнок TempOwner", tempOwner);
    LogLabel *detachedChild =
        new LogLabel("Отсоединённый ребёнок TempOwner", tempOwner);

    qDebug() << "[DEMO] Число детей до setParent(nullptr):"
             << tempOwner->children().count();

    detachedChild->setParent(nullptr);

    qDebug() << "[DEMO] Число детей после setParent(nullptr):"
             << tempOwner->children().count();

    setStatus("Один объект остался ребёнком TempOwner, второй отсоединён. "
              "Оба будут безопасно удалены через deleteLater().");

    tempOwner->deleteLater();
    detachedChild->deleteLater();
}
