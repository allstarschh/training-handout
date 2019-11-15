This example shows some effects the choice of the container type can have
on your performance.

First of all, note that using QList is often a very bad idea. In the example,
the Particle type is larger than a pointer type. Thus, every element of the list
is heap-allocated which is not good at all for cache-locality.

A quick fix for this is to replace QList by QVector and recompile. In my test,
this improves the performance by ca. 25% (3s instead of 4s).

To further improve the performance, we can split the Particle type. Only its
coordinate and velocity members are required for the actually CPU demanding
simulation. The size and color would only be required for displaying the results
in a later step. Thus, it would be a good idea to separate the data, which
improved the performance for me down to 2.5s.
