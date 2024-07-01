#include "IVStars.hpp"
#include <ronin/framework.h>

IVStars::IVStars() : _dir(Vec2::up_right), _speed(2.0f), _stars {}
{
}

void IVStars::set(Vec2 direction, float speed, int count, bool startOfScreen)
{
    if(count <= 0)
    {
        clear();
        return;
    }

    if(speed <= 0)
        return;

    _dir = direction.normalized();
    _speed = speed;

    clear();
    for(; count--;)
    {
        Vec2 newPoint = Camera::ViewportToWorldPoint(Random::RandomVector());

        _stars.emplace_back(Primitive::CreateBox2D(newPoint)->spriteRenderer());
        _stars.back()->setColor(Color::MakeTransparency(Color::white, .6f));
        _stars.back()->setSize(Vec2::one / 70);
    }
}

void IVStars::clear()
{
    std::for_each(_stars.begin(), _stars.end(), [](SpriteRenderer *target) { target->gameObject()->Destroy(); });
    _stars.clear();
}



void IVStars::play()
{
    Rectf area;
    Vec2 point;

    area.setXY(Camera::ViewportToWorldPoint(Vec2::zero));
    area.setWH(Camera::ViewportToWorldPoint(Vec2::one)-area.getXY());

    for(SpriteRenderer *self : _stars)
    {
        self->transform()->Translate(_dir * Time::deltaTime() * _speed);

        point = self->transform()->position();

        if(!Vec2::HasIntersection(point, area))
        {
            //TODO: сделать так, чтобы point был установлен начальный -_dir позиция стартовой точки.
            //point = closestPointOnBorder(point, area, _dir);
            //self->transform()->position(point);
            self->transform()->position(Camera::ViewportToWorldPoint(Vec2::Abs(Random::RandomVector())));
        }
    }
}
