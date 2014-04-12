/***************************************************************************//**
* @file
* Portable serial and timer wrapper library.
*
* @version @n 1.0
* @date @n 8/3/2012
*
* @authors @n Kwabena W. Agyeman & Christopher J. Leaf
* @copyright @n (c) 2012 Kwabena W. Agyeman & Christopher J. Leaf
* @n All rights reserved - Please see the end of the file for the terms of use
*
* @par Update History:
* @n v0.1 - Beta code - 3/20/2012
* @n v0.9 - Original release - 4/18/2012
* @n v1.0 - Documented and updated release - 8/3/2012
*******************************************************************************/

#include "CMUcom4.h"

/*******************************************************************************
* Constructor Functions
*******************************************************************************/

CMUcom4::CMUcom4()
{
    _timer.start();
    _port = CMUCOM4_SERIAL;
}

CMUcom4::CMUcom4(int port)
{
    _timer.start();
    _port = port;
}

/*******************************************************************************
* Public Functions
*******************************************************************************/

void CMUcom4::begin(unsigned long baud)
{
    Sleep::msleep(CMUCOM4_BEGIN_DELAY);
    CMUcom.setBaudRate((BaudRateType) baud);
    Sleep::msleep(CMUCOM4_BEGIN_DELAY);
}

void CMUcom4::end()
{
    Sleep::msleep(CMUCOM4_END_DELAY);
    CMUcom.flush();
    Sleep::msleep(CMUCOM4_END_DELAY);
}

int CMUcom4::read()
{
    char c;

    return (CMUcom.read(&c, 1) == 1) ? c : -1;
}

size_t CMUcom4::write(uint8_t c)
{
    return (CMUcom.write((const char *) &c, 1) == 1) ? 1 : 0;
}

size_t CMUcom4::write(const char * str)
{
    qint64 len = CMUcom.write(str);

    return (len < 0) ? 0 : (size_t) len;
}

size_t CMUcom4::write(const uint8_t * buffer, size_t size)
{
    qint64 len = CMUcom.write((const char *) buffer, (qint64) size);

    return (len < 0) ? 0 : (size_t) len;
}

int CMUcom4::available()
{
    qint64 bytes = CMUcom.bytesAvailable();

    if(bytes <= 0)
    {
        QEventLoop loop;

        QObject::connect(&CMUcom, SIGNAL(readyRead()), &loop, SLOT(quit()));
        QTimer::singleShot(1, &loop, SLOT(quit()));

        loop.exec();

        bytes = CMUcom.bytesAvailable();
    }

    return (bytes < 0) ? 0 : (int) bytes;
}

void CMUcom4::flush()
{
    CMUcom.flush();
}

int CMUcom4::peek()
{
    char data;

    return (CMUcom.peek(&data, 1) == 1) ? data : -1;
}

void CMUcom4::delayMilliseconds(unsigned long ms)
{
    Sleep::msleep(ms);
}

unsigned long CMUcom4::milliseconds()
{
    return (unsigned long) _timer.elapsed();
}

/***************************************************************************//**
* @file
* @par MIT License - TERMS OF USE:
* @n Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* @n
* @n The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* @n
* @n THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/
