#include "serial.hh"

#include <thread>


int main()
{
    serial::serial_connection conneciton(50000);

    while (true)
    {
        conneciton.write_data({255, 255, 0, 0, 255, 0});
        std::this_thread::sleep_for(std::chrono::duration<double>(1.0));
    }
}
