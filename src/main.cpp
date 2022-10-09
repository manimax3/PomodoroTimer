#include <QDesktopServices>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QSettings>
#include <QStandardPaths>
#include <QTimer>
#include <QUrl>

class PomoManager : public QObject {
	Q_OBJECT

	Q_PROPERTY(int countPomodoros MEMBER countPomodoros NOTIFY settingsChanged)
	Q_PROPERTY(int pomoLength MEMBER pomoLength NOTIFY settingsChanged)
	Q_PROPERTY(int smallPauseLength MEMBER smallPauseLength NOTIFY settingsChanged)
	Q_PROPERTY(int bigPauseLength MEMBER bigPauseLength NOTIFY settingsChanged)

	Q_PROPERTY(int activePomoIndex MEMBER activePomoIndex WRITE setActivePomoIndex NOTIFY activePomoIndexChanged)

	Q_PROPERTY(QString state READ getPomoState NOTIFY pomoStateChanged)
	Q_PROPERTY(QString timeRemaining READ fmtTimeRemaining NOTIFY timeRemainingChanged)

	Q_PROPERTY(float progress READ getProgress NOTIFY timeRemainingChanged)

public:
	enum class PomoState { Pomo, ShortPause, LongPause };
	Q_ENUM(PomoState);

	PomoManager(QObject *parent = nullptr)
		: QObject(parent)
	{
		configPath   = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QStringLiteral("/Pomodoro/");
		settingsPath = configPath + "Pomodoro.conf";

		reloadSettings();

		timeRemaining = pomoLength;
		timer         = new QTimer(this);
		timer->setInterval(1000);
		connect(timer, &QTimer::timeout, this, &PomoManager::onTimer);
	}

	void reloadSettings()
	{

		QSettings settings{ settingsPath, QSettings::IniFormat };
		countPomodoros   = settings.value("countPomodoros", 4).toInt();
		pomoLength       = settings.value("pomoLength", 25 * 60).toInt();
		smallPauseLength = settings.value("smallPauseLength", 5 * 60).toInt();
		bigPauseLength   = settings.value("bigPauseLength", 15 * 60).toInt();

		settings.setValue("countPomodoros", countPomodoros);
		settings.setValue("pomoLength", pomoLength);
		settings.setValue("smallPauseLength", smallPauseLength);
		settings.setValue("bigPauseLength", bigPauseLength);

		emit settingsChanged();
	}

	Q_INVOKABLE void viewSettingsFolder() const
	{
		QDesktopServices::openUrl(QUrl::fromLocalFile(configPath));
	}

public slots:
	void start()
	{
		timer->start();
	}

	void pause()
	{
		timer->stop();
	}

	void stop()
	{
		timer->stop();
		setActivePomoIndex(0);
	}

signals:
	void settingsChanged();

	void activePomoIndexChanged();

	void pomoStateChanged();
	void timeRemainingChanged();

private:
	void onTimer()
	{
		if (timeRemaining > 0) {
			timeRemaining -= 1;
			emit timeRemainingChanged();
			return;
		}

		if (currentState == PomoState::Pomo) {
			if ((activePomoIndex + 1) % countPomodoros == 0) {
				currentState  = PomoState::LongPause;
				timeRemaining = bigPauseLength;
			} else {
				currentState  = PomoState::ShortPause;
				timeRemaining = smallPauseLength;
			}
		} else {
			currentState    = PomoState::Pomo;
			timeRemaining   = pomoLength;
			activePomoIndex = (activePomoIndex + 1) % countPomodoros;
			emit activePomoIndexChanged();
		}

		emit pomoStateChanged();
		emit timeRemainingChanged();
	}

	void setActivePomoIndex(int index)
	{
		if (activePomoIndex != index) {
			activePomoIndex = index;
			emit activePomoIndexChanged();
		}

		if (timeRemaining != pomoLength) {
			timeRemaining = pomoLength;
			emit timeRemainingChanged();
		}

		if (currentState != PomoState::Pomo) {
			currentState = PomoState::Pomo;
			emit pomoStateChanged();
		}
	}

	QString getPomoState() const
	{
		switch (currentState) {
		case PomoState::Pomo:
			return QStringLiteral("Work");
		case PomoState::ShortPause:
			return QStringLiteral("Short Break");
		case PomoState::LongPause:
			return QStringLiteral("Long Break");
		}

		return QStringLiteral();
	}

	QString fmtTimeRemaining() const
	{
		return QStringLiteral("%1:%2")
			.arg(timeRemaining / 60, 2, 10, QLatin1Char('0'))
			.arg(timeRemaining % 60, 2, 10, QLatin1Char('0'));
	}

	float getProgress() const
	{
		const float remaining = static_cast<float>(timeRemaining);

		switch (currentState) {
		case PomoState::Pomo:
			return remaining / pomoLength;
		case PomoState::ShortPause:
			return remaining / smallPauseLength;
		case PomoState::LongPause:
			return remaining / bigPauseLength;
		}

		return 1.f;
	}

	int countPomodoros = 4;

	int pomoLength       = 10;
	int smallPauseLength = 5;
	int bigPauseLength   = 15;

	int activePomoIndex = 0;
	int timeRemaining   = 0;

	QString configPath, settingsPath;

	PomoState currentState = PomoState::Pomo;

	QTimer *timer;
};

int main(int argc, char **argv)
{
	QGuiApplication app{ argc, argv };
	QCoreApplication::setApplicationName(QStringLiteral("Pomodoro"));

	app.setApplicationDisplayName("Pomodoro");
	app.setApplicationName("Pomodoro");
	app.setWindowIcon(QIcon(":/images/appicon.svg"));

	qmlRegisterCustomType<PomoManager>("Backend", 1, 0, "PomoManager", 0);

	QQmlApplicationEngine engine;

	engine.load(":/qml/main.qml");

	return app.exec();
}

#include "main.moc"
