/*************************************************************************
 *
 * Copyright (c) 2019, Klaralvdalens Datakonsult AB (KDAB)
 * All rights reserved.
 *
 * See the LICENSE.txt file shipped along with this file for the license.
 *
 *************************************************************************/

#include <QVector3D>
#include <QColor>

#include <QList>
#include <QVector>

#include <QMap>
#include <QHash>

#include <list>
#include <vector>
#include <deque>

#include <map>
#include <unordered_map>

#include <iostream>

struct Particle
{
    QVector3D coordinate;
    QVector3D velocity;
    QColor color;
    double size = 0;
};

Q_DECLARE_TYPEINFO(Particle, Q_MOVABLE_TYPE);

// support map and list containers with one common API
namespace Container {
void insert(std::map<int, Particle>* map, int i, const Particle& p)
{
    map->insert({i, p});
}

void insert(std::unordered_map<int, Particle>* map, int i, const Particle& p)
{
    map->insert({i, p});
}

void insert(QMap<int, Particle>* map, int i, const Particle& p)
{
    map->insert(i, p);
}

void insert(QHash<int, Particle>* map, int i, const Particle& p)
{
    map->insert(i, p);
}

template <typename T>
void insert(T* list, int /*i*/, const Particle& p) { list->push_back(p); }

Particle& value(std::map<int, Particle>::iterator it) { return it->second; }

Particle& value(std::unordered_map<int, Particle>::iterator it) { return it->second; }

Particle& value(QMap<int, Particle>::iterator it) { return it.value(); }

Particle& value(QHash<int, Particle>::iterator it) { return it.value(); }

Particle& value(std::list<Particle>::iterator item) { return *item; }

Particle& value(std::vector<Particle>::iterator item) { return *item; }

Particle& value(std::deque<Particle>::iterator item) { return *item; }

Particle& value(QList<Particle>::iterator item) { return *item; }

Particle& value(QVector<Particle>::iterator item) { return *item; }
}

#if Q_CC_MSVC
// disable inlining on MSVC, as it does not emit information about inline frames
#define MAYBE_DISABLE_INLINING Q_NEVER_INLINE
#else
// other platforms are fine
#define MAYBE_DISABLE_INLINING
#endif

template <typename ParticleList>
MAYBE_DISABLE_INLINING ParticleList generateParticles(int points)
{
    ParticleList list;
    for (int i = 0; i < points; ++i) {
        Particle p;
        // NOTE: we don't care about the values in this example
        Container::insert(&list, i, p);
    }
    return list;
}

template <typename ParticleList>
MAYBE_DISABLE_INLINING void simulateTimestep(ParticleList* particles, double deltaT)
{
    for (auto it = particles->begin(), end = particles->end(); it != end; ++it) {
        auto& particle = Container::value(it);
        particle.coordinate += deltaT * particle.velocity;
    }
}

template <typename ParticleList>
MAYBE_DISABLE_INLINING void runExperiment()
{
    const int numParticles = 10000;
    const int numTimeSteps = 100000;
    auto particles = generateParticles<ParticleList>(numParticles);

    for (int i = 0; i < numTimeSteps; ++i) {
        simulateTimestep(&particles, 0.1);
    }
}

int main(int argc, char** argv)
{
    const char* arg = argc == 2 ? argv[1] : "";
    if (!strcmp(arg, "list")) {
        runExperiment<std::list<Particle>>();
    } else if (!strcmp(arg, "vector")) {
        runExperiment<std::vector<Particle>>();
    } else if (!strcmp(arg, "deque")) {
        runExperiment<std::deque<Particle>>();
    } else if (!strcmp(arg, "QList")) {
        runExperiment<QList<Particle>>();
    } else if (!strcmp(arg, "QVector")) {
        runExperiment<QVector<Particle>>();
    } else if (!strcmp(arg, "map")) {
        runExperiment<std::map<int, Particle>>();
    } else if (!strcmp(arg, "unordered_map")) {
        runExperiment<std::unordered_map<int, Particle>>();
    } else if (!strcmp(arg, "QMap")) {
        runExperiment<QMap<int, Particle>>();
    } else if (!strcmp(arg, "QHash")) {
        runExperiment<QHash<int, Particle>>();
    } else {
        std::cerr << "Usage: ex_container [list|vector|deque|QList|QVector|map|unordered_map|QMap|QHash]" << std::endl;
        if (argc != 2) {
            std::cerr << "Expected one argument, " << (argc - 1) << " given." << std::endl;
        }
        return 1;
    }
}
