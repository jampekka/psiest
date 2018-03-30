#include "xtensor/xarray.hpp"
#include "xtensor/xview.hpp"
#include "xtensor/xsort.hpp"
#include "xtensor/xmath.hpp"
#include "xtensor/xvectorize.hpp"

namespace psiest {

using namespace xt;

using xa = xarray<float>;
using xv = xtensor<float,1>;

struct gumbel {
	float threshold;
	float slope;
	float guess_rate;
	float lapse_rate;

	static float eval(float t, float s, float g, float l, float x) {
		auto raw = 1.0 - exp(-pow(10.0, (s*(x - t))));
		return g + (1.0 - g - l)*raw;
	}

	gumbel(float t, float s, float g, float l)
	:threshold(t), slope(s), guess_rate(g), lapse_rate(l)
	{
		
	}

	float operator()(float x) {
		return gumbel::eval(threshold, slope, guess_rate, lapse_rate, x);
	}
};

struct PsiEstimator {
	float guess_rate;
	xv lapses;
	xv thresholds;
	xv slopes;
	xtensor<float,4> probs;
	xv xs;
	xtensor<float,3> weights;
	
	PsiEstimator(float guess_rate,
		xv lapses=linspace<float>(0.01, 0.2, 10),
		xv thresholds=linspace<float>(-0.1, 1.1, 30),
		xv slopes=linspace<float>(0.0, 10, 30),
		xv xs=linspace<float>(0.0, 1.0, 100)
		)
		: guess_rate(guess_rate), lapses(lapses), thresholds(thresholds), slopes(slopes),
		xs(xs)
	{
		auto [xx, ll, tt, ss] = meshgrid(xs, lapses, thresholds, slopes);
		
		auto f = vectorize(gumbel::eval);
		probs = f(tt, ss, guess_rate, ll, xx);
		weights = ones<float>({lapses.size(), thresholds.size(), slopes.size()});
		weights /= sum(weights);
	}

	void measure_xi(size_t xi, bool r) {
		auto ps = view(probs, xi);
		if(!r) {
			ps = 1.0 - ps;
		}
		weights *= ps;
		weights /= sum(weights)();
	}

	void measure_x(float x, bool r) {
		int xi = argmin(abs(x - xs))(0);
		measure_xi(xi, r);
	}

	float entropy() {
		return -nansum(log(weights)*weights)();
	}

	auto estimate() {
		auto [ll, tt, ss] = meshgrid(lapses, thresholds, slopes);
		auto l = sum(ll*weights)();
		auto t = sum(tt*weights)();
		auto s = sum(ss*weights)();
		return gumbel(t, s, guess_rate, l);
	}
	
	auto propose_xi() {
		auto pst = probs*weights;
		auto pst_total = nansum(pst, {1,2,3});
		auto pst_normed = pst/view(pst_total, all(), newaxis(), newaxis(), newaxis());
		auto Ht = nansum(log(pst)*pst, {1,2,3})*pst_total;
		
		auto psf = (1.0 - probs)*weights;
		auto psf_total = nansum(psf, {1,2,3});
		auto psf_normed = psf/view(psf_total, all(), newaxis(), newaxis(), newaxis());
		auto Hf = nansum(log(psf)*psf, {1,2,3})*psf_total;
		
		auto H_exp = Ht*pst_total + Hf*psf_total;
		return argmax(H_exp)(0);
	}

	float propose_x() {
		return xs(propose_xi());
	}
};
}
