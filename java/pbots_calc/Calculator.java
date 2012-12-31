package pbots_calc;

import org.bridj.Pointer;

public class Calculator {

	public static Results calc(String hands, String board, String dead,
			int iters) {
		Pointer<Pbots_calcResults> res = Pbots_calcLibrary.alloc_results();
		Results results = null;
		if (Pbots_calcLibrary.calc(Pointer.pointerToCString(hands),
				Pointer.pointerToCString(board),
				Pointer.pointerToCString(dead), iters, res) > 0) {
			results = new Results(res.get());
		}
		Pbots_calcLibrary.free_results(res);
		return results;
	}

	public static void main(String[] args) {
		if (args.length < 1) {
			System.out.println("options: hand1:hand2:hand... [board [dead]]");
			System.exit(0);
		}
		String board = "";
		String dead = "";
		if (args.length >= 2) {
			board = args[1];
			if (args.length >= 3) {
				dead = args[2];
			}
		}
		Results r = calc(args[0], board, dead, 1000000);
		for (int i = 0; i < r.getSize(); i++) {
			System.out.println(r.getHands().get(i) + ":" + r.getEv().get(i));
		}
	}
}
