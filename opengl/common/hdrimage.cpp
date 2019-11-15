#include "hdrimage.h"

#include <QFile>

#include <limits>
#include <cmath>
#include <stdio.h>

typedef unsigned char RGBE[4];
#define R 0
#define G 1
#define B 2
#define E 3

#define MINELEN 8       // minimum scanline length for encoding
#define MAXELEN 0x7fff  // maximum scanline length for encoding

namespace {

float convertComponent( int expo, int val )
{
    float v = val / 256.0f;
    float d = std::pow( 2.0f, expo );
    return v * d;
}

void workOnRGBE( RGBE *scan, int len, float *cols )
{
    while ( len-- > 0 )
    {
        int expo = scan[0][E] - 128;
        cols[0] = convertComponent( expo, scan[0][R] );
        cols[1] = convertComponent( expo, scan[0][G] );
        cols[2] = convertComponent( expo, scan[0][B] );
        cols += 3;
        scan++;
    }
}

bool oldDecrunch( RGBE *scanline, int len, QFile& file )
{
    int i;
    int rshift = 0;

    while (len > 0)
    {
        char tmp;
        file.getChar( &tmp );
        scanline[0][R] = tmp;
        file.getChar( &tmp );
        scanline[0][G] = tmp;
        file.getChar( &tmp );
        scanline[0][B] = tmp;
        file.getChar( &tmp );
        scanline[0][E] = tmp;
        if ( file.atEnd() )
            return false;

        if ( scanline[0][R] == 1 &&
             scanline[0][G] == 1 &&
             scanline[0][B] == 1 )
        {
            for ( i = scanline[0][E] << rshift; i > 0; i-- )
            {
                memcpy( &scanline[0][0], &scanline[-1][0], 4 );
                scanline++;
                len--;
            }
            rshift += 8;
        }
        else
        {
            scanline++;
            len--;
            rshift = 0;
        }
    }
    return true;
}

bool decrunch( RGBE *scanline, int len, QFile& file )
{
    int  i, j;

    if ( len < MINELEN || len > MAXELEN )
        return oldDecrunch(scanline, len, file);

    char tmp;
    file.getChar( &tmp );
    i = int(tmp);
    if (i != 2) {
        file.seek( file.pos() - 1 );
        return oldDecrunch( scanline, len, file );
    }

    file.getChar( &tmp );
    scanline[0][G] = tmp;
    file.getChar( &tmp );
    scanline[0][B] = tmp;
    file.getChar( &tmp );
    i = tmp;

    if (scanline[0][G] != 2 || scanline[0][B] & 128)
    {
        scanline[0][R] = 2;
        scanline[0][E] = i;
        return oldDecrunch( scanline + 1, len - 1, file );
    }

    // read each component
    for ( i = 0; i < 4; i++ )
    {
        for ( j = 0; j < len; )
        {
            unsigned char code;
            file.getChar( &tmp );
            code = tmp;

            if ( code > 128 )
            { // run
                code &= 127;
                unsigned char val;
                file.getChar( &tmp );
                val = tmp;
                while ( code-- )
                    scanline[j++][i] = val;
            }
            else
            { // non-run
                while ( code-- )
                {
                    file.getChar( &tmp );
                    scanline[j++][i] = tmp;
                }
            }
        }
    }

    return !file.atEnd();
}

} // anonymous namespace

HdrImage HdrImage::loadFromFile( const QString &fileName )
{
    HdrImage image;

    QFile file( fileName );
    if ( !file.open( QIODevice::ReadOnly ) )
        return image;

    char str[200];

    file.read( str, 10 );
    if (memcmp(str, "#?RADIANCE", 10))
        return image;

    file.seek( file.pos() + 1 );

    char cmd[200];
    int i;
    i = 0;
    char c = 0, oldc;
    while ( true )
    {
        oldc = c;
        file.getChar( &c );
        if ( c == 0xa && oldc == 0xa )
            break;
        cmd[i++] = c;
    }

    char reso[200];
    i = 0;
    while ( true )
    {
        file.getChar( &c );
        reso[i++] = c;
        if ( c == 0xa )
            break;
    }

    int w, h;
    if ( !sscanf( reso, "-Y %d +X %d", &h, &w ) )
        return image;

    image.m_width = w;
    image.m_height = h;

    QVector<float> pixelData( w * h * 3 );
    float *data = pixelData.data();

    RGBE *scanline = new RGBE[w];

    // convert image
    for ( int y = h - 1; y >= 0; y-- )
    {
        if ( !decrunch( scanline, w, file ) )
            break;
        workOnRGBE( scanline, w, data );
        data += w * 3;
    }

    image.m_pixelData = pixelData;

    delete[] scanline;

    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();
    data = pixelData.data();
    for ( int j = 0; j < image.height(); ++j )
    {
        for ( int i = 0; i < image.width(); ++i )
        {
            float value = *(data + j * image.width() + i);
            if (value > max)
                max = value;
            if (value < min)
                min = value;
        }
    }
    image.m_min = min;
    image.m_max = max;

    return image;
}
