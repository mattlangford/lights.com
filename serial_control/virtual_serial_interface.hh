#include "serial_control/abstract_serial_interface.hh"


namespace serial
{

class virtual_serial_interface : public abstract_serial_interface
{
public: // deconstructor //////////////////////////////////////////////////////
    virtual ~virtual_serial_interface() = default;

public: // public methods /////////////////////////////////////////////////////
    bool write_data(ByteVector_t data) const override { return true; }
};
}
