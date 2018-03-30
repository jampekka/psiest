psi = require './psijs.js'

lapse_rate = 1/4
target = new psi.gumbel(0.1, 2.0, lapse_rate, 0.05)
est = new psi.PsiEstimator lapse_rate
for i from [0...100]
	if (i+1)%3 == 0
		# Seems to make the convergence more
		# robust to have a random value every
		# now and then
		x = Math.random()
	else
		x = est.propose_x()
	est.measure_x x, Math.random() < target.prob(x)
	console.log "#{i}\t#{(target.threshold - est.estimate().threshold).toFixed 3}\t#{x.toFixed 3}"
