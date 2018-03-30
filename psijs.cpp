#include "psi.hpp"
#include <emscripten/bind.h>
using namespace emscripten;
using P = psiest::PsiEstimator;
using gumbel = psiest::gumbel;

EMSCRIPTEN_BINDINGS(psiest) {
	class_<gumbel>("gumbel")
		.constructor<double, double, double, double>()
		.function("prob", &gumbel::operator())
		.property("threshold", &gumbel::threshold)
		.property("slope", &gumbel::slope)
		.property("guess_rate", &gumbel::guess_rate)
		.property("lapse_rate", &gumbel::lapse_rate)
	;
	class_<P>("PsiEstimator")
		.constructor<double>()
		.function("measure_x", &P::measure_x)
		.function("propose_x", &P::propose_x)
		.function("entropy", &P::entropy)
		.function("estimate", &P::estimate)
	;
}
