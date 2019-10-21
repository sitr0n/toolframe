#include "qstatebutton.h"

QStateButton::QStateButton(QWidget *parent)
    : QPushButton(parent)
    , currentState(0)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void QStateButton::addState(const QString &name, QIcon *icon)
{
    states.emplace_back(name);
    if (icon != nullptr) {
        icons.insert(std::make_pair(name, std::move(icon)));
    }
    if (states.size() > 0) {
        updateButton();
    }
}

void QStateButton::addState(const QString &state, std::function<bool ()> process)
{
    m_actions.insert(state, process);
}

void QStateButton::setState(const QString &name)
{
    int index = 0;
    for (const auto& state: states) {
        if (QString::compare(state, name) == 0) {
            currentState = index;
            updateButton();
            break;
        } else {
            ++index;
        }
    } // -------------------------------
    if (!m_queue.contains(name)) {
        return;
    }
    while (m_queue.front != name) {
        auto state = m_queue.front();
        m_queue.pop_front();
        m_queue.push_back(state);
    }
}

void QStateButton::setMinimumSize(const QSize &size)
{
    QAbstractButton::setMinimumSize(size);
    setIconSize(size);
}

void QStateButton::mousePressEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    if (!states.empty()) {
        emit state_change(states.at(currentState));
    }
    if (++currentState >= states.size()) {
        currentState = 0;
    }
    updateButton();
}

void QStateButton::updateButton()
{
    clearButton();
    auto state = states.at(currentState);
    if (icons[state] != nullptr) {
        setIcon(*icons[state]);
    } else {
        setText(state);
    }
}

void QStateButton::clearButton()
{
    setIcon(QIcon());
    setText(QString());
}
