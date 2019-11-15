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

#include <list>
#include <vector>
#include <deque>

#include <iostream>

#ifdef HAVE_PAPI
#include <papi.h>
#else
using long_long = quint64;
#endif

struct Ipc // Instructions per cycle
{
    static Ipc measure()
    {
        Ipc data;
#ifdef HAVE_PAPI
        int ret = PAPI_ipc(&data.realTime, &data.processTime,
                           &data.instructions, &data.ipc);
        if (ret != 0) {
            std::cerr << "IPC measurement failed with code " << ret << ": "
                      << PAPI_strerror(ret) << std::endl;
        }
#endif
        return data;
    }

    void print(const char* label) const
    {
#ifndef HAVE_PAPI
        std::cerr << "PAPI support not enabled" << std::endl;
#endif
        std::cout << label
                  << "\n\trealtime elapsed: " << realTime
                  << ", process time elapsed: " << processTime
                  << "\n\tinstructions executed: " << instructions
                  << ", cycles: " << (instructions / ipc)
                  << ", IPC: " << ipc
                  << "\n";
    }

    Ipc diff(const Ipc& rhs) const
    {
        Ipc ret;
        ret.realTime = realTime - rhs.realTime;
        ret.processTime = processTime - rhs.processTime;
        ret.instructions = instructions - rhs.instructions;
        ret.ipc = ipc; // no diff here
        return ret;
    }

    float realTime = 0;
    float processTime = 0;
    long_long instructions = 0;
    float ipc = 0;
};

struct Particle
{
    QVector3D coordinate;
    QVector3D velocity;
    QColor color;
    double size;
};

Q_DECLARE_TYPEINFO(Particle, Q_MOVABLE_TYPE);

// support map and list containers with one common API

template<typename ParticleList>
ParticleList generateParticles(int points)
{
    ParticleList list;
    for (int i = 0; i < points; ++i) {
        Particle p;
        // NOTE: we don't care about the values in this example
        list.push_back(p);
    }
    return list;
}

template<typename ParticleList>
void simulateTimestep(ParticleList* particles, double deltaT)
{
    for (auto it = particles->begin(), end = particles->end(); it != end; ++it) {
        auto& particle = *it;
        particle.coordinate += deltaT * particle.velocity;
    }
}

template<typename ParticleList>
void runExperiment()
{
    std::cout << Q_FUNC_INFO << std::endl;
    // start measurement
    const auto start = Ipc::measure();

    const int numParticles = 10000;
    const int numTimeSteps = 10000;
    auto particles = generateParticles<ParticleList>(numParticles);
    const auto mid = Ipc::measure();

    for (int i = 0; i < numTimeSteps; ++i) {
        simulateTimestep(&particles, 0.1);
    }
    const auto end = Ipc::measure();

    mid.diff(start).print("    generating particles:");
    end.diff(mid).print("    simulating timesteps:");
}

int main()
{
    runExperiment<std::list<Particle>>();
    runExperiment<std::vector<Particle>>();
    runExperiment<std::deque<Particle>>();
    return 0;
}
