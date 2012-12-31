package pbots_calc;

import java.util.ArrayList;
import java.util.List;

public class Results {
	private final List<Double> ev;
	private final List<String> hands;
	// Did we use montecarlo simulation?
	private final boolean MC_used;
	private final int size;
	private final int iters;

	public Results(Pbots_calcResults res) {
		this.size = res.size();
		this.iters = res.iters();
		this.MC_used = res.MC() == 1;
		this.ev = new ArrayList<Double>();
		this.hands = new ArrayList<String>();
		for (int i = 0; i < this.size; i++) {
			this.ev.add(res.ev().get(i));
			this.hands.add(res.hands().get(i).getCString());
		}
	}

	public List<Double> getEv() {
		return ev;
	}

	public List<String> getHands() {
		return hands;
	}

	/**
	 * Check if the returned results were achieved by Monte Carlo methods, or if
	 * there were few enough total cases that exhaustive enumeration was used.
	 * 
	 * @return True if Monte Carlo simulation was run.
	 */
	public boolean isMC_used() {
		return MC_used;
	}

	public int getSize() {
		return size;
	}

	public int getIters() {
		return iters;
	}
}
