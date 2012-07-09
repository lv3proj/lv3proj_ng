/*
Copyright (C) 2007, 2010 - Bit-Blot

This file is part of Aquaria.

Aquaria is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "Vector.h"
#include <assert.h>
#include <stddef.h>

/*************************************************************************/


static Vector lerp(const Vector &v1, const Vector &v2, float dt, LerpType lerpType)
{
    switch(lerpType)
    {
        case LERP_NONE:
        {
            return (v2-v1)*dt+v1;
        }

        case LERP_EASE:
        {
            // ease in and out
            const float dt2 = dt * dt;
            const float dt3 = dt2 * dt;
            return v1*(2*dt3 - 3*dt2 + 1) + v2*(3*dt2 - 2*dt3);
        }

        case LERP_EASEIN:
        {
            float lerpAvg = 1.0f-dt;
            return (v2-v1)*(sinf(dt*PI_HALF)*(1.0f-lerpAvg)+dt*lerpAvg)+v1;
        }

        case LERP_EASEOUT:
        {
            return (v2-v1)*-sinf(-dt*PI_HALF)+v1;
        }
    }

    assert(false); // not reached
    return Vector();
}

/*************************************************************************/

/*
float SmoothCurve( float x )
{
    return (1 - cosf( x * PI_F )) * 0.5f;
}

float SimpleSpline( float value )
{
    float valueSquared = value * value;

    // Nice little ease-in, ease-out spline-like curve
    return (3 * valueSquared - 2 * valueSquared * value);
}
*/

void VectorPath::addPathNode(Vector v, float p)
{
    VectorPathNode node;
    node.value = v;
    node.percent = p;
    pathNodes.push_back(node);
}

void VectorPath::flip()
{
    std::vector<VectorPathNode> copyNodes;
    copyNodes = pathNodes;
    pathNodes.clear();
    for (size_t i = copyNodes.size()-1; i >=0; i--)
    {
        copyNodes[i].percent = 1 - copyNodes[i].percent;
        pathNodes.push_back(copyNodes[i]);
    }
}

void VectorPath::realPercentageCalc()
{
    float totalLen = getLength();
    float len = 0;
    for (size_t i = 1; i < pathNodes.size(); i++)
    {
        Vector diff = pathNodes[i].value - pathNodes[i-1].value;
        len += diff.getLength2D();

        pathNodes[i].percent = len/totalLen;
    }
}

float VectorPath::getLength()
{
    float len = 0;
    for (size_t i = 1; i < pathNodes.size(); i++)
    {
        Vector diff = pathNodes[i].value - pathNodes[i-1].value;
        len += diff.getLength2D();
    }
    return len;
}

void VectorPath::clear()
{
    pathNodes.clear();
}

Vector VectorPath::getValue(float p) // between 0 and 1
{
    if (pathNodes.empty())
    {
        return Vector(0,0,0);
    }

    VectorPathNode *target = 0;
    VectorPathNode *from = &pathNodes[0];
    for (size_t i = 0; i < pathNodes.size(); ++i)
    {
        if (pathNodes[i].percent >= p)
        {
            target = &pathNodes[i];
            break;
        }
        from = &pathNodes[i];
    }

    if (!target)
    {
        return from->value;
    }

    float perc = ((p - from->percent) / (target->percent - from->percent));
    Vector v = (target->value - from->value) * perc;
    v += from->value;
    return v;
}

/*************************************************************************/

float InterpolatedVector::interpolateTo(Vector vec, float timePeriod, int loop, bool pingPong, bool ease)
{
    if (timePeriod == 0)
    {
        this->stop();
        this->x = vec.x;
        this->y = vec.y;
        this->z = vec.z;
        return 0;
    }

    InterpolatedVectorData *data = ensureData();

    data->ease = ease;
    data->timePassed = 0;
    if (timePeriod < 0)
    {
        timePeriod = -timePeriod;
        timePeriod = (vec-Vector(x,y,z)).getLength3D() / timePeriod;
    }
    data->timePeriod = timePeriod;
    data->from = Vector (this->x, this->y, this->z);
    data->target = vec;

    data->loop = loop;
    data->pingPong = pingPong;

    data->interpolating = true;

    return data->timePeriod;
}

void InterpolatedVector::stop()
{
    if (data)
        data->interpolating = false;
}

void InterpolatedVector::startPath(float time)
{
    InterpolatedVectorData *data = ensureData();

    if (data->path.getNumPathNodes()==0)
        return;
    data->pathTimer = 0;
    data->pathTime = time;
    data->followingPath = true;
    data->loop = 0;
    data->pingPong = false;
    // get the right values to start off with
    _updatePath(0);
}

void InterpolatedVector::stopPath()
{
    if (data)
        data->followingPath = false;
}

void InterpolatedVector::resumePath()
{
    InterpolatedVectorData *data = ensureData();
    data->followingPath = true;
}

void InterpolatedVector::_updatePath(float dt)
{
    InterpolatedVectorData *data = ensureData();
    if (data->pathTimer > data->pathTime)
    {
        Vector value = data->path.getPathNode(data->path.getNumPathNodes()-1)->value;
        this->x = value.x;
        this->y = value.y;
        this->z = value.z;
        if (data->loop != 0)
        {
            if (data->loop > 0)
                data->loop -= 1;

            int oldLoop = data->loop;

            if (data->pingPong)
            {
                // flip path
                data->path.flip();
                startPath(data->pathTime);
                data->loop = oldLoop;
            }
            else
            {
                startPath(data->pathTime);
                data->loop = oldLoop;
            }
        }
        else
        {
            stopPath();
        }
    }
    else
    {
        data->pathTimer += dt * data->pathTimeMultiplier;

        float perc = data->pathTimer/data->pathTime;
        Vector value = data->path.getValue(perc);
        this->x = value.x;
        this->y = value.y;
        this->z = value.z;
    }
}

float InterpolatedVector::getPercentDone()
{
    return data ? data->timePassed/data->timePeriod : 0;
}

void InterpolatedVector::_doInterpolate(float dt)
{
    InterpolatedVectorData *data = ensureData();

    data->timePassed += dt;
    if (data->timePassed >= data->timePeriod)
    {
        this->x = data->target.x;
        this->y = data->target.y;
        this->z = data->target.z;
        data->interpolating = false;

        if (data->loop != 0)
        {
            if (data->loop > 0)
                data->loop -= 1;

            if (data->pingPong)
            {
                interpolateTo (data->from, data->timePeriod, data->loop, data->pingPong, data->ease);
            }
            else
            {
                this->x = data->from.x;
                this->y = data->from.y;
                this->z = data->from.z;
                interpolateTo (data->target, data->timePeriod, data->loop, data->pingPong, data->ease);
            }
        }

    }
    else
    {
        Vector v = lerp(data->from, data->target, (data->timePassed / data->timePeriod), data->ease ? LERP_EASE : LERP_NONE);

        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
    }
}
