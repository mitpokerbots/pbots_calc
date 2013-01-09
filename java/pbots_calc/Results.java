/**
 * Copyright (C) 2012-2013 Owen Derby (ocderby@gmail.com)
 *
 * This file is part of pbots_calc.
 *
 * pbots_calc is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * pbots_calc is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * pbots_calc in a file in teh toplevel directory called "GPLv3".  If not, see
 * <http://www.gnu.org/licenses/>.
 */
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
