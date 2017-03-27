#include "recorder.h"


Recorder::Recorder()
{

}

void Recorder::startStream()
{
    if(!cap.open(0))
        return;
}

void Recorder::endStream()
{
    cap.release();
}

void Recorder::grabFrame()
{
    cap >> _frame;
}
