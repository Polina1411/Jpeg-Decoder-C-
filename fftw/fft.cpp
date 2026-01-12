#include <fft.h>
#include <fftw3.h>
#include <cmath>
#include <stdexcept>
class DctCalculator::Impl {
public:
    fftw_plan p;
    size_t n;
    double* i;
    double* o;
    Impl() : p(nullptr), n(0), i(nullptr), o(nullptr) {
    }
};
DctCalculator::DctCalculator(size_t w, std::vector<double>* in, std::vector<double>* out)
    : impl_(std::make_unique<Impl>()) {
    if (!in || !out) {
        throw std::invalid_argument("ввод нулевой");
    }
    if (in->size() != w * w || out->size() != w * w) {
        throw std::invalid_argument("размер не тот");
    }
    impl_->n = w;
    impl_->i = in->data();
    impl_->o = out->data();

    impl_->p = fftw_plan_r2r_2d(static_cast<int>(w), static_cast<int>(w), impl_->i, impl_->o,
                                FFTW_REDFT01, FFTW_REDFT01, FFTW_ESTIMATE);
    if (!impl_->p) {
        throw std::runtime_error("план не создался");
    }
}
void DctCalculator::Inverse() {
    size_t n = impl_->n;
    double s = std::sqrt(2.0);
    for (size_t x = 0; x < n; ++x) {
        impl_->i[x * n] *= s;
    }
    for (size_t y = 0; y < n; ++y) {
        impl_->i[y] *= s;
    }
    fftw_execute(impl_->p);
    double f = 1.0 / (2.0 * n);
    for (size_t k = 0; k < n * n; ++k) {
        impl_->o[k] *= f;
    }
}
DctCalculator::~DctCalculator() {
    if (impl_ && impl_->p) {
        fftw_destroy_plan(impl_->p);
    }
}