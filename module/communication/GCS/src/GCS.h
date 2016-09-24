#ifndef MODULE_COMMUNICATION_GCS_H
#define MODULE_COMMUNICATION_GCS_H

#include <nuclear>
#include <mutex>
#include "utility/io/uart.h"

namespace module {
namespace communication {

    class GCS : public NUClear::Reactor {

    public:
        /// @brief Called by the powerplant to build and setup the GCS reactor.
        explicit GCS(std::unique_ptr<NUClear::Environment> environment);
    private:
        utility::io::uart uart;
    };
}
}

#endif  // MODULE_COMMUNICATION_GCS_H