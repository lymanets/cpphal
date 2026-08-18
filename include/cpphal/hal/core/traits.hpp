#pragma once

namespace hal {

namespace uart {
template <int Number>
class tag {
};
}

namespace spi {
template <int Number>
class tag {
};
}

namespace i2c {
template <int Number>
class tag {
};
}

namespace timer {
template <int Number>
class tag {
};
}

template <class Tag>
struct traits;
}