#include "light.hh"
#include "context.hh"

namespace lights {
class Missyee : public light::Light {
public:
    Missyee(uint16_t start);

    void write_to_buffer(std::vector<uint8_t>& buffer) override;

public:
    // Brighness, R, G, B
    size_t input_count() const override { return 4; }

    void callback(runner::Context& context) override;

private:
    uint8_t red_ = 0;
    uint8_t green_ = 0;
    uint8_t blue_ = 0;
    uint8_t brightness_ = 255;
};
}