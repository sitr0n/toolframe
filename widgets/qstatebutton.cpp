#include "qstatebutton.h"

QStateButton::QStateButton(QWidget *parent)
    : QPushButton(parent)
    , currentState(0)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

void QStateButton::addState(const QString &name, QIcon *icon)
{
    m_states.emplace_back(name);
    if (icon != nullptr) {
        icons.insert(std::make_pair(name, std::move(icon)));
    }
    if (m_states.size() > 0) {
        updateButton();
    }
}

void QStateButton::addState(const QString &state, std::function<bool ()> process)
{
    m_states.emplace_back(state);
    m_actions.insert(state, process);
    if (m_states.size() == 1) {
        updateButton();
    }
}

void QStateButton::setState(const QString &name)
{
    int index = 0;
    for (const auto& state: m_states) {
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
    while (m_queue.front() != name) {
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
    if (m_states.empty()) {
        return;
    }
    auto action = m_actions.value(m_states.at(currentState));
    if (action()) {
        if (++currentState >= m_states.size()) {
            currentState = 0;
        }
        updateButton();
    }
}

void QStateButton::updateButton()
{
    clearButton();
    auto state = m_states.at(currentState);
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
