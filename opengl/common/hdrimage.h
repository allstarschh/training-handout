#ifndef HDRIMAGELOADER_H
#define HDRIMAGELOADER_H

#include <QVector>

class HdrImage {
public:
    HdrImage()
        : m_width( 0 )
        , m_height( 0 )
        , m_pixelData()
    {}

    static HdrImage loadFromFile( const QString &fileName );

    int width() const { return m_width; }
    int height() const { return m_height; }

    float minValue() const { return m_min; }
    float maxValue() const { return m_max; }

    const float *bits() const { return m_pixelData.constData(); }

private:
    int m_width;
    int m_height;
    float m_min;
    float m_max;

    // Each pixel consists of 3 floats
    QVector<float> m_pixelData;
};

#endif // HDRIMAGELOADER_H
