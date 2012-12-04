#include "player.h"

int Player::_volume_level = 0;
QPointer<Player> Player::_instance = 0;

QPointer<Player> Player::getInstance(QObject *parent)
{
    Q_UNUSED(parent);
    return _instance;
}

Player::Player(QObject *parent) :
    QObject(parent)
{
}

Player::~Player()
{
}

void Player::registerInstance(Player *player)
{
    _instance = player;
}
